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

#include <unistd.h>

void fwiStartNativeModuleWindow(void) {}

void fwiStartNativeModuleNetwork(void) {}

void fwiStartNativeModuleMultimedia(void) {}

void fwiStartNativeModuleRenderer(void) {}

void fwiStopNativeModuleWindow(void) {}

void fwiStopNativeModuleNetwork(void) {}

void fwiStopNativeModuleMultimedia(void) {}

void fwiStopNativeModuleRenderer(void) {}

fwError fwGetSystemCoreCount(int* res) {
    const long count = sysconf(_SC_NPROCESSORS_ONLN);
    if (count != -1) {
        *res = count;
        return fwErrorSuccess;
    }
    // could only fail if param is invalid which is isnt
    return fwErrorGoodJob;
}

#endif // PLATFORM_LINUX
