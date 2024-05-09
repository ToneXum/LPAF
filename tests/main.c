#include "framework.h"

int main()
{
    struct fwStartModuleInfo startInfo = {};
    startInfo.kModule = fwModuleWindow;
    fwStartModule(&startInfo);

    return 0;
}