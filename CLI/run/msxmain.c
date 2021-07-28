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
#include <math.h>

#include "legacytoolkit.h"                 // EPANET-MSX toolkit header file
#include "coretoolkit.h"

#define CALL(err, f) (err = ( (err>100) ? (err) : (f) ))

int example1(char *fname);
int batchExample(char *fname);

#define MINUTE 60
#define HOUR 3600

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
    // CALL(err, example1(argv[1]));
    // if (err != 0) printf("Error occured!\nError code: %d\n", err);
    CALL(err, batchExample(argv[1]));
    if (err != 0) printf("Error occured!\nError code: %d\n", err);

    // MSXproject MSX;
    // CALL(err, MSXrunLegacy(MSX, argc, argv));
}

int example1(char *fname) {
    int err = 0;
    MSXproject MSX;
    CALL(err, MSX_open(&MSX));

    // Builing the network from example.inp
    CALL(err, MSXsetFlowFlag(MSX, CMH));
    CALL(err, MSXsetTimeParameter(MSX, DURATION, 80*HOUR));
    CALL(err, MSXsetTimeParameter(MSX, HYDSTEP, 1*HOUR));
    CALL(err, MSXsetTimeParameter(MSX, QUALSTEP, 8*HOUR));
    CALL(err, MSXsetTimeParameter(MSX, REPORTSTEP, 8*HOUR));
    CALL(err, MSXsetTimeParameter(MSX, REPORTSTART, 0));

    // Add nodes
    CALL(err, MSXaddNode(MSX, "a"));
    CALL(err, MSXaddNode(MSX, "b"));
    CALL(err, MSXaddNode(MSX, "c"));
    CALL(err, MSXaddNode(MSX, "e"));
    CALL(err, MSXaddReservoir(MSX, "source", 0,0,0));
    // Add links
    CALL(err, MSXaddLink(MSX, "1", "source", "a", 1000, 200, 100));
    CALL(err, MSXaddLink(MSX, "2", "a", "b", 800, 150, 100));
    CALL(err, MSXaddLink(MSX, "3", "a", "c", 1200, 200, 100));
    CALL(err, MSXaddLink(MSX, "4", "b", "c", 1000, 150, 100));
    CALL(err, MSXaddLink(MSX, "5", "c", "e", 2000, 150, 100));

    // Add Options
    CALL(err, MSXaddOption(MSX, AREA_UNITS_OPTION, "M2"));
    CALL(err, MSXaddOption(MSX, RATE_UNITS_OPTION, "HR"));
    CALL(err, MSXaddOption(MSX, SOLVER_OPTION, "RK5"));
    CALL(err, MSXaddOption(MSX, TIMESTEP_OPTION, "28800"));
    CALL(err, MSXaddOption(MSX, RTOL_OPTION, "0.001"));
    CALL(err, MSXaddOption(MSX, ATOL_OPTION, "0.0001"));

    // Add Species
    CALL(err, MSXaddSpecies(MSX, "AS3", BULK, UG, 0.0, 0.0));
    CALL(err, MSXaddSpecies(MSX, "AS5", BULK, UG, 0.0, 0.0));
    CALL(err, MSXaddSpecies(MSX, "AStot", BULK, UG, 0.0, 0.0));
    CALL(err, MSXaddSpecies(MSX, "AS5s", WALL, UG, 0.0, 0.0));
    CALL(err, MSXaddSpecies(MSX, "NH2CL", BULK, MG, 0.0, 0.0));
    
    //Add Coefficents
    CALL(err, MSXaddCoefficeint(MSX, CONSTANT, "Ka", 10.0));
    CALL(err, MSXaddCoefficeint(MSX, CONSTANT, "Kb", 0.1));
    CALL(err, MSXaddCoefficeint(MSX, CONSTANT, "K1", 5.0));
    CALL(err, MSXaddCoefficeint(MSX, CONSTANT, "K2", 1.0));
    CALL(err, MSXaddCoefficeint(MSX, CONSTANT, "Smax", 50));

    //Add terms
    CALL(err, MSXaddTerm(MSX, "Ks", "K1/K2"));

    //Add Expressions
    CALL(err, MSXaddExpression(MSX, LINK, RATE, "AS3", "-Ka*AS3*NH2CL"));
    CALL(err, MSXaddExpression(MSX, LINK, RATE, "AS5", "Ka*AS3*NH2CL-Av*(K1*(Smax-AS5s)*AS5-K2*AS5s)"));
    CALL(err, MSXaddExpression(MSX, LINK, RATE, "NH2CL", "-Kb*NH2CL"));
    CALL(err, MSXaddExpression(MSX, LINK, EQUIL, "AS5s", "Ks*Smax*AS5/(1+Ks*AS5)-AS5s"));
    CALL(err, MSXaddExpression(MSX, LINK, FORMULA, "AStot", "AS3 + AS5"));

    CALL(err, MSXaddExpression(MSX, TANK, RATE, "AS3", "-Ka*AS3*NH2CL"));
    CALL(err, MSXaddExpression(MSX, TANK, RATE, "AS5", "Ka*AS3*NH2CL"));
    CALL(err, MSXaddExpression(MSX, TANK, RATE, "NH2CL", "-Kb*NH2CL"));
    CALL(err, MSXaddExpression(MSX, TANK, FORMULA, "AStot", "AS3+AS5"));
    
    //Add Quality
    CALL(err, MSXaddQuality(MSX, "NODE", "AS3", 10.0, "source"));
    CALL(err, MSXaddQuality(MSX, "NODE", "NH2CL", 2.5, "source"));

    //Setup Report
    CALL(err, MSXsetReport(MSX, "NODE", "c", 2));
    CALL(err, MSXsetReport(MSX, "NODE", "e", 2));
    CALL(err, MSXsetReport(MSX, "LINK", "5", 2));
    CALL(err, MSXsetReport(MSX, "SPECIE", "AStot", 2));
    CALL(err, MSXsetReport(MSX, "SPECIE", "AS3", 2));
    CALL(err, MSXsetReport(MSX, "SPECIE", "AS5", 2));
    CALL(err, MSXsetReport(MSX, "SPECIE", "AS5s", 2));
    CALL(err, MSXsetReport(MSX, "SPECIE", "NH2CL", 2));

    // Finish Setup
    CALL(err, MSX_init(MSX));



    // Run
    float demands[] = {0.040220, 0.033353, 0.053953, 0.022562, -0.150088};
    float heads[] = {327.371979, 327.172974, 327.164185, 326.991211, 328.083984};
    float flows[] = {0.150088, 0.039916, 0.069952, 0.006563, 0.022562};
    MSXsetHydraulics(MSX, demands, heads, flows);
    long t = 0;
    long tleft = 1;
    int oldHour = -1;
    int newHour = 0;

    // Example of using the printQuality function in the loop rather than
    // saving results to binary out file and then calling the MSXreport function
    // while (tleft >= 0 && err == 0) {
    //     if ( oldHour != newHour )
    //     {
    //         printf("\r  o Computing water quality at hour %-4d", newHour);
    //         fflush(stdout);
    //         oldHour = newHour;
    //     }
    //     CALL(err, MSXprintQuality(MSX, NODE, "c", "NH2CL", fname));
    //     CALL(err, MSXprintQuality(MSX, LINK, "5", "AS5s", fname));
    //     CALL(err, MSXstep(MSX, &t, &tleft));
    //     newHour = t / 3600;
    // }


    while (tleft >= 0 && err == 0) {
        if ( oldHour != newHour )
        {
            printf("\r  o Computing water quality at hour %-4d", newHour);
            fflush(stdout);
            oldHour = newHour;
        }
        CALL(err, MSXsaveResults(MSX));
        CALL(err, MSXstep(MSX, &t, &tleft));
        newHour = t / 3600;
    }
    CALL(err, MSXsaveFinalResults(MSX));
    CALL(err, MSXreport(MSX, fname));


    // Close
    CALL(err, MSX_close(MSX));
    //if (err == 0) printf("Simulation successfully completed.\nReport written to: %s\n", fname);
    return err;
}


