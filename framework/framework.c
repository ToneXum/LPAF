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

#include <stdio.h>

#include "internal.h"

fwError fwStartModule(const struct fwStartModuleInfo* kpStartInfo) {
    if (!(fwiGetState()->activeModules & fwModuleBase)) {
        fwiStartNativeModuleBase();
    }

    switch (kpStartInfo->module) {
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
        default: {
            return fwErrorInvalidParameter;
        }
    }
    return fwErrorSuccess;
}

fwError fwStopModule(const enum fwModule module) {
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
        default: {
            return fwErrorInvalidParameter;
        }
    }
    if (fwiGetState()->activeModules == 0b1) {
        fwiStopNativeModuleBase();
    }

    return fwErrorSuccess;
}

void fwStopAllModules(void) {
    const struct fwiState* state = fwiGetState();
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
