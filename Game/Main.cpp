#include "Framework.hpp"
#include <ostream>
#include <string>
#include "resource.h"

// This is a simple test program to try out and find bugs in GAFW
int main()
{
    TSD_CALL(tsd::Initialise(IDI_ICON1, IDC_CURSOR1), true); // Intellisense? Are you good?

    short handle1 = 0;
    TSD_CALL_RET(handle1, tsd::CreateWindow(L"Dependant1", 500, 500, 0, 0, nullptr, 0), true);

    short handle2 = 0;
    TSD_CALL_RET(handle2, tsd::CreateWindow(L"Dependant2", 500, 500, 0, 0, nullptr, 0), true);

    short windowHandle; short dep[] = {handle1, handle2};
    TSD_CALL_RET(windowHandle, tsd::CreateWindow(L"Boring Box", 500, 500, 0, 0, dep, sizeof(dep) / sizeof(short)), true);

    while (tsd::Running())
    {
        // simulate computation
        tsd::Halt(16);
    }

    tsd::Uninitialise();
    return 0;
}