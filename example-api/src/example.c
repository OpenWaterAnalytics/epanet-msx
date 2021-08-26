
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
    CALL(err, MSX_setFlowFlag(MSX, CMH));
    CALL(err, MSX_setTimeParameter(MSX, DURATION, 80*HOUR));
    CALL(err, MSX_setTimeParameter(MSX, HYDSTEP, 1*HOUR));
    CALL(err, MSX_setTimeParameter(MSX, QUALSTEP, 8*HOUR));
    CALL(err, MSX_setTimeParameter(MSX, REPORTSTEP, 8*HOUR));
    CALL(err, MSX_setTimeParameter(MSX, REPORTSTART, 0));

    // Add nodes
    CALL(err, MSX_addNode(MSX, "a"));
    CALL(err, MSX_addNode(MSX, "b"));
    CALL(err, MSX_addNode(MSX, "c"));
    CALL(err, MSX_addNode(MSX, "e"));
    CALL(err, MSX_addReservoir(MSX, "source", 0,0,0));
    // Add links
    CALL(err, MSX_addLink(MSX, "1", "source", "a", 1000, 200, 100));
    CALL(err, MSX_addLink(MSX, "2", "a", "b", 800, 150, 100));
    CALL(err, MSX_addLink(MSX, "3", "a", "c", 1200, 200, 100));
    CALL(err, MSX_addLink(MSX, "4", "b", "c", 1000, 150, 100));
    CALL(err, MSX_addLink(MSX, "5", "c", "e", 2000, 150, 100));

    // Add Options
    CALL(err, MSX_addOption(MSX, AREA_UNITS_OPTION, "M2"));
    CALL(err, MSX_addOption(MSX, RATE_UNITS_OPTION, "HR"));
    CALL(err, MSX_addOption(MSX, SOLVER_OPTION, "RK5"));
    CALL(err, MSX_addOption(MSX, TIMESTEP_OPTION, "28800"));
    CALL(err, MSX_addOption(MSX, RTOL_OPTION, "0.001"));
    CALL(err, MSX_addOption(MSX, ATOL_OPTION, "0.0001"));

    // Add Species
    CALL(err, MSX_addSpecies(MSX, "AS3", BULK, UG, 0.0, 0.0));
    CALL(err, MSX_addSpecies(MSX, "AS5", BULK, UG, 0.0, 0.0));
    CALL(err, MSX_addSpecies(MSX, "AStot", BULK, UG, 0.0, 0.0));
    CALL(err, MSX_addSpecies(MSX, "AS5s", WALL, UG, 0.0, 0.0));
    CALL(err, MSX_addSpecies(MSX, "NH2CL", BULK, MG, 0.0, 0.0));
    
    //Add Coefficents
    CALL(err, MSX_addCoefficeint(MSX, CONSTANT, "Ka", 10.0));
    CALL(err, MSX_addCoefficeint(MSX, CONSTANT, "Kb", 0.1));
    CALL(err, MSX_addCoefficeint(MSX, CONSTANT, "K1", 5.0));
    CALL(err, MSX_addCoefficeint(MSX, CONSTANT, "K2", 1.0));
    CALL(err, MSX_addCoefficeint(MSX, CONSTANT, "Smax", 50));

    //Add terms
    CALL(err, MSX_addTerm(MSX, "Ks", "K1/K2"));

    //Add Expressions
    CALL(err, MSX_addExpression(MSX, LINK, RATE, "AS3", "-Ka*AS3*NH2CL"));
    CALL(err, MSX_addExpression(MSX, LINK, RATE, "AS5", "Ka*AS3*NH2CL-Av*(K1*(Smax-AS5s)*AS5-K2*AS5s)"));
    CALL(err, MSX_addExpression(MSX, LINK, RATE, "NH2CL", "-Kb*NH2CL"));
    CALL(err, MSX_addExpression(MSX, LINK, EQUIL, "AS5s", "Ks*Smax*AS5/(1+Ks*AS5)-AS5s"));
    CALL(err, MSX_addExpression(MSX, LINK, FORMULA, "AStot", "AS3 + AS5"));

    CALL(err, MSX_addExpression(MSX, TANK, RATE, "AS3", "-Ka*AS3*NH2CL"));
    CALL(err, MSX_addExpression(MSX, TANK, RATE, "AS5", "Ka*AS3*NH2CL"));
    CALL(err, MSX_addExpression(MSX, TANK, RATE, "NH2CL", "-Kb*NH2CL"));
    CALL(err, MSX_addExpression(MSX, TANK, FORMULA, "AStot", "AS3+AS5"));
    
    //Add Quality
    CALL(err, MSX_addQuality(MSX, "NODE", "AS3", 10.0, "source"));
    CALL(err, MSX_addQuality(MSX, "NODE", "NH2CL", 2.5, "source"));

    //Setup Report
    CALL(err, MSX_setReport(MSX, "NODE", "c", 2));
    CALL(err, MSX_setReport(MSX, "NODE", "e", 2));
    CALL(err, MSX_setReport(MSX, "LINK", "4", 2));
    CALL(err, MSX_setReport(MSX, "LINK", "5", 2));
    CALL(err, MSX_setReport(MSX, "SPECIE", "AStot", 2));
    CALL(err, MSX_setReport(MSX, "SPECIE", "AS3", 2));
    CALL(err, MSX_setReport(MSX, "SPECIE", "AS5", 2));
    CALL(err, MSX_setReport(MSX, "SPECIE", "AS5s", 5));
    CALL(err, MSX_setReport(MSX, "SPECIE", "NH2CL", 2));

    // Finish Setup
    CALL(err, MSX_init(MSX));



    // Run
    float demands[] = {0.040220, 0.033353, 0.053953, 0.022562, -0.150088};
    float heads[] = {327.371979, 327.172974, 327.164185, 326.991211, 328.083984};
    float flows[] = {0.150088, 0.039916, 0.069952, 0.006563, 0.022562};
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