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
#include "internal.h"

fwError fwStartModule(const struct fwStartModuleInfo* pStartInfo) {
    if (!(fwiGetState()->activeModules & fwModuleBase)) {
        fwiStartNativeModuleBase();
    }

    switch (pStartInfo->module) {
        case fwModuleWindow: {
            fwiStartNativeModuleWindow();
            break;
        }
        case fwModuleRender: {
            fwiStartNativeModuleRenderer();
            break;
        }
        case fwModuleNetwork: {
            fwiStartNativeModuleNetwork();
            break;
        }
        case fwModuleMultimedia: {
            fwiStartNativeModuleMultimedia();
            break;
        }
        default: {}
    }
    return 0;
}

void fwStopModule(enum fwModule module) {
    switch (module) {
        case fwModuleWindow: {
            fwiStopNativeModuleWindow();
            break;
        }
        case fwModuleNetwork: {
            fwiStopNativeModuleNetwork();
            break;
        }
        case fwModuleMultimedia: {
            fwiStopNativeModuleMultimedia();
            break;
        }
        case fwModuleRender: {
            fwiStopNativeModuleRenderer();
            break;
        }
        default: {}
    }
    if (fwiGetState()->activeModules == 0) {
        fwiStopNativeModuleBase();
    }
}

void fwStopAllModules(void) {
    struct fwiState* state = fwiGetState();
    if (state->activeModules & fwModuleWindow) {
        fwiStopNativeModuleWindow();
    }
    if (state->activeModules & fwModuleNetwork) {
        fwiStopNativeModuleNetwork();
    }
    if (state->activeModules & fwModuleMultimedia) {
        fwiStopNativeModuleMultimedia();
    }
    if (state->activeModules & fwModuleRender) {
        fwiStopNativeModuleRenderer();
    }
    if (state->activeModules & fwModuleBase) {
        fwiStopNativeModuleBase();
    }
}
