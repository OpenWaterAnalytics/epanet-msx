/******************************************************************************
**  MODULE:        MSXPROJ.C
**  PROJECT:       EPANET-MSX
**  DESCRIPTION:   project data manager used by the EPANET Multi-Species
**                 Extension toolkit.
**  COPYRIGHT:     Copyright (C) 2007 Feng Shang, Lewis Rossman, and James Uber.
**                 All Rights Reserved. See license information in LICENSE.TXT.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**                 K. Arrowood, Xylem intern
**  VERSION:       1.1.00
**  LAST UPDATE:   Refer to git history
**  Bug fix:       Bug ID 08, Feng Shang 01/07/2008
**                 Memory leak fixed, T. Taxon - 9/7/10
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "msxtypes.h"
#include "msxutils.h"
#include "hash.h"
#include "msxobjects.h"


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
     "Error 401 - (too many characters)",
     "Error 402 - (too few input items)",
     "Error 403 - (invalid keyword)",
     "Error 404 - (invalid numeric value)",
     "Error 405 - (reference to undefined object)",
     "Error 406 - (illegal use of a reserved name)",
     "Error 407 - (name already used by another object)",
     "Error 408 - (species already assigned an expression)", 
     "Error 409 - (illegal math expression)"};                                  

//  Imported functions
//--------------------
int    MSXinp_countMsxObjects(MSXproject MSX);
int    MSXinp_countNetObjects(MSXproject MSX);
int    MSXinp_readNetData(MSXproject MSX);
int    MSXinp_readMsxData(MSXproject MSX);

//  Exported functions
//--------------------
int    MSXproj_open(MSXproject MSX, char *fname);
char * MSXproj_getErrmsg(int errcode);

//  Local functions
//-----------------
static int    createObjects(MSXproject MSX);

static int    openRptFile(MSXproject MSX);                                               //(LR-11/20/07)

//=============================================================================

int  MSXproj_open(MSXproject MSX, char *fname)
/**
**  Purpose:
**    opens an EPANET-MSX project.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**    fname = name of EPANET-MSX input file
**
**  Returns:
**    an error code (0 if no error)
*/
{
// --- initialize data to default values

    int errcode = 0;
    MSX->ProjectOpened = FALSE;
    MSX->QualityOpened = FALSE;
    setDefaults(MSX);

// --- open the MSX input file

    strcpy(MSX->MsxFile.name, fname);
    if ((MSX->MsxFile.file = fopen(fname,"rt")) == NULL) return ERR_OPEN_MSX_FILE;

// --- create hash tables to look up object ID names

    CALL(errcode, createHashTables());

// --- allocate memory for the required number of objects

    CALL(errcode, MSXinp_countMsxObjects(MSX));
    CALL(errcode, MSXinp_countNetObjects(MSX));
    CALL(errcode, createObjects(MSX));

// --- read in the EPANET and MSX object data

    CALL(errcode, MSXinp_readNetData(MSX));
    CALL(errcode, MSXinp_readMsxData(MSX));


    if (strcmp(MSX->RptFile.name, ""))                                          //(FS-01/07/2008, to fix bug 08)
	CALL(errcode, openRptFile(MSX));                                              //(LR-11/20/07, to fix bug 08)
    else strcpy(MSX->RptFile.name, "EPA");     // Just a placeholder so that the program knows that EPANET is handling the report file

// --- convert user's units to internal units

    CALL(errcode, convertUnits(MSX));



    // Build nodal adjacency lists 
    if (MSX->Adjlist == NULL)
    {
        errcode = buildadjlists(MSX);
        if (errcode) return errcode;
    }
    // printf("ptr: %ld\n", MSX->Adjlist);


// --- close input file

    if ( MSX->MsxFile.file ) fclose(MSX->MsxFile.file);
    MSX->MsxFile.file = NULL;
    if ( !errcode ) MSX->ProjectOpened = TRUE;
    return errcode;
}

//=============================================================================

void MSXproj_close(MSXproject MSX)
/**
**  Purpose:
**    closes the current EPANET-MSX project.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
*/
{
    // --- close all files

    if ( MSX->RptFile.file ) fclose(MSX->RptFile.file);                          //(LR-11/20/07, to fix bug 08)
    if ( MSX->HydFile.file ) fclose(MSX->HydFile.file);
    if ( MSX->TmpOutFile.file && MSX->TmpOutFile.file != MSX->OutFile.file )
        fclose(MSX->TmpOutFile.file);
    if ( MSX->OutFile.file ) fclose(MSX->OutFile.file);

    // --- delete all temporary files

    if ( MSX->HydFile.mode == SCRATCH_FILE ) remove(MSX->HydFile.name);
    if ( MSX->OutFile.mode == SCRATCH_FILE ) remove(MSX->OutFile.name);
    remove(MSX->TmpOutFile.name);

    // --- free all allocated memory

    MSX->RptFile.file = NULL;                                                   //(LR-11/20/07, to fix bug 08)
    MSX->HydFile.file = NULL;
    MSX->OutFile.file = NULL;
    MSX->TmpOutFile.file = NULL;
    deleteObjects(MSX);
    deleteHashTables();
    MSX->ProjectOpened = FALSE;
}

