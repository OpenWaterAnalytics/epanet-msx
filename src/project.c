/******************************************************************************
**  TITLE:         PROJECT.C
**  DESCRIPTION:   project data manager used by the EPANET Multi-Species
**                 Extension toolkit.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**  VERSION:       1.00
**  LAST UPDATE:   10/5/06
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "typesmsx.h"
#define  EXTERN_MSX extern
#include "globals.h"
#include "utils.h"
#include "mempool.h"
#include "hash.h"

//  Local variables
//-----------------
static alloc_handle_t  *HashPool;           // Memory pool for hash tables
static HTtable  *Htable[MAX_OBJECTS];       // Hash tables for object ID names

static char * Errmsg[] =
    {"unknown error code",
     "insufficient memory available",
     "no EPANET data file supplied",
     "could not open MSX input file",
     "could not open hydraulic results file",
     "read error on hydraulic results file",
     "error in parsing MSX input file",
     "too few pipe reaction expressions",
     "too few tank reaction expressions",
     "could not open ODE solver",
     "could not open algebraic solver",
     "could not open binary results file",
     "read/write error on binary results file",
     "numerical integration error",
     "convergence error when balancing equilibrium expressions",
     "function call contains invalid parameter code",
     "could not find an open MSX project",
     "an MSX project is already open",
     "undefined time pattern in function call",
     "illegal numeric value in function call",
     "function call contains invalid node, link, pattern, or specie index"};

//  Imported functions
//--------------------
int    input_countMSobjects(void);
int    input_countENobjects(void);
int    input_readENdata(void);
int    input_readMSdata(void);

//  Exported functions
//--------------------
int    project_open(char *fname);
int    project_addObject(int type, char *id, int n);
int    project_findObject(int type, char *id);
char * project_findID(int type, char *id);
char * project_getErrmsg(int errcode);

//  Local functions
//-----------------
static void   setDefaults(void);
static int    convertUnits(void);
static int    createObjects(void);
static void   deleteObjects(void);
static int    createHashTables(void);
static void   deleteHashTables(void);

//=============================================================================

int  project_open(char *fname)
/*
**  Purpose:
**    opens an EPANET-MSX project.
**
**  Input:
**    fname = name of EPANET-MSX input file
**
**  Returns:
**    an error code (0 if no error)
*/
{
// --- initialize data to default values

    int errcode = 0;
    MSXProjectOpened = FALSE;
    MSXQualityOpened = FALSE;
    setDefaults();

// --- open the MSX input file

    strcpy(MSXInpFile.name, fname);
    if ((MSXInpFile.file = fopen(fname,"rt")) == NULL) return 302;

// --- create hash tables to look up object ID names

    ERRCODE(createHashTables());

// --- allocate memory for the required number of objects

    ERRCODE(input_countMSobjects());
    ERRCODE(input_countENobjects());
    ERRCODE(createObjects());

// --- read in the EPANET and MSX object data

    ERRCODE(input_readENdata());
    ERRCODE(input_readMSdata());

// --- convert user's units to internal units

    ERRCODE(convertUnits());

// --- close input file

    if ( MSXInpFile.file ) fclose(MSXInpFile.file);
    MSXInpFile.file = NULL;
    if ( !errcode ) MSXProjectOpened = TRUE;
    return errcode;
}

//=============================================================================

void project_close()
/*
**  Purpose:
**    closes the current EPANET-MSX project.
**
**  Input:
**    none
*/
{
    if ( MSXHydFile.file ) fclose(MSXHydFile.file);
    if ( MSXHydFile.mode == SCRATCH_FILE ) remove(MSXHydFile.name);
    if ( MSXOutFile.file ) fclose(MSXOutFile.file);
    if ( MSXOutFile.mode == SCRATCH_FILE ) remove(MSXOutFile.name);
    MSXHydFile.file = NULL;
    MSXOutFile.file = NULL;
    deleteObjects();
    deleteHashTables();
    MSXProjectOpened = FALSE;
}

//=============================================================================

