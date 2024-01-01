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

#include "Framework.hpp"
#include "Internals.hpp"

void in::DoNothing_V()
{}

bool in::DoNothing_B()
{ return true; }

void in::CreateWin32DebugError(int line)
{
    int e = GetLastError();
    std::ostringstream msg;
    char* eMsg = nullptr;

    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        e,
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        reinterpret_cast<LPSTR>(&eMsg),
        0,
        nullptr
    );

    if (eMsg)
    {
        msg << "A Win32 API call resulted in a fatal error " << e << " at line " << line << " in the source of the framework.\n\n" << eMsg;
        msg << "\n" << "This is an internal error likely caused by the framework itself, the application must quit now." << std::endl;
    }
    else
    {
        msg << "An Error occoured which even the error handler could not handle. This is usually caused by the error";
        msg << " message being to long" << "\n\n";
        msg << "I guess I fucked up..." << std::endl;
    }

    MessageBoxA(nullptr, msg.str().c_str(), "Internal Error!", MB_ICONERROR | MB_TASKMODAL | MB_OK);
    LocalFree((LPSTR)eMsg);

    std::exception exc;
    throw exc;
}

void in::CreateWin32ReleaseError(int line)
{
    std::ofstream file("Last_Log.txt");
    
    int e = GetLastError();

    file << "[ " << __TIMESTAMP__ << " ]: " << "Unhandled Win32 error! " << e << " at " << line << "\n";
    file << "[ " << __TIMESTAMP__ << " ]: " << "Fatal Error, application must abort!" << std::endl;

    file.close();

    std::ostringstream msg;
    msg << "A critical error occoured, the application must quit now!\n\nFor more information check the logfiles in the application";
    msg << " directory" << std::endl; 

    MessageBoxA(nullptr, msg.str().c_str(), "Critical Error!", MB_TASKMODAL | MB_OK | MB_ICONERROR);

    std::exception exc;
    throw exc;
}

void in::CreateManualError(const char* msg, const char* func)
{
    std::ostringstream str;

    str << "An oparation within the framework has cause an error:\n\n";
    str << msg << "\n";
    str << "Origin: " << func << "\n\n";
    str << "This is an internal error likely caused by the framework itself. ";
    str << "The program is unable to recover, the application must quit now!";
    str << std::endl;

    MessageBoxA(nullptr, str.str().c_str(), "Internal Error", MB_TASKMODAL | MB_OK | MB_ICONERROR);

    throw std::exception();
}

void in::SetLastError(int code)
{
    AppInfo.lastErrorCode = code;
}

