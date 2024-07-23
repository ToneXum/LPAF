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
#include <stdlib.h>

#include "framework.h"

int main()
{
    fwStartModule(fwModuleNetwork, 0);

    fwSocket socket;
    fwError call1 = fwSocketCreate(fwSocketAddressFamilyIPv4, fwSocketProtocolStream, &socket);

    struct fwSocketAddress connection = {};
    connection.target_p     = "tonexum.org";
    connection.port_p       = "80";
    fwError call2 = fwSocketConnect(socket, &connection);

    // Okay so what really confused me here was the response the socket got.
    // Turns out, nginx only responds to off-standard requests when they are invalid.
    // Therefore I got confused when the **correctly** formatted request did not get a response from
    // my webserver.

    const char httpRequest[] = "GET / HTTP/1.1\r\nHost: www.tonexum.org\r\n";

    char* buffer = malloc(4096);

    fwError call3 = fwSocketSend(socket, httpRequest, sizeof(httpRequest));
    fwError call4 = fwSocketReceive(socket, buffer, 4096);

    fwError call5 = fwSocketClose(socket);

    printf("%s", buffer);

    fwStopModule(fwModuleNetwork);

    free(buffer);

    return 0;
}