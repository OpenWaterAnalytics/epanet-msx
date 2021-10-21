/******************************************************************************
**  MODULE:        MSXBJECTS.C
**  PROJECT:       EPANET-MSX
**  DESCRIPTION:   Functions to mutate the hash table and do work with objects.         
**  COPYRIGHT:     Copyright (C) 2007 Feng Shang, Lewis Rossman, and James Uber.
**                 All Rights Reserved. See license information in LICENSE.TXT.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**                 K. Arrowood, Xylem intern
**  VERSION:       1.1.00
**  LAST UPDATE:   Refer to git history
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "msxtypes.h"
#include "hash.h"
#include "msxobjects.h"
#include "msxdict.h"
#include "msxutils.h"

//  Local variables
//-----------------
alloc_handle_t  *HashPool;           // Memory pool for hash tables
HTtable  *Htable[MAX_OBJECTS];       // Hash tables for object ID names

//=============================================================================

int addObject(int type, char *id, int n)
/**
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

    if ( findObject(type, id) > 0 ) return 0;

// --- use memory from the hash tables' common memory pool to store
//     a copy of the object's ID string

    len = (int) strlen(id) + 1;
    newID = (char *) Alloc(len*sizeof(char));
    strcpy(newID, id);

// --- insert object's ID into the hash table for that type of object

    result = (int) HTinsert(Htable[type], newID, n);
    if ( result == 0 ) result = -1;
    return result;
}

//=============================================================================

int findObject(int type, char *id)
/**
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

char * findID(int type, char *id)
/**
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

int createHashTables()
/**
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
         if ( Htable[j] == NULL ) return ERR_MEMORY;
    }

// --- initialize the memory pool used to store object ID's

    HashPool = AllocInit();
    if ( HashPool == NULL ) return ERR_MEMORY;
    return 0;
}

//=============================================================================

void deleteHashTables()
/**
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

//=============================================================================

int setDefaults(MSXproject MSX)
/**
**  Purpose:
**    assigns default values to project variables.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
*/
{
    int i;
    MSX->ProjectOpened = 1;     //Should only be called from opening a project
    MSX->RptFile.file = NULL;                                                   //(LR-11/20/07)
    MSX->HydFile.file = NULL;
    MSX->HydFile.mode = USED_FILE;
    MSX->OutFile.file = NULL;
    MSX->OutFile.mode = SCRATCH_FILE;
    MSX->TmpOutFile.file = NULL;
    MSXutils_getTempName(MSX->OutFile.name);                                    //1.1.00
    MSXutils_getTempName(MSX->TmpOutFile.name);                                 //1.1.00
    strcpy(MSX->RptFile.name, "");
    strcpy(MSX->Title, "");
    MSX->Rptflag = 0;
    for (i=0; i<MAX_OBJECTS; i++) MSX->Nobjects[i] = 0;
    for (i=0; i<MAX_OBJECTS; i++) MSX->Sizes[i] = 0;
    MSX->Unitsflag = US;
    MSX->Flowflag = GPM;
    MSX->Statflag = SERIES;
    MSX->DefRtol = 0.001;
    MSX->DefAtol = 0.01;
    MSX->Solver = EUL;
    MSX->Coupling = NO_COUPLING;
    MSX->Compiler = NO_COMPILER;                                                //1.1.00
    MSX->AreaUnits = FT2;
    MSX->RateUnits = DAYS;
    MSX->Qstep = 300;
    MSX->Rstep = 3600;
    MSX->Rstart = 0;
    MSX->Dur = 0;
    MSX->Node = NULL;
    MSX->Link = NULL;
    MSX->Tank = NULL;
    MSX->D = NULL;
    MSX->Q = NULL;
    MSX->H = NULL;
    MSX->Species = NULL;
    MSX->Term = NULL;
    MSX->Const = NULL;
    MSX->Pattern = NULL;
    MSX->K = NULL;                                                              //1.1.00
    MSX->C0 = NULL;                                                              //1.1.00
    MSX->C1 = NULL;                                                              //1.1.00
    MSX->Adjlist = NULL;
    MSX->Param = NULL;
    // MSX->Pstart = 0;
    // MSX->Pstep = 0;
    return 0;
}

