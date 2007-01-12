/*******************************************************************************
**  TITLE:         TOOLKITMSX.C
**  DESCRIPTION:   contains the exportable set of functions that comprise the
**                 EPANET Multi-Species Extension toolkit.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**  VERSION:       1.00
**  LAST UPDATE:   8/1/06
**
**  These functions can be used in conjunction with the original EPANET
**  toolkit functions to model water quality fate and transport of
**  multiple interacting chemcial species within piping networks. See the
**  EPANETMSX.C module for an example of how these functions were used to
**  extend the original command line version of EPANET to include multiple
**  chemical species. Consult the EPANET and EPANET-MSX Users Manuals for
**  detailed descriptions of the input data file formats required by both
**  the original EPANET and its multi-species extension.
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <float.h>

#include "typesmsx.h"
#define  EXTERN_MSX
#include "globals.h"
#include "epanet2.h"
#include "epanetmsx.h"

//  Imported functions
//--------------------
int    project_open(char *fname);
int    project_close(void);
int    project_findObject(int type, char *id);
char * project_getErrmsg(int errcode);
int    quality_open(void);
int    quality_init(void);
int    quality_step(long *t, long *tleft);
int    quality_close(void);
double quality_getNodeQual(int j, int m);
double quality_getLinkQual(int k, int m);
int    report_write(void);

//=============================================================================

int  DLLEXPORT  ENMSXopen(char *fname)
/*
**  Purpose:
**    opens an EPANET-MSX input file.
**
**  Input:
**    fname = name of file.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    int errcode = 0;

    ERRCODE(project_open(fname));
    ERRCODE(quality_open());
    return errcode;
}

//=============================================================================

int   DLLEXPORT  ENMSXsolveH()
/*
**  Purpose:
**    solves for system hydraulics which are written to a temporary file.
**
**  Input:
**    none.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    int errcode = 0;

// --- close & remove any existing hydraulics file

    if ( HydFile.file )
    {
        fclose(HydFile.file);
        HydFile.file = NULL;
    }
    if ( HydFile.mode == SCRATCH_FILE ) remove(HydFile.name);

// --- create a temporary hydraulics file

    tmpnam(HydFile.name);

// --- use EPANET to solve for & save hydraulics results

    ERRCODE(ENsolveH());
    ERRCODE(ENsavehydfile(HydFile.name));
    ERRCODE(ENMSXusehydfile(HydFile.name));

// --- make sure file is declared temporary

    HydFile.mode = SCRATCH_FILE;
    return errcode;
}

//=============================================================================

int   DLLEXPORT  ENMSXusehydfile(char *fname)
/*
**  Purpose:
**    registers a hydraulics solution file with the MSX system.
**
**  Input:
**    fname = name of binary hydraulics results file.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    int magic;
    int version;
    int n;

// --- open hydraulics file

    if ( HydFile.file ) fclose(HydFile.file);
    HydFile.mode = USED_FILE;
    HydFile.file = fopen(fname, "rb");
    if (!HydFile.file) return 504;

// --- check that file is really a hydraulics file for current project

    fread(&magic, sizeof(int), 1, HydFile.file);
    if ( magic != MAGICNUMBER ) return 505;
    fread(&version, sizeof(int), 1, HydFile.file);
    //if ( version != VERSION ) return 505;
    fread(&n, sizeof(int), 1, HydFile.file);
    if ( n != Nobjects[NODE] ) return 505;
    fread(&n, sizeof(int), 1, HydFile.file);
    if ( n != Nobjects[LINK] ) return 505;
    fseek(HydFile.file, 3*sizeof(int), SEEK_CUR);

// --- read length of simulation period covered by file

    fread(&Dur, sizeof(int), 1, HydFile.file);
    HydOffset = ftell(HydFile.file);
    return 0;
}

//=============================================================================

int  DLLEXPORT  ENMSXinit(int saveFlag)
/*
**  Purpose:
**    initializes a MSX water quality analysis.
**
**  Input:
**    saveFlag = 1 if results saved to binary file, 0 if not.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    int errcode = 0;
    Saveflag = saveFlag;
    ERRCODE(quality_init());
    return errcode;
}

//=============================================================================

int  DLLEXPORT  ENMSXgetspecieindex(char *id, int *index)
/*
**  Purpose:
**    retrieves the index of a named WQ species.
**
**  Input:
**    id = name of water quality species.
**
**  Output:
**    index = index (base 1) of the species in the list of all species.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    int i = project_findObject(SPECIE, id);
    *index = 0;
    if (i >= 1) *index = i;
    return 0;
}

//=============================================================================

int  DLLEXPORT  ENMSXgetspecieID(int index, char *id)
/*
**  Purpose:
**    retrieves the name of a WQ species given its index.
**
**  Input:
**    index = index (base 1) of the species in the list of all species.
**
**  Output:
**    id = name of the species.
**
**  Returns:
**    an error code (or 0 for no error).
**
**  Notes:
**    The character array id must be dimensioned to accept a string of
**    up to 16 characters.
*/
{
    if ( index >= 1 && index <= Nobjects[SPECIE] )
    {
        strcpy(id, Specie[index].id);
    }
    else strcpy(id, "");
    return 0;
}

