#include "Framework.hpp"
#include "Internals.hpp"

void in::CreateWin32Error(int line)
{
    int e = GetLastError();
    MessageBox(nullptr, "An error occoured, the application must quit now", "Error!", MB_ICONERROR | MB_TASKMODAL | MB_OK);
}

// Pumps the windows messages from the queue
void in::MessageHandler(tsd::Window* wnd, HWND* hWnd)
{
    WIN32_EC_RET(*hWnd, CreateWindowEx(
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

    ShowWindow(*hWnd, 1);
    
    // work is done, let the main thread continue
    std::unique_lock<std::mutex> lock(in::WindowInfo.mtx);
    WindowInfo.windowIsFinished = true;
    lock.unlock();
    in::WindowInfo.cv.notify_one();

    // message pump for the window
    MSG msg = { };
    while (GetMessage(&msg, *hWnd, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

std::thread& in::WindowData::GetThread(tsd::Window* wnd, HWND* hWnd)
{
    static std::thread trd(MessageHandler, wnd, hWnd);
    return trd;
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
        WindowData window = *GetWindowData(hWnd);
        if (window.wnd->GetId() == 1) // quit program if origin window is closed
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
    for (WindowData i : WindowInfo.windows)
    {
        if (i.hWnd == handle)
        {
            return &i;
        }
    }
    return nullptr;
}

in::WindowData* in::GetWindowData(int id)
{
    for (WindowData i : WindowInfo.windows)
    {
        if (i.wnd->GetId() == id)
        {
            return &i;
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
    in::WindowData wd = *in::GetWindowData(1);
    wd.GetThread(wd.wnd, &wd.hWnd).join(); // join the thread of the origin window, all other must be closed
    WIN32_EC(UnregisterClass(in::WindowInfo.windowClassName, in::WindowInfo.hInstance));
}

int tsd::GetWindowCount(void)
{
    return in::WindowInfo.windowCount;
}

tsd::Window* tsd::GetWindow(int id)
{
    for (in::WindowData i : in::WindowInfo.windows)
    {
        if (i.wnd->GetId() == id)
        {
            return i.wnd;
        }
    }
    return reinterpret_cast<Window*>(0); // bruh
}

tsd::Window* tsd::GetWindow(const char* name)
{
    for (in::WindowData i : in::WindowInfo.windows)
    {
        if (i.wnd->GetName() == name)
        {
            return i.wnd;
        }
    }
    return reinterpret_cast<Window*>(0); // bruh
}

tsd::Window::Window(const char* name, int width, int height)
    : id(in::WindowInfo.windowsOpened + 1),
      isVisible(true),
      name(const_cast<char*>(name)),
      width(width), height(height), xPos(0), yPos(0)
{
    HWND hWnd{};
    in::WindowData wndData{};
    wndData.GetThread(this, &hWnd); // start the thread to construct the window
    wndData.wnd = this;

    // guard the getting of the handle
    std::unique_lock<std::mutex> lock(in::WindowInfo.mtx);
    in::WindowInfo.cv.wait(lock, []{ return in::WindowInfo.windowIsFinished; });

    wndData.hWnd = hWnd;
    in::WindowInfo.windows.push_back(wndData);
}

tsd::Window::~Window()
{
    in::WindowInfo.windowCount -= 1;
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

bool tsd::Running()
{
    return in::WindowInfo.isRunning;
}

void tsd::Halt(int ms)
{
    Sleep(ms);
}