//=============================================================================

int getVariableCode(MSXproject MSX, char *id)
/**
**  Purpose:
**    finds the index assigned to a species, intermediate term,
**    parameter, or constant that appears in a math expression.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**    id = ID name being sought
**
**  Returns:
**    index of the symbolic variable or term named id.
**
**  Note:
**    Variables are assigned consecutive code numbers starting from 1 
**    and proceeding through each Species, Term, Parameter and Constant.
*/
{
    int j = findObject(SPECIES, id);
    if ( j >= 1 ) return j;
    j = findObject(TERM, id);
    if ( j >= 1 ) return MSX->Nobjects[SPECIES] + j;
    j = findObject(PARAMETER, id);
    if ( j >= 1 ) return MSX->Nobjects[SPECIES] + MSX->Nobjects[TERM] + j;
    j = findObject(CONSTANT, id);
    if ( j >= 1 ) return MSX->Nobjects[SPECIES] + MSX->Nobjects[TERM] + 
                         MSX->Nobjects[PARAMETER] + j;
    j = MSXutils_findmatch(id, HydVarWords);
    if ( j >= 1 ) return MSX->Nobjects[SPECIES] + MSX->Nobjects[TERM] + 
                         MSX->Nobjects[PARAMETER] + MSX->Nobjects[CONSTANT] + j;
    return -1;
}

//=============================================================================

int  buildadjlists(MSXproject MSX)   //from epanet2.2 for node sorting in WQ routing
/**
**--------------------------------------------------------------
** Input:
**    MSX = the underlying MSXproject data struct.
** Output:  returns error code
** Purpose: builds linked list of links adjacent to each node
**--------------------------------------------------------------
*/
{
    int       i, j, k;
    int       errcode = 0;
    Padjlist  alink;

    // Create an array of adjacency lists
    freeadjlists(MSX);
    MSX->Adjlist = (Padjlist*)calloc(MSX->Nobjects[NODE]+1, sizeof(Padjlist));
    if (MSX->Adjlist == NULL) return 101;
    for (i = 0; i <= MSX->Nobjects[NODE]; i++) 
        MSX->Adjlist[i] = NULL;

    // For each link, update adjacency lists of its end nodes
    for (k = 1; k <= MSX->Nobjects[LINK]; k++)
    {
        i = MSX->Link[k].n1;
        j = MSX->Link[k].n2;

        // Include link in start node i's list
        alink = (struct Sadjlist*) malloc(sizeof(struct Sadjlist));
        if (alink == NULL)
        {
            errcode = 101;
            break;
        }
        alink->node = j;
        alink->link = k;
        alink->next = MSX->Adjlist[i];
        MSX->Adjlist[i] = alink;

        // Include link in end node j's list
        alink = (struct Sadjlist*) malloc(sizeof(struct Sadjlist));
        if (alink == NULL)
        {
            errcode = 101;
            break;
        }
        alink->node = i;
        alink->link = k;
        alink->next = MSX->Adjlist[j];
        MSX->Adjlist[j] = alink;
    }
    if (errcode) freeadjlists(MSX);
    return errcode;
}

//=============================================================================

void  freeadjlists(MSXproject MSX)            //from epanet2.2 for node sorting in WQ routing
/**
**--------------------------------------------------------------
** Input:
**    MSX = the underlying MSXproject data struct.
** Output:  none
** Purpose: frees memory used for nodal adjacency lists
**--------------------------------------------------------------
*/
{
    int   i;
    Padjlist alink;

    if (MSX->Adjlist == NULL) return;
    for (i = 0; i <= MSX->Nobjects[NODE]; i++)
    {
        for (alink = MSX->Adjlist[i]; alink != NULL; alink = MSX->Adjlist[i])
        {
            MSX->Adjlist[i] = alink->next;
            free(alink);
        }
    }
    free(MSX->Adjlist);
}

//=============================================================================

