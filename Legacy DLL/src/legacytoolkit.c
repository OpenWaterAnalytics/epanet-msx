/*******************************************************************************
**  MODULE:        LEGACYTOOLKIT.C
**  PROJECT:       EPANET-MSX
**  DESCRIPTION:   Contains the exportable set of functions that comprise the
**                 EPANET Multi-Species Extension toolkit.
**  COPYRIGHT:     Copyright (C) 2007 Feng Shang, Lewis Rossman, and James Uber.
**                 All Rights Reserved. See license information in LICENSE.TXT.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**                 K. Arrowood, Xylem intern
**  VERSION:       1.1.00
**  LAST UPDATE:   Refer to git history
**  BUG FIXES:     BUG ID 22. MSXsetpattern, Feng Shang, 04/17/2008
**                 File mode bug in MSXsolveH & MSXusehydfile, L. Rossman 10/05/2008
**                 Possible unterminated string copy, L. Rossman 11/01/10
**
**  These functions can be used in conjunction with the original EPANET
**  toolkit functions to model water quality fate and transport of
**  multiple interacting chemcial species within piping networks. See the
**  MSXrunLegacy function below for an example of how these functions were used to
**  extend the original command line version of EPANET to include multiple
**  chemical species. Consult the EPANET and EPANET-MSX Users Manuals for
**  detailed descriptions of the input data file formats required by both
**  the original EPANET and its multi-species extension.
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>

#include "msxtypes.h"
#include "msxutils.h"                                                          //1.1.00
#include "epanet2.h"
#include "legacytoolkit.h"
#include "coretoolkit.h"
#include "msxtoolkit.h"
#include "msxobjects.h"


//  Imported functions
//--------------------
int    MSXproj_open(MSXproject MSX, char *fname);
int    MSXproj_close(MSXproject MSX);
char * MSXproj_getErrmsg(int errcode);
int    MSXqual_open(MSXproject MSX);
int    MSXqual_init(MSXproject MSX);
int    MSXqual_step(MSXproject MSX, long *t, long *tleft);
int    MSXqual_close(MSXproject MSX);
int    MSXrpt_write(MSXproject MSX, char *fname);
int    MSXfile_save(MSXproject MSX, FILE *f);
int    MSXout_open(MSXproject MSX);
int    MSXout_saveResults(MSXproject MSX);
int    MSXout_saveFinalResults(MSXproject MSX);

//=============================================================================

int  DLLEXPORT  Legacyopen(MSXproject *MSX, char *argv[])
/**
**  Purpose:
**    opens the EPANET-MSX toolkit system.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**    argv = the arguments given in the main function.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    int err = 0;
    err = ENopen(argv[1], argv[3], "");
    if (err)
    {
        printf("\n\n... Cannot read EPANET file; error code = %d\n", err);
        ENclose();
        return err;
    }
    char *fname = argv[2];
    struct Project *p = (struct Project *) calloc(1, sizeof(struct Project));
    *MSX = p;
    CALL(err, MSXproj_open(*MSX, fname));
    CALL(err, MSXqual_open(*MSX));

    if ( err )
    {
        ENwriteline(MSXproj_getErrmsg(err));
        ENwriteline("");
    }

    return err;
}

//=============================================================================

int   DLLEXPORT  MSXsolveH(MSXproject MSX)
/**
**  Purpose:
**    solves for system hydraulics which are written to a temporary file.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    int err = 0;

// --- check that an MSX project was opened

    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;

// --- close & remove any existing hydraulics file

    if ( MSX->HydFile.file )
    {
        fclose(MSX->HydFile.file);
        MSX->HydFile.file = NULL;
    }
    if ( MSX->HydFile.mode == SCRATCH_FILE ) remove(MSX->HydFile.name);

// --- create a temporary hydraulics file

    MSXutils_getTempName(MSX->HydFile.name);                                    //1.1.00
    MSX->HydFile.mode = SCRATCH_FILE;                                           //(LR-10/05/08)

// --- use EPANET to solve for & save hydraulics results

    CALL(err, ENsolveH());
    CALL(err, ENsavehydfile(MSX->HydFile.name));
    CALL(err, MSXusehydfile(MSX));
    return err;
}

//=============================================================================

int   DLLEXPORT  MSXusehydfile(MSXproject MSX)
/**
**  Purpose:
**    registers a hydraulics solution file with the MSX system.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    char *fname = MSX->HydFile.name;
    INT4 magic;
    INT4 version;
    INT4 n;

// --- check that an MSX project was opened

    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;

// --- close any existing hydraulics file 

    if ( MSX->HydFile.file )
    {
        fclose(MSX->HydFile.file);
        if ( MSX->HydFile.mode == SCRATCH_FILE ) remove(MSX->HydFile.name);      //(LR-10/05/08)   
    } 
	

// --- open hydraulics file

    //MSX->HydFile.mode = USED_FILE;                                            //(LR-10/05/08)
    MSX->HydFile.file = fopen(fname, "rb");
    if (!MSX->HydFile.file) return ERR_OPEN_HYD_FILE;

// --- check that file is really a hydraulics file for current project

    fread(&magic, sizeof(INT4), 1, MSX->HydFile.file);
    if ( magic != MAGICNUMBER ) return ERR_READ_HYD_FILE;
    fread(&version, sizeof(INT4), 1, MSX->HydFile.file);
    fread(&n, sizeof(INT4), 1, MSX->HydFile.file);
    if ( n != MSX->Nobjects[NODE] ) return ERR_READ_HYD_FILE;
    fread(&n, sizeof(INT4), 1, MSX->HydFile.file);
    if ( n != MSX->Nobjects[LINK] ) return ERR_READ_HYD_FILE;
    fseek(MSX->HydFile.file, 3*sizeof(INT4), SEEK_CUR);

// --- read length of simulation period covered by file

    fread(&MSX->Dur, sizeof(INT4), 1, MSX->HydFile.file);
    MSX->HydOffset = ftell(MSX->HydFile.file);
    return 0;
}

//=============================================================================

int  DLLEXPORT  MSXsolveQ(MSXproject MSX)
/**
**  Purpose:
**    runs a MSX water quality analysis over the entire simulation period.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    long t, tleft = 0;
    int err = 0;
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    CALL(err, Legacyinit(MSX, 1));
    do CALL(err, MSXstep(&t, &tleft));
    while (tleft > 0 && err == 0);
    return err;
}

//=============================================================================

int  DLLEXPORT  Legacyinit(MSXproject MSX, int saveFlag)
/**
**  Purpose:
**    initializes a MSX water quality analysis.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**    saveFlag = 1 if results saved to binary file, 0 if not.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    int err= 0;
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    MSX->Saveflag = saveFlag;
    err = MSXqual_init(MSX);
    return err;
}

//=============================================================================

int  DLLEXPORT  MSXsaveoutfile(MSXproject MSX, char *fname)
/**
**  Purpose:
**    saves all results of the WQ simulation to a binary file.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**    fname = name of the binary results file.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    FILE *f;
    int   c;

    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    if ( !MSX->OutFile.file ) return ERR_OPEN_OUT_FILE;
    if ( (f = fopen(fname,"w+b") ) == NULL) return ERR_OPEN_OUT_FILE;
    fseek(MSX->OutFile.file, 0, SEEK_SET);
    while ( (c = fgetc(MSX->OutFile.file)) != EOF) fputc(c, f);
    fclose(f);
    return 0;
}

//=============================================================================

int  DLLEXPORT  MSXreport(MSXproject MSX, char *fname)
/**
**  Purpose:
**    writes requested WQ simulation results to a text file.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**    fname = filename to report to , leave empty if using EPANET to write
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
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    if ( MSX->Rptflag ) return MSXrpt_write(MSX, fname);
    else return 0;
}

//=============================================================================

int  DLLEXPORT  Legacyclose(MSXproject MSX)
/**
**  Purpose:
**    closes the EPANET-MSX toolkit system.
**
**  Input:
**    MSX = the underlying MSXproject data struct.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    MSXqual_close(MSX);
    MSXproj_close(MSX);
    ENclose();
    free(MSX);
    return 0;
}

//=============================================================================

int  DLLEXPORT MSXsavemsxfile(MSXproject MSX, char *fname)
/**
**  Purpose:
**      saves msx file.
**
**  Input:
**      MSX = the underlying MSXproject data struct.
**      fname = name of the msx file
** 
**  Output:
**    None
**
**  Returns:
**    an error code (or 0 for no error).
 */
{
    int errcode;
    FILE *f;
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    if ((f = fopen(fname,"wt")) == NULL) return ERR_OPEN_OUT_FILE;
    errcode = MSXfile_save(MSX, f);
    fclose(f);
    return errcode;
}