void in::WindowData::MessageHandler()
{
    WIN32_EC_RET(hWnd, CreateWindowExW(
        0,
        in::AppInfo.windowClassName,
        name,
        WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU,
        !xPos ? CW_USEDEFAULT : xPos, !yPos ? CW_DEFAULT : yPos, // man do I love tenary expression :)
        width, height,
        nullptr,
        nullptr,
        in::AppInfo.hInstance,
        nullptr
    ));
    in::AppInfo.windowCount += 1;
    in::AppInfo.windowsOpened += 1;

    id = in::AppInfo.windowsOpened; 
    
    ShowWindow(hWnd, 1);
    
    // work is done, let the main thread continue
    std::unique_lock<std::mutex> lock1(in::AppInfo.mtx);
    AppInfo.windowIsFinished = true;
    lock1.unlock();
    in::AppInfo.cv.notify_one();

    // message pump for the window
    MSG msg = { };
    while (GetMessage(&msg, hWnd, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    // Window is closed, update window information
    isValid = false;

    EraseUnusedWindowData();
}

LRESULT in::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    ///////////////////////////////////
    // Closing the window
    case WM_CLOSE: // closing of a window has been requested
    {
        if (GetWindowData(hWnd)->OnCloseAttempt())
            DestroyWindow(hWnd);
        return 0;
    }
    case WM_DESTROY: // closing a window was ordered and confirmed
    {
        GetWindowData(hWnd)->OnClose();
        if (tsd::GetWindowCount() == 1) // quit program if last window remaining is closed
        {
            AppInfo.isRunning = false;
            break;
        }
        
        AppInfo.windowCount -= 1;
        return -1;
    }
    // Closing the window
    ///////////////////////////////////

    ///////////////////////////////////
    // Keboard and text input
    case WM_KEYDOWN:
    {
        if (!(lParam & 0x40000000)) // bitmask, check previous keystate
        {
            in::AppInfo.keystates.set(wParam);
        }
        break;
    }
    case WM_KEYUP:
    {
        in::AppInfo.keystates.reset(wParam);
        break;
    }
    case WM_SYSKEYDOWN:
    {
        if (!(lParam & 0x40000000)) // bitmask, check previous keystate
        {
            in::AppInfo.keystates.set(wParam);
        }
        break;
    }
    case WM_SYSKEYUP:
    {
        in::AppInfo.keystates.reset(wParam);
        break;
    }
    case WM_CHAR:
    { 
        if (in::AppInfo.textInputEnabled)
        {
            if (wParam != 0x0008) // is not backspace
            {
                in::AppInfo.textInput[in::AppInfo.textInputIndex] = wParam;
                in::AppInfo.textInputIndex++;
            }
            else
            {
                if (in::AppInfo.textInputIndex > 0)
                    in::AppInfo.textInputIndex--;
                in::AppInfo.textInput[in::AppInfo.textInputIndex] = 0;
            }
        }
        break;
    }
    // Keboard and text input
    ///////////////////////////////////

    ///////////////////////////////////
    // Mouse input
    case WM_MOUSEMOVE:
    {
        in::AppInfo.mouse.xPos = GET_X_LPARAM(lParam);
        in::AppInfo.mouse.yPos = GET_Y_LPARAM(lParam);
        GetWindowData(hWnd)->hasMouseInClientArea = true;

        TRACKMOUSEEVENT msEv = {};
        msEv.cbSize     = sizeof(TRACKMOUSEEVENT);
        msEv.dwFlags    = TME_LEAVE;
        msEv.hwndTrack  = hWnd;

        WIN32_EC(TrackMouseEvent(&msEv));

        break;
    }
    case WM_LBUTTONDOWN:
    {
        SetCapture(hWnd);
        in::AppInfo.mouse.leftButton = true;
        break;
    }
    case WM_LBUTTONUP:
    {
        ReleaseCapture();
        in::AppInfo.mouse.leftButton = false;
        break;
    }
    case WM_MBUTTONDOWN:
    {
        SetCapture(hWnd);
        in::AppInfo.mouse.middleButton = true;
        break;
    }
    case WM_MBUTTONUP:
    {
        in::AppInfo.mouse.middleButton = false;
        break;
    }
    case WM_RBUTTONDOWN:
    {
        in::AppInfo.mouse.rightButton = true;
        break;
    }
    case WM_RBUTTONUP:
    {
        in::AppInfo.mouse.rightButton = false;
        break;
    }
    case WM_XBUTTONDOWN:
    {
        int button = HIWORD(wParam);
        switch (button)
        {
        case 1:
        {
            in::AppInfo.mouse.x1Button = true;
            break;
        }
        case 2:
        {
            in::AppInfo.mouse.x2Button = true;
            break;
        }
        }
    }
    case WM_XBUTTONUP:
    {
        int button = HIWORD(wParam);
        switch (button)
        {
        case 1:
        {
            in::AppInfo.mouse.x1Button = false;
            break;
        }
        case 2:
        {
            in::AppInfo.mouse.x2Button = false;
            break;
        }
        }
    }
    case WM_MOUSEWHEEL:
    {
        AppInfo.mouse.wheelDelta += GET_WHEEL_DELTA_WPARAM(wParam);
        break;
    }
    case WM_MOUSELEAVE:
    {
        GetWindowData(hWnd)->hasMouseInClientArea = false;
    }
    // Mouse input
    ///////////////////////////////////

    ///////////////////////////////////
    // Focus gain and loss
    case WM_SETFOCUS:
    {
        if (AppInfo.windows.empty())
            break;
        WindowData* wndData = in::GetWindowData(hWnd);
        if (wndData)
        {
            wndData->hasFocus = true;
        }

        break;
    }
    case WM_KILLFOCUS:
    {
        if (AppInfo.windows.empty())
            break;
        WindowData* wndData = in::GetWindowData(hWnd);
        if (wndData)
        {
            wndData->hasFocus = false;
        }
        in::AppInfo.keystates.reset();
        break;
    }
    // Focus gain and loss
    ///////////////////////////////////
    }
    
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

in::WindowData* in::GetWindowData(HWND handle)
{
    for (WindowData* i : AppInfo.windows)
    {
        if (i->hWnd == handle)
        {
            return i;
        }
    }
    return nullptr;
}

in::WindowData* in::GetWindowData(short id)
{
    for (WindowData* i : AppInfo.windows)
    {
        if (i->id == id)
        {
            return i;
        }
    }
    return nullptr;
}

void in::EraseUnusedWindowData()
{
    int i = 0;
    while (i < AppInfo.windows.size())
    {
        WindowData* wndDt = AppInfo.windows.at(i);
        if (!wndDt->isValid)
        {
            delete wndDt->msgThread;
            delete wndDt;
            std::vector<WindowData*>::iterator it = AppInfo.windows.begin() + i;
            AppInfo.windows.erase(it);
        }
        else
        {
            i++;
        }
    }
}

bool tsd::Initialise(int iconId, int cursorId)
{
    bool success = true;

    // Get hInstance since the program does not use the winMain entry point
    in::AppInfo.hInstance = GetModuleHandle(0);

    if (iconId)
    {
        in::AppInfo.hIcon = LoadIcon(in::AppInfo.hInstance, MAKEINTRESOURCE(iconId));
        if (in::AppInfo.hIcon == 0) { in::SetLastError(5); success = false; }
    }

    if (cursorId)
    {
        in::AppInfo.hCursor = LoadCursor(in::AppInfo.hInstance, MAKEINTRESOURCE(cursorId));
        if (!in::AppInfo.hCursor) { in::SetLastError(6); success = false; }
    }
    
    WNDCLASSEXW wc = {};

    wc.cbClsExtra       = 0;
    wc.cbSize           = sizeof(WNDCLASSEXW);
    wc.cbWndExtra       = 0;
    wc.hbrBackground    = nullptr;
    wc.hCursor          = in::AppInfo.hCursor;
    wc.hIcon            = in::AppInfo.hIcon;
    wc.hIconSm          = in::AppInfo.hIcon;
    wc.hInstance        = in::AppInfo.hInstance;
    wc.lpfnWndProc      = in::WindowProc;
    wc.lpszClassName    = in::AppInfo.windowClassName;
    wc.lpszMenuName     = nullptr;
    wc.style            = 0;

    WIN32_EC_RET(in::AppInfo.classAtom, RegisterClassExW(&wc));

    // allocate memory for text input field
    in::AppInfo.textInput = new wchar_t[100000]{0}; // thats 200 whole kilobytes of ram right there

    in::AppInfo.isInitialised = true;

    return success;
}

void tsd::Uninitialise()
{
    // No update to AppData needed since the program will quit now

    WIN32_EC(UnregisterClassW(in::AppInfo.windowClassName, in::AppInfo.hInstance));
}

// Whoops
#undef CreateWindow

short tsd::CreateWindow(const wchar_t* name, int width, int height, int xPos, int yPos)
{
    if (!in::AppInfo.isInitialised) { in::SetLastError(2); return 0; } // init was not called
    if (!name) { in::SetLastError(3); return 0; } // name is nullptr
    if ((height <= 0) || (width <= 0)) { in::SetLastError(3); return 0; }

    in::WindowData* wndData = new in::WindowData;
    wndData->msgThread = new std::thread(&in::WindowData::MessageHandler, wndData);
    wndData->msgThread->detach();

    wndData->name       = const_cast<wchar_t*>(name);
    wndData->width      = width;
    wndData->height     = height;
    wndData->xPos       = xPos;
    wndData->yPos       = yPos;
    wndData->isValid    = true;
    wndData->hasFocus   = true;
    wndData->hasMouseInClientArea = false;

    // wait for window creation to finish
    std::unique_lock<std::mutex> lock(in::AppInfo.mtx);
    in::AppInfo.cv.wait(lock, [] { return in::AppInfo.windowIsFinished; });

    in::AppInfo.windows.push_back(wndData);

    in::AppInfo.windowIsFinished = false;

    // ran out of range
    if (wndData->id == SHRT_MAX)
    {
        in::SetLastError(7); 
        return 0;
    }

    return wndData->id;
}

void tsd::OnWindowCloseAttempt(short handle, bool(*func)(void))
{
    in::GetWindowData(handle)->OnCloseAttempt = func;
}

void tsd::OnWindowClose(short handle, void(*func)(void))
{
    in::GetWindowData(handle)->OnClose = func;
}

void tsd::CreateAutoError(int line, bool quit)
{
    std::ostringstream msg;
    msg << "Error " << tsd::GetLastError() << " has occoured at line " << line << ".\n\n";
    msg << tsd::GetErrorInformation(tsd::GetLastError()) << "\n\n";
    if (quit) { msg << "The application must quit now."; }
    msg << std::endl;

    MessageBoxA(nullptr, msg.str().c_str(), "Error!", MB_TASKMODAL | MB_OK | MB_ICONERROR);

    if (quit)
    {
        std::exception exc;
        throw exc;
    }
}

int tsd::GetLastError()
{
    return in::AppInfo.lastErrorCode;
}

const char* tsd::GetErrorInformation(int code)
{
    // Determine whether the requested error code is in the hashmap, if so, retrieve the error code. If not return direct error message
    return in::errors.find(code) != in::errors.end() ? in::errors[code] : "Invalid error Code!"; // best one-liner so far
}

wchar_t* tsd::GetWindowName(short id)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) { in::SetLastError(4); return nullptr; }
    return wndData->name;
}

