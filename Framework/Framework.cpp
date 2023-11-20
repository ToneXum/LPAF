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
        msg << "\n" << "This is an internal error likely caused by the framework itself, the application must quit now. If this ";
        msg << "keeps happening then notify the dev about his fuckup." << std::endl;
    }
    else
    {
        msg << "An Error occoured which even the error handler could not handle." << "\n\n";
        msg << "I guess I fucked up...";
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

    file << "[ " << __TIMESTAMP__ << " ]: " << "Unhandled Win32 error! " << e << " at " << line << std::endl;
    file << "[ " << __TIMESTAMP__ << " ]: " << "Fatal Error, application must abort!";

    file.close();

    std::exception exc;
    throw exc;
}

void in::SetLastError(int code)
{
    WindowInfo.lastErrorCode = code;
}

in::WindowData::~WindowData()
{
    if (msgThread->joinable())
    {
        msgThread->join();
    }
    delete msgThread;
}

void in::WindowData::MessageHandler()
{
    WIN32_EC_RET(hWnd, CreateWindowEx(
        0,
        in::WindowInfo.windowClassName,
        wnd->GetName(),
        WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, // this comment is here in memory of my stupidity
        wnd->GetWidth(), wnd->GetHeight(),
        nullptr,
        nullptr,
        in::WindowInfo.hInstance,
        nullptr
    ));
    in::WindowInfo.windowCount += 1;
    in::WindowInfo.windowsOpened += 1;

    ShowWindow(hWnd, 1);
    
    // work is done, let the main thread continue
    std::unique_lock<std::mutex> lock(in::WindowInfo.mtx);
    WindowInfo.windowIsFinished = true;
    lock.unlock();
    in::WindowInfo.cv.notify_one();

    // message pump for the window
    MSG msg = { };
    while (GetMessage(&msg, hWnd, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
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
        if (in::GetWindowData(hWnd)->wnd->GetId() == 1) // quit program if origin window is closed
        {
            WindowInfo.isRunning = false;
            PostQuitMessage(0);
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
        if (i->hWnd == handle)
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
        if (i->wnd->GetId() == id)
        {
            return i;
        }
    }
    return nullptr;
}

void tsd::Initialise(void)
{
    // Get hInstance since the program does not use the winMain entry point
    in::WindowInfo.hInstance = GetModuleHandle(0);

    WNDCLASSEX wc = {};

    wc.cbClsExtra       = 0;
    wc.cbSize           = sizeof(WNDCLASSEX);
    wc.cbWndExtra       = 0;
    wc.hbrBackground    = nullptr;
    wc.hCursor          = nullptr;
    wc.hIcon            = nullptr;
    wc.hIconSm          = nullptr;
    wc.hInstance        = in::WindowInfo.hInstance;
    wc.lpfnWndProc      = in::WindowProc;
    wc.lpszClassName    = in::WindowInfo.windowClassName;
    wc.lpszMenuName     = nullptr;
    wc.style            = 0;

    WIN32_EC_RET(in::WindowInfo.classAtom, RegisterClassEx(&wc));
}

void tsd::Uninitialise(void)
{
    WIN32_EC(UnregisterClass(in::WindowInfo.windowClassName, in::WindowInfo.hInstance));
}

tsd::Window::Window(const char* name, int width, int height)
    : id(in::WindowInfo.windowsOpened + 1),
      isVisible(true),
      name(const_cast<char*>(name)),
      width(width), height(height), xPos(0), yPos(0)
{
    in::WindowData* wndData = new in::WindowData;
    wndData->msgThread = new std::thread(&in::WindowData::MessageHandler, wndData);    
    wndData->wnd = this;

    // wait for window creation to finish
    std::unique_lock<std::mutex> lock(in::WindowInfo.mtx);
    in::WindowInfo.cv.wait(lock, []{ return in::WindowInfo.windowIsFinished; });

    in::WindowInfo.windows.push_back(wndData);

    in::WindowInfo.windowIsFinished = false;
}

int tsd::GetLastError()
{
    return in::WindowInfo.lastErrorCode;
}

const char* tsd::GetErrorInformation(int code)
{
    return e::errors.find(code) != e::errors.end() ? e::errors[code] : "Invalid error Code!"; // best one-liner so far
}

tsd::Window::~Window()
{
    delete in::GetWindowData(id);
    in::WindowInfo.windowCount -= 1;
}

tsd::Window* tsd::GetWindow(int id)
{
    for (in::WindowData* i : in::WindowInfo.windows)
    {
        if (i->wnd->GetId() == id)
        {
            return i->wnd;
        }
    }
    in::SetLastError(1); // item was now found
    return nullptr;
}

tsd::Window* tsd::GetWindow(const char* name)
{
    for (in::WindowData* i : in::WindowInfo.windows)
    {
        if (i->wnd->GetName() == name)
        {
            return i->wnd;
        }
    }
    in::SetLastError(1); // item was now found
    return nullptr;
}

void tsd::Window::ChangeName(const char* newName)
{
    SetWindowText(in::GetWindowData(id)->hWnd, newName);
}

unsigned int tsd::Window::GetId(void)
{
    return id;
}

char* tsd::Window::GetName(void)
{
    return name;
}

bool tsd::Window::GetVisibility(void)
{
    return isVisible;
}

int tsd::Window::GetWidth(void)
{
    return width;
}

int tsd::Window::GetHeight(void)
{
    return height;
}

int tsd::GetWindowCount(void)
{
    return in::WindowInfo.windowCount;
}

bool tsd::Running()
{
    return in::WindowInfo.isRunning;
}

void tsd::Halt(int ms)
{
    Sleep(ms);
}
