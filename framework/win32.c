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


#ifdef PLATFORM_WINDOWS

#include "framework.h"
#include "internal.h"
#include "win32.h"

void fwiStartNativeModuleWindow() {
    if (fwiGetState()->activeModules & fwModuleWindow) {
        fwiLogA(fwiLogLevelWarning, "Tried starting active window module");
        return;
    }

    // Initialise Win32 state data
    fwiGetWinState()->className = L"LPAF Window Class";
    fwiGetWinState()->instance = GetModuleHandleW(nullptr);

    WNDCLASSEXW wndClass = {};
    wndClass.cbSize         = sizeof(WNDCLASSEXW);
    wndClass.hInstance      = fwiGetWinState()->instance;
    wndClass.lpszClassName  = fwiGetWinState()->className;
    wndClass.lpfnWndProc    = &fwiWindowProc;

    RegisterClassExW(&wndClass);

    //CreateThread(nullptr, 0x80000, &fwiEventProcedure, nullptr, 0,
    //             &fwiGetWinState()->nativeEventThreadHandle);

    fwiLogA(fwiLogLevelInfo, "Window module was stated");
    fwiGetState()->activeModules |= fwModuleWindow;
}

void fwiStartNativeModuleNetwork() {

}

void fwiStartNativeModuleMultimedia() {

}

void fwiStartNativeModuleRenderer() {

}

void fwiStopNativeModuleWindow() {
    if (fwiGetState()->activeModules & fwModuleWindow) {
        struct fwiWinAPIState* state = fwiGetWinState();
        UnregisterClassW(state->className, state->instance);
        fwiLogA(fwiLogLevelInfo, "Window module was stopped");
        fwiGetState()->activeModules &= ~fwModuleWindow;
    }
    else {
        fwiLogA(fwiLogLevelWarning, "Tried stopping inactive window module");
    }
}

void fwiStopNativeModuleNetwork() {

}

void fwiStopNativeModuleMultimedia() {

}

void fwiStopNativeModuleRenderer() {

}

LRESULT CALLBACK fwiWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

DWORD fwiEventProcedure(void* param) {
    return 0;
}

struct fwiWinAPIState* fwiGetWinState() {
    static struct fwiWinAPIState data = {};
    return &data;
}

void fwiHandleWin32Error(const char* sourceFunc, int line, int code) {

}

#endif // PLATFORM_WINDOWS
