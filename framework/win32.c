#include "framework.h"
#include "internal.h"
#include "win32.h"

#ifdef PLATFORM_WINDOWS

void fwiStartNativeModuleWindow() {
    if (fwiState.activeModules & fwModuleWindow)
        return;

    // Initialise Win32 state data
    fwiWinState.className = L"LPAF Window Class";
    fwiWinState.instance = GetModuleHandleW(nullptr);

    WNDCLASSEXW wndClass = {};
    wndClass.cbSize         = sizeof(WNDCLASSEXW);
    wndClass.hInstance      = fwiWinState.instance;
    wndClass.lpszClassName  = fwiWinState.className;
    wndClass.lpfnWndProc    = &fwiWindowProc;

    RegisterClassExW(&wndClass);

    CreateThread(nullptr, 0x80000, &fwiEventProcedure, nullptr, 0,
                 &fwiWinState.nativeEventThreadHandle);

    fwiState.activeModules |= fwModuleWindow;
}

#endif
