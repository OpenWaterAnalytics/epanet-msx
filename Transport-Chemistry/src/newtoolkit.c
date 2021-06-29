
#include <stdio.h>
#include <malloc.h>


#include "newtoolkit.h"


int DLLEXPORT Proj_init(MSXproject *MSX)
{
    MSX->ProjectOpened = 0;
    return 0;
}

int DLLEXPORT Proj_free(MSXproject *MSX)
{
    free(MSX);
    MSX = NULL;
    return 0;
}
