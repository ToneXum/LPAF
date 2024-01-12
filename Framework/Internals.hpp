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
#include <windowsx.h>

#undef ERROR

#include <vector>
#include <iostream>
#include <thread>
#include <sstream>
#include <mutex>
#include <memory>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <unordered_map>
#include <bitset>
#include <complex>
#include <chrono>
#include <time.h>

// Error check for Win32 API calls
#define WIN32_EC(x) { if (!x) { in::CreateWin32Error(__LINE__, GetLastError(), __FUNCDNAME__); } }
// Error check for Win32 API calls but the return value is saved
#define WIN32_EC_RET(var, func) { var = func; if (!var) { in::CreateWin32Error(__LINE__, GetLastError(), __FUNCDNAME__); } }

// Manual error creation with automatic additional information
#define FRMWRK_ERR(msg) { in::CreateManualError(__LINE__, __FUNCDNAME__, msg); }

namespace in
{
    // Performance is not important here so yes, have a hashmap
    extern std::unordered_map<int, const char*> errors =
    {
    { 0, "The operation went smoothly." }, // no error
    { 1, "The framework is already initialised." }, // tsd::Initialise was called more than once
    { 2, "The framework is not initialised." }, // tsd::Initialise was not called
    { 3, "Invalid parameter data."}, // general missuse
    { 4, "Invalid window handle."},
    { 5, "Invalid Icon Resource." }, // tsd::Initialise
    { 6, "Invalid Cursor Resource." }, // tsd::Initialise
    { 7, "32767 windows have been opened, cannot create more." }, // I hope no one will have to fetch this...
    { 8, "Flagset invalid or incorectly formatted." },
    { 9, "Invalid set of window dependants." }
    };

    void DoNothing_V();
    bool DoNothing_B();

    struct WindowData // additional data associated to each window
    {
        // Window creator and message pump
        void MessageHandler();

        HWND hWnd = {};
        std::thread* msgThread = {};


        short id = 0;
        wchar_t* name = nullptr;
        bool isVisible, isValid, hasFocus, hasMouseInClientArea = 0;
        short xPos, yPos, width, height = 0;

        void (*OnClose)() = DoNothing_V;
        bool (*OnCloseAttempt)() = DoNothing_B;

        std::vector<HWND> dependers; // handles to windows that depend on this one
    };

    struct // general information about the state of the application
    {
        std::vector<WindowData*> windows{}; // window specific information container
        HINSTANCE hInstance = 0; // handle to window class
        ATOM classAtom = 0; // idk what this is even supposed to do
        HICON hIcon{};
        HCURSOR hCursor{};

        std::mutex windowCreationMtx;
        std::condition_variable windowCreationCv;
        bool windowCreationIsFinished = false; // creation of a window is finished, prevent usage of unitialised mem
        
        std::mutex threadsDoneMtx;
        std::condition_variable threadsDoneCv;
        bool threadsDone = false; // all window threads have closed, prevent the class being deleted while windows are open

        std::mutex logMtx; // this probably as close as this gets when it comes to how a mutex is supposed to be used

        // Bitset for keyboard key states
        std::bitset<256> keystates = 0;

        // stream to a log file for runtime information about the framework
        std::wofstream logFile;

        // Charfield for text input
        wchar_t* textInput = nullptr; // pointer to the character field
        bool textInputEnabled = false;
        int textInputIndex = 0; // numbers of written characters in textInput

        // Mouse information
        struct
        {
            bool leftButton, rightButton, middleButton, x1Button, x2Button = false;
            int xPos, yPos = 0;
            int wheelDelta = 0;
        } mouse;

        const wchar_t* windowClassName = L"GGFW Window Class";
        int windowCount = 0;  // guess what, its the count of the currently open windows
        int windowsOpened = 0; // ammount of windows this program has opened in the past
        bool isRunning = true; // becomes false when no window is open anymore
        int lastErrorCode = 0;
        bool isInitialised = false; // becomes true when initialise is called
    } AppInfo;

    // Log level for in::Log, this will determine the prefix of the message
    enum class LL
    {
        INFO,
        DEBUG,
        WARNING,
        ERROR
    };

    // Win32 Error creation
    void CreateWin32Error(int line, int c, const char* func);

    // Manual and instant error creation
    void CreateManualError(int line, const char* func, const char* msg);

    // Error handling for the user
    void SetLastError(int code);

    LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Look through AppInfo.windows and return the instance matching the handle
    // Time complexity is linear to the ammount of windows open
    WindowData* GetWindowData(HWND handle);

    // Look through AppInfo.windows and return the instance matching the id of the underlying window
    // Time complexity is linear to the ammount of windows open
    WindowData* GetWindowData(short id);

    // Loops through AppData.windows and erases all WindowData that is invalid
    void EraseUnusedWindowData();

    // Deallocates everything, closes handles and cleans up
    // Call when the program needs to end abruptly
    void DeAlloc();

    // Writes to the log file using the handle stored in in::AppInfo
    void Log(const wchar_t* msg, LL ll);
}
