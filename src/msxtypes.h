/************************************************************************
**  MODULE:        TYPES.H
**  PROJECT:       EPANET-MSX
**  DESCRIPTION:   Global constants and data types used by the EPANET
**                 Multi-Species Extension toolkit.
**  COPYRIGHT:     Copyright (C) 2007 Feng Shang, Lewis Rossman, and James Uber.
**                 All Rights Reserved. See license information in LICENSE.TXT.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**  VERSION:       1.1.00
**  LAST UPDATE:   2/8/11
**  Bug Fix:       Bug ID 08, Feng Shang, 01/07/08 
**                 Bug ID 09 (add roughness as hydraulic variable) Feng Shang 01/29/2008
***********************************************************************/

#include "mathexpr.h"

//-----------------------------------------------------------------------------
//  Definition of 4-byte integers & reals
//-----------------------------------------------------------------------------
typedef  int   INT4;
typedef  float REAL4;

//-----------------------------------------------------------------------------
//  Macros for memory allocation
//-----------------------------------------------------------------------------
#define  MEMCHECK(x)  (((x) == NULL) ? ERR_MEMORY : 0 )
#define  FREE(x) { if (x) { free(x); x = NULL; } }

//-----------------------------------------------------------------------------
//  Conversion macros to be used in place of functions
//-----------------------------------------------------------------------------
#define INT(x)   ((int)(x))                   // integer portion of x
#define FRAC(x)  ((x)-(int)(x))               // fractional part of x
#define ABS(x)   (((x)<0) ? -(x) : (x))       // absolute value of x
#define MIN(x,y) (((x)<=(y)) ? (x) : (y))     // minimum of x and y
#define MAX(x,y) (((x)>=(y)) ? (x) : (y))     // maximum of x and y
#define ROUND(x) (((x)>=0) ? (int)((x)+.5) : (int)((x)-.5))
                                              // round-off of x
#define MOD(x,y) ((x)%(y))                    // x modulus y
#define SQR(x)   ((x)*(x))                    // x-squared
#define SGN(x)   (((x)<0) ? (-1) : (1))       // sign of x
#define UCHAR(x) (((x) >= 'a' && (x) <= 'z') ? ((x)&~32) : (x))
                                              // uppercase char of x
//-----------------------------------------------------------------------------
//  Macro to evaluate function f with error checking
//  (Fatal errors are numbered higher than 100)
//-----------------------------------------------------------------------------
#define CALL(err, f) (err = ( (err>100) ? (err) : (f) ))


//-----------------------------------------------------------------------------
//  Defined Constants
//-----------------------------------------------------------------------------
#define   MAGICNUMBER  516114521
#define   VERSION      100000
#define   MAXMSG       1024            // Max. # characters in message text
#define   MAXLINE      1024            // Max. # characters in input line
#define   TRUE         1
#define   FALSE        0
#define   BIG          1.E10
#define   TINY         1.E-6
#define   MISSING      -1.E10
#define   PI           3.141592654
#define   VISCOS       1.1E-5          // Kinematic viscosity of water
                                       // @ 20 deg C (sq ft/sec)

//-----------------------------------------------------------------------------
//  Various conversion factors
//-----------------------------------------------------------------------------
#define   M2perFT2     0.09290304
#define   CM2perFT2    929.0304
#define   DAYperSEC    1.1574E-5
#define   HOURperSEC   2.7778E-4
#define   MINUTEperSEC 0.016667
#define   GPMperCFS    448.831
#define   AFDperCFS    1.9837
#define   MGDperCFS    0.64632
#define   IMGDperCFS   0.5382
#define   LPSperCFS    28.317
#define   LPMperCFS    1699.0
#define   CMHperCFS    101.94
#define   CMDperCFS    2446.6
#define   MLDperCFS    2.4466
#define   M3perFT3     0.028317
#define   LperFT3      28.317
#define   MperFT       0.3048
#define   PSIperFT     0.4333
#define   KPAperPSI    6.895
#define   KWperHP      0.7457
#define   SECperDAY    86400


