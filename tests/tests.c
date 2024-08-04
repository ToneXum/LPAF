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

#include "tests.h"

#include <stdio.h>

void tstLogFrameworkFail(const fwError error, const char* location, const int32_t line) {
    printf("Call in %s failed with %d at line %d\n", location, error, line);
}

void tstUnitNetworkClient(void) {
    TST(fwStartModule(fwModuleNetwork, 0));

    fwSocket socket = 0;
    TST(fwSocketCreate(&socket, fwSocketAddressFamilyIPv4, fwSocketProtocolStream));

    struct fwSocketAddress address = {};
    address.target_p = "tonexum.org";
    address.port_p = "80";
    TST(fwSocketConnect(socket, &address));

    const char invalidHttpRequestBecauseNginxWontRespondOtherwise[] = "GET / HTTP/1.1\r\n"
                                                                "Host: www.tonexum.org\n";

    TST(fwSocketSend(socket, invalidHttpRequestBecauseNginxWontRespondOtherwise,
        sizeof(invalidHttpRequestBecauseNginxWontRespondOtherwise)));

    char theResponseTellingMeThatTheHttpRequestIsInvalid[512] = {};
    TST(fwSocketReceive(socket, theResponseTellingMeThatTheHttpRequestIsInvalid,
        sizeof(theResponseTellingMeThatTheHttpRequestIsInvalid)));

    TST(fwSocketClose(socket));

    TST(fwStopModule(fwModuleNetwork));
}

void tstUnitNetworkServer(void) {
    TST(fwStartModule(fwModuleNetwork, 0));

    fwSocket socket;
    TST(fwSocketCreate(&socket, fwSocketAddressFamilyIPv4, fwSocketProtocolStream));

    struct fwSocketAddress address = {};
    address.target_p = "any";
    address.port_p = "1024";
    TST(fwSocketBind(socket, &address));

    fwSocket newSocket;
    char peer[16];
    TST(fwSocketAccept(socket, &newSocket, peer));

    TST(fwSocketClose(newSocket));
    TST(fwSocketClose(socket));

    TST(fwStopModule(fwModuleNetwork));
}
