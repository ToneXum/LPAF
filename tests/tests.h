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

#ifndef LPAF_TESTS_H
#define LPAF_TESTS_H

#include "framework.h"

#define TST(f) { fwError e = 0; if ((e = f) != 0) { tstLogFrameworkFail(e, __func__, __LINE__); } }

void tstLogFrameworkFail(
    fwError error,
    const char* location,
    int32_t line);

void tstUnitNetworkClient(
    void
    );

void tstUnitNetworkServer(
    void
    );

void tstUnitWindow(
    void
    );

#endif //LPAF_TESTS_H