int convertUnits(MSXproject MSX)
/**
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

    if ( MSX->Unitsflag == US )
    {
        MSX->Ucf[LENGTH_UNITS] = 1.0;
        MSX->Ucf[DIAM_UNITS]   = 12.0;
        MSX->Ucf[VOL_UNITS]    = 1.0;
    }
    else
    {
        MSX->Ucf[LENGTH_UNITS] = MperFT;
        MSX->Ucf[DIAM_UNITS]   = 1000.0*MperFT;
        MSX->Ucf[VOL_UNITS]    = M3perFT3;
    }

// --- conversion for surface area

    MSX->Ucf[AREA_UNITS] = 1.0;
    switch (MSX->AreaUnits)
    {
        case M2:  MSX->Ucf[AREA_UNITS] = M2perFT2;  break;
        case CM2: MSX->Ucf[AREA_UNITS] = CM2perFT2; break;
    }

// --- conversion for flow rate

    MSX->Ucf[FLOW_UNITS] = fcf[MSX->Flowflag];
    MSX->Ucf[CONC_UNITS] = LperFT3;

// --- conversion for reaction rate time

    MSX->Ucf[RATE_UNITS] = rcf[MSX->RateUnits];

// --- convert pipe diameter & length

    for (i=1; i<=MSX->Nobjects[LINK]; i++)
    {
        MSX->Link[i].diam /= MSX->Ucf[DIAM_UNITS];
        MSX->Link[i].len /=  MSX->Ucf[LENGTH_UNITS];
    }

// --- convert initial tank volumes

    for (i=1; i<=MSX->Nobjects[TANK]; i++)
    {
        MSX->Tank[i].v0 /= MSX->Ucf[VOL_UNITS];
        MSX->Tank[i].vMix /= MSX->Ucf[VOL_UNITS];
    }

// --- assign default tolerances to species

    for (m=1; m<=MSX->Nobjects[SPECIES]; m++)
    {
        if ( MSX->Species[m].rTol == 0.0 ) MSX->Species[m].rTol = MSX->DefRtol;
        if ( MSX->Species[m].aTol == 0.0 ) MSX->Species[m].aTol = MSX->DefAtol;
    }
    return errcode;
}

//=============================================================================

void deleteObjects(MSXproject MSX)
/**
**  Purpose:
**    deletes multi-species data objects.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
*/
{
    int i;
    SnumList *listItem;
    Psource  source;                                                           //ttaxon - 9/7/10

// --- free memory used by nodes, links, and tanks

    if (MSX->Node) for (i=1; i<=MSX->Nobjects[NODE]; i++)
    {
        FREE(MSX->Node[i].c);
        FREE(MSX->Node[i].c0);

        // --- free memory used by water quality sources                       //ttaxon - 9/7/10

        if(MSX->Node[i].sources)
        { 
            source = MSX->Node[i].sources; 
            while (source != NULL)
	        { 
                MSX->Node[i].sources = source->next; 
                FREE(source); 
                source = MSX->Node[i].sources; 
            } 
        }
    }
    if (MSX->Link) for (i=1; i<=MSX->Nobjects[LINK]; i++)
    {
        FREE(MSX->Link[i].c0);
        FREE(MSX->Link[i].param);
        FREE(MSX->Link[i].reacted);
    }
    if (MSX->Tank) for (i=1; i<=MSX->Nobjects[TANK]; i++)
    {
        FREE(MSX->Tank[i].param);
        FREE(MSX->Tank[i].c);
        FREE(MSX->Tank[i].reacted);
    }

// --- free memory used by time patterns

    if (MSX->Pattern) for (i=1; i<=MSX->Nobjects[PATTERN]; i++)
    {
        listItem = MSX->Pattern[i].first;
        while (listItem)
        {
            MSX->Pattern[i].first = listItem->next;
            free(listItem);
            listItem = MSX->Pattern[i].first;
        }
    }
    FREE(MSX->Pattern);

// --- free memory used for hydraulics results

    FREE(MSX->D);
    FREE(MSX->H);
    FREE(MSX->Q);
    FREE(MSX->C0);

// --- delete all nodes, links, and tanks

    FREE(MSX->Node);
    FREE(MSX->Link);
    FREE(MSX->Tank);

// --- free memory used by reaction rate & equilibrium expressions

    if (MSX->Species) for (i=1; i<=MSX->Nobjects[SPECIES]; i++)
    {
    // --- free the species tank expression only if it doesn't
    //     already point to the species pipe expression
        if ( MSX->Species[i].tankExpr != MSX->Species[i].pipeExpr )
        {
            mathexpr_delete(MSX->Species[i].tankExpr);
        }
        mathexpr_delete(MSX->Species[i].pipeExpr);
    }

// --- delete all species, parameters, and constants

    FREE(MSX->Species);
    FREE(MSX->Param);
    FREE(MSX->Const);
    FREE(MSX->K);                                                               //1.1.00

// --- free memory used by intermediate terms

    if (MSX->Term) for (i=1; i<=MSX->Nobjects[TERM]; i++)
        mathexpr_delete(MSX->Term[i].expr);
    FREE(MSX->Term);
}

