#include "Framework.hpp"
#include <exception>
#include <ostream>

// This is a simple test program
int main()
{
    try
    {
        TSD_CALL(tsd::Initialise(0, 105), true);

        short windowHandle = 0;
        TSD_CALL_RET(windowHandle, tsd::CreateWindow(L"Boring Box", 500, 500), true);

        short windowHandle2 = 0;
        TSD_CALL_RET(windowHandle2, tsd::CreateWindow(L"Even more boring box", 1000, 400), true)

        while (tsd::Running())
        {
            if (!tsd::IsTextInputEnabled())
            {
                tsd::WindowChangeName(windowHandle, L"Text input is not enabled");
                if (tsd::IsKeyPressed(tsd::Key::ENTER))
                    tsd::SetTextInputState(true);
            }
            else
            {
                tsd::WindowChangeName(windowHandle, tsd::GetTextInput());
                if (tsd::IsKeyPressed(tsd::Key::ENTER))
                    tsd::SetTextInputState(false);
            }
            
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