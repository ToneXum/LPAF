#include "Framework.hpp"
#include <exception>

// This is a simple test program
int main()
{
    try
    {
        bool state = tsd::Initialise(104, 105);

        const char* windowName = "Boring Box";
        short handle1 = tsd::CreateWindow(windowName, 1000, 500);
        short handle2 = tsd::CreateWindow(windowName, 600, 1000);

        while (tsd::Running())
        {
            // simulate computation
            tsd::Halt(16);            
        }

        tsd::Uninitialise();
    }
    catch (const std::exception&)
    {
        tsd::Uninitialise();
        return -1;
    }

    return 0;
}