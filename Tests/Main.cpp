#include "../Framework/Framework.hpp"

int main()
{
    f::Initialize(0, 0); // Intellisense? Are you good?

    f::WindowCreateData cdt{};
    cdt.pName = L"Dependant"; cdt.height = 500; cdt.width = 500;
    f::WND_H dependant = f::CreateWindow(cdt);

    cdt.pName = L"Uninvited guest";
    f::WND_H whoInvitedThisGuy = f::CreateWindow(cdt);

    f::SetTextInputState(true);
    while (f::Running())
    {        
        f::ChangeWindowName(dependant, f::GetTextInput());
        
        if (f::IsKeyPressedOnce(f::K_ENTER))
        {
            if (f::GetTextInputState())
            {
                f::SetTextInputState(false);
            }
            else
            {
                f::SetTextInputState(true);
            }
        }

        if (f::IsKeyPressedOnce(f::K_ESCAPE))
            f::CloseWindow(whoInvitedThisGuy);

        // simulate computation
        f::Halt(16);
    }
    
    f::Uninitialize();
    return 0;
}