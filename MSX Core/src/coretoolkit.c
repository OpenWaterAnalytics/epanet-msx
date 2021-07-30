/******************************************************************************
**  MODULE:        CORETOOLKIT.C
**  PROJECT:       EPANET-MSX
**  DESCRIPTION:   Contains the exportable set of functions that comprise the
**                 new core MSX toolkit. 
**  COPYRIGHT:     Copyright (C) 2007 Feng Shang, Lewis Rossman, and James Uber.
**                 All Rights Reserved. See license information in LICENSE.TXT.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**                 K. Arrowood, Xylem intern
**  VERSION:       1.1 
**  LAST UPDATE:   Refer to git history
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "msxtypes.h"
#include "msxobjects.h"
#include "msxdict.h"
#include "msxutils.h"
#include "coretoolkit.h"

// Imported Functions
double MSXqual_getNodeQual(MSXproject MSX, int j, int m);
double MSXqual_getLinkQual(MSXproject MSX, int k, int m);
int    MSXqual_open(MSXproject MSX);
int    MSXqual_init(MSXproject MSX);
int    MSXqual_step(MSXproject MSX, long *t, long *tleft);
int    MSXqual_close(MSXproject MSX);

//=============================================================================

int DLLEXPORT MSX_open(MSXproject *MSX)
/**
**  Purpose:
**    opens the MSX data struct.
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
    struct Project *p = (struct Project *) calloc(1, sizeof(struct Project));
    *MSX = p;
    setDefaults(*MSX);
    createHashTables();
    return 0;
}

//=============================================================================

int DLLEXPORT MSX_close(MSXproject MSX)
{

    // --- close all files

    if ( MSX->RptFile.file ) fclose(MSX->RptFile.file);                          //(LR-11/20/07, to fix bug 08)
    if ( MSX->HydFile.file ) fclose(MSX->HydFile.file);
    if ( MSX->TmpOutFile.file && MSX->TmpOutFile.file != MSX->OutFile.file )
        fclose(MSX->TmpOutFile.file);
    if ( MSX->OutFile.file ) fclose(MSX->OutFile.file);

    // --- delete all temporary files

    // if ( MSX->HydFile.mode == SCRATCH_FILE ) remove(MSX->HydFile.name);
    if ( MSX->OutFile.mode == SCRATCH_FILE ) remove(MSX->OutFile.name);
    remove(MSX->TmpOutFile.name);

    // --- free all allocated memory

    MSX->RptFile.file = NULL;                                                   //(LR-11/20/07, to fix bug 08)
    MSX->HydFile.file = NULL;
    MSX->OutFile.file = NULL;
    MSX->TmpOutFile.file = NULL;
    MSXqual_close(MSX);
    deleteObjects(MSX);
    deleteHashTables();
    MSX->ProjectOpened = FALSE;
    free(MSX);
    return 0;
}

//=============================================================================

int DLLEXPORT MSX_init(MSXproject MSX)
/**
**  Purpose:
**    Initializes the MSX project.
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
    if (!MSX->ProjectOpened) return ERR_MSX_NOT_OPENED;
    int err = 0;
    err = finishInit(MSX);
    if (!err) err = MSXqual_open(MSX);
    if (!err) err = MSXqual_init(MSX);
    return err;
}

//=============================================================================

int DLLEXPORT MSXprintQuality(MSXproject MSX, int type, char *id, char *species, char *fname)
/**
**  Purpose:
**    writes requested WQ simulation results to a text file or to the command line if no file given.
**
**  Input:
**    type = MSX_NODE (0) for a node or MSX_LINK (1) for a link.
**    id = id of the node or link of interest.
**    species = id of the species of interest.
**    fname = filename to write or blank if supposed to print to the command line.
**
**  Returns:
**    an error code (or 0 for no error).
**
*/
{
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    int err = 0;
    double value = 0.0;
    err = MSXgetQualityByID(MSX, type, id, species, &value);
    if (err != 0) return err;
    FILE *f = NULL;
    if (fname != NULL) f = fopen(fname, "a");
    int hrs = MSX->Qtime / 3600;
    int mins = (MSX->Qtime % 3600) / 60;
    if (type == NODE) {
        if (f != NULL) fprintf(f, "Node: %s     Species: %6s     Time: %4d:%02d\nConcentration: %f\n\n", id, species, hrs, mins, value);
        else {
            printf("\r\nNode: %s     Species: %6s     Time: %4d:%02d\nConcentration: %f\n", id, species, hrs, mins, value);
            fflush(stdout);
        }
    }
    else if (type == LINK) {
        if (f != NULL) fprintf(f, "Link: %s     Species: %6s     Time: %4d:%02d\nConcentration: %f\n\n", id, species, hrs, mins, value);
        else {
            printf("\r\nLink: %s     Species: %6s     Time: %4d:%02d\nConcentration: %f\n", id, species, hrs, mins, value);
            fflush(stdout);
        }
    }
    else return ERR_INVALID_OBJECT_TYPE;
    if (f != NULL) fclose(f);
    f = NULL;
    return err;
}

