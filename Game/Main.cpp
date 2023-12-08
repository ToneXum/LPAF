#include "Framework.hpp"
#include <exception>

// This is a simple test program
int main()
{
    try
    {
        TSD_CALL(tsd::Initialise(0, 105), true);

        const char* windowName = "Boring Box";

        short handle1;
        TSD_CALL_RET(handle1, tsd::CreateWindow(windowName, 1000, 500), true);

        short handle2;
        TSD_CALL_RET(handle2, tsd::CreateWindow(windowName, 600, 1000), true);

        while (tsd::Running())
        {
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