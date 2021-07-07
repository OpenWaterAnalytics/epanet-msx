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
    int err = 0;
    MSXproject MSX;
    call(MSX_open(&MSX));
    
    
    // Builing the network from example.inp
    call(MSXsetFlowFlag(&MSX, CMH));
    call(MSXsetTimeParameter(&MSX, DURATION, 48*60));
    call(MSXsetTimeParameter(&MSX, QUALSTEP, 5*60));
    call(MSXsetTimeParameter(&MSX, REPORTSTEP, 2*60));
    call(MSXsetTimeParameter(&MSX, REPORTSTART, 0));
    // Add nodes
    call(MSXaddNode(&MSX, "a"));
    call(MSXaddNode(&MSX, "b"));
    call(MSXaddNode(&MSX, "c"));
    call(MSXaddNode(&MSX, "e"));
    call(MSXaddReservoir(&MSX, "source", 0,0,0));
    // Add links
    call(MSXaddLink(&MSX, "1", "source", "a", 200, 1000, 100));
    call(MSXaddLink(&MSX, "2", "a", "b", 150, 800, 100));
    call(MSXaddLink(&MSX, "3", "a", "c", 200, 1200, 100));
    call(MSXaddLink(&MSX, "4", "b", "c", 150, 1000, 100));
    call(MSXaddLink(&MSX, "5", "c", "e", 150, 2000, 100));

    // Add Options
    call(MSXaddOption(&MSX, AREA_UNITS_OPTION, "M2"));
    call(MSXaddOption(&MSX, RATE_UNITS_OPTION, "HR"));
    call(MSXaddOption(&MSX, SOLVER_OPTION, "RK5"));
    call(MSXaddOption(&MSX, TIMESTEP_OPTION, "360"));
    call(MSXaddOption(&MSX, RTOL_OPTION, "0.001"));
    call(MSXaddOption(&MSX, ATOL_OPTION, "0.0001"));

    // Add Species
    call(MSXaddSpecies(&MSX, "AS3", BULK, UG, 0.0, 0.0));
    call(MSXaddSpecies(&MSX, "AS5", BULK, UG, 0.0, 0.0));
    call(MSXaddSpecies(&MSX, "AStot", BULK, UG, 0.0, 0.0));
    call(MSXaddSpecies(&MSX, "AS5s", WALL, UG, 0.0, 0.0));
    call(MSXaddSpecies(&MSX, "NH2CL", BULK, MG, 0.0, 0.0));
    
    //Add Coefficents
    call(MSXaddCoefficeint(&MSX, CONSTANT, "Ka", 10.0));
    call(MSXaddCoefficeint(&MSX, CONSTANT, "Kb", 0.1));
    call(MSXaddCoefficeint(&MSX, CONSTANT, "K1", 5.0));
    call(MSXaddCoefficeint(&MSX, CONSTANT, "K2", 1.0));
    call(MSXaddCoefficeint(&MSX, CONSTANT, "Smax", 50));

    //Add terms
    call(MSXaddTerm(&MSX, "Ks", "K1/K2"));

    //Add Expressions
    call(MSXaddExpression(&MSX, LINK, RATE, "AS3", "-Ka*AS3*NH2CL"));
    call(MSXaddExpression(&MSX, LINK, RATE, "AS5", "Ka*AS3*NH2CL - Av*(K1*(Smax-AS5s)*AS5 - K2*AS5s)"));
    call(MSXaddExpression(&MSX, LINK, RATE, "NH2CL", "-Kb*NH2CL"));
    call(MSXaddExpression(&MSX, LINK, RATE, "AS5s", "Ks*Smax*AS5/(1+Ks*AS5) - AS5s"));
    call(MSXaddExpression(&MSX, LINK, RATE, "AStot", "AS3 + AS5"));

    call(MSXaddExpression(&MSX, TANK, RATE, "AS3", "-Ka*AS3*NH2CL"));
    call(MSXaddExpression(&MSX, TANK, RATE, "AS5", "Ka*AS3*NH2CL"));
    call(MSXaddExpression(&MSX, TANK, RATE, "NH2CL", "-Kb*NH2CL"));
    call(MSXaddExpression(&MSX, TANK, RATE, "AStot", "AS3 + AS5"));
    
    //Add Quality
    call(MSXaddQuality(&MSX, "NODE", "AS3", 10.0, "source"));
    call(MSXaddQuality(&MSX, "NODE", "NH2CL", 2.5, "source"));

    //Setup Report
    call(MSXsetReport(&MSX, "NODE", "c", 0));
    call(MSXsetReport(&MSX, "NODE", "e", 0));
    call(MSXsetReport(&MSX, "LINK", "5", 0));
    call(MSXsetReport(&MSX, "SPECIE", "AStot", 0));
    call(MSXsetReport(&MSX, "SPECIE", "AS5", 0));
    call(MSXsetReport(&MSX, "SPECIE", "AS5s", 0));
    call(MSXsetReport(&MSX, "SPECIE", "NH2CL", 0));

    // call(MSXsolveH(&MSX));

    // Run
    call(MSXrun(&MSX));

    // Close
    call(MSX_close(&MSX));
    
    //If legacy then
    // call(runLegacy(&MSX, argc, argv));
    return err;
}
