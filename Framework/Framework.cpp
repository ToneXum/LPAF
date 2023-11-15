#include "Framework.hpp"
#include "Internals.hpp"

void CreateWin32Error(int line)
{
    MessageBox(nullptr, "An error occoured, the application must quit now", "Error!", MB_ICONERROR | MB_TASKMODAL | MB_OK);
}

// Pumps the windows messages from the queue
void MessageHandler(void)
{
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

// Get window by window handle

LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
        WindowData window = GetWindow(hWnd);
        if (window.wnd->GetId() == 1) // quit program if origin window is closed
        {
            PostQuitMessage(0);
            return 0;
        }
        break;
    }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

WindowData GetWindow(HWND handle)
{
    for (WindowData i : WindowInfo.windows)
    {
        if (i.hWnd == handle)
        {
            return i;
        }
    }
    return (WindowData)0;
}

void tsd::Initialise(void)
{
    // Get hInstance since the program does not use the winMain entry point
    WindowInfo.hInstance = GetModuleHandle(0);

    WNDCLASSEX wc = {};

    wc.cbClsExtra       = 0;
    wc.cbSize           = sizeof(WNDCLASSEX);
    wc.cbWndExtra       = 0;
    wc.hbrBackground    = nullptr;
    wc.hCursor          = nullptr;
    wc.hIcon            = nullptr;
    wc.hIconSm          = nullptr;
    wc.hInstance        = WindowInfo.hInstance;
    wc.lpfnWndProc      = WindowProc;
    wc.lpszClassName    = WindowInfo.windowClassName;
    wc.lpszMenuName     = nullptr;
    wc.style            = 0;

    WIN32_EC_RET(WindowInfo.classAtom, RegisterClassEx(&wc));

    //static std::thread MessageThread(MessageHandler);
    //WindowInfo.msgThread = &MessageThread;
}

void tsd::Uninitialise(void)
{
    PostQuitMessage(0);
    //WindowInfo.msgThread->join();
    UnregisterClass(WindowInfo.windowClassName, WindowInfo.hInstance);
}

tsd::Window::Window(const char* name, int width, int height)
    : id(WindowInfo.windowsOpened + 1),
      isVisible(true),
      name(const_cast<char*>(name))
{
    HWND hWnd;
    
    WIN32_EC_RET(hWnd, CreateWindowEx(
        0,
        WindowInfo.windowClassName,
        name,
        WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU,
        100, 100, // to do: place in the middle of screen by getting resolution
        width, height,
        nullptr,
        nullptr,
        WindowInfo.hInstance,
        nullptr
    ));
    WindowInfo.windowCount += 1;
    WindowInfo.windowsOpened += 1;

    WindowData wndData{ this, hWnd };
    WindowInfo.windows.push_back(wndData);
    ShowWindow(hWnd, 1);
}

tsd::Window::~Window()
{
    WindowInfo.windowCount -= 1;
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
