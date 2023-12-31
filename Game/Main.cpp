#include "Framework.hpp"
#include <exception>
#include <ostream>
#include <string>

// This is a simple test program to try out and find bugs in GAFW
int main()
{
    try
    {
        TSD_CALL(tsd::Initialise(0, 105), true);

        short windowHandle = 0;
        TSD_CALL_RET(windowHandle, tsd::CreateWindow(L"Boring Box", 500, 500), true);

        short windowHandle2 = 0;
        TSD_CALL_RET(windowHandle2, tsd::CreateWindow(L"Even more boring box", 1000, 400), true)

        std::wostringstream oss;
        while (tsd::Running())
        {
            oss.str(L"");
            tsd::MouseInfo msInfo = tsd::GetMouseInfo();
            oss << "Position: " << msInfo.xPos << ", " << msInfo.yPos << " Button states: ";
            oss << msInfo.left << ", " << msInfo.middle << ", " << msInfo.right << ", " << msInfo.x1 << ", " << msInfo.x2;
            oss << " Mouse: " << msInfo.containingWindow;

            tsd::ChangeWindowName(windowHandle2, oss.str().c_str());

            // simulate computation
            tsd::Halt(16);
        }

        tsd::Uninitialise();
        return 0;
    }
    catch (const std::exception&)
    {
        tsd::Uninitialise();
        return -1;
    }
}