int batchExample(char *fname) {
    int err = 0;
    MSXproject MSX;
    CALL(err, MSX_open(&MSX));
    

    // Builing the network from batch-nh2cl.inp
    CALL(err, MSXsetFlowFlag(MSX, GPM));
    CALL(err, MSXsetTimeParameter(MSX, DURATION, 168*HOUR));
    CALL(err, MSXsetTimeParameter(MSX, HYDSTEP, 1*HOUR));
    CALL(err, MSXsetTimeParameter(MSX, QUALSTEP, 5*MINUTE));
    CALL(err, MSXsetTimeParameter(MSX, REPORTSTEP, 1*HOUR));
    CALL(err, MSXsetTimeParameter(MSX, REPORTSTART, 0));
    CALL(err, MSXsetTimeParameter(MSX, PATTERNSTEP, 1*HOUR));
    CALL(err, MSXsetTimeParameter(MSX, PATTERNSTART, 0));


    // Add nodes
    CALL(err, MSXaddNode(MSX, "2"));
    CALL(err, MSXaddTank(MSX, "1", 19634.953125,0,39269.906250));

    // Add links
    CALL(err, MSXaddLink(MSX, "1", "1", "2", 1000, 12, 100));

    // Add Options
    CALL(err, MSXaddOption(MSX, AREA_UNITS_OPTION, "FT2"));
    CALL(err, MSXaddOption(MSX, RATE_UNITS_OPTION, "HR"));
    CALL(err, MSXaddOption(MSX, SOLVER_OPTION, "ROS2"));
    CALL(err, MSXaddOption(MSX, COUPLING_OPTION, "FULL"));
    CALL(err, MSXaddOption(MSX, TIMESTEP_OPTION, "30"));
    CALL(err, MSXaddOption(MSX, RTOL_OPTION, "0.0001"));
    CALL(err, MSXaddOption(MSX, ATOL_OPTION, "1.0e-8"));

    // Add Species
    CALL(err, MSXsetSize(MSX, SPECIES, 14));
    CALL(err, MSXaddSpecies(MSX, "HOCL", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSXaddSpecies(MSX, "NH3", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSXaddSpecies(MSX, "NH2CL", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSXaddSpecies(MSX, "NHCL2", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSXaddSpecies(MSX, "I", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSXaddSpecies(MSX, "OCL", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSXaddSpecies(MSX, "NH4", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSXaddSpecies(MSX, "ALK", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSXaddSpecies(MSX, "H", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSXaddSpecies(MSX, "OH", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSXaddSpecies(MSX, "CO3", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSXaddSpecies(MSX, "HCO3", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSXaddSpecies(MSX, "H2CO3", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSXaddSpecies(MSX, "chloramine", BULK, MMOLE, 0.0, 0.0));

    //Add Coefficents
    CALL(err, MSXaddCoefficeint(MSX, PARAMETER, "k1", 1.5e10));
    CALL(err, MSXaddCoefficeint(MSX, PARAMETER, "k2", 7.6e-2));
    CALL(err, MSXaddCoefficeint(MSX, PARAMETER, "k3", 1.0e6));
    CALL(err, MSXaddCoefficeint(MSX, PARAMETER, "k4", 2.3e-3));
    CALL(err, MSXaddCoefficeint(MSX, PARAMETER, "k6", 2.2e8));
    CALL(err, MSXaddCoefficeint(MSX, PARAMETER, "k7", 4.0e5));
    CALL(err, MSXaddCoefficeint(MSX, PARAMETER, "k8", 1.0e8));
    CALL(err, MSXaddCoefficeint(MSX, PARAMETER, "k9", 3.0e7));
    CALL(err, MSXaddCoefficeint(MSX, PARAMETER, "k10", 55.0));

    //Add terms
    CALL(err, MSXaddTerm(MSX, "k5", "(2.5e7*H) + (4.0e4*H2CO3) + (800*HCO3)"));
    CALL(err, MSXaddTerm(MSX, "a1", "k1*HOCL*NH3"));
    CALL(err, MSXaddTerm(MSX, "a2", "k2*NH2CL"));
    CALL(err, MSXaddTerm(MSX, "a3", "k3*HOCL*NH2CL"));
    CALL(err, MSXaddTerm(MSX, "a4", "k4*NHCL2"));
    CALL(err, MSXaddTerm(MSX, "a5", "k5*NH2CL*NH2CL"));
    CALL(err, MSXaddTerm(MSX, "a6", "k6*NHCL2*NH3*H"));
    CALL(err, MSXaddTerm(MSX, "a7", "k7*NHCL2*OH"));
    CALL(err, MSXaddTerm(MSX, "a8", "k8*I*NHCL2"));
    CALL(err, MSXaddTerm(MSX, "a9", "k9*I*NH2CL"));
    CALL(err, MSXaddTerm(MSX, "a10", "k10*NH2CL*NHCL2"));

    //Add Expressions
    CALL(err, MSXaddExpression(MSX, LINK, RATE, "HOCL", "-a1 + a2 - a3 + a4 + a8"));
    CALL(err, MSXaddExpression(MSX, LINK, RATE, "NH3", "-a1 + a2 + a5 - a6"));
    CALL(err, MSXaddExpression(MSX, LINK, RATE, "NH2CL", "a1 - a2 - a3 + a4 - a5 + a6 - a9 - a10"));
    CALL(err, MSXaddExpression(MSX, LINK, RATE, "NHCL2", "a3 - a4 + a5 - a6 - a7 - a8 - a10"));
    CALL(err, MSXaddExpression(MSX, LINK, RATE, "I", "a7 - a8 - a9"));
    CALL(err, MSXaddExpression(MSX, LINK, RATE, "H", "0"));
    CALL(err, MSXaddExpression(MSX, LINK, RATE, "ALK", "0"));

    CALL(err, MSXaddExpression(MSX, LINK, EQUIL, "OCL", "H*OCL - 3.16E-8*HOCL"));
    CALL(err, MSXaddExpression(MSX, LINK, EQUIL, "NH4", "H*NH3 - 5.01E-10*NH4"));
    CALL(err, MSXaddExpression(MSX, LINK, EQUIL, "CO3", "H*CO3 - 5.01E-11*HCO3"));
    CALL(err, MSXaddExpression(MSX, LINK, EQUIL, "H2CO3", "H*HCO3 - 5.01E-7*H2CO3"));
    CALL(err, MSXaddExpression(MSX, LINK, EQUIL, "HCO3", "ALK - HCO3 - 2*CO3 - OH + H"));
    CALL(err, MSXaddExpression(MSX, LINK, EQUIL, "OH", "H*OH - 1.0E-14"));

    CALL(err, MSXaddExpression(MSX, LINK, FORMULA, "chloramine", "1000*NH2CL"));

    
    //Add Quality
    CALL(err, MSXaddQuality(MSX, "GLOBAL", "NH2CL", 0.05E-3, ""));
    CALL(err, MSXaddQuality(MSX, "GLOBAL", "ALK", 0.004, ""));
    CALL(err, MSXaddQuality(MSX, "GLOBAL", "H", 2.82e-8, ""));

    //Setup Report
    CALL(err, MSXsetReport(MSX, "NODE", "1", 0));
    CALL(err, MSXsetReport(MSX, "SPECIE", "chloramine", 4));

    // Finish Setup
    CALL(err, MSX_init(MSX));

    // Run
    float demands[] = {0.0, 0.0};
    float heads[] = {10.0, 10.0};
    float flows[] = {0.0};
    CALL(err, MSXsetHydraulics(MSX, demands, heads, flows));

    long t = 0;
    long tleft = 1;
    int oldHour = -1;
    int newHour = 0;
    while (tleft >= 0 && err == 0) {
        if ( oldHour != newHour )
        {
            printf("\r  o Computing water quality at hour %-4d", newHour);
            fflush(stdout);
            oldHour = newHour;
        }
        CALL(err, MSXsaveResults(MSX));
        CALL(err, MSXstep(MSX, &t, &tleft));
        newHour = t / 3600;
    }
    CALL(err, MSXsaveFinalResults(MSX));


    CALL(err, MSXreport(MSX, fname));

    // Close
    CALL(err, MSX_close(MSX));
    return err;
}
