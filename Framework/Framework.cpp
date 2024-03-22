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

#include <chrono>
#include <format>
#include <fstream>
#include <iostream>

#include "Framework.hpp"
#include "Internals.hpp"

void f::Initialise(const f::FrameworkInitData& initialisationData)
{
    // Create thread as early as possible. Since the execution does not start immediately this function will wait for it
    // to do so. In the meantime, it can perform work.
    i::GetState()->pWindowThread = new std::thread(i::WindowProcedureThread);

    // Get hInstance since the program does not use the winMain entry point
    i::GetState()->win32.instance = GetModuleHandle(nullptr);

    WNDCLASSEXW wndC = {};

    wndC.cbClsExtra       = 0;
    wndC.cbSize           = sizeof(WNDCLASSEXW);
    wndC.cbWndExtra       = 0;
    wndC.hbrBackground    = nullptr;
    wndC.hCursor          = i::GetState()->win32.cursor;
    wndC.hIcon            = i::GetState()->win32.icon;
    wndC.hIconSm          = i::GetState()->win32.icon;
    wndC.hInstance        = i::GetState()->win32.instance;
    wndC.lpfnWndProc      = i::WindowProc;
    wndC.lpszClassName    = i::GetState()->win32.pClassName;
    wndC.lpszMenuName     = nullptr;
    wndC.style            = 69;

    WIN32_EC(RegisterClassExW(&wndC))

    // Wait for the window thread to start its execution if it has not already
    std::unique_lock<std::mutex> lock(i::GetState()->windowThreadMutex);
    i::GetState()->windowThreadConditionVar.wait(lock, []() -> bool { return i::GetState()->windowThreadIsRunning; });

    i::GetState()->isInitialised = true;

    v::InitialiseVulkan();
    i::Log(L"Framework was successfully initialised", i::LlInfo);

    // Specify the year the program is running in; Saves space on the timestamp for logged messages
    // Surely the thing won't run through new year... right?
    std::wostringstream timeMsg;
    timeMsg << "The current year is " << std::format("{:%Y}", std::chrono::system_clock::now()).c_str();
    i::Log(timeMsg.str().c_str(), i::LlInfo);
}

void f::UnInitialise()
{
    f::CloseAllWindowsForce();

    // As soon as all windows are closed (aka the manager thread is not running anymore), execution will continue
    std::unique_lock<std::mutex> lock(i::GetState()->windowThreadMutex);
    i::GetState()->windowThreadConditionVar.wait(lock, []{ return !i::GetState()->windowThreadIsRunning; });

    WIN32_EC(UnregisterClassW(i::GetState()->win32.pClassName, i::GetState()->win32.instance))

    v::UnInitializeVulkan();
    delete i::GetState();

    i::Log(L"Framework was successfully uninitialised", i::LlInfo);
}

f::WndH f::CreateWindowAsync(const f::WindowCreateData& wndCrtData)
{
    if (!i::GetState()->isInitialised) { i::Log(L"CreateWindowAsync() was called before initialisation", i::LlError);
        return 0; } // init was not called
    if (!wndCrtData.pName) { i::Log(L"Nullptr was passed to a required parameter at CreateWindowAsync()", i::LlError);
        return 0; } // name is nullptr
    if ((wndCrtData.height <= 0) || (wndCrtData.width <= 0)) { i::Log(L"Invalid height or width amount for window "
                                                                      "creation", i::LlError); return 0; }

    auto* wndData = new i::WindowData;

    i::GetState()->windowCount += 1;
    i::GetState()->windowsOpened += 1;

    wndData->name       = const_cast<wchar_t*>(wndCrtData.pName);
    wndData->width      = wndCrtData.width;
    wndData->height     = wndCrtData.height;
    wndData->xPos       = wndCrtData.xPos;
    wndData->yPos       = wndCrtData.yPos;
    wndData->isValid    = true;
    wndData->hasFocus   = true;
    wndData->hasMouseInClientArea = false;
    wndData->id         = i::GetState()->windowsOpened;
    
    if (!wndCrtData.dependants.empty())
    {
        for (WndH dep : wndCrtData.dependants)
        {
            wndData->dependants.push_back(i::GetWindowData(dep)->window);
        }
    }

    WIN32_EC(PostThreadMessageW(
            i::GetState()->win32.nativeThreadId,
            WM_CREATE_WINDOW_REQ,
            reinterpret_cast<WPARAM>(nullptr),
            reinterpret_cast<LPARAM>(wndData)
            )
    )

    return wndData->id;
}

