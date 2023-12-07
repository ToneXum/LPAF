#include "Framework.hpp"
#include <exception>

// This is a simple test program
int main()
{
    try
    {
        tsd::Initialise();

        const char* windowName = "Boring Box";
        short handle1 = tsd::CreateWindow(windowName, 1000, 500);
        short handle2 = tsd::CreateWindow(windowName, 600, 1000);

        tsd::CreateMessageBox(handle1, "Yank", "Hehehehaw", tsd::MBF::TASKMODAL | tsd::MBF::BUTTON_OK | tsd::MBF::ICON_INFO);

        while (tsd::Running())
        {
            // simulate computation
            tsd::Halt(16);            
        }

        tsd::Uninitialise();
    }
    catch (const std::exception&)
    {
        return -1;
    }

    return 0;
}