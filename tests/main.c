// LPAF - lightweight and performant application framework
// Copyright (C) 2024 ToneXum (Toni Stein)
//
// This program is free software: you can redistribute it and/or modify it under the terms of the
// GNU General Public License as published by the Free Software Foundation, either version 3 of
// the License, or any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program. If
// not, see <https://www.gnu.org/licenses/>.

#include <stdio.h>
#include <string.h>

#include "framework.h"
#include "tests.h"

//#define SERVER
#define CLIENT

#ifdef SERVER
int main(int argc, char* argv[]) {
    fwStartModule(fwModuleNetwork, 0);

    fwSocket socket;
    TST(fwSocketCreate(fwSocketAddressFamilyIPv4, fwSocketProtocolStream, &socket));

    struct fwSocketAddress localAddress = {};
    localAddress.target_p = FW_SOCKET_ADDRESS_ANY;
    localAddress.port_p = "1024"; // root needed for anything bellow 1024
    TST(fwSocketBind(socket, &localAddress));

    fwSocket newSocket;
    char peerAddress[17] = {};
    TST(fwSocketAccept(socket, &newSocket, peerAddress));

    char receiveBuffer[1024] = {};

    bool quit = false;
    do {
        TST(fwSocketReceive(newSocket, receiveBuffer, 1024));

        if (!strcmp(receiveBuffer, "I love you")) {
            const char response[] = "I love you too";
            TST(fwSocketSend(newSocket, response, sizeof(response)));
            quit = true;
        } else {
            const char response[] = "Meh";
            TST(fwSocketSend(newSocket, response, sizeof(response)));
        }
    } while (!quit);

    const char tellToQuitMessage[] = "exit";
    TST(fwSocketSend(newSocket, tellToQuitMessage, sizeof(tellToQuitMessage)));

    TST(fwSocketClose(socket));

    TST(fwStopModule(fwModuleNetwork));

    return 0;
}
#endif // SERVER

#ifdef CLIENT
int main(int argc, char* argv[]) {
    TST(fwStartModule(fwModuleNetwork, 0));

    fwSocket socket;
    TST(fwSocketCreate(fwSocketAddressFamilyIPv4, fwSocketProtocolStream, &socket));

    struct fwSocketAddress address = {};
    address.target_p = "server";
    address.port_p = "1024";
    TST(fwSocketConnect(socket, &address));

    char message[1024] = {};
    fgets(message, 1024, stdin);
    TST(fwSocketSend(socket, message, 1024));

    bool quit = false;
    do {
        char response[1024] = {};
        TST(fwSocketReceive(socket, response, sizeof(response)));

        printf("%s\n", response);

        if (!strcmp(response, "exit")) {
            quit = true;
        }
    } while (!quit);

    TST(fwSocketClose(socket));

    TST(fwStopModule(fwModuleNetwork));

    return 0;
}
#endif // CLIENT
