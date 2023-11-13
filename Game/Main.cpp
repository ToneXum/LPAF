#include "Framework.hpp"

int WINAPI wWinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    PWSTR pCmdLine,
    int nCmdShow)
{
    tsd::WindowClass::Get();
    return 0;
}