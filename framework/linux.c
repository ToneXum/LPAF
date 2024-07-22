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

#ifdef PLATFORM_LINUX

#include "framework.h"
#include "internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>

#define FWI_LOG_ERRNO fwiLogErrno(__func__, __LINE__)

struct fwiNativeSocketState {
    char* targetAddress;
    int32_t addressFamily;
    int32_t protocol;
    int32_t fileDescriptor;
    bool connected, bound;
};

union fwiNativeSocket {
    fwSocket id;
    struct fwiNativeSocketState* pt;
};

void fwiStartNativeModuleWindow(void) {}

void fwiStartNativeModuleNetwork(void) {
    fwiLogA(fwiLogLevelInfo, "Networking module for Linux was started");

    // Because Linux is just better there is no state to be set before networking syscall can be
    // used
}

void fwiStartNativeModuleMultimedia(void) {}

void fwiStartNativeModuleRenderer(void) {}

void fwiStopNativeModuleWindow(void) {}

void fwiStopNativeModuleNetwork(void) {
    fwiLogA(fwiLogLevelInfo, "Networking module for Linux was stopped");

    // Because Linux is just better there is no state to be set after networking syscalls are done
    // being used
}

void fwiStopNativeModuleMultimedia(void) {}

void fwiStopNativeModuleRenderer(void) {}

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

    union fwiNativeSocket nativeSocket = {};

    // This memory is not leaked, is it put into the hands of the user in the form of an unsigned
    // long, which is then reinterpreted. Freed in fwSocketClose
    // ReSharper disable once CppDFAMemoryLeak
    if ((nativeSocket.pt = malloc(sizeof(struct fwiNativeSocketState)) + targetAddressSize) == nullptr) {
        return fwErrorOutOfMemory;
    }
    memset(nativeSocket.pt, 0, sizeof(struct fwiNativeSocketState) + targetAddressSize);
    nativeSocket.pt->targetAddress = (char*)&nativeSocket.pt->targetAddress +
                                     sizeof(struct fwiNativeSocketState);

    nativeSocket.pt->protocol       = realProtocol;
    nativeSocket.pt->addressFamily  = realAddressFamily;
    if ((nativeSocket.pt->fileDescriptor = socket(realAddressFamily, realProtocol, 0)) == -1) {
        FWI_LOG_ERRNO;
    }

    *sfdop_p = nativeSocket.id;

    fwiLogA(fwiLogLevelInfo, "New socket (ID: %X) was created", nativeSocket.id);
    return fwErrorSuccess;
}

