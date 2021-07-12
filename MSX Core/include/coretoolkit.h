
// Define msxtpyes.h
#ifndef MAGICNUMBER
#include "msxtypes.h"
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

#ifndef DLLEXPORT                                                              // ttaxon - 9/7/10
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
#endif

// --- define MSX constants

// #define MSX_NODE      0
// #define MSX_LINK      1
// #define MSX_TANK      2
// #define MSX_SPECIES   3
// #define MSX_TERM      4
// #define MSX_PARAMETER 5
// #define MSX_CONSTANT  6
// #define MSX_PATTERN   7

#define MSX_BULK      0
#define MSX_WALL      1

#define MSX_NOSOURCE  -1
#define MSX_CONCEN     0
#define MSX_MASS       1
#define MSX_SETPOINT   2
#define MSX_FLOWPACED  3

//Project Functions
int DLLEXPORT MSX_open(MSXproject *MSX);
int DLLEXPORT MSX_close(MSXproject *MSX);
int DLLEXPORT MSX_init(MSXproject *MSX);
int DLLEXPORT MSXresults(MSXproject *MSX, char *fname);


//Simulation Options
int DLLEXPORT MSXsetFlowFlag(MSXproject *MSX, int flag);
int DLLEXPORT MSXsetTimeParameter(MSXproject *MSX, int type, long value);

//Network building functions
int DLLEXPORT MSXaddNode(MSXproject *MSX, char *id);
int DLLEXPORT MSXaddTank(MSXproject *MSX, char *id, double initialVolume, int mixModel, double volumeMix);
int DLLEXPORT MSXaddReservoir(MSXproject *MSX, char *id, double initialVolume, int mixModel, double volumeMix);
int DLLEXPORT MSXaddLink(MSXproject *MSX, char *id, char *startNode, char *endNode, double length, double diameter, double roughness);

//Species/Chemistry option functions
int DLLEXPORT MSXaddOption(MSXproject *MSX, int optionType, char * value);
int DLLEXPORT MSXaddSpecies(MSXproject *MSX, char *id, int type, int units, double aTol, double rTol);
int DLLEXPORT MSXaddCoefficeint(MSXproject *MSX, int type, char *id, double value);
int DLLEXPORT MSXaddTerm(MSXproject *MSX, char *id, char *equation);
int DLLEXPORT MSXaddExpression(MSXproject *MSX, int classType, int expressionType, char *species, char *equation);
int DLLEXPORT MSXaddSource(MSXproject *MSX, int sourceType, char *nodeId, char *speciesId, double strength, char *timePattern);
int DLLEXPORT MSXaddQuality(MSXproject *MSX, char *type, char *speciesId, double value, char *id);
int DLLEXPORT MSXaddParameter(MSXproject *MSX, char *type, char *paramId, double value, char *id);
int DLLEXPORT MSXsetReport(MSXproject *MSX, char *reportType, char *id, int precision);

//Hydraulic Functions
int DLLEXPORT MSXsetHydraulics(MSXproject *MSX, REAL4 *demands, REAL4 *heads, REAL4 *flows);



// Below is from the legacy epanetmsx.h

int  DLLEXPORT MSXgetindex(MSXproject *MSX, int type, char *id, int *index);
int  DLLEXPORT MSXgetIDlen(MSXproject *MSX, int type, int index, int *len);
int  DLLEXPORT MSXgetID(MSXproject *MSX, int type, int index, char *id, int len);
int  DLLEXPORT MSXgetcount(MSXproject *MSX, int type, int *count);
int  DLLEXPORT MSXgetspecies(MSXproject *MSX, int index, int *type, char *units, double *aTol,
               double *rTol);
int  DLLEXPORT MSXgetconstant(MSXproject *MSX, int index, double *value);
int  DLLEXPORT MSXgetparameter(MSXproject *MSX, int type, int index, int param, double *value);
int  DLLEXPORT MSXgetsource(MSXproject *MSX, int node, int species, int *type, double *level,
               int *pat);
int  DLLEXPORT MSXgetpatternlen(MSXproject *MSX, int pat, int *len);
int  DLLEXPORT MSXgetpatternvalue(MSXproject *MSX, int pat, int period, double *value);
int  DLLEXPORT MSXgetinitqual(MSXproject *MSX, int type, int index, int species, double *value);
int  DLLEXPORT MSXgetqual(MSXproject *MSX, int type, int index, int species, double *value);
int  DLLEXPORT MSXsetconstant(MSXproject *MSX, int index, double value);
int  DLLEXPORT MSXsetparameter(MSXproject *MSX, int type, int index, int param, double value);
int  DLLEXPORT MSXsetinitqual(MSXproject *MSX, int type, int index, int species, double value);
int  DLLEXPORT MSXsetsource(MSXproject *MSX, int node, int species, int type, double level,
               int pat);
int  DLLEXPORT MSXsetpatternvalue(MSXproject *MSX, int pat, int period, double value);
int  DLLEXPORT MSXaddpattern(MSXproject *MSX, char *id);
int  DLLEXPORT MSXsetpattern(MSXproject *MSX, int pat, double mult[], int len);

int DLLEXPORT MSXstep(MSXproject *MSX, long *t, long *tleft);

