/*******************************************************************************
**  TITLE:         CHEMISTRY.C
**  DESCRIPTION:   Water quality chemistry functions for the EPANET
**                 Multi-Species Extension toolkit.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**  VERSION:       1.00
**  LAST UPDATE:   11/9/06
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "typesmsx.h"
#define  EXTERN_MSX extern
#include "globals.h"
#include "rk5.h"
#include "ros2.h"
#include "newton.h"

//  Constants
//-----------
int    MAXIT = 20;                     // Max. number of iterations used
                                       // in nonlinear equation solver
int    NUMSIG = 3;                     // Number of significant digits in
                                       // nonlinear equation solver error

//  Imported variables
//--------------------
extern Pseg     *FirstSeg;             // First (downstream) segment in each pipe
extern double   *C1;                   // Vector of species concentrations

//  Local variables
//-----------------
static Pseg   TheSeg;                  // Current water quality segment
static int    TheLink;                 // Index of current link
static int    TheNode;                 // Index of current node
static int    NumPipeRateSpecies;      // Number of species with pipe rates
static int    NumTankRateSpecies;      // Number of species with tank rates
static int    NumPipeFormulaSpecies;   // Number of species with pipe formulas
static int    NumTankFormulaSpecies;   // Number of species with tank formulas
static int    NumPipeEquilSpecies;     // Number of species with pipe equilibria
static int    NumTankEquilSpecies;     // Number of species with tank equilibria
static int    *PipeRateSpecies;        // Species governed by pipe reactions
static int    *TankRateSpecies;        // Species governed by tank reactions
static int    *PipeEquilSpecies;       // Species governed by pipe equilibria
static int    *TankEquilSpecies;       // Species governed by tank equilibria
static int    LastIndex[MAX_OBJECTS];  // Last index of given type of variable
static double *Atol;                   // Absolute concentration tolerances
static double *Rtol;                   // Relative concentration tolerances
static double *Y;                      // Species concentrations
static double HydVar[MAX_HYD_VARS];    // Values of hydraulic variables

//  Exported functions
//--------------------
int  chemistry_open(void);
int  chemistry_react(long dt);
int  chemistry_equil(int zone, double *c);
void chemistry_close(void);

//  Local functions
//-----------------
static void   setSpeciesChemistry(void);
static void   setTankChemistry(void);
static void   evalHydVariables(int k);
static int    evalPipeReactions(int k, long dt);
static int    evalTankReactions(int k, long dt);
static int    evalPipeEquil(double *c);
static int    evalTankEquil(double *c);
static void   evalPipeFormulas(double *c);
static void   evalTankFormulas(double *c);
static double getPipeVariableValue(int i);
static double getTankVariableValue(int i);
static void   getPipeDcDt(double t, double y[], int n, double deriv[]);
static void   getTankDcDt(double t, double y[], int n, double deriv[]);
static void   getPipeEquil(double t, double y[], int n, double f[]);
static void   getTankEquil(double t, double y[], int n, double f[]);

//=============================================================================

int  chemistry_open()
/*
**  Purpose:
**    opens the multi-species chemistry system.
**
**  Input:
**    none.
**
**  Returns:
**    an error code (0 if no error).
*/
{
    int m;
    int numWallSpecies;
    int numBulkSpecies;
    int numTankExpr;
    int numPipeExpr;
    int errcode = 0;

// --- allocate memory

    PipeRateSpecies = NULL;
    TankRateSpecies = NULL;
    PipeEquilSpecies = NULL;
    TankEquilSpecies = NULL;
    Atol = NULL;
    Rtol = NULL;
    Y = NULL;
    m = Nobjects[SPECIE] + 1;
    PipeRateSpecies = (int *) calloc(m, sizeof(int));
    TankRateSpecies = (int *) calloc(m, sizeof(int));
    PipeEquilSpecies = (int *) calloc(m, sizeof(int));
    TankEquilSpecies = (int *) calloc(m, sizeof(int));
    Atol = (double *) calloc(m, sizeof(double));
    Rtol = (double *) calloc(m, sizeof(double));
    Y    = (double *) calloc(m, sizeof(double));
    ERRCODE(MEMCHECK(PipeRateSpecies));
    ERRCODE(MEMCHECK(TankRateSpecies));
    ERRCODE(MEMCHECK(PipeEquilSpecies));
    ERRCODE(MEMCHECK(TankEquilSpecies));
    ERRCODE(MEMCHECK(Atol));
    ERRCODE(MEMCHECK(Rtol));
    ERRCODE(MEMCHECK(Y));
    if ( errcode ) return errcode;

// --- assign species to each type of chemical expression

    setSpeciesChemistry();
    numPipeExpr = NumPipeRateSpecies + NumPipeFormulaSpecies + NumPipeEquilSpecies;
    numTankExpr = NumTankRateSpecies + NumTankFormulaSpecies + NumTankEquilSpecies;

// --- use pipe chemistry for tanks if latter was not supplied

    if ( numTankExpr == 0 )
    {
        setTankChemistry();
        numTankExpr = numPipeExpr;
    }

// --- check if enough equations were specified

    numWallSpecies = 0;
    numBulkSpecies = 0;
    for (m=1; m<=Nobjects[SPECIE]; m++)
    {
        if ( Specie[m].type == WALL ) numWallSpecies++;
        if ( Specie[m].type == BULK ) numBulkSpecies++;
    }
    if ( numPipeExpr != Nobjects[SPECIE] ) return ERR_NUM_PIPE_EXPR;
    if ( numTankExpr != numBulkSpecies   ) return ERR_NUM_TANK_EXPR;

// --- open the diff. eqn. solver: arguments are max. number of ODE's,
//     max. number of steps to be taken,
//     1 if automatic step sizing used (or 0 if not used),

    if ( Solver == RK5 )
    {
        if ( rk5_open(Nobjects[SPECIE], 1000, 1) == FALSE )
            return ERR_ODE_OPEN;
    }
    if ( Solver == ROS2 )
    {
        if ( ros2_open(Nobjects[SPECIE], 1) == FALSE )
            return ERR_ODE_OPEN;
    }

// --- open the algebraic eqn. solver

    m = MAX(NumPipeEquilSpecies, NumTankEquilSpecies);
    if ( newton_open(m) == FALSE ) return ERR_ALG_OPEN;

// --- assign entries to LastIndex array

    LastIndex[SPECIE] = Nobjects[SPECIE];
    LastIndex[TERM] = LastIndex[SPECIE] + Nobjects[TERM];
    LastIndex[PARAMETER] = LastIndex[TERM] + Nobjects[PARAMETER];
    LastIndex[CONSTANT] = LastIndex[PARAMETER] + Nobjects[CONSTANT];
    return 0;
}

