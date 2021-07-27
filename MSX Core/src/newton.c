/******************************************************************************
**  MODULE:        NEWTON.C
**  PROJECT:       EPANET-MSX
**  DESCRIPTION:   Newton-Raphson algorithm used to solve a set of nonlinear
**                 algebraic equations.
**  COPYRIGHT:     Copyright (C) 2007 Feng Shang, Lewis Rossman, and James Uber.
**                 All Rights Reserved. See license information in LICENSE.TXT.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**                 K. Arrowood, Xylem intern
**  VERSION:       1.1.00
**  LAST UPDATE:   Refer to git history
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "msxutils.h"
#include "newton.h"
#include "msxtypes.h"

// Local declarations
//-------------------
MSXNewton MSXNewtonSolver;

#ifdef _OPENMP
#pragma omp threadprivate(MSXNewtonSolver)
#endif

//=============================================================================

int newton_open(int n)
/**
**  Purpose:
**    opens the algebraic solver to handle a system of n equations.
**
**  Input:
**    n = number of equations
**
**  Returns:
**    1 if successful, 0 if not.
**
**  Note:
**    All arrays are 1-based so an extra memory location
**    must be allocated for the unused 0-th position.
*/
{
    int errorcode = 1;
#ifdef _OPENMP 
#pragma omp parallel
    {
#endif
        MSXNewtonSolver.Nmax = 0;
        MSXNewtonSolver.Indx = NULL;
        MSXNewtonSolver.F = NULL;
        MSXNewtonSolver.W = NULL;
        MSXNewtonSolver.Indx = (int*)calloc(n + 1, sizeof(int));
        MSXNewtonSolver.F = (double*)calloc(n + 1, sizeof(double));
        MSXNewtonSolver.W = (double*)calloc(n + 1, sizeof(double));
        MSXNewtonSolver.J = createMatrix(n + 1, n + 1);
#ifdef _OPENMP
#pragma omp critical
        {
#endif
            if (!MSXNewtonSolver.Indx || !MSXNewtonSolver.F || !MSXNewtonSolver.W || !MSXNewtonSolver.J) 
                errorcode = 0;
#ifdef _OPENMP 
        }
#endif
        MSXNewtonSolver.Nmax = n;
#ifdef _OPENMP
    }
#endif
    return errorcode;
}

//=============================================================================

void newton_close()
/**
**  Purpose:
**    closes the algebraic solver.
**
**  Input:
**    none
*/
{
#ifdef _OPENMP
#pragma omp parallel
    {
#endif
        if (MSXNewtonSolver.Indx) { free(MSXNewtonSolver.Indx); MSXNewtonSolver.Indx = NULL; }
        if (MSXNewtonSolver.F) { free(MSXNewtonSolver.F); MSXNewtonSolver.F = NULL; }
        if (MSXNewtonSolver.W) { free(MSXNewtonSolver.W); MSXNewtonSolver.W = NULL; }
        freeMatrix(MSXNewtonSolver.J);
        MSXNewtonSolver.J = NULL;
#ifdef _OPENMP
    }
#endif
}

//=============================================================================

int newton_solve(MSXproject MSX, double x[], int n, int maxit, int numsig, 
                 void (*func)(MSXproject, double, double*, int, double*))
/**
**  Purpose:
**    uses newton-raphson iterations to solve n nonlinear eqns.
**
**  Input:
**    x[] = solution vector
**    n = number of equations
**    maxit = max. number of iterations allowed
**    numsig = number of significant digits in error
**    func = pointer to the function that returns the function values at x.
**
**  Returns:
**    number of iterations if successful, -1 if Jacobian is singular,
**    -2 if it didn't converge, or -3 if n exceeds allowable size.
**
**  Note:
**    the arguments to the function func are:
**      t = a time value (not used here)
**      x = vector of unknowns being solved for
**      n = number of unknowns
**      f = vector of function values evaluated at x.
*/
{
    int i, k;
	double errx, errmax, cscal, relconvg = pow(10.0, -numsig);

    // --- check that system was sized adequetely

    if ( n > MSXNewtonSolver.Nmax ) return -3;

    // --- use up to maxit iterations to find a solution

	for (k=1; k<=maxit; k++) 
	{
        // --- evaluate the Jacobian matrix
        jacobian(MSX, x, n, MSXNewtonSolver.F, MSXNewtonSolver.W, MSXNewtonSolver.J, func);

        // --- factorize the Jacobian

        if ( !factorize(MSXNewtonSolver.J, n, MSXNewtonSolver.W, MSXNewtonSolver.Indx) ) return -1;

        // --- solve for the updates to x (returned in F)

		for (i=1; i<=n; i++) MSXNewtonSolver.F[i] = -MSXNewtonSolver.F[i];
        solve(MSXNewtonSolver.J, n, MSXNewtonSolver.Indx, MSXNewtonSolver.F);
		
		// --- update solution x & check for convergence

        errmax = 0.0;
        for (i=1; i<=n; i++)
        {
			cscal = x[i];
            if (cscal < relconvg) cscal = relconvg;
			x[i] += MSXNewtonSolver.F[i];
            errx = fabs(MSXNewtonSolver.F[i]/cscal);
            if (errx > errmax) errmax = errx;
        }
		if (errmax <= relconvg) return k;
	}

    // --- return error code if no convergence

	return -2;
}