//=============================================================================

char * MSXproj_getErrmsg(int errcode)
/**
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
    if (errcode <= ERR_FIRST && errcode >= 400) return Errmsg[errcode - 400 + 24];
    if ( errcode <= ERR_FIRST || errcode >= ERR_MAX ) return Errmsg[0];
    else return Errmsg[errcode - ERR_FIRST];
}

//=============================================================================

int createObjects(MSXproject MSX)
/**
**  Purpose:
**    creates multi-species data objects.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**
**  Returns:
**    an error code (0 if no error).
*/
{
    int i;

// --- create nodes, links, & tanks

    MSX->Node = (Snode *) calloc(MSX->Nobjects[NODE]+1, sizeof(Snode));
    MSX->Link = (Slink *) calloc(MSX->Nobjects[LINK]+1, sizeof(Slink));
    MSX->Tank = (Stank *) calloc(MSX->Nobjects[TANK]+1, sizeof(Stank));

// --- create species, terms, parameters, constants & time patterns

    MSX->Species = (Sspecies *) calloc(MSX->Nobjects[SPECIES]+1, sizeof(Sspecies));
    MSX->Term    = (Sterm *)    calloc(MSX->Nobjects[TERM]+1,  sizeof(Sterm));
    MSX->Param   = (Sparam *)   calloc(MSX->Nobjects[PARAMETER]+1, sizeof(Sparam));
    MSX->Const   = (Sconst *)   calloc(MSX->Nobjects[CONSTANT]+1, sizeof(Sconst));
    MSX->Pattern = (Spattern *) calloc(MSX->Nobjects[PATTERN]+1, sizeof(Spattern));
    MSX->K       = (double *)   calloc(MSX->Nobjects[CONSTANT]+1, sizeof(double));  //1.1.00

// --- create arrays for demands, heads, & flows

    MSX->D = (float *) calloc(MSX->Nobjects[NODE]+1, sizeof(float));
    MSX->H = (float *) calloc(MSX->Nobjects[NODE]+1, sizeof(float));
    MSX->Q = (float *) calloc(MSX->Nobjects[LINK]+1, sizeof(float));

// --- create arrays for current & initial concen. of each species for each node

    MSX->C0 = (double *) calloc(MSX->Nobjects[SPECIES]+1, sizeof(double));
    for (i=1; i<=MSX->Nobjects[NODE]; i++)
    {
        MSX->Node[i].c = (double *) calloc(MSX->Nobjects[SPECIES]+1, sizeof(double));
        MSX->Node[i].c0 = (double *) calloc(MSX->Nobjects[SPECIES]+1, sizeof(double));
        MSX->Node[i].rpt = 0;
    }

// --- create arrays for init. concen. & kinetic parameter values for each link

    for (i=1; i<=MSX->Nobjects[LINK]; i++)
    {
        MSX->Link[i].c0 = (double *)
            calloc(MSX->Nobjects[SPECIES]+1, sizeof(double));
        MSX->Link[i].reacted = (double *)
            calloc(MSX->Nobjects[SPECIES] + 1, sizeof(double));
        MSX->Link[i].param = (double *)
            calloc(MSX->Nobjects[PARAMETER]+1, sizeof(double));
        MSX->Link[i].rpt = 0;
    }

// --- create arrays for kinetic parameter values & current concen. for each tank

    for (i=1; i<=MSX->Nobjects[TANK]; i++)
    {
        MSX->Tank[i].param = (double *)
            calloc(MSX->Nobjects[PARAMETER]+1, sizeof(double));
        MSX->Tank[i].c = (double *)
            calloc(MSX->Nobjects[SPECIES]+1, sizeof(double));
        MSX->Tank[i].reacted = (double*)
            calloc(MSX->Nobjects[SPECIES] + 1, sizeof(double));
    }

// --- initialize contents of each time pattern object

    for (i=1; i<=MSX->Nobjects[PATTERN]; i++)
    {
        MSX->Pattern[i].length = 0;
        MSX->Pattern[i].first = NULL;
        MSX->Pattern[i].current = NULL;
    }

// --- initialize reaction rate & equil. formulas for each species

    for (i=1; i<=MSX->Nobjects[SPECIES]; i++)
    {
        MSX->Species[i].pipeExpr     = NULL;
        MSX->Species[i].tankExpr     = NULL;
        MSX->Species[i].pipeExprType = NO_EXPR;
        MSX->Species[i].tankExprType = NO_EXPR;
        MSX->Species[i].precision    = 2;
        MSX->Species[i].rpt = 0;
    }

// --- initialize math expressions for each intermediate term

    for (i=1; i<=MSX->Nobjects[TERM]; i++) MSX->Term[i].expr = NULL;
    return 0;
}

//=============================================================================

// New function added (LR-11/20/07, to fix bug 08)
int openRptFile(MSXproject MSX)
{
    if ( MSX->RptFile.file ) fclose(MSX->RptFile.file);
    MSX->RptFile.file = fopen(MSX->RptFile.name, "wt");
    if ( MSX->RptFile.file == NULL ) return ERR_OPEN_RPT_FILE;
    return 0;
}

//=============================================================================
