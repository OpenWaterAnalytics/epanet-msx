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
#define ENMSX_TERMCOUNT       1
#define ENMSX_PARAMCOUNT      2
#define ENMSX_CONSTCOUNT      3
#define ENMSX_PATCOUNT        4
#define ENMSX_INITQUAL        0     /* Node and Link values */
#define ENMSX_SOURCEQUAL      0     /* Source values */
#define ENMSX_SOURCEPAT       1
#define ENMSX_SOURCETYPE      2
#define ENMSX_CONCEN          0     /* Source types */
#define ENMSX_MASS            1
#define ENMSX_SETPOINT        2
#define ENMSX_FLOWPACED       3

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
int  DLLEXPORT ENMSXsavemsxfile(char *filename);
int  DLLEXPORT ENMSXgetpatternindex(char *id, int *index);
int  DLLEXPORT ENMSXgetpatternid(int index, char *id);
int  DLLEXPORT ENMSXgetpatternlen(int index, int *len);
int  DLLEXPORT ENMSXgetpatternvalue(int index, int period, double *value);
int  DLLEXPORT ENMSXsetpattern(int index, double *f, int n);
int  DLLEXPORT ENMSXsetpatternvalue(int index, int period, double value);
int  DLLEXPORT ENMSXcreatepattern(char *id);
int  DLLEXPORT  ENMSXsetnodequal(int node, int specie, int code, double value);
int  DLLEXPORT  ENMSXsetlinkqual(int link, int specie, int code, double value);

#endif
