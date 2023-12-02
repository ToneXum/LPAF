#pragma once

/// About this file:
/// 
/// This header is containing the internal symbols that are not supposed to be
/// exposed to the end user.

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
#define WIN32_EC_RET(x, y) { x = y; if (!x) { in::CreateWin32DebugError(__LINE__); } }

#endif // DEBUG
#ifdef NDEBUG

// Error check for Win32 API calls
#define WIN32_EC(x) { if (!x) {in::CreateWin32ReleaseError(__LINE__); } }

// Error check for Win32 API calls but the return value is saved
#define WIN32_EC_RET(x, y) { x = y; if (!x) { in::CreateWin32ReleaseError(__LINE__); } }

#endif // NDEBUG


namespace in
{
    // Win32 Error creation
    void CreateWin32DebugError(int line);
    void CreateWin32ReleaseError(int line);

    // Error handling for the user
    void SetLastError(int code);

    struct WindowData // additional data that is not exposed to the end user
    {
        // Window creator and message pump
        void MessageHandler();

        HWND hWnd;
        std::thread* msgThread;

        short id;
        char* name;
        bool isVisible;
        int xPos, yPos, width, height;
        bool isValid;
    };

    struct // general information about the state of the application
    {

        std::vector<WindowData*> windows{}; // contains information about the currently open windows
        const char* windowClassName{ "GGFW Window Class" };
        HINSTANCE hInstance = 0; // hinstance handle to window class
        ATOM classAtom = 0; // idk what this is even supposed to do

        int windowCount = 0;  // guess what, its the count of the currently open windows
        int windowsOpened = 0; // ammount of windows this program has opened in the past
        bool isRunning = true; // becomes false when origin window is closed

        std::mutex mtx; // mutex used to prevent funny shit from happening when creating a window
        std::condition_variable cv; // goes along side mtx
        bool windowIsFinished = false; // creation of a window is finished

        int lastErrorCode = 0;
        bool isInitialised = false; // becomes true when initialise is called
    } WindowInfo;

    LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Look through WindowInfo and return the instance matching the handle
    WindowData* GetWindowData(HWND handle);

    // Look through WindowInfo and return the instance matching the id of the underlying window
    WindowData* GetWindowData(int id);
}
