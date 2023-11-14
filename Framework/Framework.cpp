
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

#include <Windows.h>

#include <vector>
#include <iostream>
#include <thread>
#include <sstream>

#include "Framework.hpp"

void CreateWin32Error(int line)
{
    MessageBox(nullptr, "An error occoured, the application must quit now", "Error!", MB_ICONERROR | MB_TASKMODAL | MB_OK);
}

// Error check for Win32 API calls
#define WIN32_EC(x) { if (!x) { CreateWin32Error(__LINE__); } }

// Error check for Win32 API calls but the return value is saved
#define WIN32_EC_RET(x, y) { x = y; if (!x) { CreateWin32Error(__LINE__); } } // 

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

struct
{
    std::vector<tsd::Window> windows{};
    const char* windowClassName{"GGFW Window Class"};
    HINSTANCE hInstance{0};
    ATOM classAtom{0};
    int windowCount{0};
    std::thread* msgThread;
} WindowInfo;

LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void tsd::Initialise(void)
{
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
    : id(WindowInfo.windowCount + 1),
      isVisible(true),
      name(const_cast<char*>(name))
{
    WIN32_EC_RET(HWND(hWnd), CreateWindowEx(
        0,
        WindowInfo.windowClassName,
        name,
        WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU,
        CW_DEFAULT, CW_DEFAULT,
        width, height,
        nullptr,
        nullptr,
        WindowInfo.hInstance,
        nullptr
    ));
    WindowInfo.windowCount += 1;

    ShowWindow(HWND(hWnd), 1);
}

tsd::Window::~Window()
{

}