int   project_addObject(int type, char *id, int n)
/*
**  Purpose:
**    adds an object ID to the project's hash tables.
**
**  Input:
**    type = object type
**    id   = object ID string
**    n    = object index.
**
**  Returns:
**    0 if object already added, 1 if not, -1 if hashing fails.
*/
{
    int  result;
    int  len;
    char *newID;

// --- do nothing if object already exists in a hash table

    if ( project_findObject(type, id) > 0 ) return 0;

// --- use memory from the hash tables' common memory pool to store
//     a copy of the object's ID string

    len = strlen(id) + 1;
    newID = (char *) Alloc(len*sizeof(char));
    strcpy(newID, id);

// --- insert object's ID into the hash table for that type of object

    result = HTinsert(Htable[type], newID, n);
    if ( result == 0 ) result = -1;
    return result;
}

//=============================================================================

int   project_findObject(int type, char *id)
/*
**  Purpose:
**    uses hash table to find index of an object with a given ID.
**
**  Input:
**    type = object type
**    id   = object ID.
**
**  Returns:
**    index of object with given ID, or -1 if ID not found.
*/
{
    return HTfind(Htable[type], id);
}

//=============================================================================

char * project_findID(int type, char *id)
/*
**  Purpose:
**    uses hash table to find address of given string entry.
**
**  Input:
**    type = object type
**    id   = ID name being sought.
**
**  Returns:
**    pointer to location where object's ID string is stored.
*/
{
    return HTfindKey(Htable[type], id);
}

//=============================================================================

char * project_getErrmsg(int errcode)
/*
**  Purpose:
**    gets the text of an error message.
**
**  Input:
**    errcode = error code.
**
**  Returns:
**    text of error message.
*/
{
    if ( errcode <= ERR_FIRST || errcode >= ERR_MAX ) return Errmsg[0];
    else return Errmsg[errcode - ERR_FIRST];
}

//=============================================================================

void setDefaults()
/*
**  Purpose:
**    assigns default values to project variables.
**
**  Input:
**    none.
*/
{
    int i;
    MSXHydFile.file = NULL;
    MSXHydFile.mode = USED_FILE;
    MSXOutFile.file = NULL;
    MSXOutFile.mode = SCRATCH_FILE;
    tmpnam(MSXOutFile.name);
    strcpy(MSXRptFile.name, "");
    strcpy(MSXTitle, "");
    MSXRptflag = 0;
    for (i=0; i<MAX_OBJECTS; i++) MSXNobjects[i] = 0;
    strcpy(MSXTitle, "");
    MSXUnitsflag = US;
    MSXFlowflag = GPM;
    MSXDefRtol = 0.001;
    MSXDefAtol = 0.01;
    MSXSolver = EUL;
    MSXAreaUnits = FT2;
    MSXRateUnits = DAYS;
    MSXQstep = 300;
    MSXRstep = 3600;
    MSXRstart = 0;
    MSXDur = 0;
    MSXNode = NULL;
    MSXLink = NULL;
    MSXTank = NULL;
    MSXD = NULL;
    MSXQ = NULL;
    MSXH = NULL;
    MSXSpecie = NULL;
    MSXTerm = NULL;
    MSXConst = NULL;
    MSXPattern = NULL;
}

//=============================================================================

