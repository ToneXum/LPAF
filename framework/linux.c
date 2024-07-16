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
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

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

fwError fwSocketCreate(const struct fwSocketCreateInfo* sockCrtInf, fwSocket* sfdop_p) {
    int32_t domain, type;

    switch (sockCrtInf->addressFamily) {
        case fwSocketAddressFamilyLocal: {
            domain = AF_LOCAL;
            break;
        }
        case fwSocketAddressFamilyIPv4: {
            domain = AF_INET;
            break;
        }
        case fwSocketAddressFamilyIPv6: {
            domain = AF_INET6;
            break;
        }
        case fwSocketAddressFamilyBlueTooth: {
            domain = AF_BLUETOOTH;
            break;
        }
        default: {
            return fwErrorInvalidParameter;
        }
    }

    switch (sockCrtInf->socketType) {
        case fwSocketTypeStream: {
            type = SOCK_STREAM;
            break;
        }
        case fwSocketTypeDatagram: {
            type = SOCK_DGRAM;
            break;
        }
        case fwSocketTypeRaw: {
            type = SOCK_RAW;
            break;
        }
        default: {
            return fwErrorInvalidParameter;
        }
    }

    *sfdop_p = socket(domain, type, 0);
    return fwErrorSuccess;
}

fwError fwSocketConnect(const fwSocket sfdop, const struct fwSocketCreateInfo* createInfo_p,
                        const fwSocketConnectionInfo* connectInfo_p) {
    if (connectInfo_p->targetKind == fwSocketConnectionTargetInternetDomainName) {
        struct addrinfo hint      = {};
        struct addrinfo* res      = {};
        const struct addrinfo *it = {};
        hint.ai_family            = createInfo_p->addressFamily;
        hint.ai_socktype          = createInfo_p->socketType;

        if (getaddrinfo(connectInfo_p->target_p, connectInfo_p->port_p, &hint, &res)) {
            return fwErrorSocketDomainName;
        }

        for (it = res; it->ai_next != nullptr; it = it->ai_next) {
            if (connect(sfdop, res->ai_addr, res->ai_addrlen)) {
                close(sfdop);
                continue;
            }
            break;
        }

        if (it->ai_next == nullptr) {
            return fwErrorSocketConnection;
        }

        freeaddrinfo(res);
        return fwErrorSuccess;
    }

    if (strlen(connectInfo_p->target_p) > 14) {
        return fwErrorInvalidParameter;
    }

    struct sockaddr foreignAddr = {};
    switch (createInfo_p->addressFamily) {
        case fwSocketAddressFamilyLocal: {
            foreignAddr.sa_family = AF_LOCAL;
            break;
        }
        case fwSocketAddressFamilyIPv4: {
            foreignAddr.sa_family = AF_INET;
            break;
        }
        case fwSocketAddressFamilyIPv6: {
            foreignAddr.sa_family = AF_INET6;
            break;
        }
        case fwSocketAddressFamilyBlueTooth: {
            foreignAddr.sa_family = AF_BLUETOOTH;
            break;
        }
        default: {
            return fwErrorInvalidParameter;
        }
    }

    strcpy(foreignAddr.sa_data, connectInfo_p->target_p);

    if (connect(sfdop, &foreignAddr, sizeof(foreignAddr))) {
        int e = errno;
        close(sfdop);
        return fwErrorSocketConnection;
    }

    return fwErrorSuccess;
}

fwError fwSocketSend(const fwSocket sfdop, const void* data, const size_t ammount) {
    if (write(sfdop, data, ammount) == -1) {
        return fwErrorSocketSend;
    }
    return fwErrorSuccess;
}

fwError fwSocketReceive(const fwSocket sfdop, void* buffer, const size_t ammount) {
    if (read(sfdop, buffer, ammount) == -1) {
        return fwErrorSocketReceive;
    }
    return fwErrorSuccess;
}

fwError fwSocketClose(const fwSocket sfdop) {
    close(sfdop);
    return fwErrorSuccess;
}

void fwiLogErrno(void) {
    const int32_t err = errno;
    const char* msg;

    switch (err) {
        default: {
            msg = "";
        }
    }

    fwiLogA(fwiLogLevelError, "System call failure: %s", msg);
}

#endif // PLATFORM_LINUX
