#include "framework.h"

int main()
{
    struct fwStartModuleInfo startInfo = {};
    startInfo.module = fwModuleWindow;
    fwStartModule(&startInfo);

    // do shit here

    fwStopAllModules();
    return 0;
}