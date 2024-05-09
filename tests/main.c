#include "framework.h"

int main()
{
    struct fwStartModuleInfo startInfo = {fwModuleWindow, 0};
    fwStartModule(&startInfo);

    return 0;
}