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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

void fwiStartNativeModuleWindow(void) {}

void fwiStartNativeModuleNetwork(void) {}

void fwiStartNativeModuleMultimedia(void) {}

void fwiStartNativeModuleRenderer(void) {}

void fwiStopNativeModuleWindow(void) {}

void fwiStopNativeModuleNetwork(void) {}

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
        return fwErrorCouldNotOpenFile;
    }

    const int32_t fileDescriptor = fileno(file); // should not error since file stream must be valid

    struct stat fileStats;
    if (fstat(fileDescriptor, &fileStats)) { // -1 on error
        return fwErrorFailedToGetFileStats;
    }

    *fileSize_p = fileStats.st_size;
    *buffer_pp = malloc(*fileSize_p);
    if (!(uintptr_t)*buffer_pp) {
        return fwErrorOutOfMemory;
    }

    fread(*buffer_pp, 1, *fileSize_p, file);
    return fwErrorSuccess;
}

#endif // PLATFORM_LINUX
