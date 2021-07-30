/******************************************************************************
**  MODULE:        CORETOOLKIT.H
**  PROJECT:       EPANET-MSX
**  DESCRIPTION:   C/C++ header file for new MSX core API toolkit.
**  COPYRIGHT:     Copyright (C) 2007 Feng Shang, Lewis Rossman, and James Uber.
**                 All Rights Reserved. See license information in LICENSE.TXT.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**                 K. Arrowood, Xylem intern
**  VERSION:       1.1 
**  LAST UPDATE:   Refer to git history
*******************************************************************************/

#ifndef ENUMSOPEN
#include "msxenums.h"
#endif

// --- define WINDOWS

#undef WINDOWS
#ifdef _WIN32
  #define WINDOWS
#endif
#ifdef __WIN32__
  #define WINDOWS
#endif

// --- define DLLEXPORT

#undef DLLEXPORT
#ifdef WINDOWS
  #ifdef __cplusplus
  #define DLLEXPORT extern "C" __declspec(dllexport) __stdcall
  #else
  #define DLLEXPORT __declspec(dllexport) __stdcall
  #endif
#else
  #ifdef __cplusplus
  #define DLLEXPORT extern "C"
  #else
  #define DLLEXPORT
  #endif
#endif


#define MSX_BULK      0
#define MSX_WALL      1

#define MSX_NOSOURCE  -1
#define MSX_CONCEN     0
#define MSX_MASS       1
#define MSX_SETPOINT   2
#define MSX_FLOWPACED  3

// Opaque Pointer
typedef struct Project *MSXproject;

//Project Functions
int DLLEXPORT MSX_open(MSXproject *MSX);
int DLLEXPORT MSX_close(MSXproject MSX);
int DLLEXPORT MSX_init(MSXproject MSX);
int DLLEXPORT MSXprintQuality(MSXproject MSX, int type, char *id, char *species, char *fname);


//Network building functions
int DLLEXPORT MSXaddNode(MSXproject MSX, char *id);
int DLLEXPORT MSXaddTank(MSXproject MSX, char *id, double initialVolume, int mixModel, double volumeMix);
int DLLEXPORT MSXaddReservoir(MSXproject MSX, char *id, double initialVolume, int mixModel, double volumeMix);
int DLLEXPORT MSXaddLink(MSXproject MSX, char *id, char *startNode, char *endNode, double length, double diameter, double roughness);

//Species/Chemistry option functions
int DLLEXPORT MSXaddOption(MSXproject MSX, int optionType, char * value);
int DLLEXPORT MSXaddSpecies(MSXproject MSX, char *id, int type, int units, double aTol, double rTol);
int DLLEXPORT MSXaddCoefficeint(MSXproject MSX, int type, char *id, double value);
int DLLEXPORT MSXaddTerm(MSXproject MSX, char *id, char *equation);
int DLLEXPORT MSXaddExpression(MSXproject MSX, int classType, int expressionType, char *species, char *equation);
int DLLEXPORT MSXaddSource(MSXproject MSX, int sourceType, char *nodeId, char *speciesId, double strength, char *timePattern);
int DLLEXPORT MSXaddQuality(MSXproject MSX, char *type, char *speciesId, double value, char *id);
int DLLEXPORT MSXaddParameter(MSXproject MSX, char *type, char *paramId, double value, char *id);
int DLLEXPORT MSXsetReport(MSXproject MSX, char *reportType, char *id, int precision);

//Hydraulic Functions
int DLLEXPORT MSXsetHydraulics(MSXproject MSX, float *demands, float *heads, float *flows);

int DLLEXPORT MSXsetSize(MSXproject MSX, int type, int size);


// Below is from the legacy epanetmsx.h

int  DLLEXPORT MSXgetindex(MSXproject MSX, int type, char *id, int *index);
int  DLLEXPORT MSXgetIDlen(MSXproject MSX, int type, int index, int *len);
int  DLLEXPORT MSXgetID(MSXproject MSX, int type, int index, char *id, int len);
int  DLLEXPORT MSXgetcount(MSXproject MSX, int type, int *count);
int  DLLEXPORT MSXgetspecies(MSXproject MSX, int index, int *type, char *units, double *aTol,
               double *rTol);
int  DLLEXPORT MSXgetconstant(MSXproject MSX, int index, double *value);
int  DLLEXPORT MSXgetparameter(MSXproject MSX, int type, int index, int param, double *value);
int  DLLEXPORT MSXgetsource(MSXproject MSX, int node, int species, int *type, double *level,
               int *pat);
int  DLLEXPORT MSXgetpatternlen(MSXproject MSX, int pat, int *len);
int  DLLEXPORT MSXgetpatternvalue(MSXproject MSX, int pat, int period, double *value);
int  DLLEXPORT MSXgetinitqual(MSXproject MSX, int type, int index, int species, double *value);
int  DLLEXPORT MSXgetQualityByIndex(MSXproject MSX, int type, int index, int species, double *value);
int  DLLEXPORT MSXgetQualityByID(MSXproject MSX, int type, char *id, char *species, double *value);
int  DLLEXPORT MSXsetconstant(MSXproject MSX, int index, double value);
int  DLLEXPORT MSXsetparameter(MSXproject MSX, int type, int index, int param, double value);
int  DLLEXPORT MSXsetinitqual(MSXproject MSX, int type, int index, int species, double value);
int  DLLEXPORT MSXsetsource(MSXproject MSX, int node, int species, int type, double level,
               int pat);
int  DLLEXPORT MSXsetpatternvalue(MSXproject MSX, int pat, int period, double value);
int  DLLEXPORT MSXaddpattern(MSXproject MSX, char *id);
int  DLLEXPORT MSXsetpattern(MSXproject MSX, int pat, double mult[], int len);

int DLLEXPORT MSXstep(MSXproject MSX, long *t, long *tleft);
int  DLLEXPORT MSXgeterror(int code, char *msg, int len);

