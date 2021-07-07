
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>


#include "msxsetters.h"
#include "msxobjects.h"
#include "coretoolkit.h"
#include "msxdict.h"
#include "msxutils.h"
#include "mathexpr.h"

// Imported Functions
double MSXqual_getNodeQual(MSXproject *MSX, int j, int m);
double MSXqual_getLinkQual(MSXproject *MSX, int k, int m);
int    MSXqual_open(MSXproject *MSX);
int    MSXqual_init(MSXproject *MSX);
int    MSXqual_step(MSXproject *MSX, long *t, long *tleft);
int    MSXqual_close(MSXproject *MSX);

// Local Functions


int DLLEXPORT MSX_open(MSXproject *MSX)
/**
**  Purpose:
**    initializes the MSX data struct.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**    
**  Output:
**    None
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    MSX->ProjectOpened = 1;
    setDefaults(MSX);
    createHashTables();
    return 0;
}

int DLLEXPORT MSX_close(MSXproject *MSX)
{
    MSXqual_close(MSX);
    deleteObjects(MSX);
    deleteHashTables();
    MSX->ProjectOpened = FALSE;
    return 0;
}

int DLLEXPORT MSXrun(MSXproject *MSX)
/**
**  Purpose:
**    Runs the MSX project.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**
**  Output:
**    None
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    if ((!MSX->ProjectOpened) || (!MSX->QualityOpened)) return ERR_MSX_NOT_OPENED;
    int err = 0;

    err = finishInit(MSX);
    if (!err) err = MSXqual_open(MSX);
    if (!err) err = MSXqual_init(MSX);
    return err;
}


int DLLEXPORT MSXsetFlowFlag(MSXproject *MSX, int flag)
/**
**  Purpose:
**      sets the flow flag and units flag.
**
**  Input:
**      MSX = the underlying MSXproject data struct.
**      flag = the flag to set, for example CMH for cubic meters per hour.
** 
**  Output:
**    None
**
**  Returns:
**    an error code (or 0 for no error).
 */
{
    int err = 0;
    // Cannot modify network structure while solvers are active
    if ((!MSX->ProjectOpened) || (!MSX->QualityOpened)) return ERR_MSX_NOT_OPENED;

    if (flag > 9) return ERR_INVALID_OBJECT_TYPE;
    MSX->Flowflag = flag;
    if ( MSX->Flowflag >= LPS ) MSX->Unitsflag = SI;
    else                          MSX->Unitsflag = US;
    return 0;
}

int DLLEXPORT MSXsetTimeParameter(MSXproject *MSX, int type, long value)
/**
**  Purpose:
**      sets a specified time parameter.
**
**  Input:
**      MSX = the underlying MSXproject data struct.
**      type = the type of time parameter
**      value = that actual value to set.
** 
**  Output:
**    None
**
**  Returns:
**    an error code (or 0 for no error).
 */
{
    int err = 0;
    // Cannot modify network structure while solvers are active
    if ((!MSX->ProjectOpened) || (!MSX->QualityOpened)) return ERR_MSX_NOT_OPENED;

    switch (type)
    {
    case DURATION:
        MSX->Dur = value;
        break;
    case QUALSTEP:
        MSX->Qstep = value;
        break;
    case PATTERNSTEP:
        MSX->Pstep = value;
        break;
    case PATTERNSTART:
        MSX->Pstart = value;
        break;
    case REPORTSTEP:
        MSX->Rstep = value;
        break;
    case REPORTSTART:
        MSX->Rstart = value;
        break;
    case STATISTIC:
        MSX->Statflag = value;
        break;
    default:
        return ERR_INVALID_OBJECT_TYPE;
        break;
    }
    return 0;
}



