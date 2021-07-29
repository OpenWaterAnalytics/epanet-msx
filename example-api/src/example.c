
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

int DLLEXPORT example1(char *fname) {
    // Removes old contents of file given
    FILE *f = NULL;
    if (fname != NULL) f = fopen(fname, "w");
    if (f != NULL) fclose(f);

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
    //         CALL(err, MSXprintQuality(MSX, NODE, "1", "chloramine", fname));
    //         oldHour = newHour;
    //     }
    //     CALL(err, MSXstep(MSX, &t, &tleft));
    //     newHour = t / 3600;
    // }
    // printf("\n");


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