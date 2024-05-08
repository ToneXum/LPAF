#include "framework.h"
#include "internal.h"

void fwiStartNativeModuleWindow() {
    if (fwiState.activeModules & fwModuleWindow)
        return;

    fwiState.activeModules |= fwModuleWindow;
}