//=============================================================================

int  DLLEXPORT MSXsaveResults(MSXproject MSX)
/**
**  Purpose:
**      saves results to the binary out file.
**
**  Input:
**      MSX = the underlying MSXproject data struct.
** 
**  Output:
**    None
**
**  Returns:
**    an error code (or 0 for no error).
 */
{
    int errcode = 0;
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    
    if (MSX->Saveflag && MSX->Qtime == MSX->Rtime)
    {
        if (MSX->OutFile.file == NULL) errcode = MSXout_open(MSX);
        errcode = MSXout_saveResults(MSX);
        MSX->Rtime += MSX->Rstep;
        MSX->Nperiods++;
    }
    return errcode;
}

//=============================================================================

int  DLLEXPORT MSXsaveFinalResults(MSXproject MSX)
/**
**  Purpose:
**      saves final results to the binary out file.
**
**  Input:
**      MSX = the underlying MSXproject data struct.
** 
**  Output:
**    None
**
**  Returns:
**    an error code (or 0 for no error).
 */
{
    int errcode = 0;
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    if (MSX->Saveflag) errcode = MSXout_saveFinalResults(MSX);
    return errcode;
}

//=============================================================================

void DLLEXPORT MSXrunLegacy(int argc, char *argv[])
/**
**  Purpose:
**      runs the legacy main method.
**
**  Input:
**      MSX = the underlying MSXproject data struct.
**      argc = number of arguments
**      argv = arguments.
** 
**  Output:
**    None
**
**  Returns:
**    an error code (or 0 for no error).
 */
{
    int err = 0;
    int done = 1;
    long   t, tleft;
    long   oldHour, newHour;
    MSXproject MSX;

    // --- check command line arguments

    if ( argc < 4 )
    {
        printf("\n Too few command line arguments.\n");
        return;
    }

    printf("\n... EPANET-MSX Version 1.1\n");                                  //1.1.00
    printf("\n  o Processing EPANET input file");
    do
    {

    // --- open the MSX input file
        printf("\n  o Processing MSX input file   ");
        err = Legacyopen(&MSX, argv);
        if (err)
        {
            printf("\n\n... Cannot read EPANET-MSX file; error code = %d\n", err);
            break;
        }

    //--- solve hydraulics

        printf("\n  o Computing network hydraulics");
        err = MSXsolveH(MSX);
        if (err)
        {
            printf("\n\n... Cannot obtain network hydraulics; error code = %d\n", err);
            break;
        }

    //--- Initialize the multi-species analysis

        printf("\n  o Initializing network water quality");
        err = Legacyinit(MSX, 1);
        if (err)
        {
            printf("\n\n... Cannot initialize EPANET-MSX; error code = %d\n", err);
            break;
        }
        t = 0;
        oldHour = -1;
        newHour = 0;
        printf("\n");

    //--- Run the multi-species analysis at each time step

        do
        {
            if ( oldHour != newHour )
            {
                printf("\r  o Computing water quality at hour %-4d", newHour);
                fflush(stdout);
                oldHour = newHour;
            }
            err = MSXsaveResults(MSX);
            err = MSX_step(MSX, &t, &tleft);
            newHour = t / 3600;

        } while (!err && tleft > 0);
        if (err)
        {
            printf("\n\n... EPANET-MSX runtime error; error code = %d\n", err);
            break;
        }
        else 
            printf("\r  o Computing water quality at hour %-4d", t/3600);
            fflush(stdout);
        
        err = MSXsaveResults(MSX);
        err = MSXsaveFinalResults(MSX);

    // --- report results

        printf("\n  o Reporting water quality results");
        err = MSXreport(MSX, "");
        if (err)
        {
            printf("\n\n... EPANET-MSX report writer error; error code = %d\n", err);
            break;
        }

    // --- save results to binary file if a file name was provided

        if ( argc >= 5 )
        {
            err = MSXsaveoutfile(MSX, argv[4]);
            if ( err > 0 )
            {
                printf("\n\n... Cannot save EPANET-MSX results file; error code = %d\n", err);
                break;
            }
        }

    } while(!done);

//--- Close both the multi-species & EPANET systems

    Legacyclose(MSX);
    if ( !err ) printf("\n\n... EPANET-MSX completed successfully.");
    printf("\n");
}

//=============================================================================
