// MIT License
//
// Copyright(c) 2023 ToneXum
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#ifdef _WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // prevent Windows.h from including winsock-1.1
#endif

#include <Windows.h>
#include <Windowsx.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <xaudio2.h>

#undef ERROR
#endif // _WINDOWS

#include <bitset>
#include <condition_variable>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

#include "Vulkan.hpp"
#include "Framework.hpp"

// Error check for Win32 API calls.
// Yes I know it's unreadable, check the doc.
#define WIN32_EC(expr, suc, type) { type res = expr; if (((res) && (suc == 0)) || (!(res) && (suc != 0))) \
{ i::CreateWin32Error(__LINE__, GetLastError(), __func__); } }

// Error check for Win32 API calls but the return value is saved
#define WIN32_EC_RET(var, expr, suc) { var = expr; if (((var) && (suc == 0)) || (!(var) && (suc != 0))) \
{ i::CreateWin32Error(__LINE__, GetLastError(), __func__); } }

// Error check for Winsock API calls
#define WSA_EC(expr, suc, type) { type res = expr; if (((res) && (suc == 0)) || (!(res) && (suc != 0))) \
{ i::CreateWinsockError(__LINE__, WSAGetLastError(), __func__); } }

// Error check for Winsock API calls but the return value is saved
#define WSA_EC_RET(var, expr) { var = expr; if (((var) && (suc == 0)) || (!(var) && (suc != 0))) \
{ i::CreateWinsockError(__LINE__, WSAGetLastError(), __func__); } }

// Manual error creation with automatic additional information
#define FRAMEWORK_ERR(msg) { i::CreateManualError(__LINE__, __func__, msg); }

#ifdef _DEBUG
#ifdef _WINDOWS
#define DEBUG_BREAK DebugBreak();
#else // anything else than windows should be POSIX compliant
#include <csignal>
#define DEBUG_BREAK raise(SIGTRAP); // Its a trap!
#endif
#else
#define DEBUG_BREAK
#endif

// Custom Win32 messages
#define CWM_USER_BASE               (WM_USER + 0)
#define CWM_CREATE_WINDOW_REQ       (WM_USER + 1) // window creation request; sent by CreateWindowSync / -Async
#define CWM_DESTROY_WINDOW          (WM_USER + 2) // sent by CloseWindowForce()
#define CWM_DESTROY_ALL_WINDOWS     (WM_USER + 3) // sent by CloseAllWindowsForce()

namespace i
{
// Log level for in::Log(), this will determine the prefix of the message
enum LogLvl : uint8_t
{
    LlInfo,
    LlDebug,
    LlValidation,
    LlWarning,
    LlError,
    LlBench
};

enum InitFlags : uint8_t
{
    IfFramework = 0b1,
    IfNetwork   = 0b10,
    IfRenderer  = 0b100
};

struct WindowData // additional data associated to each window
{
    std::vector<HWND> dependants; // handles to windows that depend on this one
    HWND window{};

    void (*pfOnClose)();
    bool (*pfOnCloseAttempt)();

    wchar_t* name = nullptr;
    int16_t xPos = 0, yPos = 0;
    uint16_t width = 0, height = 0;

    f::WndH id = 0;

    bool isVisible = true, isValid = true, hasFocus = true, hasMouseInClientArea = false;
} __attribute__((aligned(128)));

class Win32State
{
public:
    Win32State() = default;
    ~Win32State() = default;

    // Make it singleton
    Win32State(const Win32State&) = delete;
    Win32State(const Win32State&&) = delete;
    Win32State operator=(const Win32State&) = delete;
    Win32State operator=(const Win32State&&) = delete;

    std::map<f::WndH, WindowData*> handleMap;
    std::map<HWND, WindowData*> nativeHandleMap;

    const wchar_t* pClassName = L"LPAF Window Manager Class";
    HINSTANCE instance{}; // handle to window class
    HICON icon{};
    HCURSOR cursor{};
    DWORD nativeThreadId{};
};

class ProgramState
{
public:
    ProgramState();
    ~ProgramState();

    ProgramState(const ProgramState&) = delete;
    ProgramState(const ProgramState&&) = delete;
    ProgramState operator=(const ProgramState&) = delete;
    ProgramState operator=(const ProgramState&&) = delete;

    Win32State win32;
    v::VulkanState vulkan;

    // Mouse information
    struct Mouse
    {
        int xPos{}, yPos{};
        int wheelDelta{};
        bool leftButton{}, rightButton{}, middleButton{}, x1Button{}, x2Button{};
    } __attribute__((aligned(32))) mouse;

    // Bitset for keyboard key states
    std::bitset<256> keyStates = 0;

    std::thread* pWindowThread{};

    std::mutex loggerMutex; // thread safety for logging
    std::mutex windowDataMutex; // thread safety for accesses on the window data maps

    std::condition_variable windowThreadConditionVar;
    std::mutex windowThreadMutex;

    wchar_t* textInput;

    int16_t windowCount = 0, windowsOpened = 0;
    uint8_t initialisationState = 0; // set of flags indicating what parts of LPAF are running
    bool isRunning = true; // becomes false when no window is open anymore
    bool textInputEnabled = false;
    bool windowThreadIsRunning = false;
};

struct Socket
{
    Socket() = default;
    ~Socket();

    f::SocketCreateInfo socketCreateInfo;
    addrinfoexW* nativeAddressInformation;
    SOCKET nativeSocket;
    f::SocketStatus status;
} __attribute__((aligned(64)));

class NetworkState
{
public:
    NetworkState() = default;
    ~NetworkState() = default;

    NetworkState(const NetworkState&) = delete;
    NetworkState(const NetworkState&&) = delete;
    NetworkState operator=(const NetworkState&) = delete;
    NetworkState operator=(const NetworkState&&) = delete;

    std::map<f::SockH, Socket*> socketMap;

    uint16_t socketsCreated{}, connectedSockets{}, existingSockets{};
};

// Functions that do nothing
// First letter corresponds with the return type the rest indicate arguments
// Types are abbreviated, B for bool; V for void; I for int; U for unsigned; etc...
void DoNothingVv();
bool DoNothingBv(); // returns true

// Gets pointer to state
ProgramState* GetState(bool dealloc = false);

// Gets pointer to network state
NetworkState* GetNetworkState(bool dealloc = false);

void WindowProcedureThread();

void CreateWin32Window(i::WindowData* wndDt);

// Win32 Error creation
void CreateWin32Error(int line, int code, const char* func);

// Winsock error creation
void CreateWinsockError(int line, int code, const char* func);

// Manual and instant error creation
void CreateManualError(int line, const char* func, const char* msg);

LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Look through AppInfo.windows and return the instance matching the handle
// Time complexity is linear to the amount of windows open
WindowData* GetWindowData(HWND handle);

// Look through AppInfo.windows and return the instance matching the id of the underlying window
// Time complexity is linear to the amount of windows open
WindowData* GetWindowData(f::WndH handle);

// Loops through AppData.windows and erases all WindowData that is invalid
void EraseWindowData(HWND hWnd);

// Deallocates everything, closes handles and cleans up
// Call when the program needs to end abruptly
void DeAlloc();

// Writes to the log file using the handle stored in i::ProgramState
void Log(const wchar_t* msg, LogLvl logLvl);
void Log(const char* msg, LogLvl logLvl);
} // end namespace i
