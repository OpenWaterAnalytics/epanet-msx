/******************************************************************************
**  MODULE:        MSXTOOLKIT.C
**  PROJECT:       EPANET-MSX
**  DESCRIPTION:   Contains the exportable set of functions that comprise the
**                 new core MSX toolkit that do not require a MSXproject as a
**                 parameter. 
**  COPYRIGHT:     Copyright (C) 2007 Feng Shang, Lewis Rossman, and James Uber.
**                 All Rights Reserved. See license information in LICENSE.TXT.
**  AUTHORS:       K. Arrowood, Xylem intern
**  VERSION:       1.1 
**  LAST UPDATE:   Refer to git history
*******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "msxtypes.h"
#include "coretoolkit.h"
#include "msxtoolkit.h"


// This single global variable is used only when the library is called
// without a data structure.
MSXproject _project;
MSXproject *project = &_project;

//Project Functions
int DLLEXPORT MSXopen(void) {
    return MSX_open(project);
}
int DLLEXPORT MSXclose(void) {
    return MSX_close(*(project));
}
int DLLEXPORT MSXinit(void) {
    return MSX_init(*(project));
}
int DLLEXPORT MSXprintQuality(int type, char *id, char *species, char *fname) {
    return MSX_printQuality(*(project), type, id, species, fname);
}


//Network building functions
int DLLEXPORT MSXaddNode(char *id) {
    return MSX_addNode(*(project), id);
}
int DLLEXPORT MSXaddTank(char *id, double initialVolume, int mixModel, double volumeMix) {
    return MSX_addTank(*(project), id, initialVolume, mixModel, volumeMix);
}
int DLLEXPORT MSXaddReservoir(char *id, double initialVolume, int mixModel, double volumeMix) {
    return MSX_addReservoir(*(project), id, initialVolume, mixModel, volumeMix);
}
int DLLEXPORT MSXaddLink(char *id, char *startNode, char *endNode, double length, double diameter, double roughness) {
    return MSX_addLink(*(project), id, startNode, endNode, length, diameter, roughness);
}

//Species/Chemistry option functions
int DLLEXPORT MSXaddOption(int optionType, char * value) {
    return MSX_addOption(*(project), optionType, value);
}
int DLLEXPORT MSXaddSpecies(char *id, int type, int units, double aTol, double rTol) {
    return MSX_addSpecies(*(project), id, type, units, aTol, rTol);
}
int DLLEXPORT MSXaddCoefficeint(int type, char *id, double value) {
    return MSX_addCoefficeint(*(project), type,id, value);
}
int DLLEXPORT MSXaddTerm(char *id, char *equation) {
    return MSX_addTerm(*(project), id, equation);
}
int DLLEXPORT MSXaddExpression(int classType, int expressionType, char *species, char *equation) {
    return MSX_addExpression(*(project), classType, expressionType, species, equation);
}
int DLLEXPORT MSXaddSource(int sourceType, char *nodeId, char *speciesId, double strength, char *timePattern) {
    return MSX_addSource(*(project), sourceType, nodeId, speciesId, strength, timePattern);
}
int DLLEXPORT MSXaddQuality(char *type, char *speciesId, double value, char *id) {
    return MSX_addQuality(*(project), type, speciesId, value, id);
}
int DLLEXPORT MSXaddParameter(char *type, char *paramId, double value, char *id) {
    return MSX_addParameter(*(project), type, paramId, value, id);
}
int DLLEXPORT MSXsetReport(char *reportType, char *id, int precision) {
    return MSX_setReport(*(project), reportType, id, precision);
}

//Hydraulic Functions
int DLLEXPORT MSXsetHydraulics(float *demands, float *heads, float *flows) {
    return MSX_setHydraulics(*(project), demands, heads, flows);
}

int DLLEXPORT MSXsetSize(int type, int size) {
    return MSX_setSize(*(project), type, size);
}

int  DLLEXPORT MSXgetindex(int type, char *id, int *index) {
    return MSX_getindex(*(project), type, id, index);
}
int  DLLEXPORT MSXgetIDlen(int type, int index, int *len) {
    return MSX_getIDlen(*(project), type, index, len);
}
int  DLLEXPORT MSXgetID(int type, int index, char *result, int len) {
    return MSX_getID(*(project), type, index, result, len);
}
int  DLLEXPORT MSXgetcount(int type, int *count) {
    return MSX_getcount(*(project), type, count);
}
int  DLLEXPORT MSXgetspecies(int index, int *type, char *units, double *aTol, double *rTol) {
    return MSX_getspecies(*(project), index, type, units, aTol, rTol);
}
int  DLLEXPORT MSXgetconstant(int index, double *value) {
    return MSX_getconstant(*(project), index, value);
}
int  DLLEXPORT MSXgetparameter(int type, int index, int param, double *value) {
    return MSX_getparameter(*(project), type, index, param, value);
}
int  DLLEXPORT MSXgetsource(int node, int species, int *type, double *level, int *pat) {
    return MSX_getsource(*(project), node, species, type, level, pat);
}
int  DLLEXPORT MSXgetpatternlen(int pat, int *len) {
    return MSX_getpatternlen(*(project), pat, len);
}
int  DLLEXPORT MSXgetpatternvalue(int pat, int period, double *value) {
    return MSX_getpatternvalue(*(project), pat, period, value);
}
int  DLLEXPORT MSXgetinitqual(int type, int index, int species, double *value) {
    return MSX_getinitqual(*(project), type, index, species, value);
}
int  DLLEXPORT MSXgetQualityByIndex(int type, int index, int species, double *value) {
    return MSX_getQualityByIndex(*(project), type, index, species, value);
}
int  DLLEXPORT MSXgetQualityByID(int type, char *id, char *species, double *value) {
    return MSX_getQualityByID(*(project), type, id, species, value);
}
int  DLLEXPORT MSXsetconstant(int index, double value) {
    return MSX_setconstant(*(project), index, value);
}
int  DLLEXPORT MSXsetparameter(int type, int index, int param, double value) {
    return MSX_setparameter(*(project), type, index, param, value);
}
int  DLLEXPORT MSXsetinitqual(int type, int index, int species, double value) {
    return MSX_setinitqual(*(project), type, index, species, value);
}
int  DLLEXPORT MSXsetsource(int node, int species, int type, double level, int pat)  {
    return MSX_setsource(*(project), node, species, type, level, pat);
}
int  DLLEXPORT MSXsetpatternvalue(int pat, int period, double value) {
    return MSX_setpatternvalue(*(project), pat, period, value);
}
int  DLLEXPORT MSXaddpattern(char *id) {
    return MSX_addpattern(*(project), id);
}
int  DLLEXPORT MSXsetpattern(int pat, double mult[], int len) {
    return MSX_setpattern(*(project), pat, mult, len);
}

int DLLEXPORT MSXstep(long *t, long *tleft) {
    return MSX_step(*(project), t, tleft);
}
int  DLLEXPORT MSXgeterror(int code, char *msg, int len) {
        // Error codes
    static char * Errmsg[] =
    {"unknown error code.",
     "Error 501 - insufficient memory available.",
     "Error 502 - no EPANET data file supplied.",
     "Error 503 - could not open MSX input file.",
     "Error 504 - could not open hydraulic results file.",
     "Error 505 - could not read hydraulic results file.",
     "Error 506 - could not read MSX input file.",
     "Error 507 - too few pipe reaction expressions.",
     "Error 508 - too few tank reaction expressions.",
     "Error 509 - could not open differential equation solver.",
     "Error 510 - could not open algebraic equation solver.",
     "Error 511 - could not open binary results file.",
     "Error 512 - read/write error on binary results file.",
     "Error 513 - could not integrate reaction rate expressions.",
     "Error 514 - could not solve reaction equilibrium expressions.",
     "Error 515 - reference made to an unknown type of object.",
     "Error 516 - reference made to an illegal object index.",
     "Error 517 - reference made to an undefined object ID.",
     "Error 518 - invalid property values were specified.",
     "Error 519 - an MSX project was not opened.",
     "Error 520 - an MSX project is already opened.",
     "Error 521 - could not open MSX report file.",                            //(LR-11/20/07)
     "Error 522 - could not compile chemistry functions.",                     //1.1.00
     "Error 523 - could not load functions from compiled chemistry file.",     //1.1.00
	 "Error 524 - illegal math operation.",                                    //1.1.00
     "Error 525 - No hydraulics given",
     "Error 526 - MSX project not initialized",
     "Error 401 - (too many characters)",
     "Error 402 - (too few input items)",
     "Error 403 - (invalid keyword)",
     "Error 404 - (invalid numeric value)",
     "Error 405 - (reference to undefined object)",
     "Error 406 - (illegal use of a reserved name)",
     "Error 407 - (name already used by another object)",
     "Error 408 - (species already assigned an expression)", 
     "Error 409 - (illegal math expression)"}; 

    if (code <= ERR_FIRST && code >= 400) strncpy(msg, Errmsg[code-400+26], len);
    else if ( code <= ERR_FIRST || code >= ERR_MAX ) strncpy(msg, Errmsg[0], len);
    else strncpy(msg, Errmsg[code - ERR_FIRST], len);
    return 0;
}
int DLLEXPORT MSXsetFlowFlag(int flag) {
    return MSX_setFlowFlag(*(project), flag);
}
int DLLEXPORT MSXsetTimeParameter(int type, long value) {
    return MSX_setTimeParameter(*(project), type, value);
}