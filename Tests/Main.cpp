#include "../Framework/Framework.hpp"

int main()
{
    f::Initialize(0, 0); // Intellisense? Are you good?

    f::WindowCreateData cdt{};
    cdt.pName = L"Dependant"; cdt.height = 500; cdt.width = 500;
    f::WndH dependant = f::CreateWindowAsync(cdt);

    cdt.pName = L"Uninvited guest";
    f::WndH whoInvitedThisGuy = f::CreateWindowAsync(cdt);

    f::SetTextInputState(true);
    while (f::Running())
    {        
        f::ChangeWindowName(dependant, f::GetTextInput());
        
        if (f::IsKeyPressedOnce(f::KeyEnter))
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

        if (f::IsKeyPressedOnce(f::KeyEscape))
            f::CloseWindow(whoInvitedThisGuy);

        // simulate computation
        f::Halt(16);
    }

    f::UnInitialize();
    return 0;
}