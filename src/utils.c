/*******************************************************************************
**  TITLE:         UTILS.C
**  DESCRIPTION:   utility functions used by the EPANET Multi-Species Extension
**                 toolkit.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**  VERSION:       1.00
**  LAST UPDATE:   8/1/06
*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <float.h>
#include "utils.h"

#define UCHAR(x) (((x) >= 'a' && (x) <= 'z') ? ((x)&~32) : (x))

//=============================================================================

int  strcomp(char *s1, char *s2)
/*
**  Purpose:
**    performs case insensitive comparison of two strings.
**
**  Input:
**    s1 = character string
**    s2 = character string.
**  
**  Returns:
**    1 if strings are the same, 0 otherwise.
*/
{
   int i;
   for (i=0; UCHAR(s1[i]) == UCHAR(s2[i]); i++)
     if (!s1[i+1] && !s2[i+1]) return(1);
   return(0);
}

//=============================================================================

int  findmatch(char *s, char *keyword[])
/*
**  Purpose:
**    finds a match between a string and an array of keyword strings.
**
**  Input:
**    s = character string
**    keyword = array of keyword strings.
**
**  Returns:
**    index of matching keyword or -1 if no match found.
*/
{
   int i = 0;
   while (keyword[i] != NULL)
   {
      if (match(s, keyword[i])) return(i);
      i++;
   }
   return(-1);
}

//=============================================================================

int  match(char *str, char *substr)
/*
**  Purpose:
**    sees if a sub-string of characters appears in a string
**    (not case sensitive).
**
**  Input:
**    str = character string being searched
**    substr = sub-string being searched for.
**
**  Returns:
**    1 if sub-string found, 0 if not.
*/
{
    int i,j;

    // --- fail if substring is empty
    if (!substr[0]) return(0);

    // --- skip leading blanks of str
    for (i=0; str[i]; i++)
        if (str[i] != ' ') break;

    // --- check if substr matches remainder of str
    for (i=i,j=0; substr[j]; i++,j++)
        if (!str[i] || UCHAR(str[i]) != UCHAR(substr[j]))
            return(0);
    return(1);
}

//=============================================================================

int strToSeconds(char *s, long *seconds)
/*
**  Purpose:
**    converts a string in either decimal hours or hr:min:sec
**    format to number of seconds.
**
**  Input:
**    s = string value of a time. 
**
**  Output:
**    seconds = number of seconds.
**
**  Returns:
**    1 if conversion successful, 0 if not.
*/
{
    int  n, hr = 0, min = 0, sec = 0;
    double hours;
    *seconds = 0;
    if ( getDouble(s, &hours) )
    {
        *seconds = (long)(3600.0*hours);
        return 1;
    }
    n = sscanf(s, "%d:%d:%d", &hr, &min, &sec);
    if ( n == 0 ) return 0;
    *seconds = 3600*hr + 60*min + sec;
    return 1;
}

//=============================================================================

int  getInt(char *s, int *y)
/*
**  Purpose:
**    converts a string to an integer number.
**
**  Input:
**    s = a character string.
**
**  Output:
**    y = converted value of s.
**
**  Returns:
**    1 if conversion successful, 0 if not.
*/
{
    double x;
    if ( getDouble(s, &x) )
    {
        if ( x < 0.0 ) x -= 0.01;
        else x += 0.01;
        *y = (int)x;
        return 1;
    }
    *y = 0;
    return 0;
}

//=============================================================================

int  getFloat(char *s, float *y)
/*
**  Purpose:
**    converts a string to a single precision floating point number.
**
**  Input:
**    s = a character string.
**
**  Output:
**    y = converted value of s
**
**  Returns:
**    1 if conversion successful, 0 if not.
*/
{
    char *endptr;
    *y = (float) strtod(s, &endptr);
    if (*endptr > 0) return(0);
    return(1);
}

//=============================================================================

int  getDouble(char *s, double *y)
/*
**  Purpose:
**    converts a string to a double precision floating point number.
**
**  Input:
**    s = a character string.
**
**  Output:
**    y = converted value of s.
**
**  Returns:
**    1 if conversion successful, 0 if not.
*/
{
    char *endptr;
    *y = strtod(s, &endptr);
    if (*endptr > 0) return(0);
    return(1);
}

//=============================================================================

double ** createMatrix(int nrows, int ncols)
/*
**  Purpose:
**    allocates memory for a 2-dimensional array of doubles.
**
**  Input:
**    nrows = number of rows (0-based)
**    ncols = number of columns (0-based).
**
**  Returns:
**    a pointer to the matrix (a = matrix(nr, nc)).
*/
{
    int i,j;
    double **a;

// --- allocate pointers to rows

    a = (double **) malloc(nrows * sizeof(double *));
    if ( !a ) return NULL;
    
// --- allocate rows and set pointers to them

    a[0] = (double *) malloc (nrows * ncols * sizeof(double));
    if ( !a[0] ) return NULL;
    for ( i = 1; i < nrows; i++ ) a[i] = a[i-1] + ncols;

    for ( i = 0; i < nrows; i++)
    {
        for ( j = 0; j < ncols; j++) a[i][j] = 0.0;
    }
    
// --- return pointer to array of pointers to rows

    return a;
}