//-----------------------------------------------------------------------------
//  Enumerated Types
//-----------------------------------------------------------------------------
 enum ObjectType                       // Object types                         //1.1.00
                {NODE,
                 LINK,
                 TANK,
                 SPECIES,
                 TERM,
                 PARAMETER,
                 CONSTANT,
                 PATTERN,
                 MAX_OBJECTS};

 enum SourceType                       // Type of source quality input
                {CONCEN,               //    inflow concentration
                 MASS,                 //    mass inflow booster
                 SETPOINT,             //    setpoint booster
                 FLOWPACED};           //    flow paced booster

 enum UnitSystemType                   // Unit system:
                 {US,                  //   US
                  SI};                 //   SI (metric)

 enum FlowUnitsType                    // Flow units:
                 {CFS,                 //   cubic feet per second
                  GPM,                 //   gallons per minute
                  MGD,                 //   million gallons per day
                  IMGD,                //   imperial million gal. per day
                  AFD,                 //   acre-feet per day
                  LPS,                 //   liters per second
                  LPM,                 //   liters per minute
                  MLD,                 //   megaliters per day
                  CMH,                 //   cubic meters per hour
                  CMD};                //   cubic meters per day

 enum MixType                          // Tank mixing regimes
                 {MIX1,                //   1-compartment model
                  MIX2,                //   2-compartment model
                  FIFO,                //   First in, first out model
                  LIFO};               //   Last in, first out model

 enum SpeciesType                      // Types of water quality species
                {BULK,                 //   bulk flow species
                 WALL};                //   pipe wall attached species

 enum ExpressionType                   // Types of math expressions
                {NO_EXPR,              //   no expression
                 RATE,                 //   reaction rate
                 FORMULA,              //   simple formula
                 EQUIL};               //   equilibrium expression

 enum SolverType                       // ODE solver options
                 {EUL,                 //   Euler
                  RK5,                 //   5th order Runge-Kutta
                  ROS2};               //   2nd order Rosenbrock

 enum CouplingType                     // Degree of coupling for solving DAE's
                 {NO_COUPLING,         //   no coupling between alg. & diff. eqns.
                  FULL_COUPLING};      //   full coupling between alg. &diff. eqns.

 enum MassUnitsType                    // Concentration mass units
                 {MG,                  //   milligram
                  UG,                  //   microgram
                  MOLE,                //   mole
                  MMOLE};              //   millimole

 enum AreaUnitsType                    // Pipe surface area units
                 {FT2,                 //   square feet
                  M2,                  //   square meters
                  CM2};                //   square centimeters

 enum RateUnitsType                    // Reaction rate time units
                 {SECONDS,             //   seconds
                  MINUTES,             //   minutes
                  HOURS,               //   hours
                  DAYS};               //   days

 enum UnitsType                        // Measurement unit types
                 {LENGTH_UNITS,        //   length
                  DIAM_UNITS,          //   pipe diameter
                  AREA_UNITS,          //   surface area
                  VOL_UNITS,           //   volume
                  FLOW_UNITS,          //   flow
                  CONC_UNITS,          //   concentration volume
                  RATE_UNITS,          //   reaction rate time units
                  MAX_UNIT_TYPES};

 enum HydVarType                       // Hydraulic variables
                 {DIAMETER = 1,        //   link diameter
                  FLOW,                //   link flow rate
                  VELOCITY,            //   link flow velocity
                  REYNOLDS,            //   Reynolds number
                  SHEAR,               //   link shear velocity
                  FRICTION,            //   friction factor
                  AREAVOL,             //   area/volume
                  ROUGHNESS,           //   roughness                          /*Feng Shang 01/29/2008*/
                  MAX_HYD_VARS};

 enum TstatType                        // Time series statistics
                 {SERIES,              //   full time series
                  AVGERAGE,            //   time-averages
                  MINIMUM,             //   minimum values
                  MAXIMUM,             //   maximum values
                  RANGE};              //   max - min values

 enum OptionType                       // Analysis options
                 {AREA_UNITS_OPTION,
                  RATE_UNITS_OPTION,
                  SOLVER_OPTION,
                  COUPLING_OPTION,
                  TIMESTEP_OPTION,
                  RTOL_OPTION,
                  ATOL_OPTION,
                  COMPILER_OPTION};                                            //1.1.00

 enum CompilerType                     // C compiler type                      //1.1.00
                 {NO_COMPILER,
                  VC,                  // MS Visual C compiler
                  GC};                 // Gnu C compiler

 enum FileModeType                     // File modes
                 {SCRATCH_FILE,
                  SAVED_FILE,
                  USED_FILE};

 enum SectionType                      // Input data file sections
                 {s_TITLE,
                  s_SPECIES,
                  s_COEFF,
                  s_TERM,
                  s_PIPE,
                  s_TANK,
                  s_SOURCE,
                  s_QUALITY,
                  s_PARAMETER,
                  s_PATTERN,
                  s_OPTION,
                  s_REPORT};

 enum ErrorCodeType                    // Error codes (501-524)
          {ERR_FIRST = 500,
           ERR_MEMORY,                 // 501
           ERR_NO_EPANET_FILE,         // 502
           ERR_OPEN_MSX_FILE,          // 503
           ERR_OPEN_HYD_FILE,          // 504
           ERR_READ_HYD_FILE,          // 505
           ERR_MSX_INPUT,              // 506
           ERR_NUM_PIPE_EXPR,          // 507
           ERR_NUM_TANK_EXPR,          // 508
           ERR_INTEGRATOR_OPEN,        // 509
           ERR_NEWTON_OPEN,            // 510
           ERR_OPEN_OUT_FILE,          // 511
           ERR_IO_OUT_FILE,            // 512
           ERR_INTEGRATOR,             // 513
           ERR_NEWTON,                 // 514
           ERR_INVALID_OBJECT_TYPE,    // 515
           ERR_INVALID_OBJECT_INDEX,   // 516
           ERR_UNDEFINED_OBJECT_ID,    // 517
           ERR_INVALID_OBJECT_PARAMS,  // 518
           ERR_MSX_NOT_OPENED,         // 519
           ERR_MSX_OPENED,             // 520
           ERR_OPEN_RPT_FILE,          // 521                                  //(LR-11/20/07, to fix bug 08)
           ERR_COMPILE_FAILED,         // 522                                  //1.1.00
           ERR_COMPILED_LOAD,          // 523                                  //1.1.00
		   ERR_ILLEGAL_MATH,           // 524                                  //1.1.00
           ERR_MAX};