void f::CloseWindow(const WndH handle)
{
    i::WindowData* windowData = i::GetWindowData(handle);
    
    if (!windowData) // window does not exist
        return;

    WIN32_EC(PostMessageA(
        windowData->window,
        WM_CLOSE,
        reinterpret_cast<WPARAM>(nullptr),
        reinterpret_cast<LPARAM>(nullptr)
    ))
}

void f::CloseWindowForce(const WndH handle)
{
    i::WindowData* windowData = i::GetWindowData(handle);

    if (!windowData) // window does not exist
        return;

    WIN32_EC(PostMessageA(
        windowData->window,
        WM_DESTROY,
        reinterpret_cast<WPARAM>(nullptr),
        reinterpret_cast<LPARAM>(nullptr)
    ))
}

void f::CloseAllWindows()
{
    for (std::pair<f::WndH, i::WindowData*>&& dataPair : i::GetState()->win32.identifiersToData)
    {
        WIN32_EC(PostMessageA(
            dataPair.second->window,
            WM_CLOSE,
            reinterpret_cast<WPARAM>(nullptr),
            reinterpret_cast<LPARAM>(nullptr)
        ))
    }
}

void f::CloseAllWindowsForce()
{
    for (std::pair<f::WndH, i::WindowData*>&& dataPair : i::GetState()->win32.identifiersToData)
    {
        WIN32_EC(PostMessageA(
                dataPair.second->window,
                WM_DESTROY,
                reinterpret_cast<WPARAM>(nullptr),
                reinterpret_cast<LPARAM>(nullptr)
        ))
    }
}

#undef MessageBox
int f::MessageBox(WndH owner, const wchar_t* title, const wchar_t* msg, uint16_t flags)
{
    // return null if the window is not found, so I don't care
    i::WindowData* ownerData = i::GetWindowData(owner);

    uint32_t rawFlags = 0;

    // Where switch statement?
    // Cant put (non-constant) expressions into switch cases
    if (flags & MbTaskModal)
        rawFlags = rawFlags | MB_TASKMODAL;

    if (flags & MbIconWarning)
        rawFlags = rawFlags | MB_ICONWARNING;
    if (flags & MbIconError)
        rawFlags = rawFlags | MB_ICONERROR;
    if (flags & MbIconInfo)
        rawFlags = rawFlags | MB_ICONINFORMATION;
    if (flags & MbIconQuestion)
        rawFlags = rawFlags | MB_ICONQUESTION;

    if (flags & MbButtonOk)
        rawFlags = rawFlags | MB_OK;
    if (flags & MbButtonOkCancel)
        rawFlags = rawFlags | MB_OKCANCEL;
    if (flags & MbButtonYesNo)
        rawFlags = rawFlags | MB_YESNO;
    if (flags & MbButtonRetryCancel)
        rawFlags = rawFlags | MB_RETRYCANCEL;

    if (flags & MbButtonYesNoCancel)
        rawFlags = rawFlags | MB_YESNOCANCEL;
    if (flags & MbButtonAbortRetryIgnore)
        rawFlags = rawFlags | MB_ABORTRETRYIGNORE;
    if (flags & MbButtonCancelRetryContinue)
        rawFlags = rawFlags | MB_CANCELTRYCONTINUE;

    int result = MessageBoxW(ownerData ? ownerData->window : nullptr, msg, title, rawFlags);
    if (result == 0)
        i::Log(L"Invalid set of flags where passed to MessageBox()", i::LlError);
#undef MB_OK
    switch (result)
    {
        case IDABORT:       return MrAbort;
        case IDCANCEL:      return MrCancel; // also sent when user presses X-button on message box
        case IDCONTINUE:    return MrContinue;
        case IDIGNORE:      return MrIgnore;
        case IDNO:          return MrNo;
        case IDOK:          return MrOk;
        case IDRETRY:       return MrRetry;
        case IDTRYAGAIN:    return MrTryAgain;
        case IDYES:         return MrYes;
        default:            return MrCancel; // should never reach this
    }
}

void f::OnWindowCloseAttempt(WndH handle, bool(*func)())
{
    i::GetWindowData(handle)->pfOnCloseAttempt = func;
}

void f::OnWindowClose(WndH handle, void(*func)())
{
    std::unique_lock<std::mutex> lock(i::GetState()->windowDataMutex);
    i::GetWindowData(handle)->pfOnClose = func;
}

wchar_t* f::GetWindowName(WndH handle)
{
    std::unique_lock<std::mutex> lock(i::GetState()->windowDataMutex);
    i::WindowData* wndData = i::GetWindowData(handle);
    if (!wndData) [[unlikely]] { return nullptr; }
    return wndData->name;
}