int DLLEXPORT MSXaddNode(MSXproject *MSX, char *id)
/**
**  Purpose:
**    adds a node to the network.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**    id = The id
**    
**  Output:
**    None
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    // Cannot modify network structure while solvers are active
    if ((!MSX->ProjectOpened) || (!MSX->QualityOpened)) return ERR_MSX_NOT_OPENED;
    if ( findObject(NODE, id) >= 1 ) return ERR_INVALID_OBJECT_PARAMS;
    int err = checkID(id);
    if ( err ) return err;
    if ( addObject(NODE, id, MSX->Nobjects[NODE]+1) < 0 )
        err = 101;  // Insufficient memory

    int numNodes = MSX->Nobjects[NODE];
    if (numNodes == 0) {
        MSX->NodesCapacity = 10;
        MSX->Node = (Snode *) calloc(MSX->NodesCapacity+1, sizeof(Snode));
    }
    if (numNodes == MSX->NodesCapacity) {
        MSX->NodesCapacity *= 2;
        MSX->Node = (Snode *) realloc(MSX->Node, (MSX->NodesCapacity+1) * sizeof(Snode));
    }
    int i = numNodes+1;
    MSX->Node[i].rpt = 0;
    MSX->Node[i].id = id;
    MSX->Nobjects[NODE]++;
    return err;
    
}


int DLLEXPORT MSXaddTank(MSXproject *MSX,char *id, double initialVolume, int mixModel, double volumeMix)
/**
**  Purpose:
**    adds a tank to the network.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**    id = The id
**    initialVolume = initial volume of the tank
**    mixModel = Mix Model type of the tank
**    volumeMix = The capacity of the mixing compartment
**
**  Output:
**    None
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    // Cannot modify network structure while solvers are active
    if ((!MSX->ProjectOpened) || (!MSX->QualityOpened)) return ERR_MSX_NOT_OPENED;
    if ( findObject(TANK, id) >= 1 ) return ERR_INVALID_OBJECT_PARAMS;
    int err = checkID(id);
    if ( err ) return err;
    if ( addObject(TANK, id, MSX->Nobjects[TANK]+1) < 0 )
        err = 101;  // Insufficient memory
    if ( addObject(NODE, id, MSX->Nobjects[NODE]+1) < 0 )
        err = 101;  // Insufficient memory
    
    int numTanks = MSX->Nobjects[TANK];
    if (numTanks == 0) {
        MSX->TanksCapacity = 10;
        MSX->Tank = (Stank *) calloc(MSX->TanksCapacity+1, sizeof(Stank));
    }
    if (numTanks == MSX->TanksCapacity) {
        MSX->TanksCapacity *= 2;
        MSX->Tank = (Stank *) realloc(MSX->Tank, (MSX->TanksCapacity+1) * sizeof(Stank));
    }
    int numNodes = MSX->Nobjects[NODE];
    if (numNodes == 0) {
        MSX->NodesCapacity = 10;
        MSX->Node = (Snode *) calloc(MSX->NodesCapacity+1, sizeof(Snode));
    }
    if (numNodes == MSX->NodesCapacity) {
        MSX->NodesCapacity *= 2;
        MSX->Node = (Snode *) realloc(MSX->Node, (MSX->NodesCapacity+1) * sizeof(Snode));
    }
    int i = numTanks+1;
    MSX->Tank[i].a = 1.0;
    MSX->Tank[i].v0 = initialVolume;
    MSX->Tank[i].mixModel = mixModel;
    MSX->Tank[i].vMix = volumeMix;
    MSX->Tank[i].id = id;
    MSX->Tank[i].node = numNodes+1;
    i = numNodes+1;
    MSX->Node[i].tank = numTanks+1;
    MSX->Node[i].rpt = 0;
    MSX->Node[i].id = id;
    MSX->Nobjects[NODE]++;
    MSX->Nobjects[TANK]++;
    return err;
}


int DLLEXPORT MSXaddReservoir(MSXproject *MSX, char *id, double initialVolume, int mixModel, double volumeMix)
/**
**  Purpose:
**    adds a reservoir to the network.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**    id = The id
**    initialVolume = initial volume of the tank
**    mixModel = Mix Model type of the tank
**    volumeMix = The capacity of the mixing compartment
**  
**  Output:
**    None
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    // Cannot modify network structure while solvers are active
    if ((!MSX->ProjectOpened) || (!MSX->QualityOpened)) return ERR_MSX_NOT_OPENED;
    if ( findObject(TANK, id) >= 1 ) return ERR_INVALID_OBJECT_PARAMS;
    int err = checkID(id);
    if ( err ) return err;
    if ( addObject(TANK, id, MSX->Nobjects[TANK]+1) < 0 )
        err = 101;  // Insufficient memory
    if ( addObject(NODE, id, MSX->Nobjects[NODE]+1) < 0 )
        err = 101;  // Insufficient memory
    
    int numTanks = MSX->Nobjects[TANK];
    if (numTanks == 0) {
        MSX->TanksCapacity = 10;
        MSX->Tank = (Stank *) calloc(MSX->TanksCapacity+1, sizeof(Stank));
    }
    if (numTanks == MSX->TanksCapacity) {
        MSX->TanksCapacity *= 2;
        MSX->Tank = (Stank *) realloc(MSX->Tank, (MSX->TanksCapacity+1) * sizeof(Stank));
    }
    int numNodes = MSX->Nobjects[NODE];
    if (numNodes == 0) {
        MSX->NodesCapacity = 10;
        MSX->Node = (Snode *) calloc(MSX->NodesCapacity+1, sizeof(Snode));
    }
    if (numNodes == MSX->NodesCapacity) {
        MSX->NodesCapacity *= 2;
        MSX->Node = (Snode *) realloc(MSX->Node, (MSX->NodesCapacity+1) * sizeof(Snode));
    }
    int i = numTanks+1;
    MSX->Tank[i].a = 0.0;
    MSX->Tank[i].v0 = initialVolume;
    MSX->Tank[i].mixModel = mixModel;
    MSX->Tank[i].vMix = volumeMix;
    MSX->Tank[i].id = id;
    MSX->Tank[i].node = numNodes+1;
    i = numNodes+1;
    MSX->Node[i].tank = numTanks+1;
    MSX->Node[i].rpt = 0;
    MSX->Node[i].id = id;
    MSX->Nobjects[NODE]++;
    MSX->Nobjects[TANK]++;
    return err;
}


int DLLEXPORT MSXaddLink(MSXproject *MSX, char *id, char *startNode, char *endNode, double diameter, double length, double roughness)
/**
**  Purpose:
**    adds a link to the network.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**    id = The id
**    startNode = Start node id
**    endNode = End node id
**    diameter = Diameter of the pipe
**    length = length of the pipe
**    roughness = roughness of the pipe
**
**  Output:
**    None
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    // Cannot modify network structure while solvers are active
    if ((!MSX->ProjectOpened) || (!MSX->QualityOpened)) return ERR_MSX_NOT_OPENED;
    
    if ( findObject(LINK, id) >= 1 ) return ERR_INVALID_OBJECT_PARAMS;
    int err = checkID(id);
    if ( err ) return err;
    if ( addObject(LINK, id, MSX->Nobjects[LINK]+1) < 0 )
        err = 101;  // Insufficient memory

    // Check that the start and end nodes exist
    int x = findObject(NODE, startNode);
    if ( x <= 0 ) return ERR_NAME;
    int y = findObject(NODE, endNode);
    if ( y <= 0 ) return ERR_NAME;
    int numLinks = MSX->Nobjects[LINK];
    if (numLinks == 0) {
        MSX->LinksCapacity = 10;
        MSX->Link = (Slink *) calloc(MSX->LinksCapacity+1, sizeof(Slink));
    }
    if (numLinks == MSX->LinksCapacity) {
        MSX->LinksCapacity *= 2;
        MSX->Link = (Slink *) realloc(MSX->Link, (MSX->LinksCapacity+1) * sizeof(Slink));
    }
    int i = numLinks+1;
    MSX->Link[i].n1 = x;
    MSX->Link[i].n2 = y;
    MSX->Link[i].diam = diameter;
    MSX->Link[i].len = length;
    MSX->Link[i].roughness = roughness;
    MSX->Link[i].rpt = 0;
    MSX->Link[i].id = id;    
    MSX->Nobjects[LINK]++;
    return err;
}


int DLLEXPORT MSXaddOption(MSXproject *MSX, int optionType, char *value)
/**
**  Purpose:
**    adds an option to the MSX data struct.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**    optionType = the type of option
**    value = the value to set the option to
**
**  Output:
**    None
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    int err = 0;
    // Cannot modify network structure while solvers are active
    if ((!MSX->ProjectOpened) || (!MSX->QualityOpened)) return ERR_MSX_NOT_OPENED;
    int k;
    switch (optionType)
    {
    case AREA_UNITS_OPTION:
        k = MSXutils_findmatch(value, AreaUnitsWords);
        if ( k < 0 ) return ERR_KEYWORD;
        MSX->AreaUnits = k;
        break;

    case RATE_UNITS_OPTION:
        k = MSXutils_findmatch(value, TimeUnitsWords);
        if ( k < 0 ) return ERR_KEYWORD;
        MSX->RateUnits = k;
        break;

    case SOLVER_OPTION:
        k = MSXutils_findmatch(value, SolverTypeWords);
        if ( k < 0 ) return ERR_KEYWORD;
        MSX->Solver = k;
        break;

    case COUPLING_OPTION:
        k = MSXutils_findmatch(value, CouplingWords);
        if ( k < 0 ) return ERR_KEYWORD;
        MSX->Coupling = k;
        break;

    case TIMESTEP_OPTION:
        k = atoi(value);
        if ( k <= 0 ) return ERR_NUMBER;
        MSX->Qstep = k;
        break;

    case RTOL_OPTION:
        if ( !MSXutils_getDouble(value, &MSX->DefRtol) ) return ERR_NUMBER;
        break;

    case ATOL_OPTION:
        if ( !MSXutils_getDouble(value, &MSX->DefAtol) ) return ERR_NUMBER;
        break;

    case COMPILER_OPTION:
        k = MSXutils_findmatch(value, CompilerWords);
        if ( k < 0 ) return ERR_KEYWORD;
        MSX->Compiler = k;
    default:
        return ERR_INVALID_OBJECT_TYPE;
        break;
    }
    return err;
}


int DLLEXPORT MSXaddSpecies(MSXproject *MSX, char *id, int type, int units, double aTol, double rTol)
/**
**  Purpose:
**    adds a species to the MSX data struct.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**    id = the species id
**    type = the species type, this will be BULK or WALL
**    units = The species units code
**    aTol = Absolute Tolerance
**    rTol = relative Tolerance
**
**  Output:
**    None
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    if (!(type == BULK || type == WALL)) return ERR_KEYWORD;
    if ((!MSX->ProjectOpened) || (!MSX->QualityOpened)) return ERR_MSX_NOT_OPENED;
    if ( findObject(SPECIES, id) >= 1 ) return ERR_INVALID_OBJECT_PARAMS;

    int err = checkID(id);
    if ( err ) return err;
    if ( addObject(SPECIES, id, MSX->Nobjects[SPECIES]+1) < 0 )
        err = 101;  // Insufficient memory
    int numSpecies = MSX->Nobjects[SPECIES];
    if (numSpecies == 0) {
        MSX->SpeciesCapacity = 10;
        MSX->Species = (Sspecies *) calloc(MSX->SpeciesCapacity+1, sizeof(Sspecies));
    }
    if (numSpecies == MSX->SpeciesCapacity) {
        MSX->SpeciesCapacity *= 2;
        MSX->Species = (Sspecies *) realloc(MSX->Species, (MSX->SpeciesCapacity+1) * sizeof(Sspecies));
    }

    int i = numSpecies+1;
    MSX->Species[i].id = id;
    MSX->Species[i].type = type;
    switch (units)
    {
    case MG:
        strncpy(MSX->Species[i].units, "MG", MAXUNITS);
        break;
    case UG:
        strncpy(MSX->Species[i].units, "UG", MAXUNITS);
        break;
    case MOLE:
        strncpy(MSX->Species[i].units, "MOLE", MAXUNITS);
        break;
    case MMOLE:
        strncpy(MSX->Species[i].units, "MMOLE", MAXUNITS);
        break;
    default:
        return ERR_KEYWORD;
        break;
    }
    
    MSX->Species[i].aTol = aTol;
    MSX->Species[i].rTol = rTol;
    MSX->Species[i].pipeExpr     = NULL;
    MSX->Species[i].tankExpr     = NULL;
    MSX->Species[i].pipeExprType = NO_EXPR;
    MSX->Species[i].tankExprType = NO_EXPR;
    MSX->Species[i].precision    = 2;
    MSX->Species[i].rpt = 0;
    MSX->Nobjects[SPECIES]++;

    // Allocate space in Node, Link, and Tank objects
    if (MSX->Nobjects[SPECIES] == 1) {
        for (i=1; i<=MSX->Nobjects[NODE]; i++) {
            MSX->Node[i].c = (double *) calloc(MSX->Nobjects[SPECIES]+1, sizeof(double));
            MSX->Node[i].c0 = (double *) calloc(MSX->Nobjects[SPECIES]+1, sizeof(double));
            
        }
        for (i=1; i<=MSX->Nobjects[TANK]; i++) {
            MSX->Tank[i].c = (double *)
                calloc(MSX->Nobjects[SPECIES]+1, sizeof(double));
            MSX->Tank[i].reacted = (double*)
                calloc(MSX->Nobjects[SPECIES] + 1, sizeof(double));
        }
        for (i=1; i<=MSX->Nobjects[LINK]; i++) {
            MSX->Link[i].c0 = (double *)
                calloc(MSX->Nobjects[SPECIES]+1, sizeof(double));
            MSX->Link[i].reacted = (double *)
                calloc(MSX->Nobjects[SPECIES] + 1, sizeof(double));
        }
    }
    else {
        for (i=1; i<=MSX->Nobjects[NODE]; i++) {
            MSX->Node[i].c = (double *) realloc(MSX->Node[i].c, (MSX->Nobjects[SPECIES]+1) * sizeof(double));
            MSX->Node[i].c0 = (double *) realloc(MSX->Node[i].c0, (MSX->Nobjects[SPECIES]+1) * sizeof(double));
        }
        for (i=1; i<=MSX->Nobjects[TANK]; i++) {
            MSX->Tank[i].c = (double *)
                realloc(MSX->Tank[i].c, (MSX->Nobjects[SPECIES]+1) * sizeof(double));
            MSX->Tank[i].reacted = (double*)
                realloc(MSX->Tank[i].reacted, (MSX->Nobjects[SPECIES]+1) * sizeof(double));
        }
        for (i=1; i<=MSX->Nobjects[LINK]; i++) {
            MSX->Link[i].c0 = (double *)
                realloc(MSX->Link[i].c0, (MSX->Nobjects[SPECIES]+1) * sizeof(double));
            MSX->Link[i].reacted = (double *)
                realloc(MSX->Link[i].reacted, (MSX->Nobjects[SPECIES]+1) * sizeof(double));
        }
    }
    return err;
}


int DLLEXPORT MSXaddCoefficeint(MSXproject *MSX, int type, char *id, double value)
/**
**  Purpose:
**    adds a coefficient to the MSX data struct.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**    type = the species type, this will be PARAMETER or CONSTANT
**    id = the coefficient id
**    value = the coefficient value
**
**  Output:
**    None
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    if ((!MSX->ProjectOpened) || (!MSX->QualityOpened)) return ERR_MSX_NOT_OPENED;
    int err = 0;
    if (type == PARAMETER) {
        if ( findObject(PATTERN, id) >= 1 ) return ERR_INVALID_OBJECT_PARAMS;
        err = checkID(id);
        if ( err ) return err;
        if ( addObject(PARAMETER, id, MSX->Nobjects[PARAMETER]+1) < 0 )
            err = 101;  // Insufficient memory
        int numParams = MSX->Nobjects[PARAMETER];
        if (numParams == 0) {
            MSX->ParamCapacity = 10;
            MSX->Param = (Sparam *) calloc(MSX->ParamCapacity+1, sizeof(Sparam));
        }
        if (numParams == MSX->ParamCapacity) {
            MSX->ParamCapacity *= 2;
            MSX->Param = (Sparam *) realloc(MSX->Param, (MSX->ParamCapacity+1) * sizeof(Sparam));
        }
        int i = numParams+1;
        MSX->Param[i].id = id;
		MSX->Param[i].value = value;
        MSX->Nobjects[PARAMETER]++;
        // Allocate space in Node, Link, and Tank objects
        if (MSX->Nobjects[PARAMETER] == 1) {
            for (i=1; i<=MSX->Nobjects[TANK]; i++) {
                MSX->Tank[i].param = (double *)
                    calloc(MSX->Nobjects[PARAMETER]+1, sizeof(double));
            }
            for (i=1; i<=MSX->Nobjects[LINK]; i++) {
                MSX->Link[i].param = (double *)
                    calloc(MSX->Nobjects[PARAMETER]+1, sizeof(double));
            }
        }
        else {
            for (i=1; i<=MSX->Nobjects[TANK]; i++) {
                MSX->Tank[i].param = (double *)
                    calloc(MSX->Nobjects[PARAMETER]+1, sizeof(double));
            }
            for (i=1; i<=MSX->Nobjects[LINK]; i++) {
                MSX->Link[i].param = (double *)
                    calloc(MSX->Nobjects[PARAMETER]+1, sizeof(double));
            }
        }
    }
    else if (type == CONSTANT) {
        if ( findObject(CONSTANT, id) >= 1 ) return ERR_INVALID_OBJECT_PARAMS;
        err = checkID(id);
        if ( err ) return err;
        if ( addObject(CONSTANT, id, MSX->Nobjects[CONSTANT]+1) < 0 )
            err = 101;  // Insufficient memory
        int numConsts = MSX->Nobjects[CONSTANT];
        if (numConsts == 0) {
            MSX->ConstCapacity = 10;
            MSX->Const = (Sconst *) calloc(MSX->ConstCapacity+1, sizeof(Sconst));
        }
        if (numConsts == MSX->ConstCapacity) {
            MSX->ConstCapacity *= 2;
            MSX->Const = (Sconst *) realloc(MSX->Const, (MSX->ConstCapacity+1) * sizeof(Sconst));
        }
        int i = numConsts+1;
        MSX->Const[i].id = id;
		MSX->Const[i].value = value;
        MSX->Nobjects[CONSTANT]++;
    }
    else err = ERR_KEYWORD;
    return err;
}

int DLLEXPORT MSXaddTerm(MSXproject *MSX, char *id, char *equation)
/**
**  Purpose:
**    adds a term to the MSX data struct.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**    id = the term id
**    equation = the term's equation
**
**  Output:
**    None
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    if ((!MSX->ProjectOpened) || (!MSX->QualityOpened)) return ERR_MSX_NOT_OPENED;
    if ( findObject(TERM, id) >= 1 ) return ERR_INVALID_OBJECT_PARAMS;
    int err = 0;
    err = checkID(id);
    if ( err ) return err;
    if ( addObject(TERM, id, MSX->Nobjects[TERM]+1) < 0 )
        err = 101;  // Insufficient memory
    int numTerms = MSX->Nobjects[TERM];
    if (numTerms == 0) {
        MSX->TermCapacity = 10;
        MSX->Term = (Sterm *) calloc(MSX->TermCapacity+1, sizeof(Sterm));
    }
    if (numTerms == MSX->TermCapacity) {
        MSX->TermCapacity *= 2;
        MSX->Term = (Sterm *) realloc(MSX->Term, (MSX->TermCapacity+1) * sizeof(Sterm));
    }
    int i = numTerms+1;
    MSX->Term[i].id = id;
    MathExpr *expr = mathexpr_create(MSX, equation, getVariableCode);
    if ( expr == NULL ) return ERR_MATH_EXPR;
    MSX->Term[i].expr = expr;
    MSX->Term[i].equation = equation;
    MSX->Nobjects[TERM]++;
    return err;
}

int DLLEXPORT MSXaddExpression(MSXproject *MSX, int classType, int expressionType, char *species, char *equation)
/**
**  Purpose:
**    adds a term to the MSX data struct.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**    classType = either LINK or TANK
**    expressionType = one of the types from the ExpressionType enum
**    species = the expression species
**    equation = the equation
**
**  Output:
**    None
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    if ((!MSX->ProjectOpened) || (!MSX->QualityOpened)) return ERR_MSX_NOT_OPENED;
    int err = 0;
    // --- determine expression type 
    if ( expressionType < 0 || expressionType > 3 ) return ERR_KEYWORD;

    // --- determine species associated with expression

    int i = findObject(SPECIES, species);
    if ( i < 1 ) return ERR_NAME;

    // --- check that species does not already have an expression

    if ( classType == LINK )
    {
        if ( MSX->Species[i].pipeExprType != NO_EXPR ) return ERR_DUP_EXPR;
    }
    else if ( classType == TANK )
    {
        if ( MSX->Species[i].tankExprType != NO_EXPR ) return ERR_DUP_EXPR;
    }
    else return ERR_INVALID_OBJECT_PARAMS;
    MathExpr *expr = mathexpr_create(MSX, equation, getVariableCode);
    if ( expr == NULL ) return ERR_MATH_EXPR;
    switch (classType)
    {
    case LINK:
        MSX->Species[i].pipeExpr = expr;
        MSX->Species[i].pipeExprType = expressionType;
        break;
    case TANK:
        MSX->Species[i].tankExpr = expr;
        MSX->Species[i].tankExprType = expressionType;
        break;
    }
    return err;
}


int DLLEXPORT MSXaddSource(MSXproject *MSX, int sourceType, char *nodeId, char *speciesId, double strength, char *timePattern)
/**
**  Purpose:
**    adds a term to the MSX data struct.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**    sourceType = The type of source
**    nodeId = Id of the node specifed
**    speciesId = Id of the species specified
**    strength = The strength of the source
**    timePattern = The time pattern of the source
**
**  Output:
**    None
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    if ((!MSX->ProjectOpened) || (!MSX->QualityOpened)) return ERR_MSX_NOT_OPENED;
    int err = 0;
    // --- determine source type 
    if ( sourceType < 0 || sourceType > 3 ) return ERR_KEYWORD;
    int j = findObject(NODE, nodeId);
    if ( j <= 0 ) return ERR_NAME;
    int m = findObject(SPECIES, speciesId);
    if ( m <= 0 ) return ERR_NAME;
    // --- check that species is a BULK species
    if ( MSX->Species[m].type != BULK ) return 0;
    // --- check if a source for this species already exists
    Psource source = MSX->Node[j].sources;
    while ( source )
    {
        if ( source->species == m ) break;
        source = source->next;
    }
    // --- otherwise create a new source object
    if ( source == NULL )
    {
        source = (struct Ssource *) malloc(sizeof(struct Ssource));
        if ( source == NULL ) return 101;
        source->next = MSX->Node[j].sources;
        MSX->Node[j].sources = source;
    }
    source->type = (char)sourceType;
    source->species = m;
    source->c0 = strength;
    int i = 0;
    i = findObject(PATTERN, timePattern);
    source->pat = i;
    return err;
}

int DLLEXPORT MSXaddQuality(MSXproject *MSX, char *type, char *speciesId, double value, char *id)
/**
**  Purpose:
**    adds a term to the MSX data struct.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**    type = The type of quality
**    speciesId = Id of the species specified
**    value = The value to be assigned
**    id = To be used for either the node or link id, if it is global, then this field doesn't matter.
**
**  Output:
**    None
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    if ((!MSX->ProjectOpened) || (!MSX->QualityOpened)) return ERR_MSX_NOT_OPENED;
    int err = 0;
    int i = 0;
    if      ( MSXutils_match(type, "GLOBAL") ) i = 1;
    else if ( MSXutils_match(type, "NODE") )   i = 2;
    else if ( MSXutils_match(type, "LINK") )   i = 3;
    else return ERR_KEYWORD;

    // --- find species index
    int m = findObject(SPECIES, speciesId);
    if ( m <= 0 ) return ERR_NAME;

    // --- for global specification, set initial quality either for
    //     all nodes or links depending on type of species

    if ( i == 1)
    {
        int j;
		MSX->C0[m] = value;
        if ( MSX->Species[m].type == BULK )
        {
            for (j=1; j<=MSX->Nobjects[NODE]; j++) MSX->Node[j].c0[m] = value;
        }
        for (j=1; j<=MSX->Nobjects[LINK]; j++) MSX->Link[j].c0[m] = value;
    }
    // --- for a specific node, get its index & set its initial quality
    else if ( i == 2 )
    {
        int j = findObject(NODE, id);
        if ( j <= 0 ) return ERR_NAME;
        if ( MSX->Species[m].type == BULK ) MSX->Node[j].c0[m] = value;
    }
    // --- for a specific link, get its index & set its initial quality
    else if ( i == 3 )
    {
        int j = findObject(LINK, id);
        if ( j <= 0 ) return ERR_NAME;
        MSX->Link[j].c0[m] = value;
    }

    return err;
}

int DLLEXPORT MSXaddParameter(MSXproject *MSX, char *type, char *paramId, double value, char *id)
/**
**  Purpose:
**    adds a term to the MSX data struct.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**    type = Should be either PIPE or TANK
**    paramId = Name for the parameter
**    value = The value to be assigned
**    id = The id of either the pipe or the tank
**
**  Output:
**    None
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    if ((!MSX->ProjectOpened) || (!MSX->QualityOpened)) return ERR_MSX_NOT_OPENED;
    int err = 0;

    // --- get parameter name
    int i = findObject(PARAMETER, paramId);

    // --- for pipe parameter, get pipe index and update parameter's value
    if ( MSXutils_match(type, "PIPE") )
    {
        int j = findObject(LINK, id);
        if ( j <= 0 ) return ERR_NAME;
        MSX->Link[j].param[i] = value;
    }
    // --- for tank parameter, get tank index and update parameter's value
    else if ( MSXutils_match(type, "TANK") )
    {
        int j = findObject(TANK, id);
        if ( j <= 0 ) return ERR_NAME;
        j = MSX->Node[j].tank;
        if ( j > 0 ) MSX->Tank[j].param[i] = value;
    }
    else return ERR_KEYWORD;
    return err;
}

int DLLEXPORT MSXsetReport(MSXproject *MSX, char *reportType, char *id, int precision)
/**
**  Purpose:
**    adds a term to the MSX data struct.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**    reportType = Specifies what is being set in the report
**    id = Name for the object being set in the report
**    precision = Only used with SPECIES, and if none then just put 0
**
**  Output:
**    None
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    if ((!MSX->ProjectOpened) || (!MSX->QualityOpened)) return ERR_MSX_NOT_OPENED;
    int err = 0;
    int k = MSXutils_findmatch(reportType, ReportWords);
    if ( k < 0 ) return ERR_KEYWORD;
    int j;
    switch(k)
    {
        // --- keyword is NODE; parse ID names of reported nodes
        case 0:
            j = findObject(NODE, id);
            if ( j <= 0 ) return ERR_NAME;
            MSX->Node[j].rpt = 1;
            break;

        // --- keyword is LINK: parse ID names of reported links
        case 1:
            j = findObject(LINK, id);
            if ( j <= 0 ) return ERR_NAME;
            MSX->Link[j].rpt = 1;
            break;
        // --- keyword is SPECIES; get YES/NO & precision
        case 2:
            j = findObject(SPECIES, id);
            if ( j <= 0 ) return ERR_NAME;
            MSX->Species[j].rpt = 1;
            MSX->Species[j].precision = precision;
            break;
        // --- keyword is FILE: get name of report file
        case 3:
            strcpy(MSX->RptFile.name, id);
            break;
        // --- keyword is PAGESIZE;
        case 4:
            if ( !MSXutils_getInt(id, &MSX->PageSize) ) return ERR_NUMBER;
            break;
    }
    return err;
}











//TODO make a function that would go before the run project that will do the intializing of like node and tank qualities that are dependent on the number of species and such
// Add tanks as nodes at the end of the nodes list just like in MSX_inp





// Below is from the legacy msxtoolkit.c

int  DLLEXPORT  MSXgetindex(MSXproject *MSX, int type, char *id, int *index)
/**
**  Purpose:
**    retrieves the index of a named MSX object.
**
**  Input:
**    type = object type code
**    id = name of water quality species.
**
**  Output:
**    index = index (base 1) in the list of all objects of the given type.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    int i;
    *index = 0;
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    switch(type)
    {
        case SPECIES:   i = findObject(SPECIES, id);   break;
        case CONSTANT:  i = findObject(CONSTANT, id);  break;
        case PARAMETER: i = findObject(PARAMETER, id); break;
        case PATTERN:   i = findObject(PATTERN, id);   break;
        default:            return ERR_INVALID_OBJECT_TYPE;
    }
    if ( i < 1 ) return ERR_UNDEFINED_OBJECT_ID;
    *index = i;
    return 0;
}

//=============================================================================

int  DLLEXPORT  MSXgetIDlen(MSXproject *MSX, int type, int index, int *len)
/**
**  Purpose:
**    retrieves the number of characters in the ID name of an MSX object.
**
**  Input:
**    type =  object type code
**    index = index (base 1) of the object in list of all objects of the
**            given type.
**
**  Output:
**    len = number of characters in the object's ID name.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    int i;
    *len = 0;
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    switch(type)
    {
        case SPECIES:   i = SPECIES;   break;
        case CONSTANT:  i = CONSTANT;  break;
        case PARAMETER: i = PARAMETER; break;
        case PATTERN:   i = PATTERN;   break;
        default:            return ERR_INVALID_OBJECT_TYPE;
    }
    if ( index < 1 || index > MSX->Nobjects[i] ) return ERR_INVALID_OBJECT_INDEX;
    switch(i)
    {
        case SPECIES:   *len = (int) strlen(MSX->Species[index].id); break;
        case CONSTANT:  *len = (int) strlen(MSX->Const[index].id);   break;
        case PARAMETER: *len = (int) strlen(MSX->Param[index].id);   break;
        case PATTERN:   *len = (int) strlen(MSX->Pattern[index].id); break;
    }
    return 0;
}

//=============================================================================

int  DLLEXPORT  MSXgetID(MSXproject *MSX, int type, int index, char *id, int len)
/**
**  Purpose:
**    retrieves the name of an object given its index.
**
**  Input:
**    type  = object type code
**    index = index (base 1) of the object in list of all objects of the
**            given type
**    len   = maximum number of characters that id can hold.
**
**  Output:
**    id = name of the object.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    int i;
    strcpy(id, "");
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    switch(type)
    {
        case SPECIES:   i = SPECIES;   break;
        case CONSTANT:  i = CONSTANT;  break;
        case PARAMETER: i = PARAMETER; break;
        case PATTERN:   i = PATTERN;   break;
        default:            return ERR_INVALID_OBJECT_TYPE;
    }
    if ( index < 1 || index > MSX->Nobjects[i] ) return ERR_INVALID_OBJECT_INDEX;
    switch(i)
    {
        case SPECIES:   strncpy(id, MSX->Species[index].id, len);  break;
        case CONSTANT:  strncpy(id, MSX->Const[index].id, len);   break;
        case PARAMETER: strncpy(id, MSX->Param[index].id, len);   break;
        case PATTERN:   strncpy(id, MSX->Pattern[index].id, len); break;
    }
	id[len] = '\0';                                                            //(L. Rossman - 11/01/10)
    return 0;
}

//=============================================================================

int DLLEXPORT  MSXgetcount(MSXproject *MSX, int type, int *count)
/**
**  Purpose:
**    retrieves the number of objects of a specific type.
**
**  Input:
**    type =  object type code
**
**  Output:
**    count = number of objects of the given type.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    *count = 0;
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    switch(type)
    {
        case SPECIES:   *count = MSX->Nobjects[SPECIES];   break;
        case CONSTANT:  *count = MSX->Nobjects[CONSTANT];  break;
        case PARAMETER: *count = MSX->Nobjects[PARAMETER]; break;
        case PATTERN:   *count = MSX->Nobjects[PATTERN];   break;
        default:            return ERR_INVALID_OBJECT_TYPE;
    }
    return 0;
}

//=============================================================================

int DLLEXPORT  MSXgetspecies(MSXproject *MSX, int index, int *type, char *units,
                             double *aTol, double * rTol)
/**
**  Purpose:
**    retrieves the attributes of a chemical species.
**
**  Input:
**    index = index (base 1) of the species in the list of all species.
**
**  Output:
**    type = MSX_BULK (0) for a bulk flow species or MSX_WALL (1) for a
**           surface species;
**    units = character string containing the mass units defined for the species -
**            must be sized in the calling program to accept up to 15 bytes
**            plus a null termination character
**    aTol = absolute concentration tolerance (concentration units);
**    rTol = relative concentration tolerance (unitless)
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    *type  = 0;
    strcpy(units, "");
    *aTol  = 0.0;
    *rTol  = 0.0;
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    if ( index < 1 || index > MSX->Nobjects[SPECIES] ) return ERR_INVALID_OBJECT_INDEX;
    *type  = MSX->Species[index].type;
    strncpy(units, MSX->Species[index].units, MAXUNITS);
    *aTol  = MSX->Species[index].aTol;
    *rTol  = MSX->Species[index].rTol;
    return 0;
}

//=============================================================================

int DLLEXPORT  MSXgetconstant(MSXproject *MSX, int index, double *value)
/**
**  Purpose:
**    retrieves the value of a particular reaction constant.
**
**  Input:
**    index = index (base 1) of the constant in the list of all constants.
**
**  Output:
**    value = value assigned to the constant.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    *value = 0.0;
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    if ( index < 1 || index > MSX->Nobjects[CONSTANT] ) return ERR_INVALID_OBJECT_INDEX;
    *value = MSX->Const[index].value;
    return 0;
}

//=============================================================================

int DLLEXPORT MSXgetparameter(MSXproject *MSX, int type, int index, int param, double *value)
/**
**  Purpose:
**    retrieves the value of a particular reaction parameter for a given pipe
**    or tank within the pipe network.
**
**  Input:
**    type = MSX_NODE (0) for a node or MSX_LINK (1) for a link;
**    index = index (base 1) assigned to the node or link;
**    param = index (base 1) assigned to the reaction parameter.
**
**  Output:
**    value = value assigned to the parameter.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    int j;
    *value = 0.0;
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    if ( param < 1 || param > MSX->Nobjects[PARAMETER] ) return ERR_INVALID_OBJECT_INDEX;
    if ( type == NODE )
    {
        if ( index < 1 || index > MSX->Nobjects[NODE] ) return ERR_INVALID_OBJECT_INDEX;
        j = MSX->Node[index].tank;
        if ( j > 0 ) *value = MSX->Tank[j].param[param];
    }
    else if ( type == LINK )
    {
        if ( index < 1 || index > MSX->Nobjects[LINK] ) return ERR_INVALID_OBJECT_INDEX;
        *value = MSX->Link[index].param[param];
    }
    else return ERR_INVALID_OBJECT_TYPE;
    return 0;
}

//=============================================================================

int  DLLEXPORT MSXgetsource(MSXproject *MSX, int node, int species, int *type, double *level,
                            int *pat)
/**
**  Purpose:
**    retrieves information on any external source of a particular chemical
**    species assigned to a specific node of the pipe network.
**
**  Input:
**    node = index number (base 1) assigned to the node of interest;
**    species = index number (base 1) of the species of interest;
**
**  Output:
**    type = one of the following of external source type codes:
**           MSX_NOSOURCE  = -1 for no source,
**           MSX_CONCEN    =  0 for a concentration source,
**           MSX_MASS      =  1 for a mass booster source,
**           MSX_SETPOINT  =	2 for a setpoint source,
**           MSX_FLOWPACED =	3 for a flow paced source;
**    level = the baseline concentration (or mass flow rate) of the species
**            in the source;
**    pat = the index of the time pattern assigned to the species at the source
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    Psource source;
    *type  = MSX_NOSOURCE;
    *level = 0.0;
    *pat   = 0;
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    if ( node < 1 || node > MSX->Nobjects[NODE] ) return ERR_INVALID_OBJECT_INDEX;
    if ( species < 1 || species > MSX->Nobjects[SPECIES] ) return ERR_INVALID_OBJECT_INDEX;
    source = MSX->Node[node].sources;
    while ( source )
    {
        if ( source->species == species )
        {
            *type  = source->type;
            *level = source->c0;
            *pat   = source->pat;
            break;
        }
        source = source->next;
    }
    return 0;
}

//=============================================================================

int  DLLEXPORT  MSXgetpatternlen(MSXproject *MSX, int pat, int *len)
/**
**  Purpose:
**    retrieves the number of time periods within a source time pattern.
**
**  Input:
**    pat = the index number (base 1) of the time pattern;
**
**  Output:
**    len = the number of time periods that appear in the pattern.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    *len = 0;
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    if ( pat < 1 || pat > MSX->Nobjects[PATTERN] ) return ERR_INVALID_OBJECT_INDEX;
    *len = MSX->Pattern[pat].length;
    return 0;
}

//=============================================================================

int  DLLEXPORT  MSXgetpatternvalue(MSXproject *MSX, int pat, int period, double *value)
/**
**  Purpose:
**    retrieves the multiplier at a specific time period for a given
**    source time pattern.
**
**  Input:
**    pat = the index number (base 1) of the time pattern;
**    period = the index of the time period (starting from 1) whose
**             multiplier is being sought;
**
**  Output:
**    value = the value of the pattern's multiplier in the desired period.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    int n = 1;
    *value = 0.0;
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    if ( pat < 1 || pat > MSX->Nobjects[PATTERN] ) return ERR_INVALID_OBJECT_INDEX;
    if ( period <= MSX->Pattern[pat].length )
    {
        MSX->Pattern[pat].current = MSX->Pattern[pat].first;
        while ( MSX->Pattern[pat].current )
        {
            if ( n == period )
            {
                *value = MSX->Pattern[pat].current->value;
                return 0;
            }
            MSX->Pattern[pat].current = MSX->Pattern[pat].current->next;
            n++;
        }
    }
    return 0;
}

//=============================================================================

int  DLLEXPORT  MSXgetinitqual(MSXproject *MSX, int type, int index, int species, double *value)
/**
**  Purpose:
**    retrieves the initial concentration of a particular chemical species
**    assigned to a specific node or link of the pipe network.
**
**  Input:
**    type = MSX_NODE (0) for a node or MSX_LINK (1) for a link;
**    index = index (base 1) of the node or link of interest;
**    species = index (base 1) of the specie of interest.
**
**  Output:
**    value = initial concentration of the specie at the node or link.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    *value = 0.0;
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    if ( species < 1 || species > MSX->Nobjects[SPECIES] ) return ERR_INVALID_OBJECT_INDEX;
    if ( type == NODE )
    {
        if ( index < 1 || index > MSX->Nobjects[NODE] ) return ERR_INVALID_OBJECT_INDEX;
        *value = MSX->Node[index].c0[species];
    }
    else if ( type == LINK )
    {
        if ( index < 1 || index > MSX->Nobjects[LINK] ) return ERR_INVALID_OBJECT_INDEX;
        *value = MSX->Link[index].c0[species];
    }
    else return ERR_INVALID_OBJECT_TYPE;
    return 0;
}

//=============================================================================

int  DLLEXPORT  MSXgetqual(MSXproject *MSX, int type, int index, int species, double *value)
/**
**  Purpose:
**    retrieves the current concentration of a species at a particular node
**    or link of the pipe network.
**
**  Input:
**    type = MSX_NODE (0) for a node or MSX_LINK (1) for a link;
**    index = index (base 1) of the node or link of interest;.
**    species = index (base 1) of the species of interest.
**
**  Output:
**    value = species concentration at the node or link.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    *value = 0.0;
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    if ( species < 1 || species > MSX->Nobjects[SPECIES] ) return ERR_INVALID_OBJECT_INDEX;
    if ( type == NODE )
    {
        if ( index < 1 || index > MSX->Nobjects[NODE] ) return ERR_INVALID_OBJECT_INDEX;
        *value = MSXqual_getNodeQual(MSX, index, species);
    }
    else if ( type == LINK )
    {
        if ( index < 1 || index > MSX->Nobjects[LINK] ) return ERR_INVALID_OBJECT_INDEX;
        *value = MSXqual_getLinkQual(MSX, index, species);
    }
    else return ERR_INVALID_OBJECT_TYPE;
    return 0;
}

//=============================================================================

int  DLLEXPORT  MSXsetconstant(MSXproject *MSX, int index, double value)
/**
**  Purpose:
**    assigns a new value to a specific reaction constant.
**
**  Input:
**    index = index (base 1) of the constant in the list of all constants;
**    value = the new value to be assigned to the constant.
**
**  Output:
**    none.
**
**  Returns:
**    an error code or 0 for no error.
*/
{
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    if ( index < 1 || index > MSX->Nobjects[CONSTANT] ) return ERR_INVALID_OBJECT_INDEX;
    MSX->Const[index].value = value;
    return 0;
}

