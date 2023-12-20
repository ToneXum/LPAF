#include "Framework.hpp"
#include <exception>

// This is a simple test program
int main()
{
    try
    {
        TSD_CALL(tsd::Initialise(0, 105), true);

        short windowHandle = 0;
        TSD_CALL_RET(windowHandle, tsd::CreateWindow(nullptr, 500, 500), true);

        short windowHandle2 = 0;
        TSD_CALL_RET(windowHandle2, tsd::CreateWindow("Even more Boring Box", 1000, 600, -1000, 200), true);

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