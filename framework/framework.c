#include "framework.h"
#include "internal.h"

bool fwStartModule(const struct fwStartModuleInfo* pStartInfo) {
    switch (pStartInfo->kModule) {
        case fwModuleWindow:
        {
            fwiStartNativeModuleWindow();
            break;
        }
        case fwModuleRender:
        {
            fwiStartNativeModuleRenderer();
            break;
        }
        case fwModuleNetwork:
        {
            fwiStartNativeModuleNetwork();
            break;
        }
        case fwModuleMultimedia:
        {
            fwiStartNativeModuleMultimedia();
            break;
        }
    }
    return 0;
}

void fwStopModule(enum fwModule module) {
    switch (module) {
        case fwModuleWindow:
        {
            fwiStopNativeModuleWindow();
            break;
        }
        case fwModuleRender:
        {
            fwiStopNativeModuleRenderer();
            break;
        }
        case fwModuleNetwork:
        {
            fwiStopNativeModuleNetwork();
            break;
        }
        case fwModuleMultimedia:
        {
            fwiStopNativeModuleMultimedia();
            break;
        }
    }
}