bool tsd::GetWindowVisibility(short id)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) { in::SetLastError(4); return false; }
    return wndData->isVisible;
}

int tsd::GetWindowWidth(short id)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) { in::SetLastError(4); return false; }
    return wndData->width;
}

int tsd::GetWindowHeight(short id)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) { in::SetLastError(4); return false; }
    return wndData->height;
}

std::pair<int, int> tsd::GetWindowDimensions(short id)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) { in::SetLastError(4); return {0, 0}; }
    return {wndData->width, wndData->height};
}

int tsd::GetWindowXPos(short id, WPR wpr)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) { in::SetLastError(4); return 0; }
    RECT rect{};
    GetWindowRect(wndData->hWnd, &rect);

    switch (wpr)
    {
    case WPR::LEFT:
        return rect.left;
    case WPR::RIGHT:
        return rect.right;
    }

    in::SetLastError(3);
    return 0;
}

int tsd::GetWindowYPos(short id, WPR wpr)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) { in::SetLastError(4); return 0; }
    RECT rect{};
    GetWindowRect(wndData->hWnd, &rect);

    switch (wpr)
    {
    case WPR::TOP:
        return rect.top;
    case WPR::BOTTOM:
        return rect.bottom;
    }

    in::SetLastError(3);
    return 0;
}

