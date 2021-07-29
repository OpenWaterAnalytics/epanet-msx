



#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>

#include "legacytoolkit.h"                 // EPANET-MSX toolkit header file
#include "coretoolkit.h"
#include "examples.h"

#define CALL(err, f) (err = ( (err>100) ? (err) : (f) ))

#define MINUTE 60
#define HOUR 3600

int DLLEXPORT batchExample(char *fname) {
    // Removes old contents of file given
    FILE *f = NULL;
    if (fname != NULL) f = fopen(fname, "w");
    if (f != NULL) fclose(f);

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
    CALL(err, MSXaddOption(MSX, TIMESTEP_OPTION, "300"));
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
    // while (tleft >= 0 && err == 0) {
    //     if ( oldHour != newHour )
    //     {
    //         printf("\r  o Computing water quality at hour %-4d", newHour);
    //         fflush(stdout);
    //         oldHour = newHour;
    //     }
    //     CALL(err, MSXsaveResults(MSX));
    //     CALL(err, MSXstep(MSX, &t, &tleft));
    //     newHour = t / 3600;
    // }
    // CALL(err, MSXsaveFinalResults(MSX));
    // CALL(err, MSXreport(MSX, fname));

    // Example of using the printQuality function in the loop rather than
    // saving results to binary out file and then calling the MSXreport function
    while (tleft >= 0 && err == 0) {
        if ( oldHour != newHour )
        {
            printf("\r  o Computing water quality at hour %-4d", newHour);
            fflush(stdout);
            CALL(err, MSXprintQuality(MSX, NODE, "1", "chloramine", fname));
            oldHour = newHour;
        }
        CALL(err, MSXstep(MSX, &t, &tleft));
        newHour = t / 3600;
    }
    printf("\n");


    // Close
    CALL(err, MSX_close(MSX));
    return err;
}
