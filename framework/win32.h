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

#ifndef LPAF_WIN32_H
#define LPAF_WIN32_H

#ifdef PLATFORM_WINDOWS

#include <windows.h>
// dbghelp.h depends on symbols from windows.h
#include <dbghelp.h>

#define FWI_HANDLE_FATAL_WIN32_ERROR fwiHandleWin32Error(__func__, __LINE__, GetLastError())

struct fwiWinAPIState {
    HINSTANCE instance;
    const wchar_t* className;
    DWORD nativeEventThreadHandle;
} __attribute__((aligned(32)));
struct fwiWinAPIState* fwiGetWinState();

LRESULT CALLBACK fwiWindowProc(
        HWND hwnd,
        UINT uMsg,
        WPARAM wParam,
        LPARAM lParam
        );

/**
 * @brief Event procedure that is executed in another thread; Calls event callbacks and such
 */
DWORD fwiEventProcedure(
        void* param
        );

/**
 * @brief Do not call, use @c FWI_HANDLE_FATAL_WIN32_ERROR macro instead
 */
void fwiHandleWin32Error(
        const char* sourceFunc,
        int line,
        int code
        );

#endif // PLATFORM_WINDOWS

#endif //LPAF_WIN32_H
