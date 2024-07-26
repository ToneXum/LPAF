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

#include "framework.h"
#include "tests.h"

int main()
{
    fwStartModule(fwModuleNetwork, 0);

    fwSocket socket;
    TST(fwSocketCreate(fwSocketAddressFamilyIPv4, fwSocketProtocolStream, &socket));

    struct fwSocketAddress localAddress = {};
    localAddress.target_p = FW_SOCKET_ADDRESS_ANY;
    localAddress.port_p = "1024"; // root needed for anything bellow 1024
    TST(fwSocketBind(socket, &localAddress));

    TST(fwSocketClose(socket));

    TST(fwStopModule(fwModuleNetwork));

    return 0;
}