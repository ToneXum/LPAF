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

        tsd::SetTextInputState(true);
        tsd::WindowChangeName(windowHandle, L"Hello my man");

        while (tsd::Running())
        {
            //if (tsd::IsKeyPressed(tsd::Key::ENTER))
            //    tsd::SetTextInputState(tsd::IsTextInputEnabled() ? false : true);

            tsd::WindowChangeName(windowHandle, tsd::GetTextInput());
            wchar_t* name = tsd::GetTextInput();

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