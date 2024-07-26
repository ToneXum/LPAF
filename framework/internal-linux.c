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

// This implementation file contains implementations for platform specific, internal symbols

#ifdef PLATFORM_LINUX

#include "internal.h"
#include "linux.h"

void fwiStartNativeModuleWindow(void) {}

void fwiStartNativeModuleNetwork(void) {
    fwiLogA(fwiLogLevelInfo, "Networking module was started");

    // Because Linux is just better there is no state to be set before networking syscall can be
    // used
}

void fwiStartNativeModuleMultimedia(void) {}

void fwiStartNativeModuleRenderer(void) {}

void fwiStopNativeModuleWindow(void) {}

void fwiStopNativeModuleNetwork(void) {
    fwiLogA(fwiLogLevelInfo, "Networking module was stopped");

    // Because Linux is just better there is no state to be set after networking syscalls are done
    // being used
}

void fwiStopNativeModuleMultimedia(void) {}

void fwiStopNativeModuleRenderer(void) {}

#endif // PLATFORM_LINUX