//=============================================================================

void chemistry_close()
/*
**  Purpose:
**    closes the multi-species chemistry system.
**
**  Input:
**    none.
*/
{
    if ( Solver == RK5 ) rk5_close();
    if ( Solver == ROS2 ) ros2_close();
    newton_close();
    FREE(PipeRateSpecies);
    FREE(TankRateSpecies);
    FREE(PipeEquilSpecies);
    FREE(TankEquilSpecies);
    FREE(Atol);
    FREE(Rtol);
    FREE(Y);
}

//=============================================================================

int chemistry_react(long dt)
/*
**  Purpose:
**    computes reactions in all pipes and tanks.
**
**  Input:
**    dt = current WQ time step (sec).
**
**  Returns:
**    an error code or 0 if no error.
*/
{
    int k, m;
    int errcode = 0;

// --- save tolerances of pipe rate species

    for (k=1; k<=NumPipeRateSpecies; k++)
    {
        m = PipeRateSpecies[k];
        Atol[k] = Specie[m].aTol;
        Rtol[k] = Specie[m].rTol;
    }

// --- examine each link

    for (k=1; k<=Nobjects[LINK]; k++)
    {
    // --- skip non-pipe links

        if ( Link[k].len == 0.0 ) continue;

    // --- evaluate hydraulic variables

        evalHydVariables(k);

    // --- compute pipe reactions

        errcode = evalPipeReactions(k, dt);
        if ( errcode ) return errcode;
    }


// --- save tolerances of tank rate species

    for (k=1; k<=NumTankRateSpecies; k++)
    {
        m = TankRateSpecies[k];
        Atol[k] = Specie[m].aTol;
        Rtol[k] = Specie[m].rTol;
    }

// --- examine each tank

    for (k=1; k<=Nobjects[TANK]; k++)
    {
    // --- skip reservoirs

        if (Tank[k].a == 0.0) continue;

    // --- compute tank reactions

        errcode = evalTankReactions(k, dt);
        if ( errcode ) return errcode;
    }
    return errcode;
}

