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

#include <Windows.h>

#include <vector>
#include <iostream>

typedef unsigned int GRESULT;

namespace tsd // tonexum software division
{
    // Start the entirety of this framework up so it can be used.
    GRESULT Initialise(void);

    // commit self delete :)
    GRESULT Uninitialise(void);

    // window procedure
    LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // A window class to represent an open window
    class Window
    {
    public:
        Window();
        ~Window();

    private:
        unsigned int id;
        wchar_t name;
    };

    struct
    {
    public:
        unsigned int GetWindowCount();
        char* GetRegisteredClassName();

    private:
        std::vector<Window> windows{};
        unsigned int windowCount;
        const char* className;
        HINSTANCE hInstance{};
    } WindowInfo;
}
