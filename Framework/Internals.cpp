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

#ifdef _DEBUG
void i::CreateWin32Error(int line, int c, const char* func)
{
    //int e = GetLastError();
    std::ostringstream msg;
    char* errorMessage = nullptr;

    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        c,
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        reinterpret_cast<LPSTR>(&errorMessage),
        0,
        nullptr
    );

    if (errorMessage)
    {
        msg << "A Win32 API call resulted in fatal error " << c << " at line " << line << " in " << func << ".\n\n" << errorMessage;
        msg << "\n" << "This is an internal error likely caused by the framework itself, the application must quit now." << std::endl;
    }
    else
    {
        msg << "An Error occoured which even the error handler could not handle. This is usually caused by the error";
        msg << " message being to long" << "\n\n";
        msg << "I guess I fucked up..." << std::endl;
    }

    MessageBoxA(nullptr, msg.str().c_str(), "Internal Error!", MB_ICONERROR | MB_TASKMODAL | MB_OK);
    LocalFree((LPSTR)errorMessage);

    DeAlloc();
    std::exit(-1);
}
#endif // _DEBUG

#ifdef NDEBUG
void i::CreateWin32Error(int line, int c, const char* func)
{
    std::wostringstream emsg;
    emsg << "Win32 error: " << c << " at " << line << " in " << func << std::flush;
    i::Log(emsg.str().c_str(), i::LL::ERROR);

    std::ostringstream msg;
    msg << "A fatal error occoured, the application must quit now!\n\nFor more information check 'Last_Log.txt' in the application";
    msg << " directory" << std::flush;

    MessageBoxA(nullptr, msg.str().c_str(), "Fatal Error!", MB_TASKMODAL | MB_OK | MB_ICONERROR);

    DeAlloc();
    std::exit(-1);
}
#endif // DEBUG

void i::CreateManualError(int line, const char* func, const char* msg)
{
    std::ostringstream str;

    str << "An oparation within the framework has caused an error:\n\n";
    str << msg << "\n\n";
    str << "Origin: " << func << " at " << line << "\n\n";
    str << "This is an internal error likely caused by the framework itself. ";
    str << "The program is unable to recover, the application must quit now!";
    str << std::flush;

    MessageBoxA(nullptr, str.str().c_str(), "Internal Error", MB_TASKMODAL | MB_OK | MB_ICONERROR);

    DeAlloc();
    std::exit(-1);
}

void i::WindowThread()
{
    i::GetState()->win32.nativeThreadId = GetCurrentThreadId();

    std::wostringstream startMsg;
    startMsg << L"The window manager thread was started";
    i::Log(startMsg.str().c_str(), i::LL::DEBUG);

    std::unique_lock<std::mutex> lock(i::GetState()->windowThreadMutex);
    i::GetState()->windowThreadIsRunning = true;
    lock.unlock();
    i::GetState()->windowThreadConditionVar.notify_one();

    // message pump for the window
    MSG msg{};
    while (GetMessageA(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);

        // DispatchMessage does not handle custom messages
        // It should theoretically only dismiss messages when in a modal loop but thats not the case
        if (msg.message >= WM_USER)
        {
            i::WindowProc(nullptr, msg.message, msg.wParam, msg.lParam);
        }
    }

    i::GetState()->windowThreadIsRunning = false;
    i::GetState()->windowThreadConditionVar.notify_one();
}

void i::CreateNativeWindow(i::WindowData* wndDt)
{
    WIN32_EC_RET(wndDt->window, CreateWindowExW(
        0,
        i::GetState()->win32.pClassName,
        wndDt->name,
        WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU,
        !wndDt->xPos ? CW_USEDEFAULT : wndDt->xPos, !wndDt->yPos ? CW_DEFAULT : wndDt->yPos, // man do I love tenary expression :)
        wndDt->width, wndDt->height,
        nullptr,
        nullptr,
        i::GetState()->win32.instance,
        nullptr
    ));

    ShowWindow(wndDt->window, 1);

    // ran out of range
    if (wndDt->id == SHRT_MAX)
    {
        i::Log(L"Handle out of range, window was opened but no handle could be created. You somehow opened 32767 windows...", i::LL::ERROR);
        return;
    }

    std::wostringstream oss;
    oss << L"Window " << wndDt->window << " was created and recieved a handle of " << wndDt->id;
    i::Log(oss.str().c_str(), i::LL::DEBUG);

    i::GetState()->win32.identifiersToData.insert({wndDt->id, wndDt});
    i::GetState()->win32.handlesToData.insert({wndDt->window, wndDt});
}