//=============================================================================

int  DLLEXPORT  MSXsetparameter(MSXproject *MSX, int type, int index, int param, double value)
/**
**  Purpose:
**    assigns a value to a particular reaction parameter for a given pipe
**    or tank within the pipe network.
**
**  Input:
**    type = MSX_NODE (0) for a node or MSX_LINK (1) for a link;
**    index = index (base 1) assigned to the node or link;
**    param = index (base 1) assigned to the reaction parameter;
**    value = value to be assigned to the parameter.
**
**  Output:
**    none.
**
**  Returns:
**    an error code or 0 for no error.
*/
{
    int j;
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    if ( param < 1 || param > MSX->Nobjects[PARAMETER] ) return ERR_INVALID_OBJECT_INDEX;
    if ( type == NODE )
    {
        if ( index < 1 || index > MSX->Nobjects[NODE] ) return ERR_INVALID_OBJECT_INDEX;
        j = MSX->Node[index].tank;
        if ( j > 0 ) MSX->Tank[j].param[param] = value;
    }
    else if ( type == LINK )
    {
        if ( index < 1 || index > MSX->Nobjects[LINK] ) return ERR_INVALID_OBJECT_INDEX;
        MSX->Link[index].param[param] = value;
    }
    else return ERR_INVALID_OBJECT_TYPE;
    return 0;
}

