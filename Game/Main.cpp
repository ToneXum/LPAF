#include "Framework.hpp"
#include <exception>

void SomeFunction(int = 1)
{
    return;
}

// This is a simple test program
int main()
{
    try
    {
        tsd::Initialise();

        const char* windowName = "Boring Box";
        short handle = tsd::CreateWindow(windowName, 1000, 500);

        while (tsd::Running())
        {
            // simulate computation
            tsd::Halt(16);            
        }

        tsd::Uninitialise();

        return 0; // Wow, just wow. How can I forget this?
    }
    catch (const std::exception&)
    {
        return -1;
    }   

    return 0;
}