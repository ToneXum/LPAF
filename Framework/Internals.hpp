// MIT License
//
// Copyright(c) 2023 ToneXum
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this softwareand associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright noticeand this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/// About this file:
/// 
/// This header is containing the internal symbols that are not supposed to be
/// exposed to the end user.

#pragma once

// throw away a bunch of windows garbage
#ifndef FULL_WINTARD
#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOKERNEL
#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE
#endif

#define NOMINMAX

#define STRICT

#include <Windows.h>

#include <vector>
#include <iostream>
#include <thread>
#include <sstream>
#include <mutex>
#include <memory>
#include <cstdlib>
#include <exception>
#include <fstream>

#include "Errors.hpp"
#include "Framework.hpp"

#ifdef _DEBUG

// Error check for Win32 API calls
#define WIN32_EC(x) { if (!x) { in::CreateWin32DebugError(__LINE__); } }

// Error check for Win32 API calls but the return value is saved
#define WIN32_EC_RET(var, func) { var = func; if (!var) { in::CreateWin32DebugError(__LINE__); } }

#endif // DEBUG
#ifdef NDEBUG

// Error check for Win32 API calls
#define WIN32_EC(x) { if (!x) {in::CreateWin32ReleaseError(__LINE__); } }

// Error check for Win32 API calls but the return value is saved
#define WIN32_EC_RET(var, func) { var = func; if (!var) { in::CreateWin32ReleaseError(__LINE__); } }

#endif // NDEBUG


namespace in
{
    // Win32 Error creation
    void CreateWin32DebugError(int line);
    void CreateWin32ReleaseError(int line);

    // Error handling for the user
    void SetLastError(int code);

    struct WindowData // additional data associated to each window
    {
        // Window creator and message pump
        void MessageHandler();

        HWND hWnd;
        std::thread* msgThread;

        unsigned short id;
        char* name;
        bool isVisible, isValid, hasFocus;
        short xPos, yPos, width, height;
    };

    struct // general information about the state of the application
    {

        std::vector<WindowData*> windows{}; // window specific information container
        const char* windowClassName{ "GGFW Window Class" };
        HINSTANCE hInstance = 0; // handle to window class
        ATOM classAtom = 0; // idk what this is even supposed to do

        int windowCount = 0;  // guess what, its the count of the currently open windows
        int windowsOpened = 0; // ammount of windows this program has opened in the past
        bool isRunning = true; // becomes false when no window is open anymore

        std::mutex mtx; // mutex used halt execution to prevent usage of initialised memory
        std::condition_variable cv; // goes along side mtx
        bool windowIsFinished = false; // creation of a window is finished

        int lastErrorCode = 0;
        bool isInitialised = false; // becomes true when initialise is called

        HICON hIcon{};
        HCURSOR hCursor{};
    } WindowInfo;

    LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Look through WindowInfo.windows and return the instance matching the handle
    // Time complexity is linear to the ammount of windows open
    WindowData* GetWindowData(HWND handle);

    // Look through WindowInfo.windows and return the instance matching the id of the underlying window
    // Time complexity is linear to the ammount of windows open
    WindowData* GetWindowData(int id);
}
