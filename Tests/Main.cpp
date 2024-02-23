#include "Framework.hpp"
#include "resource.h"

int main()
{
    f::Initialize(IDI_ICON1, IDC_CURSOR1); // Intellisense? Are you good?

    f::WindowCreateData cdt{};
    cdt.pName = L"Dependant"; cdt.height = 500; cdt.width = 500;
    f::WND_H dependant = f::CreateWindow(cdt);

    cdt.pName = L"";
    f::WND_H windowHandle = f::CreateWindow(cdt);

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