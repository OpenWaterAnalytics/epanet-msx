/******************************************************************************
**  MODULE:        NEWTON.H
**  PROJECT:       EPANET-MSX
**  DESCRIPTION:   header file for the equation solver contained in newton.c.
**  COPYRIGHT:     Copyright (C) 2007 Feng Shang, Lewis Rossman, and James Uber.
**                 All Rights Reserved. See license information in LICENSE.TXT.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**  VERSION:       1.1.00
**  LAST UPDATE:   3/1/07
******************************************************************************/

#ifndef MAGICNUMBER
#include "msxtypes.h"
#endif

typedef struct
{
    int      Nmax;          // max. number of equations
    int* Indx;         // permutation vector of row indexes     
    double* F;            // function & adjustment vector
    double* W;            // work vector
    double** J;           // Jacobian matrix
}MSXNewton;

// Opens the equation solver system
int  newton_open(int n);

// Closes the equation solver system
void newton_close(void);

// Applies the solver to a specific system of equations
int  newton_solve(MSXproject*MSX, double x[], int n, int maxit, int numsig,  
                  void (*func)(MSXproject *,double, double*, int, double*));