/*******************************************************************************
**  MODULE:        MSXMAIN.C
**  PROJECT:       EPANET-MSX
**  DESCRIPTION:   Main module of the EPANET Multi-Species Extension toolkit.
**  COPYRIGHT:     Copyright (C) 2007 Feng Shang, Lewis Rossman, and James Uber.
**                 All Rights Reserved. See license information in LICENSE.TXT.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**                 K. Arrowood, Xylem intern
**  VERSION:       1.1.00
**  LAST UPDATE:   Refer to git history
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

int example1(char *fname);


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
    int err = 0;
    err = example1(argv[1]);
    if (err != 0) printf("Error occured!\nError code: %d\n", err);
    //If legacy then
    // MSXproject MSX;
    // CALL(err, MSXrunLegacy(&MSX, argc, argv));
}

int example1(char *fname) {
    int err = 0;
    MSXproject MSX;
    CALL(err, MSX_open(&MSX));
    
    int minute = 60;
    int hour = 60*minute;
    // Builing the network from example.inp
    CALL(err, MSXsetFlowFlag(&MSX, CMH));
    CALL(err, MSXsetTimeParameter(&MSX, DURATION, 80*hour));
    CALL(err, MSXsetTimeParameter(&MSX, HYDSTEP, 1*hour));
    CALL(err, MSXsetTimeParameter(&MSX, QUALSTEP, 8*hour));
    CALL(err, MSXsetTimeParameter(&MSX, REPORTSTEP, 8*hour));
    CALL(err, MSXsetTimeParameter(&MSX, REPORTSTART, 0));
    // Add nodes
    CALL(err, MSXaddNode(&MSX, "a"));
    CALL(err, MSXaddNode(&MSX, "b"));
    CALL(err, MSXaddNode(&MSX, "c"));
    CALL(err, MSXaddNode(&MSX, "e"));
    CALL(err, MSXaddReservoir(&MSX, "source", 0,0,0));
    // Add links
    CALL(err, MSXaddLink(&MSX, "1", "source", "a", 1000, 200, 100));
    CALL(err, MSXaddLink(&MSX, "2", "a", "b", 800, 150, 100));
    CALL(err, MSXaddLink(&MSX, "3", "a", "c", 1200, 200, 100));
    CALL(err, MSXaddLink(&MSX, "4", "b", "c", 1000, 150, 100));
    CALL(err, MSXaddLink(&MSX, "5", "c", "e", 2000, 150, 100));

    // Add Options
    CALL(err, MSXaddOption(&MSX, AREA_UNITS_OPTION, "M2"));
    CALL(err, MSXaddOption(&MSX, RATE_UNITS_OPTION, "HR"));
    CALL(err, MSXaddOption(&MSX, SOLVER_OPTION, "RK5"));
    CALL(err, MSXaddOption(&MSX, TIMESTEP_OPTION, "28800"));
    CALL(err, MSXaddOption(&MSX, RTOL_OPTION, "0.001"));
    CALL(err, MSXaddOption(&MSX, ATOL_OPTION, "0.0001"));

    // Add Species
    CALL(err, MSXaddSpecies(&MSX, "AS3", BULK, UG, 0.0, 0.0));
    CALL(err, MSXaddSpecies(&MSX, "AS5", BULK, UG, 0.0, 0.0));
    CALL(err, MSXaddSpecies(&MSX, "AStot", BULK, UG, 0.0, 0.0));
    CALL(err, MSXaddSpecies(&MSX, "AS5s", WALL, UG, 0.0, 0.0));
    CALL(err, MSXaddSpecies(&MSX, "NH2CL", BULK, MG, 0.0, 0.0));
    
    //Add Coefficents
    CALL(err, MSXaddCoefficeint(&MSX, CONSTANT, "Ka", 10.0));
    CALL(err, MSXaddCoefficeint(&MSX, CONSTANT, "Kb", 0.1));
    CALL(err, MSXaddCoefficeint(&MSX, CONSTANT, "K1", 5.0));
    CALL(err, MSXaddCoefficeint(&MSX, CONSTANT, "K2", 1.0));
    CALL(err, MSXaddCoefficeint(&MSX, CONSTANT, "Smax", 50));

    //Add terms
    CALL(err, MSXaddTerm(&MSX, "Ks", "K1/K2"));

    //Add Expressions
    CALL(err, MSXaddExpression(&MSX, LINK, RATE, "AS3", "-Ka*AS3*NH2CL"));
    CALL(err, MSXaddExpression(&MSX, LINK, RATE, "AS5", "Ka*AS3*NH2CL-Av*(K1*(Smax-AS5s)*AS5-K2*AS5s)"));
    CALL(err, MSXaddExpression(&MSX, LINK, RATE, "NH2CL", "-Kb*NH2CL"));
    CALL(err, MSXaddExpression(&MSX, LINK, EQUIL, "AS5s", "Ks*Smax*AS5/(1+Ks*AS5)-AS5s"));
    CALL(err, MSXaddExpression(&MSX, LINK, FORMULA, "AStot", "AS3 + AS5"));

    CALL(err, MSXaddExpression(&MSX, TANK, RATE, "AS3", "-Ka*AS3*NH2CL"));
    CALL(err, MSXaddExpression(&MSX, TANK, RATE, "AS5", "Ka*AS3*NH2CL"));
    CALL(err, MSXaddExpression(&MSX, TANK, RATE, "NH2CL", "-Kb*NH2CL"));
    CALL(err, MSXaddExpression(&MSX, TANK, FORMULA, "AStot", "AS3+AS5"));
    
    //Add Quality
    CALL(err, MSXaddQuality(&MSX, "NODE", "AS3", 10.0, "source"));
    CALL(err, MSXaddQuality(&MSX, "NODE", "NH2CL", 2.5, "source"));

    //Setup Report
    CALL(err, MSXsetReport(&MSX, "NODE", "c", 2));
    CALL(err, MSXsetReport(&MSX, "NODE", "e", 2));
    CALL(err, MSXsetReport(&MSX, "LINK", "5", 2));
    CALL(err, MSXsetReport(&MSX, "SPECIE", "AStot", 2));
    CALL(err, MSXsetReport(&MSX, "SPECIE", "AS3", 2));
    CALL(err, MSXsetReport(&MSX, "SPECIE", "AS5", 2));
    CALL(err, MSXsetReport(&MSX, "SPECIE", "AS5s", 2));
    CALL(err, MSXsetReport(&MSX, "SPECIE", "NH2CL", 2));

    // Finish Setup
    CALL(err, MSX_init(&MSX));
    MSX.Saveflag = 1;


    // Run
    REAL4 demands[] = {0.040220, 0.033353, 0.053953, 0.022562, -0.150088};
    REAL4 heads[] = {327.371979, 327.172974, 327.164185, 326.991211, 328.083984};
    REAL4 flows[] = {0.150088, 0.039916, 0.069952, 0.006563, 0.022562};
    MSXsetHydraulics(&MSX, demands, heads, flows);
    long t = 0;
    long tleft = 1;
    // CALL(err, MSXsaveResults(&MSX));
    CALL(err, MSXprintQuality(&MSX, NODE, "c", "NH2CL", fname));
    while (tleft > 0) {
        CALL(err, MSXstep(&MSX, &t, &tleft));
        CALL(err, MSXprintQuality(&MSX, NODE, "c", "NH2CL", fname));
        // if (MSX.Saveflag) {
        //     CALL(err, MSXsaveResults(&MSX));
        // }
    }
    // if (MSX.Saveflag) {
    //     CALL(err, MSXsaveFinalResults(&MSX));
    // }


    

    // CALL(err, MSXreport(&MSX, fname));

    // Close
    CALL(err, MSX_close(&MSX));
    //if (err == 0) printf("Simulation successfully completed.\nReport written to: %s\n", fname);
    return err;
}