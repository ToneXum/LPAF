#include "Framework.hpp"

int main()
{
    f::FrameworkInitData initData{};
    initData.appStyle   = f::AsNoIntegratedRenderer;
    f::Initialise(initData);

    f::InitialiseNetworking();

    f::WindowCreateData cdt{};
    cdt.pName = L"Dependant"; cdt.height = 500; cdt.width = 500;
    f::WndH dependant = f::CreateWindowAsync(cdt);

    cdt.pName = L"Uninvited guest";
    f::WndH whoInvitedThisGuy = f::CreateWindowSync(cdt);

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

    f::DestroySocket(socket);

    // TODO: make this a full unit-test system

    f::CloseAllWindowsForce();

    f::UnInitialiseNetworking();
    f::UnInitialise();
    return 0;
}