//=============================================================================

int chemistry_equil(int zone, double *c)
/*
**  Purpose:
**    computes equilibrium concentrations for a set of chemical species.
**
**  Input:
**    zone = reaction zone (NODE or LINK)
**    c[] = array of species concentrations
**
**  Output:
**    updated value of c[].
**
**  Returns:
**    an error code or 0 if no errors.
*/
{
    int errcode = 0;
    if ( zone == LINK )
    {
        if ( NumPipeEquilSpecies > 0 ) errcode = evalPipeEquil(c);
        evalPipeFormulas(c);
    }
    if ( zone == NODE )
    {
        if ( NumTankEquilSpecies > 0 ) errcode = evalTankEquil(c);
        evalTankFormulas(c);
    }
    return errcode;
}

//=============================================================================

void setSpeciesChemistry()
/*
**  Purpose:
**    determines which species are described by reaction rate
**    expressions, equilibrium expressions, or simple formulas.
**
**  Input:
**    none.
**
**  Output:
**    updates arrays of different chemistry types.
*/
{
    int m;
    NumPipeRateSpecies = 0;
    NumPipeFormulaSpecies = 0;
    NumPipeEquilSpecies = 0;
    NumTankRateSpecies = 0;
    NumTankFormulaSpecies = 0;
    NumTankEquilSpecies = 0;
    for (m=1; m<=Nobjects[SPECIE]; m++)
    {
        switch ( Specie[m].pipeExprType )
        {
          case RATE:
            NumPipeRateSpecies++;
            PipeRateSpecies[NumPipeRateSpecies] = m;
            break;

          case FORMULA:
            NumPipeFormulaSpecies++;
            break;

          case EQUIL:
            NumPipeEquilSpecies++;
            PipeEquilSpecies[NumPipeEquilSpecies] = m;
            break;
        }
        switch ( Specie[m].tankExprType )
        {
          case RATE:
            NumTankRateSpecies++;
            TankRateSpecies[NumTankRateSpecies] = m;
            break;

          case FORMULA:
            NumTankFormulaSpecies++;
            break;

          case EQUIL:
            NumTankEquilSpecies++;
            TankEquilSpecies[NumTankEquilSpecies] = m;
            break;
        }
    }
}

//=============================================================================

void setTankChemistry()
/*
**  Purpose:
**    assigns pipe chemistry expressions to tank chemistry for
**    each chemical species.
**
**  Input:
**    none.
**
**  Output:
**    updates arrays of different tank chemistry types.
*/
{
    int m;
    for (m=1; m<=Nobjects[SPECIE]; m++)
    {
        Specie[m].tankExpr = Specie[m].pipeExpr;
        Specie[m].tankExprType = Specie[m].pipeExprType;
    }
    NumTankRateSpecies = NumPipeRateSpecies;
    for (m=1; m<=NumTankRateSpecies; m++)
    {
        TankRateSpecies[m] = PipeRateSpecies[m];
    }
    NumTankFormulaSpecies = NumPipeFormulaSpecies;
    NumTankEquilSpecies = NumPipeEquilSpecies;
    for (m=1; m<=NumTankEquilSpecies; m++)
    {
        TankEquilSpecies[m] = PipeEquilSpecies[m];
    }
}

//=============================================================================