//=============================================================================

void freeIDs(MSXproject MSX)
/**
**  Purpose:
**    frees the IDs of all of the objects.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
*/
{
    int i;
    for (i=1; i<=MSX->Nobjects[NODE]; i++) FREE(MSX->Node[i].id);
    for (i=1; i<=MSX->Nobjects[TANK]; i++) FREE(MSX->Tank[i].id);
    for (i=1; i<=MSX->Nobjects[LINK]; i++) FREE(MSX->Link[i].id);
    for (i=1; i<=MSX->Nobjects[SPECIES]; i++) FREE(MSX->Species[i].id);
    for (i=1; i<=MSX->Nobjects[PARAMETER]; i++) FREE(MSX->Param[i].id);
    for (i=1; i<=MSX->Nobjects[CONSTANT]; i++) FREE(MSX->Const[i].id);
    for (i=1; i<=MSX->Nobjects[TERM]; i++) {
        FREE(MSX->Term[i].id);
        FREE(MSX->Term[i].equation);
    }
    for (i=1; i<=MSX->Nobjects[PATTERN]; i++) FREE(MSX->Pattern[i].id);
}

//=============================================================================

int checkCyclicTerms(MSXproject MSX, double **TermArray)                                                         //1.1.00
/**
**  Purpose:
**    checks for cyclic references in Term expressions (e.g., T1 = T2 + T3
**    and T3 = T2/T1)
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**
**  Returns:
**    1 if cyclic reference found or 0 if none found.
*/
{
    int i, j, n;
    char msg[MAXMSG+1];

    n = MSX->Nobjects[TERM];
    for (i=1; i<n; i++)
    {
        for (j=1; j<=n; j++) TermArray[0][j] = 0.0;
        if ( traceTermPath(i, i, n, TermArray) )
        {
            sprintf(msg, "Error 410 - term %s contains a cyclic reference.",
                         MSX->Term[i].id);
           // ENwriteline(msg);
            printf("%s\n", msg);
            return 1;
        }
    }
    return 0;
}

//=============================================================================

int traceTermPath(int i, int istar, int n, double **TermArray)                                     //1.1.00
/**
**  Purpose:
**    checks if Term[istar] is in the path of terms that appear when evaluating
**    Term[i]
**
**  Input:
**    i = index of term whose expressions are to be traced
**    istar = index of term being searched for
**    n = total number of terms
**
**  Returns:
**    1 if term istar found; 0 if not found.
*/
{
    int j;
    if ( TermArray[0][i] == 1.0 ) return 0;
    TermArray[0][i] = 1.0;
    for (j=1; j<=n; j++)
    {
        if ( TermArray[i][j] == 0.0 ) continue;
        if ( j == istar ) return 1;
        else if ( traceTermPath(j, istar, n, TermArray) ) return 1;
    }
    return 0;
}

//=============================================================================

