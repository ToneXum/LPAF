#include "framework.h"
#include "internal.h"
#include "win32.h"

#ifdef PLATFORM_WINDOWS

void fwiStartNativeModuleWindow() {
    if (fwiGetState()->activeModules & fwModuleWindow) {
        fwiLogA("Tried starting active window module", fwiLogLevelWarning);
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

    CreateThread(nullptr, 0x80000, &fwiEventProcedure, nullptr, 0,
                 &fwiGetWinState()->nativeEventThreadHandle);

    fwiLogA("Window module was stated", fwiLogLevelInfo);
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
        fwiLogA("Window module was stopped", fwiLogLevelInfo);
        fwiGetState()->activeModules &= ~fwModuleWindow;
    }
    else {
        fwiLogA("Tried stopping inactive window module", fwiLogLevelWarning);
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
    static struct fwiWinAPIState* result = {};
    return result;
}

void fwiHandleWin32Error(const char* sourceFunc, int line, int code) {

}

#endif
