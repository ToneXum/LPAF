// LPAF - lightweight and performant application framework
// Copyright (C) 2024 ToneXum (Toni Stein)
//
// This program is free software: you can redistribute it and/or modify it under the terms of the
// GNU General Public License as published by the Free Software Foundation, either version 3 of
// the License, or any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program. If
// not, see <https://www.gnu.org/licenses/>.

// This implementation file contains implementations for Linux-specific, exposed symbols

#ifdef PLATFORM_LINUX

#include "internal.h"
#include "linux.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>

struct fwiNativeSocketState {
    char* targetAddress;
    int32_t addressFamily;
    int32_t protocol;
    int32_t fileDescriptor;
    bool connected, bound;
};

fwError fwGetSystemConfiguration(fwSystemConfiguration* res_p) {
    res_p->cores  = sysconf(_SC_NPROCESSORS_ONLN);
    // TODO: figure out why only first memory bank is counted
    res_p->memory = sysconf(_SC_PHYS_PAGES) * sysconf(_SC_AVPHYS_PAGES) / 1048576; // 1024^2, to MiB
    return fwErrorSuccess;
}

fwError fwLoadFileToMem(const char* filename_p, void** buffer_pp, uint64_t* fileSize_p) {
    FILE* file = fopen(filename_p, "rb");
    if (!(uintptr_t)file) {
        return fwErrorFileUnableToOpen;
    }

    const int32_t fileDescriptor = fileno(file); // should not error since file stream must be valid

    struct stat fileStats;
    if (fstat(fileDescriptor, &fileStats)) { // -1 on error
        return fwErrorFileStats;
    }

    *fileSize_p = fileStats.st_size;
    *buffer_pp = malloc(*fileSize_p);
    if (!(uintptr_t)*buffer_pp) {
        return fwErrorOutOfMemory;
    }

    fread(*buffer_pp, 1, *fileSize_p, file);
    return fwErrorSuccess;
}

fwError fwSocketCreate(const fwSocketAddressFamily addressFamily, const fwSocketProtocol protocol,
                       fwSocket* sfdop_p) {
    int32_t realAddressFamily, realProtocol, targetAddressSize;

    switch (addressFamily) {
        case fwSocketAddressFamilyLocal: {
            realAddressFamily = AF_LOCAL;
            targetAddressSize = 108;
            break;
        }
        case fwSocketAddressFamilyIPv4: {
            realAddressFamily = AF_INET;
            targetAddressSize = INET_ADDRSTRLEN;
            break;
        }
        case fwSocketAddressFamilyIPv6: {
            realAddressFamily = AF_INET6;
            targetAddressSize = INET6_ADDRSTRLEN;
            break;
        }
        default: {
            return fwErrorInvalidParameter;
        }
    }

    switch (protocol) {
        case fwSocketProtocolStream: {
            realProtocol = SOCK_STREAM;
            break;
        }
        case fwSocketProtocolDatagram: {
            realProtocol = SOCK_DGRAM;
            break;
        }
        case fwSocketProtocolRaw: {
            realProtocol = SOCK_RAW;
            break;
        }
        default: {
            return fwErrorInvalidParameter;
        }
    }

    struct fwiNativeSocketState* nativeSocket;

    // This memory is not leaked, is it put into the hands of the user in the form of an unsigned
    // long, which is then reinterpreted. Freed in fwSocketClose
    if ((nativeSocket = malloc(sizeof(struct fwiNativeSocketState) + targetAddressSize)) == nullptr) {
        return fwErrorOutOfMemory;
    }
    // Can you spot the difference which cost me a whole week to debug?
    //if ((nativeSocket = malloc(sizeof(struct fwiNativeSocketState)) + targetAddressSize) == nullptr) {

    memset(nativeSocket, 0, sizeof(struct fwiNativeSocketState) + targetAddressSize);
    nativeSocket->targetAddress = (char*)&nativeSocket->targetAddress +
                                     sizeof(struct fwiNativeSocketState);

    nativeSocket->protocol       = realProtocol;
    nativeSocket->addressFamily  = realAddressFamily;
    if ((nativeSocket->fileDescriptor = socket(realAddressFamily, realProtocol, 0)) == -1) {
        FWI_LOG_ERRNO;
    }

    *sfdop_p = (uintptr_t)nativeSocket;

    fwiLogA(fwiLogLevelInfo, "New socket (ID: %X) was created", nativeSocket);
    return fwErrorSuccess;
}

