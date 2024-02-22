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

// About this file:
// 
// This header is containing the internal symbols that are not supposed to be
// exposed to the end user.

#pragma once

#include "Framework.hpp"
#include "Vulkan.hpp"

#define NOMINMAX

#define STRICT

// Windows
#include <Windows.h>
#include <windowsx.h>
#include <WinUser.h>

#undef ERROR

// STL
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
#include <array>
#include <queue>
#include <map>

// Error check for Win32 API calls
#define WIN32_EC(x) { if (!x) { i::CreateWin32Error(__LINE__, GetLastError(), __FUNCDNAME__); } }
// Error check for Win32 API calls but the return value is saved
#define WIN32_EC_RET(var, func) { var = func; if (!var) { i::CreateWin32Error(__LINE__, GetLastError(), __FUNCDNAME__); } }

// Manual error creation with automatic additional information
#define FRMWRK_ERR(msg) { i::CreateManualError(__LINE__, __FUNCDNAME__, msg); }

// Custom Win32 messages
#define WM_CREATEWINDOWREQ         WM_USER + 1

namespace i
{
    // Functions that do nothing
    // First letter coresponds with the arguments
    // Second letter coresponds to the return type
    // Types are abbreviated, B for bool; V for void; I for int; U for unsigned; etc...
    void DoNothing_V_V();
    bool DoNothing_V_B();

    struct WindowData // additional data associated to each window
    {
        HWND window{};
        std::vector<HWND> dependers; // handles to windows that depend on this one

        f::WND_H id = 0;
        wchar_t* name = nullptr;
        bool isVisible = true, isValid = true, hasFocus = true, hasMouseInClientArea = 0;
        short xPos = 0, yPos = 0, width = 0, height = 0;

        void (*pfOnClose)() = DoNothing_V_V;
        bool (*pfOnCloseAttempt)() = DoNothing_V_B;
    };

    class Win32State
    {
    public:
        Win32State(){}

        // Make it singleton
        Win32State(Win32State const&) = delete;
        Win32State operator=(Win32State const&) = delete;
        Win32State operator=(Win32State const&&) = delete;

        const wchar_t* pClassName = L"GGFW Window Class";
        HINSTANCE instance{}; // handle to window class
        HICON icon{};
        HCURSOR cursor{};
        DWORD nativeThreadId{};
        std::map<f::WND_H, WindowData*> identifiersToData{};
        std::map<HWND, WindowData*> handlesToData{};
    };

    // just some ffffat 840 bytes...
    class ProgrammState
    {
    public:
        ProgrammState(){}

        // Make it singleton
        ProgrammState(ProgrammState const&) = delete;
        ProgrammState operator=(ProgrammState const&) = delete; 
        ProgrammState operator=(ProgrammState const&&) = delete;

        Win32State win32{};
        v::VulkanState vulkan{};

        int windowCount = 0;  // guess what, its the count of the currently open windows
        int windowsOpened = 0; // ammount of windows this program has opened in the past
        bool isRunning = true; // becomes false when no window is open anymore
        bool isInitialised = false; // becomes true when initialise is called

        // Mouse information
        struct Mouse
        {
            bool leftButton{}, rightButton{}, middleButton{}, x1Button{}, x2Button{};
            int xPos{}, yPos{};
            int wheelDelta{};
        } mouse;

        // Bitset for keyboard key states
        std::bitset<256> keystates = 0;

        wchar_t textInput[100000]{};
        bool textInputEnabled = false;

        std::thread* pWindowThread{};

        std::mutex loggerMutex{}; // thread safety for logging
        std::mutex windowDataMutex{}; // thread safety for accesses on the window data maps

        std::condition_variable windowThreadConditionVar{};
        std::mutex windowThreadMutex{};
        bool windowThreadIsRunning = false;
    };

    // Gets pointer to state
    inline ProgrammState* GetState();

    // Log level for in::Log(), this will determine the prefix of the message
    enum class LL
    {
        INFO,
        DEBUG,
        VALIDATION,
        WARNING,
        ERROR
    };

    void WindowThread();

    void CreateNativeWindow(WindowData* wndDt);

    // Win32 Error creation
    void CreateWin32Error(int line, int c, const char* func);

    // Manual and instant error creation
    void CreateManualError(int line, const char* func, const char* msg);

    LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Look through AppInfo.windows and return the instance matching the handle
    // Time complexity is linear to the ammount of windows open
    WindowData* GetWindowData(HWND handle);

    // Look through AppInfo.windows and return the instance matching the id of the underlying window
    // Time complexity is linear to the ammount of windows open
    WindowData* GetWindowData(f::WND_H id);

    // Loops through AppData.windows and erases all WindowData that is invalid
    void EraseWindowData(HWND hWnd);

    // Deallocates everything, closes handles and cleans up
    // Call when the program needs to end abruptly
    void DeAlloc();

    // Writes to the log file using the handle stored in in::AppInfo
    void Log(const wchar_t* msg, LL ll);
    void Log(const char* msg, LL ll);
}
