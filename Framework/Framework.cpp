#include "Framework.hpp"

LRESULT tsd::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

GRESULT tsd::Initialise(void)
{
    return 0;
}

GRESULT tsd::Uninitialise(void)
{
    return 0;
}