void evalHydVariables(int k)
/*
**  Purpose:
**    retrieves current values of hydraulic variables for the
**    current link being analyzed.
**
**  Input:
**    k = link index
**
**  Output:
**    updates values stored in vector HydVar[]
*/
{
    double dh;                         // headloss in ft
    double diam = Link[k].diam;        // diameter in ft
    double av;                         // area per unit volume

// --- pipe diameter in user's units (ft or m)
    HydVar[DIAMETER] = diam * Ucf[LENGTH_UNITS];

// --- flow rate in user's units
    HydVar[FLOW] = fabs(Q[k]) * Ucf[FLOW_UNITS];

// --- flow velocity in ft/sec
    if ( diam == 0.0 ) HydVar[VELOCITY] = 0.0;
    else HydVar[VELOCITY] = fabs(Q[k]) * 4.0 / PI / SQR(diam);

// --- Reynolds number
    HydVar[REYNOLDS] = HydVar[VELOCITY] * diam / VISCOS;

// --- flow velocity in user's units (ft/sec or m/sec)
    HydVar[VELOCITY] *= Ucf[LENGTH_UNITS];

// --- Darcy Weisbach friction factor
    if ( Link[k].len == 0.0 ) HydVar[FRICTION] = 0.0;
    else
    {
        dh = ABS(H[Link[k].n1] - H[Link[k].n2]);
        HydVar[FRICTION] = 39.725*dh*pow(diam,5)/Link[k].len/SQR(Q[k]);
    }

// --- shear velocity in user's units (ft/sec or m/sec)
    HydVar[SHEAR] = HydVar[VELOCITY] * sqrt(HydVar[FRICTION] / 8.0);

// --- pipe surface area / volume in area_units/L
    HydVar[AREAVOL] = 1.0;
    if ( diam > 0.0 )
    {
        av  = 4.0/diam;                // ft2/ft3
        av *= Ucf[AREA_UNITS];         // area_units/ft3
        av /= LperFT3;                 // area_units/L
        HydVar[AREAVOL] = av;
    }
}

//=============================================================================

int evalPipeReactions(int k, long dt)
/*
**  Purpose:
**    updates specie concentrations in each WQ segment of a pipe
**    after reactions occur over time step dt.
**
**  Input:
**    k = link index
**    dt = time step (sec).
**
**  Output:
**    updates values in the concentration vector C[] associated
**    with a pipe's WQ segments.
**
**  Returns:
**    an error code or 0 if no error.
*/
{
    int i, m;
    int errcode = 0, ierr = 0;
    double tstep = (double)dt / Ucf[RATE_UNITS];
    double c, dc, dh;

// --- start with the most downstream pipe segment

    TheLink = k;
    TheSeg = FirstSeg[TheLink];
    while ( TheSeg )
    {
    // --- store all segment specie concentrations in C1

        for (m=1; m<=Nobjects[SPECIE]; m++) C1[m] = TheSeg->c[m];
        ierr = 0;

    // --- react each reacting specie over the time step

        if ( dt > 0.0 )
        {
        // --- Euler integrator

            if ( Solver == EUL )
            {
                for (i=1; i<=NumPipeRateSpecies; i++)
                {
                    m = PipeRateSpecies[i];
                    dc = mathexpr_eval(Specie[m].pipeExpr, getPipeVariableValue)
                         * tstep;
                    c = TheSeg->c[m] + dc;
                    TheSeg->c[m] = MAX(c, 0.0);
                }
            }

        // --- other integrators
            else
            {
            // --- place current concentrations of species that react in vector Y

                for (i=1; i<=NumPipeRateSpecies; i++)
                {
                    m = PipeRateSpecies[i];
                    Y[i] = TheSeg->c[m];
                }
                dh = TheSeg->hstep;

            // --- integrate the set of rate equations

            // --- Runge-Kutta integrator

                if ( Solver == RK5 )
                    ierr = rk5_integrate(Y, NumPipeRateSpecies, 0, tstep,
                                         &dh, Atol, Rtol, getPipeDcDt);

            // --- Rosenbrock integrator

                if ( Solver == ROS2 )
                    ierr = ros2_integrate(Y, NumPipeRateSpecies, 0, tstep,
                                          &dh, Atol, Rtol, getPipeDcDt);

            // --- save new concentration values of the species that reacted

                for (i=1; i<=NumPipeRateSpecies; i++)
                {
                    m = PipeRateSpecies[i];
                    TheSeg->c[m] = MAX(Y[i], 0.0);
                }
                TheSeg->hstep = dh;
            }
            if ( ierr < 0 ) return ERR_INTEGRATOR;
        }

    // --- compute new equilibrium concentrations within segment

        errcode = chemistry_equil(LINK, TheSeg->c);
        if ( errcode ) return errcode;

    // --- move to the segment upstream of the current one

        TheSeg = TheSeg->prev;
    }
    return errcode;
}