//=============================================================================

void freeMatrix(double **a)
/*
**  Purpose:
**    frees the memory allocated for a matrix of doubles.
**
**  Input:
**    a = pointer to a matrix of doubles.
*/
{
    if ( a != NULL )
    {
        if ( a[0] != NULL ) free( a[0] );
        free( a );
    }
}

//=============================================================================

int factorize(double **a, int n, double *w, int *indx)
/*
**  Purpose:
**    performs an LU decomposition of a matrix.
**
**  Input:
**    a[1..n][1..n] = a square matrix of doubles
**    n = matrix size (1-based)
**    w[1..n] = work array of doubles.
**
**  Output:
**    a[][] = matrix that contains elements of L and U matrices
**    indx[1..n] = vector that records the row permutation
**                 effected by the partial pivoting.
**
**  Returns:
**    1 if successful, 0 if matrix is singular.
**
**  Note:
**    The arrays and matrices used in this function are 1-based, so
**    they must have been sized to n+1 when first created.
*/
{
    int i, j, k, p;
    double big, temp, eta;
    for (k=1; k<=n; k++)
    {
        big = 0.0;
        for (i=k; i<=n; i++)
        {
            temp = fabs(a[i][k]);
            if ( temp > big )
            {
                big = temp;
                p = i;
            }
        }
        if ( big == 0.0 ) return 0;
        indx[k] = p;
        for (j=k; j<=n; j++)
        {
            temp = a[k][j];
            a[k][j] = a[p][j];
            a[p][j] = temp;
        }
        for (j=k+1; j<=n; j++) w[j] = a[k][j];
        for (i=k+1; i<=n; i++)
        {
            eta = a[i][k]/a[k][k];
            a[i][k] = eta;
            for (j=k+1; j<=n; j++) a[i][j] -= eta*w[j];
        }
    }
    return 1;
}

//=============================================================================

void solve(double **a, int n, int *indx, double b[])
/*
**  Purpose:
**    solves linear equations AX = B after LU decomposition of A.
**
**  Input:
**    a[1..n][1..n] = LU decomposed square matrix A returned by factorize
**    n = matrix size
**    indx[1..n] = permutation vector returned by factorize
**    b[1..n] = right-hand side vector B.
**
**  Output:
**    b[1..n] = solution vector X.
**
**  Note:
**    The arrays and matrices used in this function are 1-based, so
**    they must have been sized to n+1 when first created.
*/
{
    int k, i, m;
    double t;

    for (k=1; k<=n; k++)
    {
        m = indx[k];
        t = b[m];
        b[m] = b[k];
        for (i=1; i<k; i++) t -= a[k][i]*b[i];
        b[k] = t;
    }
    for (k=n; k>=1; k--)
    {
        t = b[k];
        for (i=k+1; i<=n; i++) t -= a[k][i]*b[i];
        b[k] = t/a[k][k];
    }
}

//=============================================================================

void jacobian(double *x, int n, double *f, double *w, double **a,
              void (*func)(double, double*, int, double*))
/*
**  Purpose: 
**    computes Jacobian matrix of F(t,X) at given X
**
**  Input:
**    x[1..n] = vector of function variables
**    n = number of variables
**    f[1..n] = a work vector
**    w[1..n] = a work vector
**    func = user supplied routine that computes the function
**           values at x.
**
**  Output:
**    f[1..n] = function values at x
**    a[1..n][1..n] = coeffs. of the Jacobian matrix.
**
**  Notes:
**  1. Arguments for func() are:
**      t = independent variable (not used)
**      x[1..n] = vector of dependent variables
**      n = number of functions
**      f[1..n] = function values at x.
**  
**  2. The arrays and matrices used in this function are 1-based, so
**     they must have been sized to n+1 when first created.
*/
{

    int    i, j;
    double temp, eps = 1.0e-7, eps2;

    for (j=1; j<=n; j++)
    {
        temp = x[j];
        x[j] = temp + eps;
        func(0.0, x, n, f);
        if ( temp == 0.0 )
        {
            x[j] = temp;
            eps2 = eps;
        }
        else
        {
            x[j] = temp - eps;
            eps2 = 2.0*eps;
        }
        func(0.0, x, n, w);
        for (i=1; i<=n; i++) a[i][j] = (f[i] - w[i]) / eps2;
        x[j] = temp;
    }


/* --- An alternative method that uses forward differencing
	int    i,j;
	double temp, h;
    double eps = sqrt(DBL_EPSILON);

    func(0.0, x, n, f);
	for (j=1; j<=n; j++) 
	{
		temp = x[j];
		h = eps*fabs(temp);
		if (h == 0.0) h = eps;
		x[j] = temp + h;
        func(0.0, x, n, w);
		for (i=1; i<=n; i++) a[i][j] = (w[i] - f[i]) / h;
		x[j] = temp;
	}
*/

}