bool f::GetWindowVisibility(WndH handle)
{
    std::unique_lock<std::mutex> lock(i::GetState()->windowDataMutex);
    i::WindowData* wndData = i::GetWindowData(handle);
    if (!wndData) [[unlikely]] { return false; }
    return wndData->isVisible;
}

uint16_t f::GetWindowWidth(WndH handle)
{
    std::unique_lock<std::mutex> lock(i::GetState()->windowDataMutex);
    i::WindowData* wndData = i::GetWindowData(handle);
    if (!wndData) [[unlikely]] { return false; }
    return wndData->width;
}

uint16_t f::GetWindowHeight(WndH handle)
{
    std::unique_lock<std::mutex> lock(i::GetState()->windowDataMutex);
    i::WindowData* wndData = i::GetWindowData(handle);
    if (!wndData) [[unlikely]] { return false; }
    return wndData->height;
}

std::pair<uint16_t, uint16_t> f::GetWindowDimensions(WndH handle)
{
    std::unique_lock<std::mutex> lock(i::GetState()->windowDataMutex);
    i::WindowData* wndData = i::GetWindowData(handle);
    if (!wndData) [[unlikely]] { return {0, 0}; }
    return {wndData->width, wndData->height};
}

int f::GetWindowCount()
{
    return i::GetState()->windowCount;
}

void f::ChangeWindowName(WndH handle, const wchar_t* name)
{
    std::unique_lock<std::mutex> lock(i::GetState()->windowDataMutex);

    i::WindowData* wndData = i::GetWindowData(handle);
    if (!wndData) { return; }
    wndData->name = const_cast<wchar_t*>(name);

    lock.unlock();

    SetWindowTextW(wndData->window, name);
}

bool f::WindowHasFocus(WndH handle)
{
    std::unique_lock<std::mutex> lock(i::GetState()->windowDataMutex);
    i::WindowData* wndData = i::GetWindowData(handle);
    if (!wndData) [[unlikely]] { return false; }
    lock.unlock();
    return wndData->hasFocus;
}

bool f::IsValidHandle(WndH handle)
{
    return i::GetWindowData(handle) != nullptr;
}

bool f::Running()
{
    return i::GetState()->isRunning;
}

void f::Halt(int milliseconds)
{
    Sleep(milliseconds);
}

bool f::IsKeyPressed(Key code)
{
    return i::GetState()->keyStates.test(static_cast<int>(code));
}

bool f::IsKeyPressedOnce(Key code)
{
    bool state = i::GetState()->keyStates.test(static_cast<int>(code));
    if (state)
        i::GetState()->keyStates.reset(static_cast<int>(code));

    return state;
}

bool f::IsKeyReleased(Key code)
{
    static bool lastState;
    if (lastState && !IsKeyPressed(code))
    {
        lastState = IsKeyPressed(code);
        return true;
    }
    lastState = IsKeyPressed(code);
    return false;
}

bool f::IsAnyKeyPressed()
{
    return i::GetState()->keyStates.any();
}

void f::SetTextInputState(bool state, bool clear)
{
    i::GetState()->textInputEnabled = state;
    if (clear) // true by default
    {
        memset(i::GetState()->textInput, 0, 200000);
    }
}

const wchar_t* f::GetTextInput()
{
    return i::GetState()->textInput;
}

void f::ClearTextInput()
{
    memset(i::GetState()->textInput, 0, 200000);
}

bool f::GetTextInputState()
{
    return i::GetState()->textInputEnabled;
}

f::MouseInfo f::GetMouseInfo()
{
    MouseInfo msInfo = {};

    msInfo.left     = i::GetState()->mouse.leftButton;
    msInfo.right    = i::GetState()->mouse.rightButton;
    msInfo.middle   = i::GetState()->mouse.middleButton;
    msInfo.x1       = i::GetState()->mouse.x1Button;
    msInfo.x2       = i::GetState()->mouse.x2Button;
    msInfo.xPos     = i::GetState()->mouse.xPos;
    msInfo.yPos     = i::GetState()->mouse.yPos;
    msInfo.containingWindow = f::GetMouseContainerWindow();

    return msInfo;
}

int f::GetMouseX()
{
    return i::GetState()->mouse.xPos;
}

int f::GetMouseY()
{
    return i::GetState()->mouse.yPos;
}

bool f::GetMouseLeftButton()
{
    return i::GetState()->mouse.leftButton;
}

bool f::GetMouseLeftButtonOnce()
{
    bool state = i::GetState()->mouse.leftButton;
    if (state)
        i::GetState()->mouse.leftButton = false;
    return state;
}

bool f::GetMouseRightButton()
{
    return i::GetState()->mouse.rightButton;
}

