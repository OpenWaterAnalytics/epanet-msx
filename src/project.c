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
#include "hashmsx.h"

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
     "could not open algebraic solver"
     "could not open binary results file",
     "read/write error on binary results file",
     "numerical integration error"};

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
    ProjectOpened = FALSE;
    QualityOpened = FALSE;
    setDefaults();

// --- open the MSX input file

    strcpy(InpFile.name, fname);
    if ((InpFile.file = fopen(fname,"rt")) == NULL) return 302;

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

    if ( InpFile.file ) fclose(InpFile.file);
    if ( !errcode ) ProjectOpened = TRUE;
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
    if ( HydFile.file ) fclose(HydFile.file);
    if ( HydFile.mode == SCRATCH_FILE ) remove(HydFile.name);
    if ( OutFile.file ) fclose(OutFile.file);
    if ( OutFile.mode == SCRATCH_FILE ) remove(OutFile.name);
    HydFile.file = NULL;
    OutFile.file = NULL;
    deleteObjects();
    deleteHashTables();
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
    HydFile.file = NULL;
    HydFile.mode = USED_FILE;
    OutFile.file = NULL;
    OutFile.mode = SCRATCH_FILE;
    tmpnam(OutFile.name);
    strcpy(RptFile.name, "");
    strcpy(Title, "");
    Rptflag = 0;
    for (i=0; i<MAX_OBJECTS; i++) Nobjects[i] = 0;
    strcpy(Title, "");
    Unitsflag = US;
    Flowflag = GPM;
    DefRtol = 0.001;
    DefAtol = 0.01;
    Solver = EUL;
    AreaUnits = FT2;
    RateUnits = DAYS;
    Qstep = 300;
    Rstep = 3600;
    Rstart = 0;
    Dur = 0;
    Node = NULL;
    Link = NULL;
    Tank = NULL;
    D = NULL;
    Q = NULL;
    H = NULL;
    Specie = NULL;
    Term = NULL;
    Const = NULL;
    Pattern = NULL;
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

    if ( Unitsflag == US )
    {
        Ucf[LENGTH_UNITS] = 1.0;
        Ucf[DIAM_UNITS]   = 12.0;
        Ucf[VOL_UNITS]    = 1.0;
    }
    else
    {
        Ucf[LENGTH_UNITS] = MperFT;
        Ucf[DIAM_UNITS]   = 1000.0*MperFT;
        Ucf[VOL_UNITS]    = M3perFT3;
    }

// --- conversion for surface area

    Ucf[AREA_UNITS] = 1.0;
    switch (AreaUnits)
    {
        case M2:  Ucf[AREA_UNITS] = M2perFT2;  break;
        case CM2: Ucf[AREA_UNITS] = CM2perFT2; break;
    }

// --- conversion for flow rate

    Ucf[FLOW_UNITS] = fcf[Flowflag];
    Ucf[CONC_UNITS] = LperFT3;

// --- conversion for reaction rate time

    Ucf[RATE_UNITS] = rcf[RateUnits];

// --- convert pipe diameter & length

    for (i=1; i<=Nobjects[LINK]; i++)
    {
        Link[i].diam /= Ucf[DIAM_UNITS];
        Link[i].len /=  Ucf[LENGTH_UNITS];
    }

// --- convert initial tank volumes

    for (i=1; i<=Nobjects[TANK]; i++)
    {
        Tank[i].v0 /= Ucf[VOL_UNITS];
    }