std::pair<int, int> tsd::GetWindowPosition(short id, WPR wpr)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) { in::SetLastError(4); return {0, 0}; }
    RECT rect{};
    GetWindowRect(wndData->hWnd, &rect);
    
    switch (wpr)
    {
    case WPR::TOP_LEFT:
        return { rect.left, rect.top };
    case WPR::TOP_RIGHT:
        return { rect.right, rect.top };
    case WPR::BOTTOM_LEFT:
        return { rect.left, rect.bottom };
    case WPR::BOTTOM_RIGHT:
        return { rect.right, rect.bottom };
    }

    in::SetLastError(3);
    return {0, 0};
}

int tsd::GetWindowCount(void)
{
    return in::AppInfo.windowCount;
}

bool tsd::ChangeWindowName(short id, const wchar_t* name)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) { in::SetLastError(4); return false; }
    SetWindowText(wndData->hWnd, name);
    wndData->name = const_cast<wchar_t*>(name);
    return true;
}

bool tsd::WindowHasFocus(short id)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) { return false; }
    return wndData->hasFocus;
}

bool tsd::IsValidHandle(short handle)
{
    if (in::GetWindowData(handle))
        return true;
    return false;
}

bool tsd::Running()
{
    return in::AppInfo.isRunning;
}

void tsd::Halt(int ms)
{
    Sleep(ms);
}

