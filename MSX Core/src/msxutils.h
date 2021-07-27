/*******************************************************************************
**  MODULE:        MSXUTILS.H
**  PROJECT:       EPANET-MSX
**  DESCRIPTION:   Header file for the utility functions used by the EPANET
**                 Multi-Species Extension toolkit.
**  COPYRIGHT:     Copyright (C) 2007 Feng Shang, Lewis Rossman, and James Uber.
**                 All Rights Reserved. See license information in LICENSE.TXT.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**                 K. Arrowood, Xylem intern
**  VERSION:       1.1.00
**  LAST UPDATE:   Refer to git history
*******************************************************************************/

// Opaque Pointer
typedef struct Project *MSXproject;

// Gets the name of a temporary file                                           //1.1.00
char * MSXutils_getTempName(char *s);

// Case insentive comparison of two strings
int MSXutils_strcomp(char *s1, char *s2);

// Matches a string against an array of keywords
int MSXutils_findmatch(char *s, char *keyword[]);

// Case insensitive search of a string for a substring
int MSXutils_match(char *str, char *substr);

// Converts a 24-hr clocktime to number of seconds
int MSXutils_strToSeconds(char *s, long *t);

// Converts a string to an integer
int MSXutils_getInt(char *s, int *y);

// Converts a string to a float
int MSXutils_getFloat(char *s, float *y);

// Converts a string to a double
int MSXutils_getDouble(char *s, double *y);

// Creates a two dimensional array
double ** createMatrix(int nrows, int ncols);

// Deletes a two dimensional array
void freeMatrix(double **a);

// Applies L-D factorization to a square matrix
int factorize(double **a, int n, double *w, int *indx);

// Solves a factorized, linear system of equations
void solve(double **a, int n, int *indx, double b[]);

// Computes the Jacobian matrix of a set of functions
void jacobian(MSXproject MSX, double *x, int n, double *f, double *w, double **a,
              void (*func)(MSXproject, double, double*, int, double*));

// Checks for a valid ID
int checkID(char *id);