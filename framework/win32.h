#ifndef LPAF_WIN32_H
#define LPAF_WIN32_H

#include <windows.h>

struct fwiWinAPIState {
    HINSTANCE instance;
    const wchar_t* className;
} __attribute__((aligned(16))); struct fwiWinAPIState fwiWinState;

LRESULT CALLBACK fwiWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif //LPAF_WIN32_H