int convertUnits()
/*
**  Purpose:
**    converts user's units to internal EPANET units.
**
**  Input:
**    none.
**
**  Returns:
**    an error code (0 if no error).
*/
{
// --- flow conversion factors (to cfs)
    double fcf[] = {1.0, GPMperCFS, MGDperCFS, IMGDperCFS, AFDperCFS,
                    LPSperCFS, LPMperCFS, MLDperCFS, CMHperCFS, CMDperCFS};

// --- rate time units conversion factors (to sec)
    double rcf[] = {1.0, 60.0, 3600.0, 86400.0};

    int i, m, errcode = 0;

// --- conversions for length & tank volume

    if ( MSXUnitsflag == US )
    {
        MSXUcf[LENGTH_UNITS] = 1.0;
        MSXUcf[DIAM_UNITS]   = 12.0;
        MSXUcf[VOL_UNITS]    = 1.0;
    }
    else
    {
        MSXUcf[LENGTH_UNITS] = MperFT;
        MSXUcf[DIAM_UNITS]   = 1000.0*MperFT;
        MSXUcf[VOL_UNITS]    = M3perFT3;
    }

// --- conversion for surface area

    MSXUcf[AREA_UNITS] = 1.0;
    switch (MSXAreaUnits)
    {
        case M2:  MSXUcf[AREA_UNITS] = M2perFT2;  break;
        case CM2: MSXUcf[AREA_UNITS] = CM2perFT2; break;
    }

// --- conversion for flow rate

    MSXUcf[FLOW_UNITS] = fcf[MSXFlowflag];
    MSXUcf[CONC_UNITS] = LperFT3;

// --- conversion for reaction rate time

    MSXUcf[RATE_UNITS] = rcf[MSXRateUnits];

// --- convert pipe diameter & length

    for (i=1; i<=MSXNobjects[LINK]; i++)
    {
        MSXLink[i].diam /= MSXUcf[DIAM_UNITS];
        MSXLink[i].len /=  MSXUcf[LENGTH_UNITS];
    }

// --- convert initial tank volumes

    for (i=1; i<=MSXNobjects[TANK]; i++)
    {
        MSXTank[i].v0 /= MSXUcf[VOL_UNITS];
    }

// --- assign default tolerances to species

    for (m=1; m<=MSXNobjects[SPECIE]; m++)
    {
        if ( MSXSpecie[m].rTol == 0.0 ) MSXSpecie[m].rTol = MSXDefRtol;
        if ( MSXSpecie[m].aTol == 0.0 ) MSXSpecie[m].aTol = MSXDefAtol;
    }
    return errcode;
}


//=============================================================================

int createObjects()
/*
**  Purpose:
**    creates multi-species data objects.
**
**  Input:
**    none.
**
**  Returns:
**    an error code (0 if no error).
*/
{
    int i;

// --- create nodes, links, & tanks

    MSXNode = (MSXSnode *) calloc(MSXNobjects[NODE]+1, sizeof(MSXSnode));
    MSXLink = (MSXSlink *) calloc(MSXNobjects[LINK]+1, sizeof(MSXSlink));
    MSXTank = (MSXStank *) calloc(MSXNobjects[TANK]+1, sizeof(MSXStank));

// --- create species, terms, parameters, constants & time patterns

    MSXSpecie = (MSXSspecie *) calloc(MSXNobjects[SPECIE]+1, sizeof(MSXSspecie));
    MSXTerm   = (MSXSterm *)  calloc(MSXNobjects[TERM]+1,  sizeof(MSXSterm));
    MSXParam  = (MSXSparam *) calloc(MSXNobjects[PARAMETER]+1, sizeof(MSXSparam));
    MSXConst  = (MSXSconst *) calloc(MSXNobjects[CONSTANT]+1, sizeof(MSXSconst));
    MSXPattern = (MSXSpattern *) calloc(MSXNobjects[TIME_PATTERN]+1, sizeof(MSXSpattern));

// --- create arrays for demands, heads, & flows

    MSXD = (float *) calloc(MSXNobjects[NODE]+1, sizeof(float));
    MSXH = (float *) calloc(MSXNobjects[NODE]+1, sizeof(float));
    MSXQ = (float *) calloc(MSXNobjects[LINK]+1, sizeof(float));

// --- create arrays for current & initial concen. of each specie for each node

    for (i=1; i<=MSXNobjects[NODE]; i++)
    {
        MSXNode[i].c = (double *) calloc(MSXNobjects[SPECIE]+1, sizeof(double));
        MSXNode[i].c0 = (double *) calloc(MSXNobjects[SPECIE]+1, sizeof(double));
        MSXNode[i].rpt = 0;
    }

// --- create arrays for init. concen. & kinetic parameter values for each link

    for (i=1; i<=MSXNobjects[LINK]; i++)
    {
        MSXLink[i].c0 = (double *) calloc(MSXNobjects[SPECIE]+1, sizeof(double));
        MSXLink[i].param = (double *) calloc(MSXNobjects[PARAMETER]+1, sizeof(double));
        MSXLink[i].rpt = 0;
    }

// --- create arrays for kinetic parameter values & current concen. for each tank

    for (i=1; i<=MSXNobjects[TANK]; i++)
    {
        MSXTank[i].param = (double *) calloc(MSXNobjects[PARAMETER]+1, sizeof(double));
        MSXTank[i].c = (double *) calloc(MSXNobjects[SPECIE]+1, sizeof(double));
    }

// --- initialize contents of each time pattern object

    for (i=1; i<=MSXNobjects[TIME_PATTERN]; i++)
    {
        MSXPattern[i].length = 0;
        MSXPattern[i].first = NULL;
        MSXPattern[i].current = NULL;
    }

// --- initialize reaction rate & equil. formulas for each specie

    for (i=1; i<=MSXNobjects[SPECIE]; i++)
    {
        MSXSpecie[i].pipeExpr     = NULL;
        MSXSpecie[i].tankExpr     = NULL;
        MSXSpecie[i].pipeExprType = NO_EXPR;
        MSXSpecie[i].tankExprType = NO_EXPR;
        MSXSpecie[i].precision    = 2;
        MSXSpecie[i].rpt = 0;
    }

// --- initialize math expressions for each intermediate term

    for (i=1; i<=MSXNobjects[TERM]; i++) MSXTerm[i].expr = NULL;
    return 0;
}

