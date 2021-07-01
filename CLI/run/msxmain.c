/*******************************************************************************
**  MODULE:        MSXMAIN.C
**  PROJECT:       EPANET-MSX
**  DESCRIPTION:   Main module of the EPANET Multi-Species Extension toolkit.
**  COPYRIGHT:     Copyright (C) 2007 Feng Shang, Lewis Rossman, and James Uber.
**                 All Rights Reserved. See license information in LICENSE.TXT.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**  VERSION:       1.1.00
**  LAST UPDATE:   2/8/11
**
**  EPANET-MSX is an extension of the EPANET program for modeling the fate
**  and transport of multiple interacting chemical species within a water
**  distribution system over an extended period of operation. This module
**  provides a main function for producing a stand-alone console
**  application version of EPANET-MSX. It is not needed when compiling
**  EPANET-MSX into a dynamic link library (DLL) of callable functions.
**
**  To use either the console version or the DLL a user must prepare a
**  regular EPANET input file that describes the pipe network layout
**  and its hydraulic properties as well as a special EPANET-MSX input file
**  that names the chemical species being modeled and specifies the reaction
**  rate and equilbrium expressions that define their chemical behavior. The
**  format of these files is described in the EPANET and EPANET-MSX Users
**  Manuals, respectively.
*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <float.h>

#include "legacytoolkit.h"                 // EPANET-MSX toolkit header file
#include "coretoolkit.h"

void call(int err)
/**
** Purpose: to easily check errors and print out error messages.
** Input:
**      err = errorcode
** Returns:
**      none
*/
{
    if (err != 0) {
        printf("Error occured, check EPANET or EPANET MSX documents for error codes, if not in either of those, then it will be in errors.dat.\nError code: %d\n", err);
    }
}

int main(int argc, char *argv[])
/**
**  Purpose:
**    main function for the console version of EPANET-MSX.
**
**  Input:
**    argc = number of command line arguments
**    argv = array of command line arguments.
**
**  Returns:
**    an error code (or 0 for no error).
**
**  Notes:
**    The command line arguments are:
**     - the name of the regular EPANET input data file
**     - the name of the EPANET-MSX input file
**     - the name of a report file that will contain status
**       messages and output results
**     - optionally, the name of an output file that will
**       contain water quality results in binary format.
*/
{
    MSXproject MSX;
    call(MSX_init(&MSX));

    // Builing the network from example.inp
    call(MSXsetFlowFlag(&MSX, CMH));
    call(MSXsetTimeParameter(&MSX, DURATION, 48*60));
    call(MSXsetTimeParameter(&MSX, QUALSTEP, 5*60));
    call(MSXsetTimeParameter(&MSX, REPORTSTEP, 2*60));
    call(MSXsetTimeParameter(&MSX, REPORTSTART, 0));
    // Add nodes
    call(MSXaddNode(&MSX));
    call(MSXaddNode(&MSX));
    call(MSXaddNode(&MSX));
    call(MSXaddNode(&MSX));
    call(MSXaddReservoir(&MSX, 0,0,0));
    // Add links
    call(MSXaddLink(&MSX, 5, 1, 200, 1000, 100));
    call(MSXaddLink(&MSX, 1, 2, 150, 800, 100));
    call(MSXaddLink(&MSX, 1, 3, 200, 1200, 100));
    call(MSXaddLink(&MSX, 2, 3, 150, 1000, 100));
    call(MSXaddLink(&MSX, 3, 4, 150, 2000, 100));

    //TODO



    
    //If legacy then
    // call(runLegacy(&MSX, argc, argv));


   // add_node()
    //Else then run the new API
    return 0;
}
