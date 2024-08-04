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

// This implementation file contains implementations for Linux-specific, internal symbols

#ifdef PLATFORM_LINUX

#include "internal.h"
#include "linux.h"

#include <wayland-client.h>

static struct wl_display* display = {};

fwError fwiStartNativeModuleWindow(void) {
    display = wl_display_connect(nullptr);

    struct wl_registry* reg = wl_display_get_registry(display);

    if (display == nullptr) {
        return fwErrorWindowConnect;
    }
    return fwErrorSuccess;
}

fwError fwiStopNativeModuleWindow(void) {
    wl_display_disconnect(display);
    return fwErrorSuccess;
}

fwError fwiStartNativeModuleNetwork(void) {
    fwiLogA(fwiLogLevelInfo, "Networking module was started");

    // Because Linux is just better there is no state to be set before networking syscall can be
    // used

    return fwErrorSuccess;
}

fwError fwiStopNativeModuleNetwork(void) {
    fwiLogA(fwiLogLevelInfo, "Networking module was stopped");

    // Because Linux is just better there is no state to be set after networking syscalls are done
    // being used

    return fwErrorSuccess;
}

fwError fwiStartNativeModuleRenderer(void) {
    return fwErrorUnimplemented;
}

fwError fwiStopNativeModuleRenderer(void) {
    return fwErrorUnimplemented;
}

fwError fwiStartNativeModuleMultimedia(void) {
    return fwErrorUnimplemented;
}

fwError fwiStopNativeModuleMultimedia(void) {
    return fwErrorUnimplemented;
}

#endif // PLATFORM_LINUX
