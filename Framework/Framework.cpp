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

// About this file:
// Here are all the implementations that the (framework) user interacts with.

#include "Framework.hpp"
#include "Internals.hpp"

void f::Initialize(int iconId, int cursorId)
{
    // Get hInstance since the program does not use the winMain entry point
    i::GetState()->win32.instance = GetModuleHandle(nullptr);

    // Check the recourses, if invalid continue anyway
    if (iconId)
    {
        i::GetState()->win32.icon = (HICON)LoadImageA(
            i::GetState()->win32.instance, 
            MAKEINTRESOURCE(iconId), 
            IMAGE_ICON, 0, 0, 
            LR_DEFAULTCOLOR);

        if (!i::GetState()->win32.icon) 
        { 
            i::Log(L"Specified recourse Id for an icon was invalid", i::LL::ERROR);
        }
    }

    if (cursorId)
    {
        i::GetState()->win32.cursor = (HCURSOR)LoadImageA(
            i::GetState()->win32.instance, 
            MAKEINTRESOURCE(cursorId), 
            IMAGE_CURSOR, 0, 0, 
            LR_DEFAULTCOLOR);

        if (!i::GetState()->win32.cursor) 
        { 
            i::Log(L"Specified recourse Id for a mouse was invalid", i::LL::ERROR);
        }
    }
    
    WNDCLASSEXW wc = {};

    wc.cbClsExtra       = 0;
    wc.cbSize           = sizeof(WNDCLASSEXW);
    wc.cbWndExtra       = 0;
    wc.hbrBackground    = nullptr;
    wc.hCursor          = i::GetState()->win32.cursor;
    wc.hIcon            = i::GetState()->win32.icon;
    wc.hIconSm          = i::GetState()->win32.icon;
    wc.hInstance        = i::GetState()->win32.instance;
    wc.lpfnWndProc      = i::WindowProc;
    wc.lpszClassName    = i::GetState()->win32.pClassName;
    wc.lpszMenuName     = nullptr;
    wc.style            = 0;

    WIN32_EC(RegisterClassExW(&wc));

    i::GetState()->pWindowThread = new std::thread(i::WindowThread);

    std::unique_lock<std::mutex> lock(i::GetState()->windowThreadMutex);
    i::GetState()->windowThreadConditionVar.wait(lock, [] {return i::GetState()->windowThreadIsRunning; });

    i::GetState()->isInitialised = true;

    v::InitialiseVulkan();
    i::Log(L"Framework was successfully initialised", i::LL::INFO);
}

void f::Uninitialize()
{
    std::unique_lock<std::mutex> lock(i::GetState()->windowThreadMutex);
    i::GetState()->windowThreadConditionVar.wait(lock, []{ return !i::GetState()->windowThreadIsRunning; });

    WIN32_EC(UnregisterClassW(i::GetState()->win32.pClassName, i::GetState()->win32.instance));

    v::UninitialiseVulkan();

    i::Log(L"Framework was successfully uninitialised", i::LL::INFO);
}

// Whoops
#undef CreateWindow
f::WND_H f::CreateWindow(const f::WindowCreateData& wndCrtData)
{
    if (!i::GetState()->isInitialised) { i::Log(L"CreateWindow() was called before initialisation", i::LL::ERROR); return 0; } // init was not called
    if (!wndCrtData.pName) { i::Log(L"Nullptr was passed to a required parameter at CreateWindow()", i::LL::ERROR); return 0; } // name is nullptr
    if ((wndCrtData.height <= 0) || (wndCrtData.width <= 0)) { i::Log(L"Invalid heigt or width ammount for window creation", i::LL::ERROR); return 0; }

    i::WindowData* wndData = new i::WindowData;

    i::GetState()->windowCount += 1;
    i::GetState()->windowsOpened += 1;

    wndData->name       = (wchar_t*)wndCrtData.pName;
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
        for (WND_H i : wndCrtData.dependants)
        {
            wndData->dependers.push_back(i::GetWindowData(i)->window);
        }
    }

    WIN32_EC(PostThreadMessageW(
        i::GetState()->win32.nativeThreadId,
        WM_CREATEWINDOWREQ, 
        (WPARAM)nullptr, 
        (LPARAM)wndData)
    );

    return wndData->id;
}

