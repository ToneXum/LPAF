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

void in::CreateWin32DebugError(int line)
{
    int e = GetLastError();
    std::ostringstream msg;
    char* eMsg = nullptr;

    FormatMessage(
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

    MessageBox(nullptr, msg.str().c_str(), "Internal Error!", MB_ICONERROR | MB_TASKMODAL | MB_OK);
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

    MessageBox(nullptr, msg.str().c_str(), "Critical Error!", MB_TASKMODAL | MB_OK | MB_ICONERROR);

    std::exception exc;
    throw exc;
}

void in::SetLastError(int code)
{
    WindowInfo.lastErrorCode = code;
}

void in::WindowData::MessageHandler()
{
    WIN32_EC_RET(hWnd, CreateWindowEx(
        0,
        in::WindowInfo.windowClassName,
        name,
        WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU,
        !xPos ? CW_USEDEFAULT : xPos, !yPos ? CW_DEFAULT : yPos, // man do I love tenary expression :)
        width, height,
        nullptr,
        nullptr,
        in::WindowInfo.hInstance,
        nullptr
    ));
    in::WindowInfo.windowCount += 1;
    in::WindowInfo.windowsOpened += 1;

    id = in::WindowInfo.windowsOpened; 
    
    ShowWindow(hWnd, 1);
    
    // work is done, let the main thread continue
    std::unique_lock<std::mutex> lock1(in::WindowInfo.mtx);
    WindowInfo.windowIsFinished = true;
    lock1.unlock();
    in::WindowInfo.cv.notify_one();

    // message pump for the window
    MSG msg = { };
    while (GetMessage(&msg, hWnd, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Window is closed, update window information
    WindowInfo.windowCount -= 1;
    isValid = false;
}

LRESULT in::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE: // closing of a window has been requested
    {
        DestroyWindow(hWnd);
        break;
    }
    case WM_DESTROY: // closing a window was ordered and confirmed
    {
        if (tsd::WindowGetCount() == 1) // quit program if last window remaining is closed
        {
            WindowInfo.isRunning = false;
            return 0;
        }
        break;
    }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

in::WindowData* in::GetWindowData(HWND handle)
{
    for (WindowData* i : WindowInfo.windows)
    {
        if ((i->hWnd == handle) && (i->isValid))
        {
            return i;
        }
    }
    return nullptr;
}

in::WindowData* in::GetWindowData(int id)
{
    for (WindowData* i : WindowInfo.windows)
    {
        if ((i->id == id) && (i->isValid))
        {
            return i;
        }
    }
    return nullptr;
}

bool tsd::Initialise(int iconId, int cursorId)
{
    bool success = true;

    // Get hInstance since the program does not use the winMain entry point
    in::WindowInfo.hInstance = GetModuleHandle(0);

    if (iconId)
    {
        in::WindowInfo.hIcon = LoadIcon(in::WindowInfo.hInstance, MAKEINTRESOURCE(iconId));
        if (in::WindowInfo.hIcon == 0) { in::SetLastError(5); success = false; }
    }

    if (cursorId)
    {
        in::WindowInfo.hCursor = LoadCursor(in::WindowInfo.hInstance, MAKEINTRESOURCE(cursorId));
        if (!in::WindowInfo.hCursor) { in::SetLastError(6); success = false; }
    }
    
    WNDCLASSEX wc = {};

    wc.cbClsExtra       = 0;
    wc.cbSize           = sizeof(WNDCLASSEX);
    wc.cbWndExtra       = 0;
    wc.hbrBackground    = nullptr;
    wc.hCursor          = in::WindowInfo.hCursor;
    wc.hIcon            = in::WindowInfo.hIcon;
    wc.hIconSm          = in::WindowInfo.hIcon;
    wc.hInstance        = in::WindowInfo.hInstance;
    wc.lpfnWndProc      = in::WindowProc;
    wc.lpszClassName    = in::WindowInfo.windowClassName;
    wc.lpszMenuName     = nullptr;
    wc.style            = 0;

    WIN32_EC_RET(in::WindowInfo.classAtom, RegisterClassEx(&wc));

    in::WindowInfo.isInitialised = true;

    return success;
}

void tsd::Uninitialise(void)
{
    for (in::WindowData* w : in::WindowInfo.windows)
        w->msgThread->join();

    WIN32_EC(UnregisterClass(in::WindowInfo.windowClassName, in::WindowInfo.hInstance));

    for (in::WindowData* w : in::WindowInfo.windows)
    {
        delete w->msgThread;
        delete w;
    }
}

// Whoops
#undef CreateWindow

short tsd::CreateWindow(const char* name, int width, int height, int xPos, int yPos)
{
    if (!in::WindowInfo.isInitialised) { in::SetLastError(2); return 0; }
    if (!name) { in::SetLastError(3); return 0; }
    if ((height <= 0) || (width <= 0)) { in::SetLastError(3); return 0; }

    in::WindowData* wndData = new in::WindowData;
    wndData->msgThread = new std::thread(&in::WindowData::MessageHandler, wndData);

    wndData->name       = const_cast<char*>(name);
    wndData->width      = width;
    wndData->height     = height;
    wndData->xPos       = xPos;
    wndData->yPos       = yPos;
    wndData->isValid    = true;

    // wait for window creation to finish
    std::unique_lock<std::mutex> lock(in::WindowInfo.mtx);
    in::WindowInfo.cv.wait(lock, [] { return in::WindowInfo.windowIsFinished; });

    in::WindowInfo.windows.push_back(wndData);

    in::WindowInfo.windowIsFinished = false;

    return wndData->id;
}

void tsd::CreateAutoDebugError(int line, bool quit)
{
    std::ostringstream msg;
    msg << "Error " << tsd::GetLastError() << " has occoured at line " << line << ".\n\n";
    msg << tsd::GetErrorInformation(tsd::GetLastError()) << "\n\n";
    if (quit) { msg << "The application must quit now."; }
    msg << std::endl;

    MessageBox(nullptr, msg.str().c_str(), "Error!", MB_TASKMODAL | MB_OK | MB_ICONERROR);

    if (quit)
    {
    std::exception exc;
    throw exc;
    }
}

void tsd::CreateAutoReleaseError(int line, bool quit)
{
    // todo
}

int tsd::GetLastError()
{
    return in::WindowInfo.lastErrorCode;
}

const char* tsd::GetErrorInformation(int code)
{
    return in::errors.find(code) != in::errors.end() ? in::errors[code] : "Invalid error Code!"; // best one-liner so far
}

char* tsd::WindowGetName(short id)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) { in::SetLastError(4); return nullptr; }
    return wndData->name;
}

bool tsd::WindowGetVisibility(short id)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) { in::SetLastError(4); return false; }
    return wndData->isVisible;
}

