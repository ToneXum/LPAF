#include "Framework.hpp"

//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
int main()
{
    tsd::Initialise();

    const char* windowName = "Boring Box";
    tsd::Window wnd(windowName, 900, 600);
    tsd::Window wind(windowName, 1000, 500);

    while (tsd::Running())
    {
        // simulate computation
        tsd::Halt(16);
    }
    
    tsd::Uninitialise();

    return 0;
}