void f::CloseWindow(const WND_H handle)
{
    i::WindowData* windowData = i::GetWindowData(handle);
    
    if (!windowData) // window does not exist
        return;

    WIN32_EC(PostMessageA(
        windowData->window,
        WM_CLOSE,
        (WPARAM)nullptr,
        (LPARAM)nullptr
    ))
}

void f::CloseWindowForce(const WND_H handle)
{
    i::WindowData* windowData = i::GetWindowData(handle);

    if (!windowData) // window does not exist
        return;

    WIN32_EC(PostMessageA(
        windowData->window,
        WM_DESTROY,
        (WPARAM)nullptr,
        (LPARAM)nullptr
    ))
}

void f::CloseAllWindows()
{
    for (std::pair<f::WND_H, i::WindowData*>&& dataPair : i::GetState()->win32.identifiersToData)
    {
        WIN32_EC(PostMessageA(
            dataPair.second->window,
            WM_CLOSE,
            (WPARAM)nullptr,
            (LPARAM)nullptr
        ))
    }
}

void f::CloseAllWindowsForce()
{
    for (std::pair<f::WND_H, i::WindowData*>&& dataPair : i::GetState()->win32.identifiersToData)
    {
        WIN32_EC(PostMessageA(
            dataPair.second->window,
            WM_DESTROY,
            (WPARAM)nullptr,
            (LPARAM)nullptr
        ))
    }
}

#undef MessageBox
#undef IGNORE
int f::MessageBox(WND_H owner, const wchar_t* title, const wchar_t* msg, int flags)
// by now win32 is just getting anoying
#ifdef UNICODE
#define MessageBox  MessageBoxW
#else
#define MessageBox  MessageBoxA
#endif
{
    // return null if the window is not found so I dont care
    i::WindowData* ownerData = i::GetWindowData(owner);

    long rawFlags = 0;

    // Where switch statement?
    // Cant put (non-constant) expressions into switch cases
#undef MB_TASKMODAL
    if (flags & MB_TASKMODAL)
#define MB_TASKMODAL 0x00002000L
        rawFlags = rawFlags | MB_TASKMODAL;

    if (flags & MB_ICON_WARNING)
        rawFlags = rawFlags | MB_ICONWARNING;
    if (flags & MB_ICON_ERROR)
        rawFlags = rawFlags | MB_ICONERROR;
    if (flags & MB_ICON_INFO)
        rawFlags = rawFlags | MB_ICONINFORMATION;
    if (flags & MB_ICON_QUESTION)
        rawFlags = rawFlags | MB_ICONQUESTION;

    if (flags & MB_BUTTON_OK)
        rawFlags = rawFlags | MB_OK;
    if (flags & MB_BUTTON_OK_CANCEL)
        rawFlags = rawFlags | MB_OKCANCEL;
    if (flags & MB_BUTTON_YES_NO)
        rawFlags = rawFlags | MB_YESNO;
    if (flags & MB_BUTTON_RETRY_CANEL)
        rawFlags = rawFlags | MB_RETRYCANCEL;

    if (flags & MB_BUTTON_YES_NO_CANCEL)
        rawFlags = rawFlags | MB_YESNOCANCEL;
    if (flags & MB_BUTTON_ABORT_RETRY_IGNORE)
        rawFlags = rawFlags | MB_ABORTRETRYIGNORE;
    if (flags & MB_BUTTON_CANCEL_RETRY_CONTINUE)
        rawFlags = rawFlags | MB_CANCELTRYCONTINUE;

    int result = MessageBoxW(ownerData ? ownerData->window : 0, msg, title, rawFlags);
    if (result == 0)
        i::Log(L"Invalid set of flags where passed to MessageBox()", i::LL::ERROR);
#undef MB_OK
    switch (result)
    {
    case IDABORT:       return MB_ABORT;
    case IDCANCEL:      return MB_CANCEL;
    case IDCONTINUE:    return MB_CONTINUE;
    case IDIGNORE:      return MB_IGNORE;
    case IDNO:          return MB_NO;
    case IDOK:          return MB_OK;
    case IDRETRY:       return MB_RETRY;
    case IDTRYAGAIN:    return MB_TRYAGAIN;
    case IDYES:         return MB_YES;
    }
    return MB_CANCEL; // should never reach this
}
#define IGNORE 0