// --- assign default tolerances to species

    for (m=1; m<=Nobjects[SPECIE]; m++)
    {
        if ( Specie[m].rTol == 0.0 ) Specie[m].rTol = DefRtol;
        if ( Specie[m].aTol == 0.0 ) Specie[m].aTol = DefAtol;
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

    Node = (Snode *) calloc(Nobjects[NODE]+1, sizeof(Snode));
    Link = (Slink *) calloc(Nobjects[LINK]+1, sizeof(Slink));
    Tank = (Stank *) calloc(Nobjects[TANK]+1, sizeof(Stank));

// --- create species, terms, parameters, constants & time patterns

    Specie = (Sspecie *) calloc(Nobjects[SPECIE]+1, sizeof(Sspecie));
    Term   = (Sterm *)  calloc(Nobjects[TERM]+1,  sizeof(Sterm));
    Param  = (Sparam *) calloc(Nobjects[PARAMETER]+1, sizeof(Sparam));
    Const  = (Sconst *) calloc(Nobjects[CONSTANT]+1, sizeof(Sconst));
    Pattern = (Spattern *) calloc(Nobjects[TIME_PATTERN]+1, sizeof(Spattern));

// --- create arrays for demands, heads, & flows

    D = (float *) calloc(Nobjects[NODE]+1, sizeof(float));
    H = (float *) calloc(Nobjects[NODE]+1, sizeof(float));
    Q = (float *) calloc(Nobjects[LINK]+1, sizeof(float));

// --- create arrays for current & initial concen. of each specie for each node

    for (i=1; i<=Nobjects[NODE]; i++)
    {
        Node[i].c = (double *) calloc(Nobjects[SPECIE]+1, sizeof(double));
        Node[i].c0 = (double *) calloc(Nobjects[SPECIE]+1, sizeof(double));
        Node[i].rpt = 0;
    }

// --- create arrays for init. concen. & kinetic parameter values for each link

    for (i=1; i<=Nobjects[LINK]; i++)
    {
        Link[i].c0 = (double *) calloc(Nobjects[SPECIE]+1, sizeof(double));
        Link[i].param = (double *) calloc(Nobjects[PARAMETER]+1, sizeof(double));
        Link[i].rpt = 0;
    }

// --- create arrays for kinetic parameter values & current concen. for each tank

    for (i=1; i<=Nobjects[TANK]; i++)
    {
        Tank[i].param = (double *) calloc(Nobjects[PARAMETER]+1, sizeof(double));
        Tank[i].c = (double *) calloc(Nobjects[SPECIE]+1, sizeof(double));
    }

// --- initialize contents of each time pattern object

    for (i=1; i<=Nobjects[TIME_PATTERN]; i++)
    {
        Pattern[i].length = 0;
        Pattern[i].first = NULL;
        Pattern[i].current = NULL;
    }

// --- initialize reaction rate & equil. formulas for each specie

    for (i=1; i<=Nobjects[SPECIE]; i++)
    {
        Specie[i].pipeExpr     = NULL;
        Specie[i].tankExpr     = NULL;
        Specie[i].pipeExprType = NO_EXPR;
        Specie[i].tankExprType = NO_EXPR;
        Specie[i].precision    = 2;
        Specie[i].rpt = 0;
    }

// --- initialize math expressions for each intermediate term

    for (i=1; i<=Nobjects[TERM]; i++) Term[i].expr = NULL;
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

    if (Node) for (i=1; i<=Nobjects[NODE]; i++)
    {
        FREE(Node[i].c);
        FREE(Node[i].c0);
    }
    if (Link) for (i=1; i<=Nobjects[LINK]; i++)
    {
        FREE(Link[i].c0);
        FREE(Link[i].param);
    }
    if (Tank) for (i=1; i<=Nobjects[TANK]; i++)
    {
        FREE(Tank[i].param);
        FREE(Tank[i].c);
    }

// --- free memory used by time patterns

    if (Pattern) for (i=1; i<=Nobjects[TIME_PATTERN]; i++)
    {
        listItem = Pattern[i].first;
        while (listItem)
        {
            Pattern[i].first = listItem->next;
            free(listItem);
            listItem = Pattern[i].first;
        }
    }

// --- free memory used for hydraulics results

    FREE(D);
    FREE(H);
    FREE(Q);

// --- delete all nodes, links, and tanks

    FREE(Node);
    FREE(Link);
    FREE(Tank);

// --- free memory used by reaction rate & equilibrium expressions

    if (Specie) for (i=1; i<=Nobjects[SPECIE]; i++)
    {
    // --- free the specie's tank expression only if it doesn't
    //     already point to the specie's pipe expression
        if ( Specie[i].tankExpr != Specie[i].pipeExpr )
        {
            mathexpr_delete(Specie[i].tankExpr);
        }
        mathexpr_delete(Specie[i].pipeExpr);
    }

// --- delete all species, parameters, and constants

    FREE(Specie);
    FREE(Param);
    FREE(Const);

// --- free memory used by intermediate terms

    if (Term) for (i=1; i<=Nobjects[TERM]; i++) mathexpr_delete(Term[i].expr);
    FREE(Term);
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
