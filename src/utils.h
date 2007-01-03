/*******************************************************************************
**  UTILS.H -- Utility functions for the EPANET-MSX toolkit.
**
**  VERSION:    1.00
**  DATE:       8/1/06
**  AUTHOR:     L. Rossman  US EPA - NRMRL
**              F. Shang    University of Cincinnati
**              J. Uber     University of Cincinnati
*******************************************************************************/

// Case insentive comparison of two strings
int strcomp(char *s1, char *s2);

// Matches a string against an array of keywords
int findmatch(char *s, char *keyword[]);

// Case insensitive search of a string for a substring
int match(char *str, char *substr);

// Converts a 24-hr clocktime to number of seconds
int strToSeconds(char *s, long *t);

// Converts a string to an integer
int getInt(char *s, int *y);

// Converts a string to a float
int getFloat(char *s, float *y);

// Converts a string to a double
int getDouble(char *s, double *y);

// Creates a two dimensional array
double ** createMatrix(int nrows, int ncols);

// Deletes a two dimensional array
void freeMatrix(double **a);

// Applies L-D factorization to a square matrix
int factorize(double **a, int n, double *w, int *indx);

// Solves a factorized, linear system of equations
void solve(double **a, int n, int *indx, double b[]);

// Computes the Jacobian matrix of a set of functions
void jacobian(double *x, int n, double *f, double *w, double **a,
              void (*func)(double, double*, int, double*));