fwError fwSocketConnect(const fwSocket sfdop, const fwSocketAddress* connectInfo_p) {
    struct fwiNativeSocketState* nativeSocket = {(struct fwiNativeSocketState*)sfdop};

    struct addrinfo hint      = {};
    struct addrinfo* res      = {};

    hint.ai_family            = nativeSocket->addressFamily;
    hint.ai_socktype          = nativeSocket->protocol;

    if (getaddrinfo(connectInfo_p->target_p, connectInfo_p->port_p, &hint, &res)) {
        freeaddrinfo(res);
        return fwErrorSocketTargetName;
    }

    const struct addrinfo *it = res;
    do {
        if (connect(nativeSocket->fileDescriptor, it->ai_addr, sizeof(struct sockaddr_in)) == -1) {
            FWI_LOG_ERRNO;
            close(sfdop);
            it = it->ai_next;
            continue;
        }
        break;
    } while (it != nullptr);

    if (it == nullptr) {
        freeaddrinfo(res);
        return fwErrorSocketConnection;
    }

    nativeSocket->connected = true;
    nativeSocket->bound = true;
    strncpy(nativeSocket->targetAddress, connectInfo_p->target_p, 108);

    fwiLogA(fwiLogLevelInfo, "Socket (ID: %X) connected to %s", nativeSocket, connectInfo_p->target_p);
    return fwErrorSuccess;
}

fwError fwSocketBind(const fwSocket sfdop, const struct fwSocketAddress* localAddress) {
    struct fwiNativeSocketState* nativeSocket = {(struct fwiNativeSocketState*)sfdop};

    switch (nativeSocket->addressFamily) {
        case AF_INET6:
        case AF_INET: {
            struct sockaddr_in address = {};
            address.sin_family = nativeSocket->addressFamily;
            address.sin_port = htons(atoi(localAddress->port_p));

            if (strcmp(localAddress->target_p, FW_SOCKET_ADDRESS_ANY)) {
                address.sin_addr.s_addr = INADDR_ANY;
            } else {
                inet_pton(nativeSocket->addressFamily, localAddress->target_p, &address.sin_addr);
            }

            if (bind(nativeSocket->fileDescriptor, (struct sockaddr*)&address,
                sizeof(address)) == -1) {
                FWI_LOG_ERRNO;
                return fwErrorSocketBind;
            }

            break;
        }
        case AF_LOCAL: {
            struct sockaddr_un address = {};
            address.sun_family = AF_LOCAL;
            strncpy(address.sun_path, localAddress->target_p, sizeof(address.sun_path) - 1);

            if (bind(nativeSocket->fileDescriptor, (struct sockaddr*)&address,
                sizeof(address)) == -1) {
                FWI_LOG_ERRNO;
                return fwErrorSocketBind;
            }

            break;
        }
        default: {
            return fwErrorGoodJob;
        }
    }

    nativeSocket->bound = true;
    fwiLogA(fwiLogLevelInfo, "Socket (ID: %X) was bound to %s", nativeSocket, localAddress->target_p);
    return fwErrorSuccess;
}