//=============================================================================
int  DLLEXPORT  ENMSXgetcount(int code, int *count)
/*
**  Purpose: retrieves the number of components of a
**           given type in the network
**
**  Input:   code = component code (see EPANETMSX.H)
**
**  Output:  *count = number of components in network
**
**  Returns: error code
*/
{
    *count = 0;
    switch (code)
    {
    case ENMSX_SPECIESCOUNT:    *count = Nobjects[SPECIE];    break;
    default: return(ERR_PARAM_CODE);
    }
    return(0);
}

//=============================================================================

int DLLEXPORT  ENMSXgetspecietype(int index, int *code)
/*
**  Purpose:
**    retrieves the type code of a WQ species.
**
**  Input:
**    index = index (base 1) of the species in the list of all species.
**
**  Output:
**    code = 0 (BULK) for a bulk flow specie, 1 (WALL) for an attached specie.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    if ( index >= 1 && index <= Nobjects[SPECIE] )
    {
        *code = Specie[index].type;
    }
    else *code = 0;
    return 0;
}

//=============================================================================

int  DLLEXPORT  ENMSXgetnodequal(int node, int specie, double *c)
/*
**  Purpose:
**    retrieves the current concentration of a species at a particular node
**    of the pipe network.
**
**  Input:
**    node = node index (base 1).
**    specie = specie index (base 1).
**
**  Output:
**    c = specie concentration.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    *c = 0.0;
    if ( node > 0 && node <= Nobjects[NODE] &&
         specie > 0 && specie <= Nobjects[SPECIE] )
    {
        *c = quality_getNodeQual(node, specie);
    }
    return 0;
}

//=============================================================================

int  DLLEXPORT  ENMSXgetlinkqual(int link, int specie, double *c)
/*
**  Purpose:
**    retrieves the current average concentration of a species in a particular
**    link of the pipe network.
**
**  Input:
**    link = link index (base 1).
**    specie = specie index (base 1).
**
**  Output:
**    c = specie concentration.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    *c = 0.0;
    if ( link > 0 && link <= Nobjects[LINK] &&
        specie > 0 && specie <= Nobjects[SPECIE] )
    {
        *c = quality_getLinkQual(link, specie);
    }
    return 0;
}

//=============================================================================

int  DLLEXPORT  ENMSXstep(long *t, long *tleft)
/*
**  Purpose:
**    updates the WQ simulation over a single time step.
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
    return quality_step(t, tleft);
}

//=============================================================================

int  DLLEXPORT  ENMSXreport()
/*
**  Purpose:
**    writes requested WQ simulation results to a text file.
**
**  Input:
**    none
**
**  Returns:
**    an error code (or 0 for no error).
**
**  Notes:
**    Results are written to the EPANET report file unless a specific
**    water quality report file is named in the [REPORT] section of
**    the MSX input file.
*/
{
    if ( Rptflag ) return report_write();
    else return 0;
}

//=============================================================================

int  DLLEXPORT  ENMSXsaveoutfile(char *fname)
/*
**  Purpose:
**    saves all results of the WQ simulation to a binary file.
**
**  Input:
**    fname = name of the binary results file.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    FILE *f;
    int   c;

    if ( !OutFile.file ) return 511;
    if ( (f = fopen(fname,"w+b") ) == NULL) return 511;
    fseek(OutFile.file, 0, SEEK_SET);
    while ( (c = fgetc(OutFile.file)) != EOF) fputc(c, f);
    fclose(f);
    return 0;
}

//=============================================================================

int  DLLEXPORT  ENMSXclose()
/*
**  Purpose:
**    closes the EPANET-MSX system.
**
**  Input:
**    none
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    quality_close();
    project_close();
    return 0;
}

//=============================================================================

int  DLLEXPORT  ENMSXgeterror(int errcode, char *errmsg, int n)
/*
**  Purpose:
**    retrieves text of an error message.
**
**  Input:
**    errcode = error code number
**    n = maximum length of string errmsg.
**
**  Output:
**    errmsg  = text of error message.
**
**  Returns:
**    an error code which is always 0.
*/
{
    strncpy(errmsg, project_getErrmsg(errcode), n);
    return 0;
}