//=============================================================================

int evalTankReactions(int k, long dt)
/*
**  Purpose:
**    updates specie concentrations in a given storage tank
**    after reactions occur over time step dt.
**
**  Input:
**    k = tank index
**    dt = time step (sec).
**
**  Output:
**    updates values in the concentration vector Tank[k].c[]
**    for tank k.
**
**  Returns:
**    an error code or 0 if no error.
*/
{
    int i, m;
    int errcode = 0, ierr = 0;
    double tstep = (double)dt / Ucf[RATE_UNITS];
    double c, dc, dh;

// --- store all tank specie concentrations in C1

    for (m=1; m<=Nobjects[SPECIE]; m++) C1[m] = Tank[k].c[m];
    TheNode = Tank[k].node;
    ierr = 0;

// --- react each reacting specie over the time step

    if ( dt > 0.0 )
    {
        if ( Solver == EUL)
        {
            for (i=1; i<=NumTankRateSpecies; i++)
            {
                m = TankRateSpecies[i];
                dc = tstep*mathexpr_eval(Specie[m].tankExpr, getTankVariableValue);
                c = Tank[k].c[m] + dc;
                Tank[k].c[m] = MAX(c, 0.0);
            }
        }

        else
        {
            for (i=1; i<=NumTankRateSpecies; i++)
            {
                m = TankRateSpecies[i];
                Y[i] = Tank[k].c[m];
            }
            dh = Tank[k].hstep;

            if ( Solver == RK5 )
                ierr = rk5_integrate(Y, NumTankRateSpecies, 0, tstep,
                                     &dh, Atol, Rtol, getTankDcDt);

            if ( Solver == ROS2 )
                ierr = ros2_integrate(Y, NumTankRateSpecies, 0, tstep,
                                      &dh, Atol, Rtol, getTankDcDt);

            for (i=1; i<=NumTankRateSpecies; i++)
            {
                m = TankRateSpecies[i];
                Tank[k].c[m] = MAX(Y[i], 0.0);
            }
            Tank[k].hstep = dh;
        }
        if ( ierr < 0 ) return ERR_INTEGRATOR;
    }

// --- compute new equilibrium concentrations within the tank

    errcode = chemistry_equil(NODE, Tank[k].c);
    return errcode;
}

//=============================================================================

int evalPipeEquil(double *c)
/*
**  Purpose:
**    computes equilibrium concentrations for water in a pipe segment.
**
**  Input:
**    c[] = array of starting species concentrations
**
**  Output:
**    c[] = array of equilibrium concentrations.
**
**  Returns:
**    an error code or 0 if no error.
*/
{
    int i, m;
    int errcode;
    for (m=1; m<=Nobjects[SPECIE]; m++) C1[m] = c[m];
    for (i=1; i<=NumPipeEquilSpecies; i++)
    {
        m = PipeEquilSpecies[i];
        Y[i] = c[m];
    }
    errcode = newton_solve(Y, NumPipeEquilSpecies, MAXIT, NUMSIG, getPipeEquil);
    if ( errcode < 0 ) return ERR_NEWTON;
    for (i=1; i<=NumPipeEquilSpecies; i++)
    {
        m = PipeEquilSpecies[i];
        c[m] = Y[i];
    }
    return 0;
}


//=============================================================================