//-----------------------------------------------------------------------------
// Data Structures
//-----------------------------------------------------------------------------
struct  NumList                        // List of numerical values
{
   double  value;
   struct  NumList *next;
};
typedef struct NumList SnumList;


typedef struct                         // TIME PATTERN OBJECT
{
   char     *id;                       // pattern ID
   long     length;                    // number of pattern factors
   long     interval;                  // current time interval
   SnumList *first;                    // first mutiplier
   SnumList *current;                  // current multiplier
}  Spattern;


struct Ssource                         // WATER QUALITY SOURCE OBJECT
{
    char      type;                    // sourceType
    int       species;                 // species index
    double    c0;                      // base concentration
    int       pat;                     // time pattern index
    double    massRate;                // actual mass flow rate
    struct    Ssource *next;	       // next bulk species source
};
typedef struct Ssource *Psource;


typedef struct                         // NODE OBJECT
{
   Psource sources;                    // ptr. to WQ source list
   double  *c;                         // current species concentrations
   double  *c0;                        // initial species concentrations
   int     tank;                       // tank index
   char    rpt;                        // reporting flag
}  Snode;


typedef struct                         // LINK OBJECT
{
   int    n1;                          // start node index
   int    n2;                          // end node index
   double diam;                        // diameter
   double len;                         // length
   char   rpt;                         // reporting flag
   double *c0;                         // initial species concentrations
   double *param;                      // kinetic parameter values
   double roughness;		       // roughness  /*Feng Shang, Bug ID 8,  01/29/2008*/
}  Slink;


typedef struct                         // TANK OBJECT
{
   int    node;                        // node index of tank
   double hstep;                       // integration time step
   double a;                           // tank area
   double v0;                          // initial volume
   double v;                           // tank volume
   int    mixModel;                    // type of mixing model
   double vMix;                        // mixing compartment size
   double *param;                      // kinetic parameter values
   double *c;                          // current species concentrations
}  Stank;


