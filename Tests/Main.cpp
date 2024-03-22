#include "Framework.hpp"

int main()
{
    f::FrameworkInitData initData{};
    initData.appStyle   = f::AsNoCloseButton;
    f::Initialise(initData);

    f::NetworkingInitData netInitData{};

    f::WindowCreateData cdt{};
    cdt.pName = L"Dependant"; cdt.height = 500; cdt.width = 500;
    f::WndH dependant = f::CreateWindowAsync(cdt);

    cdt.pName = L"Uninvited guest";
    f::WndH whoInvitedThisGuy = f::CreateWindowAsync(cdt);

    uint8_t counter = 0;

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

        if (f::IsKeyPressedOnce(f::KeyH))
        {
            f::SetWindowVisibility(whoInvitedThisGuy, f::WvHide);
            counter++;
        }

        if (counter == 128)
        {
            f::SetWindowVisibility(whoInvitedThisGuy, f::WvNormalNoActivate);
            counter = 0;
        }
        if (counter > 0)
            counter++;

        // simulate computation
        f::Halt(16);
    }

    f::UnInitialise();
    return 0;
}