fwError fwSocketAccept(const fwSocket sfdop, fwSocket* newSocket, char* foreignAddress) {
    const struct fwiNativeSocketState* nativeSocket = {(struct fwiNativeSocketState*)sfdop};

    if (nativeSocket->bound == false) {
        return fwErrorSocketNotBound;
    }

    if (listen(nativeSocket->fileDescriptor, 128) == -1) {
        FWI_LOG_ERRNO;
        return fwErrorSocketListen;
    }

    // ReSharper disable once CppDFAMemoryLeak
    struct fwiNativeSocketState* newNativeSocket = malloc(sizeof(struct fwiNativeSocketState));
    if (newNativeSocket == nullptr) {
        return fwErrorOutOfMemory;
    }

    switch (nativeSocket->addressFamily) {
        case AF_INET: {
            struct sockaddr_in address = {};
            socklen_t sockSize = sizeof(address);

            newNativeSocket->connected      = true;
            newNativeSocket->bound          = true;
            newNativeSocket->protocol       = nativeSocket->protocol;
            newNativeSocket->addressFamily  = nativeSocket->addressFamily;
            newNativeSocket->fileDescriptor = accept(nativeSocket->fileDescriptor,
                                                       (struct sockaddr*)&address, &sockSize);
            inet_ntop(AF_INET, &address.sin_addr, nativeSocket->targetAddress, INET_ADDRSTRLEN);

            if (foreignAddress != nullptr) {
                strncpy(foreignAddress, newNativeSocket->targetAddress, INET_ADDRSTRLEN);
            }

            break;
        }
        case AF_INET6: {
            struct sockaddr_in6 address = {};
            socklen_t sockSize = sizeof(address);

            newNativeSocket->connected      = true;
            newNativeSocket->bound          = true;
            newNativeSocket->protocol       = nativeSocket->protocol;
            newNativeSocket->addressFamily  = nativeSocket->addressFamily;
            newNativeSocket->fileDescriptor = accept(nativeSocket->fileDescriptor,
                                                       (struct sockaddr*)&address, &sockSize);
            inet_ntop(AF_INET6, &address.sin6_addr, nativeSocket->targetAddress, INET6_ADDRSTRLEN);

            if (foreignAddress != nullptr) {
                strncpy(foreignAddress, newNativeSocket->targetAddress, INET6_ADDRSTRLEN);
            }

            break;
        }
        case AF_LOCAL: {
            struct sockaddr_un address = {};
            socklen_t sockSize = sizeof(address);

            newNativeSocket->connected      = true;
            newNativeSocket->bound          = true;
            newNativeSocket->protocol       = nativeSocket->protocol;
            newNativeSocket->addressFamily  = nativeSocket->addressFamily;
            newNativeSocket->fileDescriptor = accept(nativeSocket->fileDescriptor,
                                                       (struct sockaddr*)&address, &sockSize);
            inet_ntop(AF_INET, &address.sun_path, nativeSocket->targetAddress, 108);

            if (foreignAddress != nullptr) {
                strncpy(foreignAddress, newNativeSocket->targetAddress, 108);
            }

            break;
        }
        default: {
            return fwErrorGoodJob;
        }
    }

    if (newNativeSocket->fileDescriptor == -1) {
        FWI_LOG_ERRNO;
        free(newNativeSocket);
        return fwErrorSocketAccept;
    }

    *newSocket = (uintptr_t)newNativeSocket;

    return fwErrorSuccess;
}

fwError fwSocketSend(const fwSocket sfdop, const void* data, const size_t ammount) {
    struct fwiNativeSocketState* nativeSocket = {(struct fwiNativeSocketState*)sfdop};

    const size_t written = write(nativeSocket->fileDescriptor, data, ammount);
    if (written == -1) {
        FWI_LOG_ERRNO;
        return fwErrorSocketSend;
    }
    fwiLogA(fwiLogLevelDebug, "Socket (ID: %X) sent %d bytes", nativeSocket, written);
    return fwErrorSuccess;
}

fwError fwSocketReceive(const fwSocket sfdop, void* buffer, const size_t ammount) {
    struct fwiNativeSocketState* nativeSocket = {(struct fwiNativeSocketState*)sfdop};

    const size_t readden = read(nativeSocket->fileDescriptor, buffer, ammount); // grammar 100
    if (readden == -1) {
        FWI_LOG_ERRNO;
        return fwErrorSocketReceive;
    }
    fwiLogA(fwiLogLevelDebug, "Socket (ID: %X) received %d bytes", nativeSocket, readden);
    return fwErrorSuccess;
}

fwError fwSocketClose(const fwSocket sfdop) {
    struct fwiNativeSocketState* nativeSocket = {(struct fwiNativeSocketState*)sfdop};

    if (close(nativeSocket->fileDescriptor) == -1) {
        return fwErrorInvalidParameter;
    }

    free(nativeSocket);

    fwiLogA(fwiLogLevelInfo, "Socket (ID: %X) was closed", sfdop);
    return fwErrorSuccess;
}

void fwiLogErrno(const char* location, const int32_t line) {
    const int32_t err = errno;
    fwiLogA(fwiLogLevelError, "System call failure with code %d at line %d in function %s", err,
            line, location);
}

#endif // PLATFORM_LINUX
