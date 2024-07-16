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

#include <errno.h>
#include <stdlib.h>

#include "framework.h"

int main()
{
    struct fwStartModuleInfo strtInf = {};
    strtInf.module = fwModuleNetwork;
    fwStartModule(&strtInf);

    struct fwSocketCreateInfo createInfo = {0};
    createInfo.addressFamily  = fwSocketAddressFamilyIPv4;
    createInfo.socketType     = fwSocketTypeStream;
    fwSocket socket;
    fwError call1 = fwSocketCreate(&createInfo, &socket);

    struct fwSocketConnectInfo connectInfo = {};
    connectInfo.target_p = "tonexum.org";
    connectInfo.port_p = "80";
    connectInfo.targetKind = fwSocketConnectionTargetInternetDomainName;
    fwError call2 = fwSocketConnect(socket, &createInfo, &connectInfo);

    const char httpRequest[38] = "GET / HTTP/1.1\r\nHost: www.tonexum.org\r\n";
    char* buffer = malloc(4096);

    fwError call3 = fwSocketSend(socket, httpRequest, sizeof(httpRequest));
    fwError call4 = fwSocketReceive(socket, buffer, 4096);

    fwError call5 = fwSocketClose(socket);

    fwStopModule(fwModuleNetwork);

    return 0;
}