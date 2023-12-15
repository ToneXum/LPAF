#include "Framework.hpp"
#include <exception>

// This is a simple test program
int main()
{
    try
    {
        TSD_CALL(tsd::Initialise(0, 105), true);

        const char* windowName = "Boring Box";

        while (tsd::Running())
        {
            // simulate computation
            tsd::Halt(16);
            TSD_CALL(tsd::CreateWindow(windowName, 500, 500), true)
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