int finishInit(MSXproject MSX)
/**
**  Purpose:
**    finishes setting up the MSX data struct.
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

    MSX->K = (double *) calloc(MSX->Nobjects[CONSTANT]+1, sizeof(double));  //1.1.00
    // --- create arrays for demands, heads, & flows
    MSX->D = (float *) calloc(MSX->Nobjects[NODE]+1, sizeof(float));
    MSX->H = (float *) calloc(MSX->Nobjects[NODE]+1, sizeof(float));
    MSX->Q = (float *) calloc(MSX->Nobjects[LINK]+1, sizeof(float));

    // Allocate space in Node, Link, and Tank objects
    int i;
    for (i=1; i<=MSX->Nobjects[NODE]; i++) {
        MSX->Node[i].c = (double *) calloc(MSX->Nobjects[SPECIES]+1, sizeof(double));
        if (MSX->Node[i].c == NULL) return ERR_MEMORY;
    }
    for (i=1; i<=MSX->Nobjects[TANK]; i++) {
        MSX->Tank[i].c = (double *) calloc(MSX->Nobjects[SPECIES]+1, sizeof(double));
        if (MSX->Tank[i].c == NULL) return ERR_MEMORY;
        MSX->Tank[i].reacted = (double *) calloc(MSX->Nobjects[SPECIES]+1, sizeof(double));
        if (MSX->Tank[i].reacted == NULL) return ERR_MEMORY;
    }
    for (i=1; i<=MSX->Nobjects[LINK]; i++) {
        MSX->Link[i].reacted = (double *) calloc(MSX->Nobjects[SPECIES]+1, sizeof(double));
        if (MSX->Link[i].reacted == NULL) return ERR_MEMORY;
    }

    // Set parameters
    for (i=1; i<=MSX->Nobjects[PARAMETER]; i++) {
        double x = MSX->Param[i].value;
        int j;
        for (j=1; j<=MSX->Nobjects[LINK]; j++) MSX->Link[j].param[i] = x;
        for (j=1; j<=MSX->Nobjects[TANK]; j++) MSX->Tank[j].param[i] = x;
    }

    // --- create the TermArray for checking circular references in Terms          //1.1.00
    double **TermArray;             // Incidence array used to check Terms  //1.1.00
	TermArray = createMatrix(MSX->Nobjects[TERM]+1, MSX->Nobjects[TERM]+1);      //1.1.00
	if ( TermArray == NULL ) return ERR_MEMORY;

    int k;
    for (i=1; i<=MSX->Nobjects[TERM]; i++) {
        char *Tokens[MAXLINE];
        char *s = (char*) malloc(strlen(MSX->Term[i].equation)+1);
        if (s == NULL) return ERR_MEMORY;
        strncpy(s, MSX->Term[i].equation, strlen(MSX->Term[i].equation)+1);
        if (s[strlen(MSX->Term[i].equation)] != '\0') return ERR_MEMORY;
        char *f = s; //Used to free
        int len = (int)strlen(s);
        int m;
        int n = 0;
        while (len > 0)
        {
            m = (int)strcspn(s," \t\n\r");              // find token length
            if (m == 0) s++;                    // no token found
            else
            {
                if (*s == '"')                  // token begins with quote
                {
                    s++;                        // start token after quote
                    len--;                      // reduce length of s
                    m = (int)strcspn(s,"\"\n"); // find end quote or new line
                }
                s[m] = '\0';                    // null-terminate the token
                Tokens[n] = s;                     // save pointer to token 
                n++;                            // update token count
                s += m+1;                       // begin next token
            }
            len -= m+1;                         // update length of s
        }
        s = NULL;
        for (int j=0; j<n; j++)
        {                                                                          //1.1.00
            k = findObject(TERM, Tokens[j]);                                  //1.1.00
            if ( k > 0 ) TermArray[i][k] = 1.0;                                    //1.1.00
        }
        free(f);
        f = NULL;
    }


    if ( checkCyclicTerms(MSX, TermArray) ) return ERR_MSX_INPUT;                                        //1.1.00
    freeMatrix(TermArray);



    // --- convert user's units to internal units

    err = convertUnits(MSX);
    if (err) return err;

    // Build nodal adjacency lists 
    if (MSX->Adjlist == NULL)
    {
        err = buildadjlists(MSX);
        if (err) return err;
    }
    return err;
}

//=============================================================================