bool f::GetMouseRightButtonOnce()
{
    bool state = i::GetState()->mouse.rightButton;
    if (state)
        i::GetState()->mouse.rightButton = false;
    return state;
}

bool f::GetMouseMiddleButton()
{
    return i::GetState()->mouse.middleButton;
}

bool f::GetMouseMiddleButtonOnce()
{
    bool state = i::GetState()->mouse.middleButton;
    if (state)
        i::GetState()->mouse.middleButton = false;
    return state;
}

bool f::GetMouseX1Button()
{
    return i::GetState()->mouse.x1Button;
}

bool f::GetMouseX1ButtonOnce()
{
    bool state = i::GetState()->mouse.x1Button;
    if (state)
        i::GetState()->mouse.x1Button = false;
    return state;
}

bool f::GetMouseX2Button()
{
    return i::GetState()->mouse.x2Button;
}

bool f::GetMouseX2ButtonOnce()
{
    bool state = i::GetState()->mouse.x2Button;
    if (state)
        i::GetState()->mouse.x2Button = false;
    return state;
}

int f::GetMouseWheelDelta()
{
    static int lastDelta;

    if (lastDelta < i::GetState()->mouse.wheelDelta)
    {
        lastDelta = i::GetState()->mouse.wheelDelta;
        return 1;
    }

    if (lastDelta > i::GetState()->mouse.wheelDelta)
    {
        lastDelta = i::GetState()->mouse.wheelDelta;
        return -1;
    }

    lastDelta = i::GetState()->mouse.wheelDelta;
    return 0;
}

bool f::WindowContainsMouse(WndH handle)
{
    std::unique_lock<std::mutex> lock(i::GetState()->windowDataMutex);
    i::WindowData* wndData = i::GetWindowData(handle);
    if (wndData)
    {
        lock.unlock();
        return wndData->hasMouseInClientArea;
    }
    lock.unlock();
    return false;
}

f::WndH f::GetMouseContainerWindow()
{
    std::unique_lock<std::mutex> lock(i::GetState()->windowDataMutex);
    for (std::pair<WndH, i::WindowData*> pair : i::GetState()->win32.identifiersToData)
    {
        if (pair.second->hasMouseInClientArea)
        {
            lock.unlock();
            return pair.second->id;
        }
    }
    lock.unlock();
    return 0;
}

void f::SetWindowVisibility(f::WndH handle, f::WindowVisibility visibility)
{
    ShowWindow(i::GetWindowData(handle)->window, visibility);
}

bool f::GetWindowRectangle(WndH handle, Rectangle& wpr)
{
    i::WindowData* wndData = i::GetWindowData(handle);
    if (!wndData) [[unlikely]]
        return false;

    wpr.bottom      = static_cast<int16_t>(wndData->yPos + wndData->height);
    wpr.top         = wndData->yPos;
    wpr.right       = static_cast<int16_t>(wndData->xPos + wndData->width);
    wpr.left        = wndData->xPos;

    return true;
}

void* f::LoadFile[[nodiscard("memory leak if not freed")]](const char* file, size_t& bytes)
{
    std::ifstream infile(file, std::ios_base::binary | std::ios_base::in);

    if (!infile.fail())
    {
        // determine size of file
        infile.seekg(0, std::ios::end);
        bytes = infile.tellg();
        infile.seekg(0, std::ios::beg);

        auto* buffer = new char[bytes];

        infile.read(buffer, static_cast<int64_t>(bytes));

        infile.close();
        return buffer;
    }

    // failed to access specified file
    // either access was denied or file does not exist
    std::ostringstream msg;
    msg << "Failed to open " << file;
    i::Log(msg.str().c_str(), i::LlError);
    return nullptr;
}

bool f::InitialiseNetworking(const f::NetworkingInitData& networkingInitData)
{
    WSADATA winSockData{};
    int err = WSAStartup(MAKEWORD(2, 2), &winSockData);

    switch (err)
    {
        case 0: // no error
            break;
        case WSAVERNOTSUPPORTED:
        {
            i::Log("Winsock 2.2 is not supported on this system", i::LlError);
            return false;
        }
        case WSASYSNOTREADY:
        {
            i::Log("The Winsock networking subsystem is not available for use", i::LlError);
            return false;
        }
        case WSAEPROCLIM:
        {
            i::Log("Too many instances of Winsock 2.2 are running, procedure limit was reached", i::LlError);
            return false;
        }
        default:
        {
            i::Log("An error was produced by Winsock starting", i::LlError); // *should* never reach
            return false;
        }
    }

    return true;
}

bool f::UnInitialiseNetworking()
{
    WSA_EC(WSACleanup());

    return true;
}
