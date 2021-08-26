#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>

#include "legacytoolkit.h"                 // EPANET-MSX toolkit header file
#include "coretoolkit.h"
#include "msxtoolkit.h"
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
    CALL(err, MSX_setFlowFlag(MSX, GPM));
    CALL(err, MSX_setTimeParameter(MSX, DURATION, 168*HOUR));
    CALL(err, MSX_setTimeParameter(MSX, HYDSTEP, 1*HOUR));
    CALL(err, MSX_setTimeParameter(MSX, QUALSTEP, 5*MINUTE));
    CALL(err, MSX_setTimeParameter(MSX, REPORTSTEP, 1*HOUR));
    CALL(err, MSX_setTimeParameter(MSX, REPORTSTART, 0));
    CALL(err, MSX_setTimeParameter(MSX, PATTERNSTEP, 1*HOUR));
    CALL(err, MSX_setTimeParameter(MSX, PATTERNSTART, 0));

    // Add nodes
    CALL(err, MSX_addNode(MSX, "2"));
    CALL(err, MSX_addTank(MSX, "1", 19634.953125,0,39269.906250));

    // Add links
    CALL(err, MSX_addLink(MSX, "1", "1", "2", 1000, 12, 100));

    // Add Options
    CALL(err, MSX_addOption(MSX, AREA_UNITS_OPTION, "FT2"));
    CALL(err, MSX_addOption(MSX, RATE_UNITS_OPTION, "HR"));
    CALL(err, MSX_addOption(MSX, SOLVER_OPTION, "ROS2"));
    CALL(err, MSX_addOption(MSX, COUPLING_OPTION, "FULL"));
    CALL(err, MSX_addOption(MSX, TIMESTEP_OPTION, "300"));
    CALL(err, MSX_addOption(MSX, RTOL_OPTION, "0.0001"));
    CALL(err, MSX_addOption(MSX, ATOL_OPTION, "1.0e-8"));

    // Add Species
    CALL(err, MSX_setSize(MSX, SPECIES, 14));
    CALL(err, MSX_addSpecies(MSX, "HOCL", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSX_addSpecies(MSX, "NH3", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSX_addSpecies(MSX, "NH2CL", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSX_addSpecies(MSX, "NHCL2", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSX_addSpecies(MSX, "I", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSX_addSpecies(MSX, "OCL", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSX_addSpecies(MSX, "NH4", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSX_addSpecies(MSX, "ALK", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSX_addSpecies(MSX, "H", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSX_addSpecies(MSX, "OH", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSX_addSpecies(MSX, "CO3", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSX_addSpecies(MSX, "HCO3", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSX_addSpecies(MSX, "H2CO3", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSX_addSpecies(MSX, "chloramine", BULK, MMOLE, 0.0, 0.0));

    //Add Coefficents
    CALL(err, MSX_addCoefficeint(MSX, PARAMETER, "k1", 1.5e10));
    CALL(err, MSX_addCoefficeint(MSX, PARAMETER, "k2", 7.6e-2));
    CALL(err, MSX_addCoefficeint(MSX, PARAMETER, "k3", 1.0e6));
    CALL(err, MSX_addCoefficeint(MSX, PARAMETER, "k4", 2.3e-3));
    CALL(err, MSX_addCoefficeint(MSX, PARAMETER, "k6", 2.2e8));
    CALL(err, MSX_addCoefficeint(MSX, PARAMETER, "k7", 4.0e5));
    CALL(err, MSX_addCoefficeint(MSX, PARAMETER, "k8", 1.0e8));
    CALL(err, MSX_addCoefficeint(MSX, PARAMETER, "k9", 3.0e7));
    CALL(err, MSX_addCoefficeint(MSX, PARAMETER, "k10", 55.0));

    //Add terms
    CALL(err, MSX_addTerm(MSX, "k5", "(2.5e7*H) + (4.0e4*H2CO3) + (800*HCO3)"));
    CALL(err, MSX_addTerm(MSX, "a1", "k1*HOCL*NH3"));
    CALL(err, MSX_addTerm(MSX, "a2", "k2*NH2CL"));
    CALL(err, MSX_addTerm(MSX, "a3", "k3*HOCL*NH2CL"));
    CALL(err, MSX_addTerm(MSX, "a4", "k4*NHCL2"));
    CALL(err, MSX_addTerm(MSX, "a5", "k5*NH2CL*NH2CL"));
    CALL(err, MSX_addTerm(MSX, "a6", "k6*NHCL2*NH3*H"));
    CALL(err, MSX_addTerm(MSX, "a7", "k7*NHCL2*OH"));
    CALL(err, MSX_addTerm(MSX, "a8", "k8*I*NHCL2"));
    CALL(err, MSX_addTerm(MSX, "a9", "k9*I*NH2CL"));
    CALL(err, MSX_addTerm(MSX, "a10", "k10*NH2CL*NHCL2"));

    //Add Expressions
    CALL(err, MSX_addExpression(MSX, LINK, RATE, "HOCL", "-a1 + a2 - a3 + a4 + a8"));
    CALL(err, MSX_addExpression(MSX, LINK, RATE, "NH3", "-a1 + a2 + a5 - a6"));
    CALL(err, MSX_addExpression(MSX, LINK, RATE, "NH2CL", "a1 - a2 - a3 + a4 - a5 + a6 - a9 - a10"));
    CALL(err, MSX_addExpression(MSX, LINK, RATE, "NHCL2", "a3 - a4 + a5 - a6 - a7 - a8 - a10"));
    CALL(err, MSX_addExpression(MSX, LINK, RATE, "I", "a7 - a8 - a9"));
    CALL(err, MSX_addExpression(MSX, LINK, RATE, "H", "0"));
    CALL(err, MSX_addExpression(MSX, LINK, RATE, "ALK", "0"));

    CALL(err, MSX_addExpression(MSX, LINK, EQUIL, "OCL", "H*OCL - 3.16E-8*HOCL"));
    CALL(err, MSX_addExpression(MSX, LINK, EQUIL, "NH4", "H*NH3 - 5.01E-10*NH4"));
    CALL(err, MSX_addExpression(MSX, LINK, EQUIL, "CO3", "H*CO3 - 5.01E-11*HCO3"));
    CALL(err, MSX_addExpression(MSX, LINK, EQUIL, "H2CO3", "H*HCO3 - 5.01E-7*H2CO3"));
    CALL(err, MSX_addExpression(MSX, LINK, EQUIL, "HCO3", "ALK - HCO3 - 2*CO3 - OH + H"));
    CALL(err, MSX_addExpression(MSX, LINK, EQUIL, "OH", "H*OH - 1.0E-14"));

    CALL(err, MSX_addExpression(MSX, LINK, FORMULA, "chloramine", "1000*NH2CL"));

    
    //Add Quality
    CALL(err, MSX_addQuality(MSX, "GLOBAL", "NH2CL", 0.05E-3, ""));
    CALL(err, MSX_addQuality(MSX, "GLOBAL", "ALK", 0.004, ""));
    CALL(err, MSX_addQuality(MSX, "GLOBAL", "H", 2.82e-8, ""));

    //Setup Report
    CALL(err, MSX_setReport(MSX, "NODE", "1", 0));
    CALL(err, MSX_setReport(MSX, "SPECIE", "chloramine", 4));

    // Finish Setup
    CALL(err, MSX_init(MSX));

    // Run
    float demands[] = {0.0, 0.0};
    float heads[] = {10.0, 10.0};
    float flows[] = {0.0};
    CALL(err, MSX_setHydraulics(MSX, demands, heads, flows));

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
        CALL(err, MSX_step(MSX, &t, &tleft));
        newHour = t / 3600;
    }
    CALL(err, MSXsaveFinalResults(MSX));
    CALL(err, MSXreport(MSX, fname));

    // Close
    CALL(err, MSX_close(MSX));
    return err;
}


int DLLEXPORT newBatchExample(char *fname) {
    // Removes old contents of file given
    FILE *f = NULL;
    if (fname != NULL) f = fopen(fname, "w");
    if (f != NULL) fclose(f);

    int err = 0;
    CALL(err, MSXopen());
    

    // Builing the network from batch-nh2cl.inp
    CALL(err, MSXsetFlowFlag(GPM));
    CALL(err, MSXsetTimeParameter(DURATION, 168*HOUR));
    CALL(err, MSXsetTimeParameter(HYDSTEP, 1*HOUR));
    CALL(err, MSXsetTimeParameter(QUALSTEP, 5*MINUTE));
    CALL(err, MSXsetTimeParameter(REPORTSTEP, 1*HOUR));
    CALL(err, MSXsetTimeParameter(REPORTSTART, 0));
    CALL(err, MSXsetTimeParameter(PATTERNSTEP, 1*HOUR));
    CALL(err, MSXsetTimeParameter(PATTERNSTART, 0));

    // Add nodes
    CALL(err, MSXaddNode("2"));
    CALL(err, MSXaddTank("1", 19634.953125,0,39269.906250));

    // Add links
    CALL(err, MSXaddLink("1", "1", "2", 1000, 12, 100));

    // Add Options
    CALL(err, MSXaddOption(AREA_UNITS_OPTION, "FT2"));
    CALL(err, MSXaddOption(RATE_UNITS_OPTION, "HR"));
    CALL(err, MSXaddOption(SOLVER_OPTION, "ROS2"));
    CALL(err, MSXaddOption(COUPLING_OPTION, "FULL"));
    CALL(err, MSXaddOption(TIMESTEP_OPTION, "300"));
    CALL(err, MSXaddOption(RTOL_OPTION, "0.0001"));
    CALL(err, MSXaddOption(ATOL_OPTION, "1.0e-8"));

    // Add Species
    CALL(err, MSXsetSize(SPECIES, 14));
    CALL(err, MSXaddSpecies("HOCL", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSXaddSpecies("NH3", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSXaddSpecies("NH2CL", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSXaddSpecies("NHCL2", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSXaddSpecies("I", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSXaddSpecies("OCL", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSXaddSpecies("NH4", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSXaddSpecies("ALK", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSXaddSpecies("H", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSXaddSpecies("OH", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSXaddSpecies("CO3", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSXaddSpecies("HCO3", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSXaddSpecies("H2CO3", BULK, MOLE, 0.0, 0.0));
    CALL(err, MSXaddSpecies("chloramine", BULK, MMOLE, 0.0, 0.0));

    //Add Coefficents
    CALL(err, MSXaddCoefficeint(PARAMETER, "k1", 1.5e10));
    CALL(err, MSXaddCoefficeint(PARAMETER, "k2", 7.6e-2));
    CALL(err, MSXaddCoefficeint(PARAMETER, "k3", 1.0e6));
    CALL(err, MSXaddCoefficeint(PARAMETER, "k4", 2.3e-3));
    CALL(err, MSXaddCoefficeint(PARAMETER, "k6", 2.2e8));
    CALL(err, MSXaddCoefficeint(PARAMETER, "k7", 4.0e5));
    CALL(err, MSXaddCoefficeint(PARAMETER, "k8", 1.0e8));
    CALL(err, MSXaddCoefficeint(PARAMETER, "k9", 3.0e7));
    CALL(err, MSXaddCoefficeint(PARAMETER, "k10", 55.0));

    //Add terms
    CALL(err, MSXaddTerm("k5", "(2.5e7*H) + (4.0e4*H2CO3) + (800*HCO3)"));
    CALL(err, MSXaddTerm("a1", "k1*HOCL*NH3"));
    CALL(err, MSXaddTerm("a2", "k2*NH2CL"));
    CALL(err, MSXaddTerm("a3", "k3*HOCL*NH2CL"));
    CALL(err, MSXaddTerm("a4", "k4*NHCL2"));
    CALL(err, MSXaddTerm("a5", "k5*NH2CL*NH2CL"));
    CALL(err, MSXaddTerm("a6", "k6*NHCL2*NH3*H"));
    CALL(err, MSXaddTerm("a7", "k7*NHCL2*OH"));
    CALL(err, MSXaddTerm("a8", "k8*I*NHCL2"));
    CALL(err, MSXaddTerm("a9", "k9*I*NH2CL"));
    CALL(err, MSXaddTerm("a10", "k10*NH2CL*NHCL2"));

    //Add Expressions
    CALL(err, MSXaddExpression(LINK, RATE, "HOCL", "-a1 + a2 - a3 + a4 + a8"));
    CALL(err, MSXaddExpression(LINK, RATE, "NH3", "-a1 + a2 + a5 - a6"));
    CALL(err, MSXaddExpression(LINK, RATE, "NH2CL", "a1 - a2 - a3 + a4 - a5 + a6 - a9 - a10"));
    CALL(err, MSXaddExpression(LINK, RATE, "NHCL2", "a3 - a4 + a5 - a6 - a7 - a8 - a10"));
    CALL(err, MSXaddExpression(LINK, RATE, "I", "a7 - a8 - a9"));
    CALL(err, MSXaddExpression(LINK, RATE, "H", "0"));
    CALL(err, MSXaddExpression(LINK, RATE, "ALK", "0"));

    CALL(err, MSXaddExpression(LINK, EQUIL, "OCL", "H*OCL - 3.16E-8*HOCL"));
    CALL(err, MSXaddExpression(LINK, EQUIL, "NH4", "H*NH3 - 5.01E-10*NH4"));
    CALL(err, MSXaddExpression(LINK, EQUIL, "CO3", "H*CO3 - 5.01E-11*HCO3"));
    CALL(err, MSXaddExpression(LINK, EQUIL, "H2CO3", "H*HCO3 - 5.01E-7*H2CO3"));
    CALL(err, MSXaddExpression(LINK, EQUIL, "HCO3", "ALK - HCO3 - 2*CO3 - OH + H"));
    CALL(err, MSXaddExpression(LINK, EQUIL, "OH", "H*OH - 1.0E-14"));

    CALL(err, MSXaddExpression(LINK, FORMULA, "chloramine", "1000*NH2CL"));

    
    //Add Quality
    CALL(err, MSXaddQuality("GLOBAL", "NH2CL", 0.05E-3, ""));
    CALL(err, MSXaddQuality("GLOBAL", "ALK", 0.004, ""));
    CALL(err, MSXaddQuality("GLOBAL", "H", 2.82e-8, ""));

    // Finish Setup
    CALL(err, MSXinit());

    // Run
    float demands[] = {0.0, 0.0};
    float heads[] = {10.0, 10.0};
    float flows[] = {0.0};
    CALL(err, MSXsetHydraulics(demands, heads, flows));

    long t = 0;
    long tleft = 1;
    int oldHour = -1;
    int newHour = 0;

    // Example of using the printQuality function in the loop rather than
    // saving results to binary out file and then calling the MSXreport function
    while (tleft >= 0 && err == 0) {
        if ( oldHour != newHour )
        {
            printf("\r  o Computing water quality at hour %-4d", newHour);
            fflush(stdout);
            CALL(err, MSXprintQuality(NODE, "1", "chloramine", fname));
            oldHour = newHour;
        }
        CALL(err, MSXstep(&t, &tleft));
        newHour = t / 3600;
    }
    printf("\n");


    // Close
    CALL(err, MSXclose());
    return err;
}