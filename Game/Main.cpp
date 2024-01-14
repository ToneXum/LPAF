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
    TSD_CALL(tsd::Initialise(IDI_ICON1, IDC_CURSOR1), true); // Intellisense? Are you good?

    short dependant;
    TSD_CALL_RET(dependant, tsd::CreateWindow(L"Dependant", 500, 500, 0, 0, nullptr, 0), true);

    short windowHandle; short dep[] = { dependant };
    TSD_CALL_RET(windowHandle, tsd::CreateWindow(L"Boring Box", 500, 500, 0, 0, dep, 1), true);

    while (tsd::Running())
    {
        tsd::ChangeWindowName(windowHandle, tsd::IsKeyPressed(tsd::K_E) ? L"E is pressed" : L"E is not pressed");
        // simulate computation
        tsd::Halt(16);
    }

    tsd::Uninitialise();
    return 0;
}