//=============================================================================

int DLLEXPORT MSXaddNode(MSXproject MSX, char *id)
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
    if (!MSX->ProjectOpened) return ERR_MSX_NOT_OPENED;
    if ( findObject(NODE, id) >= 1 ) return ERR_INVALID_OBJECT_PARAMS;
    int err = checkID(id);
    if ( err ) return err;
    if ( addObject(NODE, id, MSX->Nobjects[NODE]+1) < 0 ) err = ERR_MEMORY;  // Insufficient memory

    int i = MSX->Nobjects[NODE]+1;
    if (i > MSX->Sizes[NODE]) err = MSXsetSize(MSX, NODE, i);
    MSX->Node[i].rpt = 0;
    MSX->Node[i].id = id;
    MSX->Node[i].tank = 0;
    MSX->Node[i].sources = NULL;
    MSX->Nobjects[NODE]++;
    return err;
    
}

//=============================================================================

int DLLEXPORT MSXaddTank(MSXproject MSX,char *id, double initialVolume, int mixModel, double volumeMix)
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
    if (!MSX->ProjectOpened) return ERR_MSX_NOT_OPENED;
    if ( findObject(TANK, id) >= 1 ) return ERR_INVALID_OBJECT_PARAMS;
    int err = checkID(id);
    if ( err ) return err;
    if ( addObject(TANK, id, MSX->Nobjects[TANK]+1) < 0 ) err = ERR_MEMORY;  // Insufficient memory
    if ( addObject(NODE, id, MSX->Nobjects[NODE]+1) < 0 ) err = ERR_MEMORY;  // Insufficient memory

    int i = MSX->Nobjects[TANK]+1;
    if (i > MSX->Sizes[TANK]) err = MSXsetSize(MSX, TANK, i);
    MSX->Tank[i].a = 1.0;
    MSX->Tank[i].v0 = initialVolume;
    MSX->Tank[i].mixModel = mixModel;
    MSX->Tank[i].vMix = volumeMix;
    MSX->Tank[i].id = id;
    MSX->Tank[i].node = MSX->Nobjects[NODE]+1;
    i = MSX->Nobjects[NODE]+1;
    if (i > MSX->Sizes[NODE]) err = MSXsetSize(MSX, NODE, i);
    MSX->Node[i].tank = MSX->Nobjects[TANK]+1;
    MSX->Node[i].rpt = 0;
    MSX->Node[i].id = id;
    MSX->Node[i].sources = NULL;
    MSX->Nobjects[NODE]++;
    MSX->Nobjects[TANK]++;
    return err;
}

//=============================================================================

int DLLEXPORT MSXaddReservoir(MSXproject MSX, char *id, double initialVolume, int mixModel, double volumeMix)
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
    if (!MSX->ProjectOpened) return ERR_MSX_NOT_OPENED;
    if ( findObject(TANK, id) >= 1 ) return ERR_INVALID_OBJECT_PARAMS;
    int err = checkID(id);
    if ( err ) return err;
    if ( addObject(TANK, id, MSX->Nobjects[TANK]+1) < 0 ) err = ERR_MEMORY;  // Insufficient memory
    if ( addObject(NODE, id, MSX->Nobjects[NODE]+1) < 0 ) err = ERR_MEMORY;  // Insufficient memory

    int i = MSX->Nobjects[TANK]+1;
    if (i > MSX->Sizes[TANK]) err = MSXsetSize(MSX, TANK, i);
    MSX->Tank[i].a = 0.0;
    MSX->Tank[i].v0 = initialVolume;
    MSX->Tank[i].mixModel = mixModel;
    MSX->Tank[i].vMix = volumeMix;
    MSX->Tank[i].id = id;
    MSX->Tank[i].node = MSX->Nobjects[NODE]+1;
    i = MSX->Nobjects[NODE]+1;
    if (i > MSX->Sizes[NODE]) err = MSXsetSize(MSX, NODE, i);
    MSX->Node[i].tank = MSX->Nobjects[TANK]+1;
    MSX->Node[i].rpt = 0;
    MSX->Node[i].id = id;
    MSX->Node[i].sources = NULL;
    MSX->Nobjects[NODE]++;
    MSX->Nobjects[TANK]++;
    return err;
}

