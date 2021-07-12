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

void example1(char *fname);

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
        printf("Error occured!\nError code: %d\n", err);
    }
}

void main(int argc, char *argv[])
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
    example1(argv[1]);
    //If legacy then
    // MSXproject MSX;
    // call(MSXrunLegacy(&MSX, argc, argv));
}

void example1(char *fname) {
    MSXproject MSX;
    call(MSX_open(&MSX));
    
    int minute = 60;
    int hour = 60*minute;
    // Builing the network from example.inp
    call(MSXsetFlowFlag(&MSX, CMH));
    call(MSXsetTimeParameter(&MSX, DURATION, 80*hour));
    call(MSXsetTimeParameter(&MSX, HYDSTEP, 1*hour));
    call(MSXsetTimeParameter(&MSX, QUALSTEP, 8*hour));
    call(MSXsetTimeParameter(&MSX, REPORTSTEP, 8*hour));
    call(MSXsetTimeParameter(&MSX, REPORTSTART, 0));
    // Add nodes
    call(MSXaddNode(&MSX, "a"));
    call(MSXaddNode(&MSX, "b"));
    call(MSXaddNode(&MSX, "c"));
    call(MSXaddNode(&MSX, "e"));
    call(MSXaddReservoir(&MSX, "source", 0,0,0));
    // Add links
    call(MSXaddLink(&MSX, "1", "source", "a", 1000, 200, 100));
    call(MSXaddLink(&MSX, "2", "a", "b", 800, 150, 100));
    call(MSXaddLink(&MSX, "3", "a", "c", 1200, 200, 100));
    call(MSXaddLink(&MSX, "4", "b", "c", 1000, 150, 100));
    call(MSXaddLink(&MSX, "5", "c", "e", 2000, 150, 100));

    // Add Options
    call(MSXaddOption(&MSX, AREA_UNITS_OPTION, "M2"));
    call(MSXaddOption(&MSX, RATE_UNITS_OPTION, "HR"));
    call(MSXaddOption(&MSX, SOLVER_OPTION, "RK5"));
    call(MSXaddOption(&MSX, TIMESTEP_OPTION, "28800"));
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
    call(MSXaddExpression(&MSX, LINK, RATE, "AS5", "Ka*AS3*NH2CL-Av*(K1*(Smax-AS5s)*AS5-K2*AS5s)"));
    call(MSXaddExpression(&MSX, LINK, RATE, "NH2CL", "-Kb*NH2CL"));
    call(MSXaddExpression(&MSX, LINK, EQUIL, "AS5s", "Ks*Smax*AS5/(1+Ks*AS5)-AS5s"));
    call(MSXaddExpression(&MSX, LINK, FORMULA, "AStot", "AS3 + AS5"));

    call(MSXaddExpression(&MSX, TANK, RATE, "AS3", "-Ka*AS3*NH2CL"));
    call(MSXaddExpression(&MSX, TANK, RATE, "AS5", "Ka*AS3*NH2CL"));
    call(MSXaddExpression(&MSX, TANK, RATE, "NH2CL", "-Kb*NH2CL"));
    call(MSXaddExpression(&MSX, TANK, FORMULA, "AStot", "AS3+AS5"));
    
    //Add Quality
    call(MSXaddQuality(&MSX, "NODE", "AS3", 10.0, "source"));
    call(MSXaddQuality(&MSX, "NODE", "NH2CL", 2.5, "source"));

    //Setup Report
    call(MSXsetReport(&MSX, "NODE", "c", 2));
    call(MSXsetReport(&MSX, "NODE", "e", 2));
    call(MSXsetReport(&MSX, "LINK", "5", 2));
    call(MSXsetReport(&MSX, "SPECIE", "AStot", 2));
    call(MSXsetReport(&MSX, "SPECIE", "AS3", 2));
    call(MSXsetReport(&MSX, "SPECIE", "AS5", 2));
    call(MSXsetReport(&MSX, "SPECIE", "AS5s", 2));
    call(MSXsetReport(&MSX, "SPECIE", "NH2CL", 2));

    // Finish Setup
    call(MSX_init(&MSX));
    MSX.Saveflag = 1;


    // Run
    REAL4 demands[] = {0.040220, 0.033353, 0.053953, 0.022562, -0.150088};
    REAL4 heads[] = {327.371979, 327.172974, 327.164185, 326.991211, 328.083984};
    REAL4 flows[] = {0.150088, 0.039916, 0.069952, 0.006563, 0.022562};
    MSXsetHydraulics(&MSX, demands, heads, flows);
    long t, tleft;
    for (int i = 0; i < 10; i++) {
        MSXstep(&MSX, &t, &tleft);
    }

    

    call(MSXresults(&MSX, fname));

    // Close
    call(MSX_close(&MSX));

    printf("Simulation successfully completed.\nReport written to: %s\n", fname);
}