int evalTankEquil(double *c)
/*
**  Purpose:
**    computes equilibrium concentrations for water in a tank.
**
**  Input:
**    c[] = array of starting species concentrations
**
**  Output:
**    c[] = array of equilibrium concentrations.
**
**  Returns:
**    an error code or 0 if no error.
*/
{
    int i, m;
    int errcode;
    for (m=1; m<=Nobjects[SPECIE]; m++) C1[m] = c[m];
    for (i=1; i<=NumTankEquilSpecies; i++)
    {
        m = TankEquilSpecies[i];
        Y[i] = c[m];
    }
    errcode = newton_solve(Y, NumTankEquilSpecies, MAXIT, NUMSIG, getTankEquil);
    if ( errcode < 0 ) return ERR_NEWTON;
    for (i=1; i<=NumTankEquilSpecies; i++)
    {
        m = TankEquilSpecies[i];
        c[m] = Y[i];
    }
    return 0;
}

//=============================================================================

void evalPipeFormulas(double *c)
/*
**  Purpose:
**    evaluates species concentrations in a pipe segment that are simple
**    formulas involving other known species concentrations.
**
**  Input:
**    c[] = array of current species concentrations.
**
**  Output:
**    c[] = array of updated concentrations.
*/
{
    int m;
    for (m=1; m<=Nobjects[SPECIE]; m++) C1[m] = c[m];
    for (m=1; m<=Nobjects[SPECIE]; m++)
    {
        if ( Specie[m].pipeExprType == FORMULA )
        {
            c[m] = mathexpr_eval(Specie[m].pipeExpr, getPipeVariableValue);
        }
    }
}

//=============================================================================

void evalTankFormulas(double *c)
/*
**  Purpose:
**    evaluates species concentrations in a tank that are simple
**    formulas involving other known species concentrations.
**
**  Input:
**    c[] = array of current species concentrations.
**
**  Output:
**    c[] = array of updated concentrations.
*/
{
    int m;
    for (m=1; m<=Nobjects[SPECIE]; m++) C1[m] = c[m];
    for (m=1; m<=Nobjects[SPECIE]; m++)
    {
        if ( Specie[m].tankExprType == FORMULA )
        {
            c[m] = mathexpr_eval(Specie[m].tankExpr, getTankVariableValue);
        }
    }
}

//=============================================================================

double getPipeVariableValue(int i)
/*
**  Purpose:
**    finds the value of a specie, a parameter, or a constant for
**    the pipe link being analyzed.
**
**  Input:
**    i = variable index.
**
**  Returns:
**    the current value of the indexed variable.
*/
{
// --- WQ species have index i between 1 & # of species
//     and their current values are stored in vector C1

    if ( i <= LastIndex[SPECIE] )
    {
    // --- if specie represented by a formula then evaluate it

        if ( Specie[i].pipeExprType == FORMULA )
        {
            return mathexpr_eval(Specie[i].pipeExpr, getPipeVariableValue);
        }

    // --- otherwise return the current concentration

        else return C1[i];
    }

// --- intermediate term expressions come next

    else if ( i <= LastIndex[TERM] )
    {
        i -= LastIndex[TERM-1];
        return mathexpr_eval(Term[i].expr, getPipeVariableValue);
    }

// --- reaction parameter indexes come after that

    else if ( i <= LastIndex[PARAMETER] )
    {
        i -= LastIndex[PARAMETER-1];
        return Link[TheLink].param[i];
    }

// --- followed by constants

    else if ( i <= LastIndex[CONSTANT] )
    {
        i -= LastIndex[CONSTANT-1];
        return Const[i].value;
    }

// --- and finally by hydraulic variables
    else
    {
        i -= LastIndex[CONSTANT];
        if (i < MAX_HYD_VARS) return HydVar[i];
        else return 0.0;
    }
    return 0.0;
}

//=============================================================================