//=============================================================================

int  DLLEXPORT  MSXsetinitqual(MSXproject *MSX, int type, int index, int species, double value)
/**
**  Purpose:
**    assigns an initial concentration of a particular chemical species
**    to a specific node or link of the pipe network.
**
**  Input:
**    type = MSX_NODE (0) for a node or MSX_LINK (1) for a link;
**    index = index (base 1) of the node or link of interest;
**    species = index (base 1) of the species of interest.
**    value = initial concentration of the species at the node or link.
**
**  Output:
**    none.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    if ( species < 1 || species > MSX->Nobjects[SPECIES] ) return ERR_INVALID_OBJECT_INDEX;
    if ( type == NODE )
    {
        if ( index < 1 || index > MSX->Nobjects[NODE] ) return ERR_INVALID_OBJECT_INDEX;
        if ( MSX->Species[species].type == BULK )
            MSX->Node[index].c0[species] = value;
    }
    else if ( type == LINK )
    {
        if ( index < 1 || index > MSX->Nobjects[LINK] ) return ERR_INVALID_OBJECT_INDEX;
        MSX->Link[index].c0[species] = value;
    }
    else return ERR_INVALID_OBJECT_TYPE;
    return 0;
}

//=============================================================================

int  DLLEXPORT  MSXsetsource(MSXproject *MSX, int node, int species, int type, double level,
                             int pat)
/**
**  Purpose:
**    sets the attributes of an external source of a particular chemical
**    species to a specific node of the pipe network.
**
**  Input:
**    node = index number (base 1) assigned to the node of interest;
**    species = index number (base 1) of the species of interest;
**    type = one of the following of external source type codes:
**           MSX_NOSOURCE  = -1 for no source,
**           MSX_CONCEN    =  0 for a concentration source,
**           MSX_MASS      =  1 for a mass booster source,
**           MSX_SETPOINT  =	2 for a setpoint source,
**           MSX_FLOWPACED =	3 for a flow paced source;
**    level = the baseline concentration (or mass flow rate) of the species
**            in the source;
**    pat = the index of the time pattern assigned to the species at the source
**
**  Output:
**    none.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    Psource source;

// --- check for valid source parameters

    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    if ( node < 1 || node > MSX->Nobjects[NODE] ) return ERR_INVALID_OBJECT_INDEX;
    if ( species < 1 || species > MSX->Nobjects[SPECIES] ) return ERR_INVALID_OBJECT_INDEX;
    if ( pat > MSX->Nobjects[PATTERN] ) return ERR_INVALID_OBJECT_INDEX;
    if ( pat < 0 ) pat = 0;
    if ( type < MSX_NOSOURCE ||
         type > MSX_FLOWPACED ) return ERR_INVALID_OBJECT_PARAMS;
    if ( MSX->Species[species].type != BULK ) return ERR_INVALID_OBJECT_PARAMS;
    if ( level < 0.0 ) return ERR_INVALID_OBJECT_PARAMS;

// --- check if a source for this species already exists at the node

    source = MSX->Node[node].sources;
    while ( source )
    {
        if ( source->species == species ) break;
        source = source->next;
    }

// --- if no current source exists then create a new one

    if ( source == NULL )
    {
        source = (struct Ssource *) malloc(sizeof(struct Ssource));
        if ( source == NULL ) return ERR_MEMORY;
        source->next = MSX->Node[node].sources;
        MSX->Node[node].sources = source;
    }

// --- assign parameters to the source

    source->type   = (char)type;
    source->species = species;
    source->c0     = level;
    source->pat    = pat;
    return 0;
}

//=============================================================================

int  DLLEXPORT  MSXsetpatternvalue(MSXproject *MSX, int pat, int period, double value)
/**
**  Purpose:
**    assigns a new value to the multiplier for a specific time period in
**    a given time pattern.
**
**  Input:
**    pat = the index number (base 1) of the time pattern;
**    period = the time period (starting from 1) whose multiplier is
**             being replaced;
**    value = the new value of the pattern's multiplier in the desired period.
**
**  Output:
**    none.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    int n = 1;

// --- check that pattern & period exists

    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    if ( pat < 1 || pat > MSX->Nobjects[PATTERN] ) return ERR_INVALID_OBJECT_INDEX;
    if ( period <= 0 || period > MSX->Pattern[pat].length )
        return ERR_INVALID_OBJECT_PARAMS;

// --- find desired time period in the pattern

    MSX->Pattern[pat].current = MSX->Pattern[pat].first;
    while ( MSX->Pattern[pat].current )
    {
        if ( n == period )
        {
            MSX->Pattern[pat].current->value = value;
            return 0;
        }
        MSX->Pattern[pat].current = MSX->Pattern[pat].current->next;
        n++;
    }
    return 0;
}

//=============================================================================

int  DLLEXPORT  MSXaddpattern(MSXproject *MSX, char *id)
/**
**  Purpose:
**    adds a new MSX time pattern to the project.
**
**  Input:
**    id = C-style character string with the ID name of the new pattern.
**
**  Output:
**    none.
**
**  Returns:
**    an error code (or 0 for no error).
**
**  Notes:
**    the new pattern is appended to the end of the existing patterns.
*/
{
    int i, n;
    Spattern *tmpPat;

// --- check if a pattern with same id already exists

    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    if ( findObject(PATTERN, id) >= 1 ) return ERR_INVALID_OBJECT_PARAMS;

// --- allocate memory for a new array of patterns

    n = MSX->Nobjects[PATTERN] + 1;
    tmpPat = (Spattern *) calloc(n+1, sizeof(Spattern));
    if ( tmpPat == NULL ) return ERR_MEMORY;

// --- copy contents of old pattern array to new one

    for (i=1; i<=MSX->Nobjects[PATTERN]; i++)
    {
        tmpPat[i].id      = MSX->Pattern[i].id;
        tmpPat[i].length  = MSX->Pattern[i].length;
        tmpPat[i].first   = MSX->Pattern[i].first;
        tmpPat[i].current = MSX->Pattern[i].current;
    }

// --- add info for the new pattern

    if ( addObject(PATTERN, id, n) < 0 )
    {
        free(tmpPat);
        return ERR_MEMORY;
    }
    tmpPat[n].id = findID(PATTERN, id);
    tmpPat[n].length = 0;
    tmpPat[n].first = NULL;
    tmpPat[n].current = NULL;

// --- replace old pattern array with new one

    FREE(MSX->Pattern);
    MSX->Pattern = tmpPat;
    MSX->Nobjects[PATTERN]++;
    return 0;
}

