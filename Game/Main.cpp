#include "Framework.hpp"

#include <Windows.h>
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
//int main()
{
    tsd::Initialise();

    const char* windowName = "Boring Box";
    tsd::Window wnd(windowName, 900, 600);
    tsd::Window wind(windowName, 400, 600);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    tsd::Uninitialise();

    return 0;
}