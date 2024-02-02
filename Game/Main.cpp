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

    tsd::WindowCreateData cdt{};
    cdt.name = L"Dependant"; cdt.height = 500; cdt.width = 500;
    tsd::WND_H dependant = tsd::CreateWindow(cdt);

    cdt.name = L"This is not even visible";
    tsd::WND_H windowHandle = tsd::CreateWindow(cdt);

    cdt.name = L"Uninvited guest";
    tsd::WND_H whoInvitedThisGuy = tsd::CreateWindow(cdt);

    while (tsd::Running())
    {
        if (tsd::IsValidHandle(windowHandle))
            tsd::ChangeWindowName(windowHandle, tsd::IsKeyPressed(tsd::K_E) ? L"E is pressed" : L"E is not pressed");
        
        if (tsd::IsValidHandle(dependant))
            tsd::ChangeWindowName(dependant, tsd::WindowContainsMouse(dependant) ? L"I have the mouse" : L"I dont have the mouse");
        // simulate computation
        tsd::Halt(16);
    }

    tsd::Uninitialise();
    return 0;
}