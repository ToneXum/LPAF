#include "Framework.hpp"
#include <ostream>
#include <string>
#include "resource.h"

#include "vulkan/vulkan.h"

#undef CreateWindow

// This is a simple test program to try out and find bugs in GAFW
int main()
{
    TSD_CALL(tsd::Initialise(IDI_ICON1, IDC_CURSOR1), true); // Intellisense? Are you good?

    short windowHandle;
    TSD_CALL_RET(windowHandle, tsd::CreateWindow(L"Boring Box", 500, 500, 0, 0, nullptr, 0), true);

    
    
    while (tsd::Running())
    {
        // simulate computation
        tsd::Halt(16);
    }

    tsd::Uninitialise();
    return 0;
}