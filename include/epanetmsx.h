/******************************************************************************
**  TITLE:         EPANETMSX.H
**  DESCRIPTION:   C/C++ header file for EPANET Multi-Species Extension Toolkit
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**  VERSION:       1.00 
**  LAST UPDATE:   10/5/06
*******************************************************************************/

#ifndef EPANETMSX_H
#define EPANETMSX_H

/* These are codes used by the API functions */
#define ENMSX_SPECIESCOUNT    0     /* Component counts */

#ifdef __cplusplus
#define DLLEXPORT extern "C" __declspec(dllexport) __stdcall
#else
#define DLLEXPORT __declspec(dllexport) __stdcall
#endif

int  DLLEXPORT ENMSXopen(char *fname);
int  DLLEXPORT ENMSXsolveH(void);
int  DLLEXPORT ENMSXusehydfile(char *fname);
int  DLLEXPORT ENMSXinit(int save);
int  DLLEXPORT ENMSXgetnodequal(int n, int specie, double *c);
int  DLLEXPORT ENMSXgetlinkqual(int n, int specie, double *c);
int  DLLEXPORT ENMSXgetspecieindex(char *id, int *index);
int  DLLEXPORT ENMSXgetspecieID(int index, char *id);
int  DLLEXPORT ENMSXgetspecietype(int index, int *code);
int  DLLEXPORT ENMSXstep(long *t, long *tleft);
int  DLLEXPORT ENMSXsaveoutfile(char *fname);
int  DLLEXPORT ENMSXreport(void);
int  DLLEXPORT ENMSXclose(void);
int  DLLEXPORT ENMSXgeterror(int errcode, char *errmsg, int n);
int  DLLEXPORT ENMSXgetcount(int code, int *count);

#endif
