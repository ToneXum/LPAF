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

// Windows
#include <Windows.h>
#include <windowsx.h>

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

// Vulkan SKD
#include "vulkan/vulkan.h"

// Error check for Win32 API calls
#define WIN32_EC(x) { if (!x) { in::CreateWin32Error(__LINE__, GetLastError(), __FUNCDNAME__); } }
// Error check for Win32 API calls but the return value is saved
#define WIN32_EC_RET(var, func) { var = func; if (!var) { in::CreateWin32Error(__LINE__, GetLastError(), __FUNCDNAME__); } }

#define VUL_EC(x) { VkResult r = x; if (r) { in::CreateVulkanError(__LINE__, r, __FUNCDNAME__); } }

// Manual error creation with automatic additional information
#define FRMWRK_ERR(msg) { in::CreateManualError(__LINE__, __FUNCDNAME__, msg); }

namespace in
{
    void DoNothing_V();
    bool DoNothing_B();

    enum WindowThreadAction
    {
        WTA_NONE,
        WTA_OPEN,
        WTA_CLOSE
    };

    struct WindowData // additional data associated to each window
    {
        HWND hWnd{};
        std::vector<HWND> dependers; // handles to windows that depend on this one

        short id = 0;
        wchar_t* name = nullptr;
        bool isVisible = true, isValid = true, hasFocus = true, hasMouseInClientArea = 0;
        short xPos = 0, yPos = 0, width = 0, height = 0;

        void (*OnClose)() = DoNothing_V;
        bool (*OnCloseAttempt)() = DoNothing_B;
    };

    struct WindowThreadActionInfo
    {
        in::WindowThreadAction action;
        in::WindowData* usrData;
    };

    struct // general information about the state of the application
    {
        std::vector<WindowData*> windows{}; // window specific information container
        HINSTANCE hInstance = 0; // handle to window class
        HICON hIcon{};
        HCURSOR hCursor{};
        std::thread* windowThread{};
        std::mutex logMtx; // this probably as close as this gets when it comes to how a mutex is supposed to be used

        // Bitset for keyboard key states
        std::bitset<256> keystates = 0;

        // Charfield for text input
        wchar_t* textInput = nullptr; // pointer to the character field
        bool textInputEnabled = false;
        int textInputIndex = 0; // numbers of written characters in textInput

        // Mouse information
        struct
        {
            bool leftButton = false, rightButton = false, middleButton = false, x1Button = false, x2Button = false;
            int xPos = 0, yPos = 0;
            int wheelDelta = 0;
        } mouse;

        const wchar_t* windowClassName = L"GGFW Window Class";
        int windowCount = 0;  // guess what, its the count of the currently open windows
        int windowsOpened = 0; // ammount of windows this program has opened in the past
        bool isRunning = true; // becomes false when no window is open anymore
        bool isInitialised = false; // becomes true when initialise is called
        
        in::WindowThreadActionInfo windowThreadAction[3]{}; // make the window thread do something
    } AppInfo;

    struct
    {
        VkInstance vkInstance{};
        VkPhysicalDevice physicalDevice{};
        VkDevice device{};

#ifdef _DEBUG
        VkDebugUtilsMessengerEXT debugMessenger{};
        std::array<const char*, 1> validationLayers
        { 
            "VK_LAYER_KHRONOS_validation" 
        };
#endif // _DEBUG

        // manual vulkan extensions, normally glfw would do this but oh well
        // pre-build: there is no way this works!
        // post-build: It fucking works!
        const std::vector<const char*> extension
        { 
            "VK_KHR_surface",
            "VK_KHR_win32_surface",
#ifdef _DEBUG
            "VK_EXT_debug_utils"
#endif // _DEBUG
        };
    } RenderInfo;

    // Log level for in::Log(), this will determine the prefix of the message
    enum class LL
    {
        INFO,
        DEBUG,
        VALIDATION,
        WARNING,
        ERROR
    };

    void InitialiseVulkan();

    void UninitialiseVulkan();

    VkPhysicalDevice ChooseBestPhysicalDevice(const std::vector<VkPhysicalDevice> &dev);

#ifdef _DEBUG
    VkBool32 VKAPI_CALL ValidationDegubCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,
        VkDebugUtilsMessageTypeFlagsEXT msgType,
        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
        void* userData
    );
#endif // _DEBUG

    void WindowsThread();
    void SetWindowThreadAction(WindowThreadActionInfo wndThrdAct);

    void CreateNativeWindow(WindowData* wndDt);
    void CloseNativeWindow(WindowData* wndDt);

    // Win32 Error creation
    void CreateWin32Error(int line, int c, const char* func);

    // Manual and instant error creation
    void CreateManualError(int line, const char* func, const char* msg);

    // Vulkan error creation
    void CreateVulkanError(int line, int c, const char* func);

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
    void Log(const char* msg, LL ll);
}

// declarations for proxy functions
namespace prx
{
    VkResult vkCreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger
    );

    void vkDestroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator
    );
}
