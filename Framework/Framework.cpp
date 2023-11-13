#include "Framework.hpp"

GRESULT tsd::Initialise(void)
{
    return 0;
}

GRESULT tsd::Uninitialise(void)
{
    return 0;
}

tsd::WindowClass& tsd::WindowClass::Get()
{
    static WindowClass wndClass;
    return wndClass;
}

tsd::WindowClass::WindowClass()
    : windowCount(0),
      className("GGFW Window Class")
{
    //WNDCLASSEX wc = {};
    //wc.cbClsExtra       = 0;
    //wc.cbSize           = sizeof(WNDCLASSEX);
    //wc.cbWndExtra       = 0;
    //wc.hbrBackground    = nullptr;
    //wc.hCursor          = nullptr;      // custom cursor, maybe something for the future
    //wc.hIcon            = nullptr;      // custom window icon, again something for the future
    //wc.hIconSm          = nullptr;      // custon window icon but small
    //wc.hInstance        = hInstance;
    //wc.lpfnWndProc      = tsd::WindowProc;
    //wc.lpszClassName    = className;
    //wc.lpszMenuName     = nullptr;
    //wc.style            = 0;
}

tsd::WindowClass::~WindowClass()
{
   std::cout << "I have been cleaned up!";
   //UnregisterClass(className, hInstance);
}
