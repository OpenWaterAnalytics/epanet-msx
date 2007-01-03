/******************************************************************************
**  TITLE:         NEWTON.H
**  DESCRIPTION:   header file for the equation solver contained in newton.c.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**  VERSION:       1.00
**  LAST UPDATE:   10/5/06
******************************************************************************/

// Opens the equation solver system
int  newton_open(int n);

// Closes the equation solver system
void newton_close(void);

// Applies the solver to a specific system of equations
int  newton_solve(double x[], int n, int maxit, int numsig,  
                  void (*func)(double, double*, int, double*));
