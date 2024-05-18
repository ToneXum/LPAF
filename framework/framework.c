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
        case fwModuleRender: {
            fwiStopNativeModuleRenderer();
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
        default: {}
    }
}

void fwStopAllModules(void) {
    struct fwiState* state = fwiGetState();
    if (state->activeModules & fwModuleBase) {
        fwiStopNativeModuleBase();
    }
    else if (state->activeModules & fwModuleWindow) {
        fwiStopNativeModuleWindow();
    }
    else if (state->activeModules & fwModuleNetwork) {
        fwiStopNativeModuleNetwork();
    }
    else if (state->activeModules & fwModuleMultimedia) {
        fwiStopNativeModuleMultimedia();
    }
    else if (state->activeModules & fwModuleRender) {
        fwiStopNativeModuleRenderer();
    }
}