struct Sseg                            // PIPE SEGMENT OBJECT
{
    double    hstep;                   // integration time step
    double    v;                       // segment volume
    double    *c;                      // species concentrations
    struct    Sseg *prev;              // ptr. to previous segment
    struct    Sseg *next;              // ptr. to next segment
};
typedef struct Sseg *Pseg;


#define MAXUNITS  16
typedef struct                         // CHEMICAL SPECIES OBJECT
{
    char      *id;                     // name
    char      units[MAXUNITS];         // mass units code
    double    aTol;                    // absolute tolerance
    double    rTol;                    // relative tolerance
    int       type;                    // BULK or WALL
    int       pipeExprType;            // type of pipe chemistry
    int       tankExprType;            // type of tank chemistry
    int       precision;               // reporting precision
    char      rpt;                     // reporting flag
    MathExpr  *pipeExpr;               // pipe chemistry expression
    MathExpr  *tankExpr;               // tank chemistry expression
}   Sspecies;


typedef struct                         // INTERMEDIATE TERM OBJECT
{
    char      *id;                     // name
    MathExpr  *expr;                   // math expression for term
}   Sterm;


typedef struct                         // REACTION RATE PARAMETER OBJECT
{
    char       *id;                    // name
    double     value;                  // value
}   Sparam;


typedef struct                         // MATH EXPRESSION CONSTANT OBJECT
{
    char       *id;                    // name
    double     value;                  // value
}   Sconst;


#define MAXFNAME 259                   // Max. # characters in file name
typedef struct                         // FILE OBJECT
{
   char          name[MAXFNAME];       // file name
   char          mode;                 // see FileModeType enumeration below
   FILE*         file;                 // FILE structure pointer
}  TFile;

typedef struct                         // MSX PROJECT VARIABLES
{
   TFile  HydFile,                     // EPANET hydraulics file
          MsxFile,                     // MSX input file
          OutFile,                     // MSX binary output file
          TmpOutFile,                  // Scratch MSX binary output file
          RptFile;                     // MSX report file

   char   Title[MAXLINE+1],            // Project title
          Msg[MAXLINE+1];              // Message string

   int    Nobjects[MAX_OBJECTS],       // Numbers of each type of object
          Unitsflag,                   // Unit system flag
          Flowflag,                    // Flow units flag
          Saveflag,                    // Save results flag
          Rptflag,                     // Report results flag
          Coupling,                    // Degree of coupling for solving DAE's
          Compiler,                    // chemistry function compiler code     //1.1.00 
          AreaUnits,                   // Surface area units
          RateUnits,                   // Reaction rate time units
          Solver,                      // Choice of ODE solver
          PageSize,                    // Lines per page in report
          Nperiods,                    // Number of reporting periods
          ErrCode,                     // Error code
          ProjectOpened,               // Project opened flag
          QualityOpened;               // Water quality system opened flag

   long   HydOffset,                   // Hydraulics file byte offset
          Qstep,                       // Quality time step (sec)
          Pstep,                       // Time pattern time step (sec)
          Pstart,                      // Starting pattern time (sec)
          Rstep,                       // Reporting time step (sec)
          Rstart,                      // Time when reporting starts
          Rtime,                       // Next reporting time (sec)
          Htime,                       // Current hydraulic time (sec)
          Qtime,                       // Current quality time (sec)
          Statflag,                    // Reporting statistic flag
          Dur;                         // Duration of simulation (sec)

   REAL4  *D,                          // Node demands
          *H,                          // Node heads
          *Q;                          // Link flows

   double Ucf[MAX_UNIT_TYPES],         // Unit conversion factors
          DefRtol,                     // Default relative error tolerance
          DefAtol,                     // Default absolute error tolerance
          *K,                          // Vector of expression constants       //1.1.00
          *C0,                         // Species initial quality vector
          *C1;                         // Species concentration vector

   Pseg   *FirstSeg,                   // First WQ segment in each pipe/tank
          *LastSeg;                    // Last WQ segment in each pipe/tank

   Sspecies *Species;                  // WQ species data
   Sparam   *Param;                    // Expression parameters
   Sconst   *Const;                    // Expression constants
   Sterm    *Term;                     // Intermediate terms
   Snode    *Node;                     // Node data
   Slink    *Link;                     // Link data
   Stank    *Tank;                     // Tank data
   Spattern *Pattern;                  // Pattern data
} MSXproject;
