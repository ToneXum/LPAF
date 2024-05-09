#include "framework.h"

int main()
{
    struct fwStartModuleInfo startInfo = {};
    startInfo.module = fwModuleWindow;
    fwStartModule(&startInfo);

    return 0;
}