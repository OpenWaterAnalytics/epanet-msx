
#include <stdio.h>

#include "msxgetters.h"
#include "msxtypes.h"

//  External variables
//--------------------
extern MSXproject  MSX;                // MSX project data

int getNobjects(int type, int *Nobjects) {
    *Nobjects = MSX.Nobjects[type];
    return 0;   //No room for error
}
