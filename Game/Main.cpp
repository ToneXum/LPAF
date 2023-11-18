#include "Framework.hpp"
#include <exception>

int main()
{
    try
    {
        tsd::Initialise();

        const char* windowName = "Borin Box";
        tsd::Window wnd(windowName, 1000, 500);

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