#undef MessageBox
#undef IGNORE
tsd::MBR tsd::MessageBox(short owner, const wchar_t* title, const wchar_t* msg, int flags)
{
    // return null if the window is not found so I dont care
    in::WindowData* ownerData = in::GetWindowData(owner);

    long rawFlags = 0;

    // Where switch statement?
    // Cant put (non-constant) expressions into switch cases
    if (flags & (int)MBF::TASKMODAL)
        rawFlags = rawFlags | MB_TASKMODAL;

    if (flags & (int)MBF::ICON_WARNING)
        rawFlags = rawFlags | MB_ICONWARNING;
    if (flags & (int)MBF::ICON_ERROR)
        rawFlags = rawFlags | MB_ICONERROR;
    if (flags & (int)MBF::ICON_INFO)
        rawFlags = rawFlags | MB_ICONINFORMATION;
    if (flags & (int)MBF::ICON_QUESTION)
        rawFlags = rawFlags | MB_ICONQUESTION;
    
    if (flags & (int)MBF::BUTTON_OK)
        rawFlags = rawFlags | MB_OK;
    if (flags & (int)MBF::BUTTON_OK_CANCEL)
        rawFlags = rawFlags | MB_OKCANCEL;
    if (flags & (int)MBF::BUTTON_YES_NO)
        rawFlags = rawFlags | MB_YESNO;
    if (flags & (int)MBF::BUTTON_RETRY_CANEL)
        rawFlags = rawFlags | MB_RETRYCANCEL;

    if (flags & (int)MBF::BUTTON_YES_NO_CANCEL)
        rawFlags = rawFlags | MB_YESNOCANCEL;
    if (flags & (int)MBF::BUTTON_ABORT_RETRY_IGNORE)
        rawFlags = rawFlags | MB_ABORTRETRYIGNORE;
    if (flags & (int)MBF::BUTTON_CANCEL_RETRY_CONTINUE)
        rawFlags = rawFlags | MB_CANCELTRYCONTINUE;

// by now win32 is just getting anoying
#ifdef UNICODE
#define MessageBox  MessageBoxW
#else
#define MessageBox  MessageBoxA
#endif

    int result = MessageBox(ownerData ? ownerData->hWnd : 0, msg, title, rawFlags);

    switch (result)
    {
    case IDABORT: return MBR::ABORT;
    case IDCANCEL: return MBR::CANCEL;
    case IDCONTINUE: return MBR::CONTINUE;
    case IDIGNORE: return MBR::IGNORE;
    case IDNO: return MBR::NO;
    case IDOK: return MBR::OK;
    case IDRETRY: return MBR::RETRY;
    case IDTRYAGAIN: return MBR::TRYAGAIN;
    case IDYES: return MBR::YES;
    }
    return MBR::CANCEL; // should never reach this
}
#define IGNORE 0

bool tsd::IsKeyPressed(Key code)
{
    return in::AppInfo.keystates.test((int)code);
}

bool tsd::IsKeyPressedOnce(Key code)
{
    bool state = in::AppInfo.keystates.test((int)code);
    if (state)
        in::AppInfo.keystates.reset((int)code);
    return state;
}

bool tsd::IsKeyReleased(Key code)
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

bool tsd::IsAnyKeyPressed()
{
    return in::AppInfo.keystates.any();
}