//=============================================================================

int DLLEXPORT MSXaddLink(MSXproject MSX, char *id, char *startNode, char *endNode, double length, double diameter, double roughness)
/**
**  Purpose:
**    adds a link to the network.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**    id = The id
**    startNode = Start node id
**    endNode = End node id
**    length = length of the pipe
**    diameter = Diameter of the pipe
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
    if (!MSX->ProjectOpened) return ERR_MSX_NOT_OPENED;
    
    if ( findObject(LINK, id) >= 1 ) return ERR_INVALID_OBJECT_PARAMS;
    int err = checkID(id);
    if ( err ) return err;
    if ( addObject(LINK, id, MSX->Nobjects[LINK]+1) < 0 ) err = ERR_MEMORY;  // Insufficient memory

    // Check that the start and end nodes exist
    int x = findObject(NODE, startNode);
    if ( x <= 0 ) return ERR_NAME;
    int y = findObject(NODE, endNode);
    if ( y <= 0 ) return ERR_NAME;

    int i = MSX->Nobjects[LINK]+1;
    if (i > MSX->Sizes[LINK]) err = MSXsetSize(MSX, LINK, i);
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

//=============================================================================

int DLLEXPORT MSXaddOption(MSXproject MSX, int optionType, char *value)
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
    if (!MSX->ProjectOpened) return ERR_MSX_NOT_OPENED;
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

//=============================================================================

int DLLEXPORT MSXaddSpecies(MSXproject MSX, char *id, int type, int units, double aTol, double rTol)
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
    if (!MSX->ProjectOpened) return ERR_MSX_NOT_OPENED;
    if ( findObject(SPECIES, id) >= 1 ) return ERR_INVALID_OBJECT_PARAMS;

    int err = checkID(id);
    if ( err ) return err;
    if ( addObject(SPECIES, id, MSX->Nobjects[SPECIES]+1) < 0 ) err = ERR_MEMORY;  // Insufficient memory

    int i = MSX->Nobjects[SPECIES]+1;
    if (i > MSX->Sizes[SPECIES]) err = MSXsetSize(MSX, SPECIES, i);
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
    return err;
}

//=============================================================================

int DLLEXPORT MSXaddCoefficeint(MSXproject MSX, int type, char *id, double value)
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
    if (!MSX->ProjectOpened) return ERR_MSX_NOT_OPENED;
    int err = 0;
    if (type == PARAMETER) {
        if ( findObject(PATTERN, id) >= 1 ) return ERR_INVALID_OBJECT_PARAMS;
        err = checkID(id);
        if ( err ) return err;
        if ( addObject(PARAMETER, id, MSX->Nobjects[PARAMETER]+1) < 0 ) err = ERR_MEMORY;  // Insufficient memory
        int i = MSX->Nobjects[PARAMETER]+1;
        if (i > MSX->Sizes[PARAMETER]) err = MSXsetSize(MSX, PARAMETER, i);
        MSX->Param[i].id = id;
		MSX->Param[i].value = value;
        MSX->Nobjects[PARAMETER]++;
    }
    else if (type == CONSTANT) {
        if ( findObject(CONSTANT, id) >= 1 ) return ERR_INVALID_OBJECT_PARAMS;
        err = checkID(id);
        if ( err ) return err;
        if ( addObject(CONSTANT, id, MSX->Nobjects[CONSTANT]+1) < 0 ) err = ERR_MEMORY;  // Insufficient memory
        int i = MSX->Nobjects[CONSTANT]+1;
        if (i > MSX->Sizes[CONSTANT]) err = MSXsetSize(MSX, CONSTANT, i);
        MSX->Const[i].id = id;
		MSX->Const[i].value = value;
        MSX->Nobjects[CONSTANT]++;
    }
    else err = ERR_KEYWORD;
    return err;
}

//=============================================================================

int DLLEXPORT MSXaddTerm(MSXproject MSX, char *id, char *equation)
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
    if (!MSX->ProjectOpened) return ERR_MSX_NOT_OPENED;
    if ( findObject(TERM, id) >= 1 ) return ERR_INVALID_OBJECT_PARAMS;
    int err = 0;
    err = checkID(id);
    if ( err ) return err;
    if ( addObject(TERM, id, MSX->Nobjects[TERM]+1) < 0 ) err = ERR_MEMORY;  // Insufficient memory

    int i = MSX->Nobjects[TERM]+1;
    if (i > MSX->Sizes[TERM]) err = MSXsetSize(MSX, TERM, i);
    MSX->Term[i].id = id;
    MSX->Nobjects[TERM]++;
    MathExpr *expr = mathexpr_create(MSX, equation, getVariableCode);
    if ( expr == NULL ) return ERR_MATH_EXPR;
    MSX->Term[i].expr = expr;
    MSX->Term[i].equation = equation;
    //Updates the ivar values since they are dependent on the number of terms
    for (int m = 1; m < MSX->Nobjects[TERM]; m++) 
     {
        if (MSX->Term[m].expr->ivar != -1 &&
            MSX->Term[m].expr->ivar > MSX->Nobjects[SPECIES]+1)
            MSX->Term[m].expr->ivar++;
     }
    return err;
}

//=============================================================================

int DLLEXPORT MSXaddExpression(MSXproject MSX, int classType, int expressionType, char *species, char *equation)
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
    if (!MSX->ProjectOpened) return ERR_MSX_NOT_OPENED;
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

//=============================================================================

int DLLEXPORT MSXaddSource(MSXproject MSX, int sourceType, char *nodeId, char *speciesId, double strength, char *timePattern)
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
    if (!MSX->ProjectOpened) return ERR_MSX_NOT_OPENED;
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
        if ( source == NULL ) return ERR_MEMORY;
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

//=============================================================================

int DLLEXPORT MSXaddQuality(MSXproject MSX, char *type, char *speciesId, double value, char *id)
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
    if (!MSX->ProjectOpened) return ERR_MSX_NOT_OPENED;
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

//=============================================================================

int DLLEXPORT MSXaddParameter(MSXproject MSX, char *type, char *paramId, double value, char *id)
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
    if (!MSX->ProjectOpened) return ERR_MSX_NOT_OPENED;
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

//=============================================================================

int DLLEXPORT MSXsetReport(MSXproject MSX, char *reportType, char *id, int precision)
/**
**  Purpose:
**    adds a term to the MSX data struct.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**    reportType = Specifies what is being set in the report
**    id = Name for the object being set in the report
**    precision = Only used with SPECIES, and if none then just put 2
**
**  Output:
**    None
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    if (!MSX->ProjectOpened) return ERR_MSX_NOT_OPENED;
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

//=============================================================================

int DLLEXPORT MSXsetHydraulics(MSXproject MSX, float *demands, float *heads, float *flows)
/**
**  Purpose:
**    sets the demands, heads, and flows of in the data structure.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**    demands = An array of the demands, one for each node
**    heads = An array of the heads, one for each node
**    flows = An array of the flows, one for each link
**
**  Output:
**    None
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    if (!MSX->ProjectOpened) return ERR_MSX_NOT_OPENED;
    if (MSX->D == NULL || MSX->H == NULL || MSX->Q == NULL) return ERR_INIT;
    MSX->HydOffset = 1;
    int err = 0;    
    int nNodes = MSX->Nobjects[NODE];
    int nLinks = MSX->Nobjects[LINK];
    int i;
    //Since arrays are 0 to n-1 and the MSX arrays are indexed 1 to n
    for (i=0; i<nNodes; i++) {
        MSX->D[i+1] = demands[i];
        MSX->H[i+1] = heads[i];
    }
    for (i=0; i<nLinks; i++) MSX->Q[i+1] = flows[i];
    return err;
}

//=============================================================================

int DLLEXPORT MSXsetSize(MSXproject MSX, int type, int size)
/**
**  Purpose:
**    Sets the size of one of the MSXproject arrays.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**    type = object type code
**    size = number of objects.
**
**  Output:
**    None.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    if (!MSX->ProjectOpened) return ERR_MSX_NOT_OPENED;
    int err = 0;
    if (type < 0 || type >= MAX_OBJECTS) return ERR_INVALID_OBJECT_TYPE;
    if (size < 0) return ERR_INVALID_OBJECT_PARAMS;
    switch (type)
    {
    case SPECIES:
        if (MSX->Sizes[type] == 0) {
            MSX->Species = (Sspecies *) calloc(size+1, sizeof(Sspecies));
            if (MSX->Species == NULL) return ERR_MEMORY;
            MSX->C0 = (double *) calloc(size+1, sizeof(double));
            if (MSX->C0 == NULL) return ERR_MEMORY;
            // Allocate space in Node, Link, and Tank objects
            int i;
            for (i=1; i<=MSX->Sizes[NODE]; i++) {
                MSX->Node[i].c0 = (double *) calloc(size+1, sizeof(double));
                if (MSX->Node[i].c0 == NULL) return ERR_MEMORY;
            }
            for (i=1; i<=MSX->Sizes[LINK]; i++) {
                MSX->Link[i].c0 = (double *) calloc(size+1, sizeof(double));
                if (MSX->Link[i].c0 == NULL) return ERR_MEMORY;
            }
        }
        else {
            MSX->Species = (Sspecies *) realloc(MSX->Species, (size+1) * sizeof(Sspecies));
            if (MSX->Species == NULL) return ERR_MEMORY;
            MSX->C0 = (double *) realloc(MSX->C0, (size+1) * sizeof(double));
            if (MSX->C0 == NULL) return ERR_MEMORY;
            for (int j=1; j<=size; j++) MSX->C0[j] = 0;
            // Allocate space in Node, Link, and Tank objects
            int i;
            for (i=1; i<=MSX->Sizes[NODE]; i++) {
                MSX->Node[i].c0 = (double *) realloc(MSX->Node[i].c0, (size+1) * sizeof(double));
                if (MSX->Node[i].c0 == NULL) return ERR_MEMORY;
                for (int j=1; j<=size; j++) MSX->Node[i].c0[j] = 0;
            }
            for (i=1; i<=MSX->Sizes[LINK]; i++) {
                MSX->Link[i].c0 = (double *) realloc(MSX->Link[i].c0, (size+1) * sizeof(double));
                if (MSX->Link[i].c0 == NULL) return ERR_MEMORY;
                for (int j=1; j<=size; j++) MSX->Link[i].c0[j] = 0;
            }
        }
        break;
    case PARAMETER:
        if (MSX->Sizes[type] == 0) {
            MSX->Param = (Sparam *) calloc(size+1, sizeof(Sparam));
            if (MSX->Param == NULL) return ERR_MEMORY;
            // Allocate parameter in tank and link
            int i;
            for (i=1; i<=MSX->Sizes[TANK]; i++) {
                MSX->Tank[i].param = (double *) calloc(size+1, sizeof(double));
                if (MSX->Tank[i].param == NULL) return ERR_MEMORY;
            }
            for (i=1; i<=MSX->Sizes[LINK]; i++) {
                MSX->Link[i].param = (double *) calloc(size+1, sizeof(double));
                if (MSX->Link[i].param == NULL) return ERR_MEMORY;
            }
        }
        else {
            MSX->Param = (Sparam *) realloc(MSX->Param, (size+1) * sizeof(Sparam));
            if (MSX->Param == NULL) return ERR_MEMORY;
            // Allocate parameter in tank and link
            int i;
            for (i=1; i<=MSX->Sizes[TANK]; i++) {
                MSX->Tank[i].param = (double *) realloc(MSX->Tank[i].param, (size+1) * sizeof(double));
                if (MSX->Tank[i].param == NULL) return ERR_MEMORY;
                for (int j=1; j<=size; j++) MSX->Tank[i].param[j] = 0;
            }
            for (i=1; i<=MSX->Sizes[LINK]; i++) {
                MSX->Link[i].param = (double *) realloc(MSX->Link[i].param, (size+1) * sizeof(double));
                if (MSX->Link[i].param == NULL) return ERR_MEMORY;
                for (int j=1; j<=size; j++) MSX->Link[i].param[j] = 0;
            }
        }
        break;
    case CONSTANT:
        if (MSX->Sizes[type] == 0) {
            MSX->Const = (Sconst *) calloc(size+1, sizeof(Sconst));
            if (MSX->Const == NULL) return ERR_MEMORY;
        }
        else {
            MSX->Const = (Sconst *) realloc(MSX->Const, (size+1) * sizeof(Sconst));
            if (MSX->Const == NULL) return ERR_MEMORY;
        }
        break;
    case TERM:
        if (MSX->Sizes[type] == 0) {
            MSX->Term = (Sterm *) calloc(size+1, sizeof(Sterm));
            if (MSX->Term == NULL) return ERR_MEMORY;
        }
        else {
            MSX->Term = (Sterm *) realloc(MSX->Term, (size+1) * sizeof(Sterm));
            if (MSX->Term == NULL) return ERR_MEMORY;
        }
        break;
    case NODE:
        if (MSX->Sizes[type] == 0) {
            MSX->Node = (Snode *) calloc(size+1, sizeof(Snode));
            if (MSX->Node == NULL) return ERR_MEMORY;
            for (int i=1; i<size+1; i++) MSX->Node[i].c0 = NULL;
            if (MSX->Sizes[SPECIES] != 0) for (int i=1; i < size+1; i++) {
                if (MSX->Node[i].c0 != NULL) free(MSX->Node[i].c0);
                MSX->Node[i].c0 = (double *) calloc(MSX->Sizes[SPECIES]+1, sizeof(double));
            }
        }
        else {
            MSX->Node = (Snode *) realloc(MSX->Node, (size+1) * sizeof(Snode));
            if (MSX->Node == NULL) return ERR_MEMORY;
            for (int i=MSX->Sizes[type]+1; i<size+1; i++) MSX->Node[i].c0 = NULL;
            if (MSX->Sizes[SPECIES] != 0) for (int i=1; i < size+1; i++) {
                if (MSX->Node[i].c0 != NULL) free(MSX->Node[i].c0);
                MSX->Node[i].c0 = (double *) calloc(MSX->Sizes[SPECIES]+1, sizeof(double));
            }
        }
        break;
    case LINK:
        if (MSX->Sizes[type] == 0) {
            MSX->Link = (Slink *) calloc(size+1, sizeof(Slink));
            if (MSX->Link == NULL) return ERR_MEMORY;
            //Initialize the initial concentration
            for (int i=1; i<size+1; i++) MSX->Link[i].c0 = NULL;
            if (MSX->Sizes[SPECIES] != 0) for (int i=1; i < size+1; i++) {
                if (MSX->Link[i].c0 != NULL) free(MSX->Link[i].c0);
                MSX->Link[i].c0 = (double *) calloc(MSX->Sizes[SPECIES]+1, sizeof(double));
            }
            //Initialize the paramaters
            for (int i=1; i<size+1; i++) MSX->Link[i].param = NULL;
            if (MSX->Sizes[PARAMETER] != 0) for (int i=1; i < size+1; i++) {
                if (MSX->Link[i].param != NULL) free(MSX->Link[i].param);
                MSX->Link[i].param = (double *) calloc(MSX->Sizes[PARAMETER]+1, sizeof(double));
            }
        }
        else {
            MSX->Link = (Slink *) realloc(MSX->Link, (size+1) * sizeof(Slink));
            if (MSX->Link == NULL) return ERR_MEMORY;
            //Initialize the initial concentration
            for (int i=MSX->Sizes[type]+1; i<size+1; i++) MSX->Link[i].c0 = NULL;
            if (MSX->Sizes[SPECIES] != 0) for (int i=1; i < size+1; i++) {
                if (MSX->Link[i].c0 != NULL) free(MSX->Link[i].c0);
                MSX->Link[i].c0 = (double *) calloc(MSX->Sizes[SPECIES]+1, sizeof(double));
            }
            //Initialize the paramaters
            for (int i=MSX->Sizes[type]+1; i<size+1; i++) MSX->Link[i].param = NULL;
            if (MSX->Sizes[PARAMETER] != 0) for (int i=1; i < size+1; i++) {
                if (MSX->Link[i].param != NULL) free(MSX->Link[i].param);
                MSX->Link[i].param = (double *) calloc(MSX->Sizes[PARAMETER]+1, sizeof(double));
            }
        }
        break;
    case TANK:
        if (MSX->Sizes[type] == 0) {
            MSX->Tank = (Stank *) calloc(size+1, sizeof(Stank));
            if (MSX->Tank == NULL) return ERR_MEMORY;
            //Initialize the paramaters
            for (int i=1; i<size+1; i++) MSX->Tank[i].param = NULL;
            if (MSX->Sizes[PARAMETER] != 0) for (int i=1; i < size+1; i++) {
                if (MSX->Tank[i].param != NULL) free(MSX->Tank[i].param);
                MSX->Tank[i].param = (double *) calloc(MSX->Sizes[PARAMETER]+1, sizeof(double));
            }
        }
        else {
            MSX->Tank = (Stank *) realloc(MSX->Tank, (size+1) * sizeof(Stank));
            if (MSX->Tank == NULL) return ERR_MEMORY;
            //Initialize the paramaters
            for (int i=MSX->Sizes[type]+1; i<size+1; i++) MSX->Tank[i].param = NULL;
            if (MSX->Sizes[PARAMETER] != 0) for (int i=1; i < size+1; i++) {
                if (MSX->Tank[i].param != NULL) free(MSX->Tank[i].param);
                MSX->Tank[i].param = (double *) calloc(MSX->Sizes[PARAMETER]+1, sizeof(double));
            }
        }
        break;
    case PATTERN:
        if (MSX->Sizes[type] == 0) {
            MSX->Pattern = (Spattern *) calloc(size+1, sizeof(Spattern));
            if (MSX->Pattern == NULL) return ERR_MEMORY;
        }
        else {
            MSX->Pattern = (Spattern *) realloc(MSX->Pattern, (size+1) * sizeof(Spattern));
            if (MSX->Pattern == NULL) return ERR_MEMORY;
        }
        break;
    default:
        return ERR_INVALID_OBJECT_TYPE;
        break;
    }

    MSX->Sizes[type] = size;
    return err;
}

//=============================================================================

int  DLLEXPORT  MSXgetindex(MSXproject MSX, int type, char *id, int *index)
/**
**  Purpose:
**    retrieves the index of a named MSX object.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
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
        case NODE:      i = findObject(NODE, id);      break;
        case LINK:      i = findObject(LINK, id);      break;
        default:            return ERR_INVALID_OBJECT_TYPE;
    }
    if ( i < 1 ) return ERR_UNDEFINED_OBJECT_ID;
    *index = i;
    return 0;
}

//=============================================================================

int  DLLEXPORT  MSXgetIDlen(MSXproject MSX, int type, int index, int *len)
/**
**  Purpose:
**    retrieves the number of characters in the ID name of an MSX object.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
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

int  DLLEXPORT  MSXgetID(MSXproject MSX, int type, int index, char *id, int len)
/**
**  Purpose:
**    retrieves the name of an object given its index.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
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

int DLLEXPORT  MSXgetcount(MSXproject MSX, int type, int *count)
/**
**  Purpose:
**    retrieves the number of objects of a specific type.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
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

int DLLEXPORT  MSXgetspecies(MSXproject MSX, int index, int *type, char *units,
                             double *aTol, double * rTol)
/**
**  Purpose:
**    retrieves the attributes of a chemical species.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
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

int DLLEXPORT  MSXgetconstant(MSXproject MSX, int index, double *value)
/**
**  Purpose:
**    retrieves the value of a particular reaction constant.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
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

int DLLEXPORT MSXgetparameter(MSXproject MSX, int type, int index, int param, double *value)
/**
**  Purpose:
**    retrieves the value of a particular reaction parameter for a given pipe
**    or tank within the pipe network.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
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

int  DLLEXPORT MSXgetsource(MSXproject MSX, int node, int species, int *type, double *level,
                            int *pat)
/**
**  Purpose:
**    retrieves information on any external source of a particular chemical
**    species assigned to a specific node of the pipe network.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
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

int  DLLEXPORT  MSXgetpatternlen(MSXproject MSX, int pat, int *len)
/**
**  Purpose:
**    retrieves the number of time periods within a source time pattern.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
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

int  DLLEXPORT  MSXgetpatternvalue(MSXproject MSX, int pat, int period, double *value)
/**
**  Purpose:
**    retrieves the multiplier at a specific time period for a given
**    source time pattern.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
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

int  DLLEXPORT  MSXgetinitqual(MSXproject MSX, int type, int index, int species, double *value)
/**
**  Purpose:
**    retrieves the initial concentration of a particular chemical species
**    assigned to a specific node or link of the pipe network.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
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

int  DLLEXPORT  MSXgetQualityByIndex(MSXproject MSX, int type, int index, int species, double *value)
/**
**  Purpose:
**    retrieves the current concentration of a species at a particular node
**    or link of the pipe network.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
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

int  DLLEXPORT  MSXgetQualityByID(MSXproject MSX, int type, char *id, char *species, double *value)
/**
**  Purpose:
**    retrieves the current concentration of a species at a particular node
**    or link of the pipe network.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**    type = MSX_NODE (0) for a node or MSX_LINK (1) for a link;
**    id = id of the node or link of interest;.
**    species = id of the species of interest.
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
    if ( type == NODE )
    {
        int err = 0;
        int index = 0;
        int speciesIndex = 0;
        err = MSXgetindex(MSX, type, id, &index);
        if (err != 0) return err;
        err = MSXgetindex(MSX, SPECIES, species, &speciesIndex);
        if (err != 0) return err;
        *value = MSXqual_getNodeQual(MSX, index, speciesIndex);
    }
    else if ( type == LINK )
    {
        int err = 0;
        int index = 0;
        int speciesIndex = 0;
        err = MSXgetindex(MSX, type, id, &index);
        if (err != 0) return err;
        err = MSXgetindex(MSX, SPECIES, species, &speciesIndex);
        if (err != 0) return err;
        *value = MSXqual_getLinkQual(MSX, index, speciesIndex);
    }
    else return ERR_INVALID_OBJECT_TYPE;
    return 0;
}

//=============================================================================

int  DLLEXPORT  MSXsetconstant(MSXproject MSX, int index, double value)
/**
**  Purpose:
**    assigns a new value to a specific reaction constant.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
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

int  DLLEXPORT  MSXsetparameter(MSXproject MSX, int type, int index, int param, double value)
/**
**  Purpose:
**    assigns a value to a particular reaction parameter for a given pipe
**    or tank within the pipe network.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
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

int  DLLEXPORT  MSXsetinitqual(MSXproject MSX, int type, int index, int species, double value)
/**
**  Purpose:
**    assigns an initial concentration of a particular chemical species
**    to a specific node or link of the pipe network.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
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

int  DLLEXPORT  MSXsetsource(MSXproject MSX, int node, int species, int type, double level,
                             int pat)
/**
**  Purpose:
**    sets the attributes of an external source of a particular chemical
**    species to a specific node of the pipe network.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
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

int  DLLEXPORT  MSXsetpatternvalue(MSXproject MSX, int pat, int period, double value)
/**
**  Purpose:
**    assigns a new value to the multiplier for a specific time period in
**    a given time pattern.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
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

int  DLLEXPORT  MSXaddpattern(MSXproject MSX, char *id)
/**
**  Purpose:
**    adds a new MSX time pattern to the project.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
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
    int err = 0;
// --- check if a pattern with same id already exists

    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    if ( findObject(PATTERN, id) >= 1 ) return ERR_INVALID_OBJECT_PARAMS;
    err = checkID(id);
    if ( err ) return err;
    if ( addObject(PATTERN, id, MSX->Nobjects[PATTERN]+1) < 0 ) err = ERR_MEMORY;  // Insufficient memory

    int i = MSX->Nobjects[PATTERN]+1;
    if (i > MSX->Sizes[PATTERN]) err = MSXsetSize(MSX, PATTERN, i);
    MSX->Pattern[i].id = id;
    MSX->Pattern[i].length = 0;
    MSX->Pattern[i].first = NULL;
    MSX->Pattern[i].current = NULL;
    MSX->Nobjects[PATTERN]++;
    return err;
}

//=============================================================================

int  DLLEXPORT  MSXsetpattern(MSXproject MSX, int pat, double mult[], int len)
/**
**  Purpose:
**    Assigns a new set of multipliers to a given time pattern.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
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

int  DLLEXPORT  MSXstep(MSXproject MSX, long *t, long *tleft)
/**
**  Purpose:
**    advances the WQ simulation over a single time step.
**
**  Input:
**    none
**
**  Output:
**    *t = current simulation time at the end of the step (sec)
**    *tleft = time left in the simulation (sec)
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    if (MSX->HydOffset == 0) return ERR_HYD;
    return MSXqual_step(MSX, t, tleft);
}

//=============================================================================

int  DLLEXPORT  MSXgeterror(int code, char *msg, int len)
/**
**  Purpose:
**    retrieves text of an error message.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**    code = error code number
**    len = maximum length of string errmsg.
**
**  Output:
**    msg  = text of error message.
**
**  Returns:
**    an error code which is always 0.
*/
{
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

    if (code <= ERR_FIRST && code >= 400) strncpy(msg, Errmsg[code-400+24], len);
    if ( code <= ERR_FIRST || code >= ERR_MAX ) strncpy(msg, Errmsg[0], len);
    else strncpy(msg, Errmsg[code - ERR_FIRST], len);
    return 0;
}

//=============================================================================