void f::OnWindowCloseAttempt(WND_H handle, bool(*func)(void))
{
    i::GetWindowData(handle)->pfOnCloseAttempt = func;
}

void f::OnWindowClose(WND_H handle, void(*func)(void))
{
    std::unique_lock<std::mutex> lock(i::GetState()->windowDataMutex);
    i::GetWindowData(handle)->pfOnClose = func;
    lock.unlock();
}

wchar_t* f::GetWindowName(WND_H id)
{
    std::unique_lock<std::mutex> lock(i::GetState()->windowDataMutex);
    i::WindowData* wndData = i::GetWindowData(id);
    if (!wndData) [[unlikely]] { i::Log(L"Invalid handle was passed to GetWindowName()", i::LL::WARNING); return nullptr; }
    lock.unlock();
    return wndData->name;
}

bool f::GetWindowVisibility(WND_H id)
{
    std::unique_lock<std::mutex> lock(i::GetState()->windowDataMutex);
    i::WindowData* wndData = i::GetWindowData(id);
    if (!wndData) [[unlikely]] { i::Log(L"Invalid handle was passed to GetWindowVisibility()", i::LL::WARNING); return false; }
    lock.unlock();
    return wndData->isVisible;
}

int f::GetWindowWidth(WND_H id)
{
    std::unique_lock<std::mutex> lock(i::GetState()->windowDataMutex);
    i::WindowData* wndData = i::GetWindowData(id);
    if (!wndData) [[unlikely]] { i::Log(L"Invalid handle was passed to GetWindowWidth()", i::LL::WARNING); return false; }
    lock.unlock();
    return wndData->width;
}

int f::GetWindowHeight(WND_H id)
{
    std::unique_lock<std::mutex> lock(i::GetState()->windowDataMutex);
    i::WindowData* wndData = i::GetWindowData(id);
    if (!wndData) [[unlikely]] { i::Log(L"Invalid handle was passed to GetWindowHeight()", i::LL::WARNING); return false; }
    lock.unlock();
    return wndData->height;
}

std::pair<int, int> f::GetWindowDimensions(WND_H id)
{
    std::unique_lock<std::mutex> lock(i::GetState()->windowDataMutex);
    i::WindowData* wndData = i::GetWindowData(id);
    if (!wndData) [[unlikely]] { i::Log(L"Invalid handle was passed to GetWindowDimensions()", i::LL::WARNING); return {0, 0}; }
    lock.unlock();
    return {wndData->width, wndData->height};
}

int f::GetWindowXPos(WND_H id, WindowPositionRelation wpr)
{
    std::unique_lock<std::mutex> lock(i::GetState()->windowDataMutex);
    i::WindowData* wndData = i::GetWindowData(id);
    if (!wndData) [[unlikely]] { i::Log(L"Invalid handle was passed to GetWindowXPos()", i::LL::WARNING); return 0; }
    RECT rect{};
    GetWindowRect(wndData->window, &rect);

    switch (wpr)
    {
    case WP_LEFT:
    {
        lock.unlock();
        return rect.left;
    }
    case WP_RIGHT:
    {
        lock.unlock();
        return rect.right;
    }
    }

    i::Log(L"Invalid positional identifier was passed to GetWindowXPos()", i::LL::WARNING);
    lock.unlock();
    return 0;
}