LRESULT i::WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        ///////////////////////////////////
        // Custom messages
    case WM_CREATEWINDOWREQ:
    {
        i::CreateNativeWindow((WindowData*)lParam);
        break;
    }
    // Custom messages
    ///////////////////////////////////

    ///////////////////////////////////
    // Closing the window
    case WM_CLOSE: // closing of a window has been requested
    {
        if (GetWindowData(window)->pfOnCloseAttempt()) // go ahead and close the window of returns true
            DestroyWindow(window);
        return 0;
    }
    case WM_DESTROY: // closing a window was ordered and confirmed
    {
        WindowData* wnd = GetWindowData(window);
        wnd->pfOnClose();
        wnd->isValid = false;
        i::GetState()->windowCount -= 1;
        i::EraseWindowData(window);

        if (i::GetState()->windowCount == 0) // quit program if last window remaining is closed
        {
            i::GetState()->isRunning = false;
            PostQuitMessage(0);
        }

        return 0;
    }
    // Closing the window
    ///////////////////////////////////

    ///////////////////////////////////
    // Keboard and text input
    case WM_KEYDOWN:
    {
        if (!(lParam & 0x40000000)) // bitmask, check previous keystate
        {
            i::GetState()->keystates.set(wParam);
        }
        break;
    }
    case WM_KEYUP:
    {
        i::GetState()->keystates.reset(wParam);
        break;
    }
    case WM_SYSKEYDOWN:
    {
        if (!(lParam & 0x40000000)) // bitmask, check previous keystate
        {
            i::GetState()->keystates.set(wParam);
        }
        break;
    }
    case WM_SYSKEYUP:
    {
        i::GetState()->keystates.reset(wParam);
        break;
    }
    case WM_CHAR:
    {
        if (i::GetState()->textInputEnabled)
        {
            if (wParam != 0x0008) // key is not backspace
            {
                wchar_t* pText = i::GetState()->textInput;
                size_t textLenght = std::wcslen(pText);
                if (textLenght == 100000) break;
                pText[textLenght] = (wchar_t)wParam;
            }
            else
            {
                wchar_t* pText = i::GetState()->textInput;
                size_t textLengh = std::wcslen(pText);
                pText[textLengh - 1] = 0;
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
        i::GetState()->mouse.xPos = GET_X_LPARAM(lParam);
        i::GetState()->mouse.yPos = GET_Y_LPARAM(lParam);
        GetWindowData(window)->hasMouseInClientArea = true;

        break;
    }
    case WM_LBUTTONDOWN:
    {
        SetCapture(window);
        i::GetState()->mouse.leftButton = true;
        break;
    }
    case WM_LBUTTONUP:
    {
        ReleaseCapture();
        i::GetState()->mouse.leftButton = false;
        break;
    }
    case WM_MBUTTONDOWN:
    {
        SetCapture(window);
        i::GetState()->mouse.middleButton = true;
        break;
    }
    case WM_MBUTTONUP:
    {
        i::GetState()->mouse.middleButton = false;
        break;
    }
    case WM_RBUTTONDOWN:
    {
        i::GetState()->mouse.rightButton = true;
        break;
    }
    case WM_RBUTTONUP:
    {
        i::GetState()->mouse.rightButton = false;
        break;
    }
    case WM_XBUTTONDOWN:
    {
        int button = HIWORD(wParam);
        switch (button)
        {
        case 1:
        {
            i::GetState()->mouse.x1Button = true;
            break;
        }
        case 2:
        {
            i::GetState()->mouse.x2Button = true;
            break;
        }
        }
        break;
    }
    case WM_XBUTTONUP:
    {
        int button = HIWORD(wParam);
        switch (button)
        {
        case 1:
        {
            i::GetState()->mouse.x1Button = false;
            break;
        }
        case 2:
        {
            i::GetState()->mouse.x2Button = false;
            break;
        }
        }
        break;
    }
    case WM_MOUSEWHEEL:
    {
        i::GetState()->mouse.wheelDelta += GET_WHEEL_DELTA_WPARAM(wParam);
        break;
    }
    case WM_MOUSELEAVE:
    {
        GetWindowData(window)->hasMouseInClientArea = false;
        break;
    }
    // Mouse input
    ///////////////////////////////////

    ///////////////////////////////////
    // Focus gain and loss
    case WM_SETFOCUS:
    {
        WindowData* wndData = i::GetWindowData(window);
        if (wndData)
        {
            wndData->hasFocus = true;
        }

        break;
    }
    case WM_KILLFOCUS:
    {
        WindowData* wndData = i::GetWindowData(window);
        if (wndData)
        {
            wndData->hasFocus = false;
        }
        i::GetState()->keystates.reset();
        break;
    }
    // Focus gain and loss
    ///////////////////////////////////
    }

    return DefWindowProcW(window, message, wParam, lParam);
}

