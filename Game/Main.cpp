#include "Framework.hpp"
#include <exception>
#include <ostream>
#include <string>

bool Troll()
{
    tsd::MessageBox(0, L"No...", L"No...", tsd::MF_TASKMODAL | tsd::MF_OK | tsd::MF_ICON_INFO);
    return false;
}

// This is a simple test program to try out and find bugs in GAFW
int main()
{
    TSD_CALL(tsd::Initialise(0, 105), true);

    short windowHandle = 0; const wchar_t* name = L"Boring Box";
    TSD_CALL_RET(windowHandle, tsd::CreateWindow(name, 500, 500), true);

    tsd::OnWindowCloseAttempt(windowHandle, Troll);

    while (tsd::Running())
    {
        // simulate computation
        tsd::Halt(16);
    }

    tsd::Uninitialise();
    return 0;
}