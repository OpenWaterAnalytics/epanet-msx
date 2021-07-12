/*******************************************************************************
**  MODULE:        MSXTOOLKIT.C
**  PROJECT:       EPANET-MSX
**  DESCRIPTION:   Contains the exportable set of functions that comprise the
**                 EPANET Multi-Species Extension toolkit.
**  COPYRIGHT:     Copyright (C) 2007 Feng Shang, Lewis Rossman, and James Uber.
**                 All Rights Reserved. See license information in LICENSE.TXT.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**  VERSION:       1.1.00
**  LAST UPDATE:   2/8/11
**  BUG FIXES:     BUG ID 22. MSXsetpattern, Feng Shang, 04/17/2008
**                 File mode bug in MSXsolveH & MSXusehydfile, L. Rossman 10/05/2008
**                 Possible unterminated string copy, L. Rossman 11/01/10
**
**  These functions can be used in conjunction with the original EPANET
**  toolkit functions to model water quality fate and transport of
**  multiple interacting chemcial species within piping networks. See the
**  MSXMAIN.C module for an example of how these functions were used to
**  extend the original command line version of EPANET to include multiple
**  chemical species. Consult the EPANET and EPANET-MSX Users Manuals for
**  detailed descriptions of the input data file formats required by both
**  the original EPANET and its multi-species extension.
*******************************************************************************/
// --- define WINDOWS

#undef WINDOWS
#ifdef _WIN32
  #define WINDOWS
#endif
#ifdef __WIN32__
  #define WINDOWS
#endif

// --- define DLLEXPORT

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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>

#include "msxutils.h"                                                          //1.1.00
#include "epanet2.h"
#include "legacytoolkit.h"
#include "coretoolkit.h"
#include "msxobjects.h"


//  Imported functions
//--------------------
int    MSXproj_open(MSXproject *MSX, char *fname);
int    MSXproj_close(MSXproject *MSX);
char * MSXproj_getErrmsg(int errcode);
int    MSXqual_open(MSXproject *MSX);
int    MSXqual_init(MSXproject *MSX);
int    MSXqual_step(MSXproject *MSX, long *t, long *tleft);
int    MSXqual_close(MSXproject *MSX);
int    MSXrpt_write(MSXproject *MSX);
int    MSXfile_save(MSXproject *MSX, FILE *f);

//=============================================================================

int  DLLEXPORT  MSXopen(MSXproject *MSX, char *argv[])
/**
**  Purpose:
**    opens the EPANET-MSX toolkit system.
**
**  Input:
**    fname = name of an MSX input file.
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
    if (!MSX->ProjectOpened) return(ERR_MSX_OPENED);
    CALL(err, MSXproj_open(MSX, fname));
    CALL(err, MSXqual_open(MSX));

    if ( err )
    {
        ENwriteline(MSXproj_getErrmsg(err));
        ENwriteline("");
    }

    return err;
}

//=============================================================================

int   DLLEXPORT  MSXsolveH(MSXproject *MSX)
/**
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

int   DLLEXPORT  MSXusehydfile(MSXproject *MSX)
/**
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

int  DLLEXPORT  MSXsolveQ(MSXproject *MSX)
/**
**  Purpose:
**    runs a MSX water quality analysis over the entire simulation period.
**
**  Input:
**    none.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    long t, tleft = 0;
    int err = 0;
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    CALL(err, MSXinit(MSX, 1));
    do CALL(err, MSXstep(MSX, &t, &tleft));
    while (tleft > 0 && err == 0);
    return err;
}

//=============================================================================

int  DLLEXPORT  MSXinit(MSXproject *MSX, int saveFlag)
/**
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
    int err= 0;
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    MSX->Saveflag = saveFlag;
    err = MSXqual_init(MSX);
    return err;
}

// //=============================================================================

// int  DLLEXPORT  MSXstep(MSXproject *MSX, long *t, long *tleft)
// /**
// **  Purpose:
// **    advances the WQ simulation over a single time step.
// **
// **  Input:
// **    none
// **
// **  Output:
// **    *t = current simulation time at the end of the step (sec)
// **    *tleft = time left in the simulation (sec)
// **
// **  Returns:
// **    an error code (or 0 for no error).
// */
// {
//     if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
//     return MSXqual_step(MSX, t, tleft);
// }

//=============================================================================

int  DLLEXPORT  MSXsaveoutfile(MSXproject *MSX, char *fname)
/**
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

    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    if ( !MSX->OutFile.file ) return ERR_OPEN_OUT_FILE;
    if ( (f = fopen(fname,"w+b") ) == NULL) return ERR_OPEN_OUT_FILE;
    fseek(MSX->OutFile.file, 0, SEEK_SET);
    while ( (c = fgetc(MSX->OutFile.file)) != EOF) fputc(c, f);
    fclose(f);
    return 0;
}

//=============================================================================

int  DLLEXPORT  MSXreport(MSXproject *MSX)
/**
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
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    if ( MSX->Rptflag ) return MSXrpt_write(MSX);
    else return 0;
}

//=============================================================================

int  DLLEXPORT  MSXclose(MSXproject *MSX)
/**
**  Purpose:
**    closes the EPANET-MSX toolkit system.
**
**  Input:
**    none
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    MSXqual_close(MSX);
    MSXproj_close(MSX);
    ENclose();
    return 0;
}


//=============================================================================

int  DLLEXPORT  MSXgeterror(MSXproject *MSX, int code, char *msg, int len)
/**
**  Purpose:
**    retrieves text of an error message.
**
**  Input:
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
    strncpy(msg, MSXproj_getErrmsg(code), len);
    return 0;
}

//=============================================================================



int  DLLEXPORT MSXsavemsxfile(MSXproject *MSX, char *fname)
{
    int errcode;
    FILE *f;
    if ( !MSX->ProjectOpened ) return ERR_MSX_NOT_OPENED;
    if ((f = fopen(fname,"wt")) == NULL) return ERR_OPEN_OUT_FILE;
    errcode = MSXfile_save(MSX, f);
    fclose(f);
    return errcode;
}


int DLLEXPORT MSXrunLegacy(MSXproject *MSX, int argc, char *argv[])
{
    int err = 0;
    int done = 1;
    long   t, tleft;
    long   oldHour, newHour;

    // --- check command line arguments

    if ( argc < 4 )
    {
        printf("\n Too few command line arguments.\n");
        return err;
    }

    printf("\n... EPANET-MSX Version 1.1\n");                                  //1.1.00
    printf("\n  o Processing EPANET input file");
    do
    {

    // --- open the MSX input file
        printf("\n  o Processing MSX input file   ");
        err = MSXopen(MSX, argv);
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
        err = MSXinit(MSX, 1);
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
                oldHour = newHour;
            }
            err = MSXstep(MSX, &t, &tleft);
            newHour = t / 3600;

        } while (!err && tleft > 0);
        if (err)
        {
            printf("\n\n... EPANET-MSX runtime error; error code = %d\n", err);
            break;
        }
        else 
            printf("\r  o Computing water quality at hour %-4d", t/3600);

    // --- report results

        printf("\n  o Reporting water quality results");
        err = MSXreport(MSX);
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

    MSXclose(MSX);
    if ( !err ) printf("\n\n... EPANET-MSX completed successfully.");
    printf("\n");
    return err;
}