//=============================================================================

void deleteObjects()
/*
**  Purpose:
**    deletes multi-species data objects.
**
**  Input:
**    none.
*/
{
    int i;
    SnumList *listItem;

// --- free memory used by nodes, links, and tanks

    if (MSXNode) for (i=1; i<=MSXNobjects[NODE]; i++)
    {
        FREE(MSXNode[i].c);
        FREE(MSXNode[i].c0);
    }
    if (MSXLink) for (i=1; i<=MSXNobjects[LINK]; i++)
    {
        FREE(MSXLink[i].c0);
        FREE(MSXLink[i].param);
    }
    if (MSXTank) for (i=1; i<=MSXNobjects[TANK]; i++)
    {
        FREE(MSXTank[i].param);
        FREE(MSXTank[i].c);
    }

// --- free memory used by time patterns

    if (MSXPattern) for (i=1; i<=MSXNobjects[TIME_PATTERN]; i++)
    {
        listItem = MSXPattern[i].first;
        while (listItem)
        {
            MSXPattern[i].first = listItem->next;
            free(listItem);
            listItem = MSXPattern[i].first;
        }
    }

// --- free memory used for hydraulics results

    FREE(MSXD);
    FREE(MSXH);
    FREE(MSXQ);

// --- delete all nodes, links, and tanks

    FREE(MSXNode);
    FREE(MSXLink);
    FREE(MSXTank);

// --- free memory used by reaction rate & equilibrium expressions

    if (MSXSpecie) for (i=1; i<=MSXNobjects[SPECIE]; i++)
    {
    // --- free the specie's tank expression only if it doesn't
    //     already point to the specie's pipe expression
        if ( MSXSpecie[i].tankExpr != MSXSpecie[i].pipeExpr )
        {
            mathexpr_delete(MSXSpecie[i].tankExpr);
        }
        mathexpr_delete(MSXSpecie[i].pipeExpr);
    }

// --- delete all species, parameters, and constants

    FREE(MSXSpecie);
    FREE(MSXParam);
    FREE(MSXConst);

// --- free memory used by intermediate terms

    if (MSXTerm) for (i=1; i<=MSXNobjects[TERM]; i++) mathexpr_delete(MSXTerm[i].expr);
    FREE(MSXTerm);
}

//=============================================================================

int createHashTables()
/*
**  Purpose:
**    allocates memory for object ID hash tables.
**
**  Input:
**    none.
**
**  Returns:
**    an error code (0 if no error).
*/
{   int j;

// --- create a hash table for each type of object

    for (j = 0; j < MAX_OBJECTS ; j++)
    {
         Htable[j] = HTcreate();
         if ( Htable[j] == NULL ) return 101;
    }

// --- initialize the memory pool used to store object ID's

    HashPool = AllocInit();
    if ( HashPool == NULL ) return 101;
	return 0;
}

//=============================================================================

void deleteHashTables()
/*
**  Purpose:
**    frees memory allocated for object ID hash tables.
**
**  Input:
**    none.
*/
{
    int j;

// --- free the hash tables

    for (j = 0; j < MAX_OBJECTS; j++)
    {
        if ( Htable[j] != NULL ) HTfree(Htable[j]);
    }

// --- free the object ID memory pool

    if ( HashPool )
    {
        AllocSetPool(HashPool);
        AllocFreePool();
    }
}
