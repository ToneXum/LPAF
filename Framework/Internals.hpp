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

#ifdef _DEBUG

// Error check for Win32 API calls
#define WIN32_EC(x) { if (!x) { in::CreateWin32DebugError(__LINE__); } }

// Error check for Win32 API calls but the return value is saved
#define WIN32_EC_RET(x, y) { x = y; if (!x) { in::CreateWin32DebugError(__LINE__); } }

// Create an error to notify the framework user that he fucked up
#define USER_ERROR in::CreateUserDebugError(__LINE__);

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

    // User error creation
    void CreateUserDebugError(int line, const char* file, const char* function);
    void CreateUserReleaseError(int line, const char* file, const char* function);

    struct WindowData // additional data that is not exposed to the end user
    {
        WindowData() = default;
        ~WindowData(void);

        tsd::Window* wnd;
        HWND hWnd;
        std::thread* msgThread;

        // Window creator and message pump
        void MessageHandler();
    };

    struct // general information about the state of the windows
    {

        std::vector<WindowData*> windows{};
        const char* windowClassName{ "GGFW Window Class" };
        HINSTANCE hInstance{ 0 };
        ATOM classAtom{ 0 }; // idk what this is even supposed to do
        int windowCount{ 0 };
        int windowsOpened{ 0 }; // ammount of windows this program has opened in the past
        bool isRunning{true}; // becomes false when origin window is closed
        std::mutex mtx; // mutex used to prevent funny shit from happening when creating a window
        std::condition_variable cv; // goes along side mtx
        bool windowIsFinished = false; // creation of a window is finished
    } WindowInfo;

    LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Look through WindowInfo and return the instance matching the handle
    WindowData* GetWindowData(HWND handle);

    // Look through WindowInfo and return the instance matching the id of the underlying window
    WindowData* GetWindowData(int id);
}