//=============================================================================

int  DLLEXPORT  MSXsetpattern(MSXproject *MSX, int pat, double mult[], int len)
/**
**  Purpose:
**    Assigns a new set of multipliers to a given time pattern.
**
**  Input:
**    pat = the index number (base 1) of the time pattern;
**    mult[] = an array of multiplier values (base 0) to replace those
**             previously used by the pattern;
**    len = the number of entries in the multiplier array mult.
**
**  Output:
**    none.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    int i;
    SnumList *listItem;

// --- check that pattern exists

    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    if ( pat < 1 || pat > MSX->Nobjects[PATTERN] ) return ERR_INVALID_OBJECT_INDEX;
    if ( len < 0) len = 0;

// --- delete current multipliers

    listItem = MSX->Pattern[pat].first;
    while (listItem)
    {
        MSX->Pattern[pat].first = listItem->next;
        free(listItem);
        listItem = MSX->Pattern[pat].first;
    }
    MSX->Pattern[pat].first = NULL;

// --- create a new set of multipliers

    MSX->Pattern[pat].length = 0;
    for ( i = 0; i < len; i++ )
    {
        listItem = (SnumList *) malloc(sizeof(SnumList));
        if ( listItem == NULL ) return ERR_MEMORY;
        listItem->value = mult[i];
        listItem->next = NULL;
        if ( MSX->Pattern[pat].first == NULL )
        {
            MSX->Pattern[pat].current = listItem;
            MSX->Pattern[pat].first = listItem;
        }
        else
        {
            MSX->Pattern[pat].current->next = listItem;
            MSX->Pattern[pat].current = listItem;
        }
        MSX->Pattern[pat].length++;
    }
	
    MSX->Pattern[pat].interval = 0;			  //Feng Shang   04/17/2008
    MSX->Pattern[pat].current = MSX->Pattern[pat].first;    //Feng Shang   04/17/2008
    return 0;
}

//=============================================================================

