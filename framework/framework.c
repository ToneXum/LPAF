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

// This implementation file contains implementations for platform independant, exposed symbols

#include "framework.h"
#include "internal.h"

fwError fwStartModule(const fwModule module, const uint32_t flags) {
    if (!fwiGetState()->baseIsUp) {
        fwiStartNativeModuleBase();
    }

    switch (module) {
        case fwModuleWindow: {
            return fwiStartNativeModuleWindow();
        }
        case fwModuleRender: {
            return fwiStartNativeModuleRenderer();
        }
        case fwModuleNetwork: {
            return fwiStartNativeModuleNetwork();
        }
        case fwModuleMultimedia: {
            return fwiStartNativeModuleMultimedia();
        }
        default: {
            return fwErrorInvalidParameter;
        }
    }
}

fwError fwStopModule(const enum fwModule module) {
    bool fail = false;
    fwError ret = 0;
    switch (module) {
        case fwModuleWindow: {
            ret = fwiStopNativeModuleWindow();
            break;
        }
        case fwModuleNetwork: {
            ret = fwiStopNativeModuleNetwork();
            break;
        }
        case fwModuleMultimedia: {
            ret = fwiStopNativeModuleMultimedia();
            break;
        }
        case fwModuleRender: {
            ret = fwiStopNativeModuleRenderer();
            break;
        }
        default: {
            fail = true;
        }
    }

    if (!fwiGetState()->baseIsUp) {
        fwiStopNativeModuleBase();
    }

    if (fail) {
        return fwErrorInvalidParameter;
    }

    return ret;
}

void fwStopAllModules(void) {
    if (fwiGetState()->activeModules & fwModuleWindow) {
        fwiStopNativeModuleWindow();
    }
    if (fwiGetState()->activeModules & fwModuleNetwork) {
        fwiStopNativeModuleNetwork();
    }
    if (fwiGetState()->activeModules & fwModuleMultimedia) {
        fwiStopNativeModuleMultimedia();
    }
    if (fwiGetState()->activeModules & fwModuleRender) {
        fwiStopNativeModuleRenderer();
    }
    if (fwiGetState()->baseIsUp) {
        fwiStopNativeModuleBase();
    }
}