fwError fwSocketConnect(const fwSocket sfdop, const fwSocketAddress* connectInfo_p) {
    union fwiNativeSocket nativeSocket = {};
    nativeSocket.id = sfdop;

    struct addrinfo hint      = {};
    struct addrinfo* res      = {};

    hint.ai_family            = nativeSocket.pt->addressFamily;
    hint.ai_socktype          = nativeSocket.pt->protocol;

    if (getaddrinfo(connectInfo_p->target_p, connectInfo_p->port_p, &hint, &res)) {
        freeaddrinfo(res);
        return fwErrorSocketTargetName;
    }

    const struct addrinfo *it = res;
    do {
        if (connect(nativeSocket.pt->fileDescriptor, it->ai_addr, sizeof(struct sockaddr_in)) == -1) {
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

    nativeSocket.pt->connected = true;
    nativeSocket.pt->bound = true;
    strncpy(nativeSocket.pt->targetAddress, connectInfo_p->target_p, 108);

    fwiLogA(fwiLogLevelInfo, "Socket (ID: %X) connected to %s", nativeSocket.id, connectInfo_p->target_p);
    return fwErrorSuccess;
}

fwError fwSocketBind(const fwSocket sfdop, const struct fwSocketAddress* localAddress) {
    union fwiNativeSocket nativeSocket = {};
    nativeSocket.id = sfdop;

    switch (nativeSocket.pt->addressFamily) {
        case AF_INET6:
        case AF_INET: {
            struct sockaddr_in address = {};
            address.sin_family = nativeSocket.pt->addressFamily;
            address.sin_port = htons(atoi(localAddress->port_p));
            inet_pton(nativeSocket.pt->addressFamily, localAddress->target_p, &address.sin_addr);

            if (bind(nativeSocket.pt->fileDescriptor, (struct sockaddr*)&address,
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

            if (bind(nativeSocket.pt->fileDescriptor, (struct sockaddr*)&address,
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

    nativeSocket.pt->bound = true;
    fwiLogA(fwiLogLevelInfo, "Socket (ID: %X) was bound to %s", nativeSocket.id, localAddress->target_p);
    return fwErrorSuccess;
}

fwError fwSocketAccept(const fwSocket sfdop, fwSocket* newSocket, char* foreignAddress) {
    union fwiNativeSocket nativeSocket = {};
    nativeSocket.id = sfdop;

    if (listen(nativeSocket.pt->fileDescriptor, 128) == -1) {
        FWI_LOG_ERRNO;
        return fwErrorSocketListen;
    }

    // ReSharper disable once CppDFAMemoryLeak
    struct fwiNativeSocketState* nativeSocketState = malloc(sizeof(struct fwiNativeSocketState));
    if (nativeSocketState == nullptr) {
        return fwErrorOutOfMemory;
    }

    switch (nativeSocket.pt->addressFamily) {
        case AF_INET: {
            struct sockaddr_in address = {};
            socklen_t sockSize = sizeof(address);

            nativeSocketState->connected      = true;
            nativeSocketState->bound          = true;
            nativeSocketState->protocol       = nativeSocket.pt->protocol;
            nativeSocketState->addressFamily  = nativeSocket.pt->addressFamily;
            nativeSocketState->fileDescriptor = accept(nativeSocket.pt->fileDescriptor,
                                                       (struct sockaddr*)&address, &sockSize);
            inet_ntop(AF_INET, &address.sin_addr, nativeSocket.pt->targetAddress, INET_ADDRSTRLEN);
            strncpy(foreignAddress, nativeSocketState->targetAddress, INET_ADDRSTRLEN);
            break;
        }
        case AF_INET6: {
            struct sockaddr_in6 address = {};
            socklen_t sockSize = sizeof(address);

            nativeSocketState->connected      = true;
            nativeSocketState->bound          = true;
            nativeSocketState->protocol       = nativeSocket.pt->protocol;
            nativeSocketState->addressFamily  = nativeSocket.pt->addressFamily;
            nativeSocketState->fileDescriptor = accept(nativeSocket.pt->fileDescriptor,
                                                       (struct sockaddr*)&address, &sockSize);
            inet_ntop(AF_INET6, &address.sin6_addr, nativeSocket.pt->targetAddress, INET6_ADDRSTRLEN);
            strncpy(foreignAddress, nativeSocketState->targetAddress, INET6_ADDRSTRLEN);
            break;
        }
        case AF_LOCAL: {
            struct sockaddr_un address = {};
            socklen_t sockSize = sizeof(address);

            nativeSocketState->connected      = true;
            nativeSocketState->bound          = true;
            nativeSocketState->protocol       = nativeSocket.pt->protocol;
            nativeSocketState->addressFamily  = nativeSocket.pt->addressFamily;
            nativeSocketState->fileDescriptor = accept(nativeSocket.pt->fileDescriptor,
                                                       (struct sockaddr*)&address, &sockSize);
            inet_ntop(AF_INET, &address.sun_path, nativeSocket.pt->targetAddress, 108);
            strncpy(foreignAddress, nativeSocketState->targetAddress, 108);
            break;
        }
        default: {
            return fwErrorGoodJob;
        }
    }

    if (nativeSocketState->fileDescriptor == -1) {
        FWI_LOG_ERRNO;
        return fwErrorSocketAccept;
    }

    union fwiNativeSocket newNativeSocket = {};
    newNativeSocket.pt = nativeSocketState;

    *newSocket = nativeSocket.id;

    return fwErrorSuccess;
}

fwError fwSocketSend(const fwSocket sfdop, const void* data, const size_t ammount) {
    union fwiNativeSocket nativeSocket = {};
    nativeSocket.id = sfdop;

    const size_t written = write(nativeSocket.pt->fileDescriptor, data, ammount);
    if (written == -1) {
        FWI_LOG_ERRNO;
        return fwErrorSocketSend;
    }
    fwiLogA(fwiLogLevelDebug, "Socket (ID: %X) sent %d bytes", nativeSocket.id, written);
    return fwErrorSuccess;
}

fwError fwSocketReceive(const fwSocket sfdop, void* buffer, const size_t ammount) {
    union fwiNativeSocket nativeSocket = {};
    nativeSocket.id = sfdop;

    const size_t readden = read(nativeSocket.pt->fileDescriptor, buffer, ammount); // grammar 100
    if (readden == -1) {
        FWI_LOG_ERRNO;
        return fwErrorSocketReceive;
    }
    fwiLogA(fwiLogLevelDebug, "Socket (ID: %X) received %d bytes", nativeSocket.id, readden);
    return fwErrorSuccess;
}

fwError fwSocketClose(const fwSocket sfdop) {
    union fwiNativeSocket nativeSocket = {};
    nativeSocket.id = sfdop;

    if (close(nativeSocket.pt->fileDescriptor) == -1) {
        return fwErrorInvalidParameter;
    }

    free(nativeSocket.pt);

    fwiLogA(fwiLogLevelInfo, "Socket (ID: %X) was closed", sfdop);
    return fwErrorSuccess;
}

void fwiLogErrno(const char* location, const int32_t line) {
    const int32_t err = errno;
    fwiLogA(fwiLogLevelError, "System call failure with code %d at line %d in function %s", err,
            line, location);
}

#endif // PLATFORM_LINUX