int tsd::WindowGetWidth(short id)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) { in::SetLastError(4); return false; }
    return wndData->width;
}

int tsd::WindowGetHeight(short id)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) { in::SetLastError(4); return false; }
    return wndData->height;
}

std::pair<int, int> tsd::WindowGetDimensions(short id)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) { in::SetLastError(4); return {0, 0}; }
    return {wndData->width, wndData->height};
}

int tsd::WindowGetXPos(short id, WPR wpr)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) { in::SetLastError(4); return 0; }
    RECT rect{};
    GetWindowRect(wndData->hWnd, &rect);

    switch (wpr)
    {
    case LEFT:
        return rect.left;
    case RIGHT:
        return rect.right;
    }

    in::SetLastError(3);
    return 0;
}

int tsd::WindowGetYPos(short id, WPR wpr)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) { in::SetLastError(4); return 0; }
    RECT rect{};
    GetWindowRect(wndData->hWnd, &rect);

    switch (wpr)
    {
    case TOP:
        return rect.top;
    case BOTTOM:
        return rect.bottom;
    }

    in::SetLastError(3);
    return 0;
}

std::pair<int, int> tsd::WindowGetPosition(short id, WPR wpr)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) { in::SetLastError(4); return {0, 0}; }
    RECT rect{};
    GetWindowRect(wndData->hWnd, &rect);
    
    switch (wpr)
    {
    case TOP_LEFT:
        return { rect.left, rect.top };
    case TOP_RIGHT:
        return { rect.right, rect.top };
    case BOTTOM_LEFT:
        return { rect.left, rect.bottom };
    case BOTTOM_RIGHT:
        return { rect.right, rect.bottom };
    }

    in::SetLastError(3);
    return {0, 0};
}

int tsd::WindowGetCount(void)
{
    return in::WindowInfo.windowCount;
}

bool tsd::WindowChangeName(short id, const char* name)
{
    in::WindowData* wndData = in::GetWindowData(id);
    if (!wndData) { in::SetLastError(4); return false; }
    SetWindowText(wndData->hWnd, name);
    wndData->name = const_cast<char*>(name);
    return true;
}

bool tsd::IsValidHandle(short handle)
{
    if (in::GetWindowData(handle))
        return true;
    return false;
}

bool tsd::Running()
{
    return in::WindowInfo.isRunning;
}

void tsd::Halt(int ms)
{
    Sleep(ms);
}

#undef MessageBox
#undef IGNORE
MBR tsd::MessageBox(short owner, const char* title, const char* msg, int flags)
{
    // return null if the window is not found so I dont care
    in::WindowData* ownerData = in::GetWindowData(owner);

    long rawFlags = 0;

    // Where switch statement?
    // Cant put (non-constant) expressions into switch cases
    if (flags & MBF::TASKMODAL)
        rawFlags = rawFlags | MB_TASKMODAL;

    if (flags & MBF::ICON_WARNING)
        rawFlags = rawFlags | MB_ICONWARNING;
    if (flags & MBF::ICON_ERROR)
        rawFlags = rawFlags | MB_ICONERROR;
    if (flags & MBF::ICON_INFO)
        rawFlags = rawFlags | MB_ICONINFORMATION;
    if (flags & MBF::ICON_QUESTION)
        rawFlags = rawFlags | MB_ICONQUESTION;
    
    if (flags & MBF::BUTTON_OK)
        rawFlags = rawFlags | MB_OK;
    if (flags & MBF::BUTTON_OK_CANCEL)
        rawFlags = rawFlags | MB_OKCANCEL;
    if (flags & MBF::BUTTON_YES_NO)
        rawFlags = rawFlags | MB_YESNO;
    if (flags & MBF::BUTTON_RETRY_CANEL)
        rawFlags = rawFlags | MB_RETRYCANCEL;

    if (flags & MBF::BUTTON_YES_NO_CANCEL)
        rawFlags = rawFlags | MB_YESNOCANCEL;
    if (flags & MBF::BUTTON_ABORT_RETRY_IGNORE)
        rawFlags = rawFlags | MB_ABORTRETRYIGNORE;
    if (flags & MBF::BUTTON_CANCEL_RETRY_CONTINUE)
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
