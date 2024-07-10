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

#include "linux.h"

#ifdef PLATFORM_LINUX

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

fwError fwGetSystemConfiguration(fwSystemConfiguration* res) {
    res->cores  = sysconf(_SC_NPROCESSORS_ONLN);
    // TODO: figure out why only first memory bank is counted
    res->memory = sysconf(_SC_PHYS_PAGES) * sysconf(_SC_AVPHYS_PAGES) / 1048576; // 1024^2, to MiB
    return fwErrorSuccess;
}

fwError fwLoadFileToMem(const char* filename, void** buffer, uint64_t* fileSize) {
    FILE* file = fopen(filename, "rb");
    if (file) {
        const int32_t fileDescriptor = fileno(file); // should not error since file stream must be
        // valid

        struct stat fileStats;
        if (fstat(fileDescriptor, &fileStats)) { // -1 on error
            return fwErrorFailedToGetFileStats;
        }

        *fileSize = fileStats.st_size;
        *buffer = malloc(*fileSize);
        if (!(uintptr_t)*buffer) { // nullptr on error
            return fwErrorOutOfMemory;
        }

        fread(*buffer, 1, *fileSize, file);
        return fwErrorSuccess;
    }

    return fwErrorCouldNotOpenFile;
}

#endif // PLATFORM_LINUX
