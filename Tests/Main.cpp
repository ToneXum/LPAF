#include "Framework.hpp"

int main()
{
    f::FrameworkInitData initData{};
    initData.appStyle   = f::AsNoIntegratedRenderer;
    f::Initialise(initData);

    f::NetworkingInitData netInitData{};
    f::InitialiseNetworking(netInitData);

    f::WindowCreateData cdt{};
    cdt.pName = L"Dependant"; cdt.height = 500; cdt.width = 500;
    f::WndH dependant = f::CreateWindowAsync(cdt);

    cdt.pName = L"Uninvited guest";
    f::WndH whoInvitedThisGuy = f::CreateWindowAsync(cdt);

    f::SocketCreateInfo socketCreateInfo{};
    socketCreateInfo.ipFamily       = f::IaIPv4;
    socketCreateInfo.port           = L"80";
    socketCreateInfo.hostName       = L"www.example.com";
    socketCreateInfo.internetProtocol = f::IpTransmissionControlProtocol;

    f::SockH socket = f::CreateSocket(socketCreateInfo);
    f::ConnectSocket(socket);

    const char httpRequest[] = "GET / HTTP/1.1\r\nHost: www.example.com\r\n\r\n";
    f::Send(socket, httpRequest, sizeof(httpRequest)); // I did it again, no wonder only 8 bytes get sent XD

    char buffer[4096]{};
    f::Receive(socket, buffer, sizeof(buffer));

    uint8_t counter = 0;
    f::SetTextInputState(true);

    goto bruh;

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
    bruh:

    f::DestroySocket(socket);

    f::UnInitialiseNetworking();
    f::UnInitialise();
    return 0;
}