int f::GetWindowYPos(WND_H id, WindowPositionRelation wpr)
{
    std::unique_lock<std::mutex> lock(i::GetState()->windowDataMutex);
    i::WindowData* wndData = i::GetWindowData(id);
    if (!wndData) [[unlikely]] { i::Log(L"Invalid handle was passed to GetWindowYPos()", i::LL::WARNING); return 0; }
    RECT rect{};
    GetWindowRect(wndData->window, &rect);

    switch (wpr)
    {
    case WP_TOP:
    {
        lock.unlock();
        return rect.top;
    }
    case WP_BOTTOM:
    {
        lock.unlock();
        return rect.bottom;
    }
    }

    i::Log(L"Invalid positional identifier was passed to GetWindowYPos()", i::LL::WARNING);
    lock.unlock();
    return 0;
}

std::pair<int, int> f::GetWindowPosition(WND_H id, WindowPositionRelation wpr)
{
    std::unique_lock<std::mutex> lock(i::GetState()->windowDataMutex);
    i::WindowData* wndData = i::GetWindowData(id);
    if (!wndData) [[unlikely]] { i::Log(L"Invalid handle was passed to GetWindowPosition()", i::LL::WARNING); return {0, 0}; }
    RECT rect{};
    GetWindowRect(wndData->window, &rect);
    
    switch (wpr)
    {
    case WP_TOP_LEFT:
    {
        lock.unlock();
        return { rect.left, rect.top };
    }
    case WP_TOP_RIGHT:
    {
        lock.unlock();
        return { rect.right, rect.top };
    }
    case WP_BOTTOM_LEFT:
    {
        lock.unlock();
        return { rect.left, rect.bottom };
    }
    case WP_BOTTOM_RIGHT:
    {
        lock.unlock();
        return { rect.right, rect.bottom };
    }
    }

    i::Log(L"Invalid positional identifier was passed to GetWindowPosition()", i::LL::WARNING);
    lock.unlock();
    return {0, 0};
}

int f::GetWindowCount(void)
{
    return i::GetState()->windowCount;
}

void f::ChangeWindowName(WND_H id, const wchar_t* name)
{
    std::unique_lock<std::mutex> lock(i::GetState()->windowDataMutex);

    i::WindowData* wndData = i::GetWindowData(id);
    if (!wndData) { return; }
    wndData->name = (wchar_t*)name;

    lock.unlock();

    SetWindowTextW(wndData->window, name);
}

bool f::WindowHasFocus(WND_H id)
{
    std::unique_lock<std::mutex> lock(i::GetState()->windowDataMutex);
    i::WindowData* wndData = i::GetWindowData(id);
    if (!wndData) [[unlikely]] { return false; }
    lock.unlock();
    return wndData->hasFocus;
}

bool f::IsValidHandle(WND_H handle)
{   // no mutex neccessary here, no data that may be volatile
    if (i::GetWindowData(handle))
        return true;
    return false;
}

bool f::Running()
{
    return i::GetState()->isRunning;
}

void f::Halt(int ms)
{
    Sleep(ms);
}

bool f::IsKeyPressed(Key code)
{
    return i::GetState()->keystates.test((int)code);
}

bool f::IsKeyPressedOnce(Key code)
{
    bool state = i::GetState()->keystates.test((int)code);
    if (state)
        i::GetState()->keystates.reset((int)code);
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
    return i::GetState()->keystates.any();
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

    else if (lastDelta > i::GetState()->mouse.wheelDelta)
    {
        lastDelta = i::GetState()->mouse.wheelDelta;
        return -1;
    }

    lastDelta = i::GetState()->mouse.wheelDelta;
    return 0;
}

bool f::WindowContainsMouse(WND_H handle)
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

f::WND_H f::GetMouseContainerWindow()
{
    std::unique_lock<std::mutex> lock(i::GetState()->windowDataMutex);
    for (std::pair<WND_H, i::WindowData*> i : i::GetState()->win32.identifiersToData)
    {
        if (i.second->hasMouseInClientArea)
        {
            lock.unlock();
            return i.second->id;
        }
    }
    lock.unlock();
    return 0;
}
