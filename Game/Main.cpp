#include "Framework.hpp"
#include <ostream>
#include <string>
#include "resource.h"

#include "vulkan/vulkan.h"
#include "Windows.h"

#undef CreateWindow

// This is a simple test program to try out and find bugs in GAFW
int main()
{
    tsd::Initialise(IDI_ICON1, IDC_CURSOR1); // Intellisense? Are you good?

    short dependant = tsd::CreateWindow(L"Dependant", 500, 500, 0, 0, nullptr, 0);

    short windowHandle = tsd::CreateWindow(L"Boring Box", 500, 500, 0, 0, nullptr, 0);

    while (tsd::Running())
    {
        if (tsd::IsValidHandle(windowHandle))
            tsd::ChangeWindowName(windowHandle, tsd::IsKeyPressed(tsd::K_E) ? L"E is pressed" : L"E is not pressed");
        // simulate computation
        tsd::Halt(16);
    }

    tsd::Uninitialise();
    return 0;
}