void tsd::SetTextInputState(bool state, bool clear)
{
    in::AppInfo.textInputEnabled = state;
    if (clear)
    {
        for (int i = 0; i < 100000; i++)
        {
            in::AppInfo.textInput[i] = 0;
        }
        in::AppInfo.textInputIndex = 0;
    }
}

wchar_t* tsd::GetTextInput()
{
    return in::AppInfo.textInput;
}

void tsd::ClearTextInput()
{
    for (int i = 0; i < 100000; i++)
    {
        in::AppInfo.textInput[i] = 0;
    }
    in::AppInfo.textInputIndex = 0;
}

bool tsd::IsTextInputEnabled()
{
    return in::AppInfo.textInputEnabled;
}

tsd::MouseInfo tsd::GetMouseInfo()
{
    MouseInfo msInfo = {};

    msInfo.left     = in::AppInfo.mouse.leftButton;
    msInfo.right    = in::AppInfo.mouse.rightButton;
    msInfo.middle   = in::AppInfo.mouse.middleButton;
    msInfo.x1       = in::AppInfo.mouse.x1Button;
    msInfo.x2       = in::AppInfo.mouse.x2Button;
    msInfo.xPos     = in::AppInfo.mouse.xPos;
    msInfo.yPos     = in::AppInfo.mouse.yPos;
    msInfo.containingWindow = tsd::GetMouseContainerWindow();

    return msInfo;
}

int tsd::GetMouseX()
{
    return in::AppInfo.mouse.xPos;
}

int tsd::GetMouseY()
{
    return in::AppInfo.mouse.yPos;
}

bool tsd::GetMouseLeftButton()
{
    return in::AppInfo.mouse.leftButton;
}

bool tsd::GetMouseLeftButtonOnce()
{
    bool state = in::AppInfo.mouse.leftButton;
    if (state)
        in::AppInfo.mouse.leftButton = false;
    return state;
}

bool tsd::GetMouseRightButton()
{
    return in::AppInfo.mouse.rightButton;
}

bool tsd::GetMouseRightButtonOnce()
{
    bool state = in::AppInfo.mouse.rightButton;
    if (state)
        in::AppInfo.mouse.rightButton = false;
    return state;
}

bool tsd::GetMouseMiddleButton()
{
    return in::AppInfo.mouse.middleButton;
}

bool tsd::GetMouseMiddleButtonOnce()
{
    bool state = in::AppInfo.mouse.middleButton;
    if (state)
        in::AppInfo.mouse.middleButton = false;
    return state;
}

bool tsd::GetMouseX1Button()
{
    return in::AppInfo.mouse.x1Button;
}

bool tsd::GetMouseX1ButtonOnce()
{
    bool state = in::AppInfo.mouse.x1Button;
    if (state)
        in::AppInfo.mouse.x1Button = false;
    return state;
}

bool tsd::GetMouseX2Button()
{
    return in::AppInfo.mouse.x2Button;
}

bool tsd::GetMouseX2ButtonOnce()
{
    bool state = in::AppInfo.mouse.x2Button;
    if (state)
        in::AppInfo.mouse.x2Button = false;
    return state;
}

int tsd::GetMouseWheelDelta()
{
    static int lastDelta;

    if (lastDelta < in::AppInfo.mouse.wheelDelta)
    {
        lastDelta = in::AppInfo.mouse.wheelDelta;
        return 1;
    }

    else if (lastDelta > in::AppInfo.mouse.wheelDelta)
    {
        lastDelta = in::AppInfo.mouse.wheelDelta;
        return -1;
    }

    lastDelta = in::AppInfo.mouse.wheelDelta;
    return 0;
}

bool tsd::WindowContainsMouse(short handle)
{
    in::WindowData* wndData = in::GetWindowData(handle);
    if (wndData)
        return wndData->hasMouseInClientArea;
    return false;
}

short tsd::GetMouseContainerWindow()
{
    for (in::WindowData* i : in::AppInfo.windows)
    {
        if (i->hasMouseInClientArea)
            return i->id;
    }
    return 0;
}