void i::DeAlloc()
{
    for (std::pair<HWND, WindowData*> i : i::GetState()->win32.handlesToData)
    {
        delete i.second;
    }
    i::GetState()->win32.handlesToData.clear();
    i::GetState()->win32.identifiersToData.clear();
}

void i::DoNothing_V_V()
{}

bool i::DoNothing_V_B()
{ return true; }

i::WindowData* i::GetWindowData(HWND handle)
{
    std::unique_lock<std::mutex> lock(i::GetState()->windowDataMutex);
    std::map<HWND, WindowData*>::iterator found = i::GetState()->win32.handlesToData.find(handle);

    if (found != i::GetState()->win32.handlesToData.end())
    {
        lock.unlock();
        return found->second;
    }

    lock.unlock();
    return nullptr;
}

i::WindowData* i::GetWindowData(f::WND_H id)
{
    std::map<f::WND_H, WindowData*>::iterator found = i::GetState()->win32.identifiersToData.find(id);

    if (found != i::GetState()->win32.identifiersToData.end())
    {
        return found->second;
    }

    return nullptr;
}

void i::EraseWindowData(HWND hWnd)
{
    std::unique_lock<std::mutex> lock(i::GetState()->windowDataMutex);

    std::map<HWND, WindowData*>::iterator res = i::GetState()->win32.handlesToData.find(hWnd); // find data to be erased

    std::wostringstream msg;
    msg << "Window data for " << res->first << " who's handle is " << res->second->id << " was deleted";
    i::Log(msg.str().c_str(), i::LL::DEBUG);

    i::GetState()->win32.identifiersToData.erase(res->second->id); // erase data from the id map using the id
    delete res->second; // free window data
    i::GetState()->win32.handlesToData.erase(res); // erase data from handle map using the iterator

    lock.unlock();
}

void i::Log(const wchar_t* msg, LL ll)
{
#ifdef _DEBUG
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t currentDate = std::chrono::system_clock::to_time_t(now);

    char timeBuf[30] = { 0 }; // minimum required size for this is 26. Who knows if this is going to run in 9997976 years?
    ctime_s(timeBuf, sizeof(timeBuf), &currentDate);
    *std::strchr(timeBuf, '\n') = 0; // replace that pesky newline with the null-char

    // extra buffer, prevents asychrony from messing with the output when this func is called from different threads at the same time
    std::wostringstream oss;
    oss << "[ " << timeBuf << " ]";

    switch (ll)
    {
    case i::LL::INFO:
    {
        oss << " [ INFO ]: ";
        break;
    }
    case i::LL::DEBUG:
    {
        oss << " [ DEBUG ]: ";
        break;
    }
    case i::LL::WARNING:
    {
        oss << " [ WARNING ]: ";
        break;
    }
    case i::LL::ERROR:
    {
        oss << " [ ERROR ]: ";
        break;
    }
    case i::LL::VALIDATION:
    {
        oss << " [ VALID ]: ";
    }
    }

    i::GetState()->loggerMutex.lock();
    std::wcout << oss.str().c_str() << msg << "\n" << std::flush;
    i::GetState()->loggerMutex.unlock();
#endif // _DEBUG
}

void i::Log(const char* msg, LL ll)
{
#ifdef _DEBUG
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t currentDate = std::chrono::system_clock::to_time_t(now);

    char timeBuf[30] = { 0 }; // minimum required size for this is 26. Who knows if this is going to run in 9997976 years?
    ctime_s(timeBuf, sizeof(timeBuf), &currentDate);
    *std::strchr(timeBuf, '\n') = 0; // replace that pesky newline with the null-char

    // extra buffer, prevents asychrony from messing with the output when this func is called from different threads at the same time
    std::ostringstream oss;
    oss << "[ " << timeBuf << " ]";

    switch (ll)
    {
    case i::LL::INFO:
    {
        oss << " [ INFO ]: ";
        break;
    }
    case i::LL::DEBUG:
    {
        oss << " [ DEBUG ]: ";
        break;
    }
    case i::LL::WARNING:
    {
        oss << " [ WARNING ]: ";
        break;
    }
    case i::LL::ERROR:
    {
        oss << " [ ERROR ]: ";
        break;
    }
    case i::LL::VALIDATION:
    {
        oss << " [ VALID ]: ";
    }
    }

    i::GetState()->loggerMutex.lock();
    std::cout << oss.str().c_str() << msg << "\n" << std::flush;
    i::GetState()->loggerMutex.unlock();
#endif // _DEBUG
}

i::ProgrammState* i::GetState()
{
    static i::ProgrammState* state{new i::ProgrammState};
    return state;
}