double getTankVariableValue(int i)
/*
**  Purpose:
**    finds the value of a specie, a parameter, or a constant for
**    the current node being analyzed.
**
**  Input:
**    i = variable index.
**
**  Returns:
**    the current value of the indexed variable.
*/
{
    int j;

// --- WQ species have index i between 1 & # of species
//     and their current values are stored in vector C1

    if ( i <= LastIndex[SPECIE] )
    {
    // --- if specie represented by a formula then evaluate it

        if ( Specie[i].tankExprType == FORMULA )
        {
            return mathexpr_eval(Specie[i].tankExpr, getTankVariableValue);
        }

    // --- otherwise return the current concentration

        else return C1[i];
    }

// --- intermediate term expressions come next

    else if ( i <= LastIndex[TERM] )
    {
        i -= LastIndex[TERM-1];
        return mathexpr_eval(Term[i].expr, getTankVariableValue);
    }

// --- next come reaction parameters associated with Tank nodes

    else if (i <= LastIndex[PARAMETER] )
    {
        i -= LastIndex[PARAMETER-1];
        j = Node[TheNode].tank;
        if ( j > 0 )
        {
            return Tank[j].param[i];
        }
        else return 0.0;
    }

// --- and then come constants

    else if (i <= LastIndex[CONSTANT] )
    {
        i -= LastIndex[CONSTANT-1];
        return Const[i].value;
    }
    else return 0.0;
}

//=============================================================================

void getPipeDcDt(double t, double y[], int n, double deriv[])
/*
**  Purpose:
**    finds reaction rate (dC/dt) for each reacting species in a pipe.
**
**  Input:
**    t = current time (not used)
**    y[] = vector of reacting species concentrations
**    n = number of reacting species.
**
**  Output:
**    deriv[] = vector of reaction rates of each reacting species.
*/
{
    int i, m;

// --- assign specie concentrations to their proper positions in the global
//     concentration vector C1

    for (i=1; i<=n; i++)
    {
        m = PipeRateSpecies[i];
        C1[m] = y[i];
    }

// --- evaluate each pipe reaction expression

    for (i=1; i<=n; i++)
    {
        m = PipeRateSpecies[i];
        deriv[i] = mathexpr_eval(Specie[m].pipeExpr, getPipeVariableValue);
    }
}

//=============================================================================

void getTankDcDt(double t, double y[], int n, double deriv[])
/*
**  Purpose:
**    finds reaction rate (dC/dt) for each reacting species in a tank.
**
**  Input:
**    t = current time (not used)
**    y[] = vector of reacting species concentrations
**    n = number of reacting species.
**
**  Output:
**    deriv[] = vector of reaction rates of each reacting species.
*/
{
    int i, m;

// --- assign specie concentrations to their proper positions in the global
//     concentration vector C1

    for (i=1; i<=n; i++)
    {
        m = TankRateSpecies[i];
        C1[m] = y[i];
    }

// --- evaluate each tank reaction expression

    for (i=1; i<=n; i++)
    {
        m = TankRateSpecies[i];
        deriv[i] = mathexpr_eval(Specie[m].tankExpr, getTankVariableValue);
    }
}

//=============================================================================

void getPipeEquil(double t, double y[], int n, double f[])
/*
**  Purpose:
**    evaluates equilibrium expressions for pipe chemistry.
**
**  Input:
**    t = current time (not used)
**    y[] = vector of equilibrium species concentrations
**    n = number of equilibrium species.
**
**  Output:
**    f[] = vector of equilibrium function values.
*/
{
    int i, m;

// --- assign specie concentrations to their proper positions in the global
//     concentration vector C1

    for (i=1; i<=n; i++)
    {
        m = PipeEquilSpecies[i];
        C1[m] = y[i];
    }

// --- evaluate each pipe equilibrium expression

    for (i=1; i<=n; i++)
    {
        m = PipeEquilSpecies[i];
        f[i] = mathexpr_eval(Specie[m].pipeExpr, getPipeVariableValue);
    }
}

//=============================================================================

void getTankEquil(double t, double y[], int n, double f[])
/*
**  Purpose:
**    evaluates equilibrium expressions for tank chemistry.
**
**  Input:
**    t = current time (not used)
**    y[] = vector of equilibrium species concentrations
**    n = number of equilibrium species
**
**  Output:
**    f[] = vector of equilibrium function values.
*/
{
    int i, m;

// --- assign specie concentrations to their proper positions in the global
//     concentration vector C1

    for (i=1; i<=n; i++)
    {
        m = TankEquilSpecies[i];
        C1[m] = y[i];
    }

// --- evaluate each tank equilibrium expression

    for (i=1; i<=n; i++)
    {
        m = TankEquilSpecies[i];
        f[i] = mathexpr_eval(Specie[m].tankExpr, getTankVariableValue);
    }
}
