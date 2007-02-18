/************************************************************************
**  TITLE:         TYPESMSX.H
**  DESCRIPTION:   Global constants and data types used by the EPANET
**                 Multi-Species Extension toolkit.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**  VERSION:       1.00
**  LAST UPDATE:   11/9/06
***********************************************************************/

#include "mathexpr.h"

//  Macros for memory allocation
#define  MEMCHECK(x)  (((x) == NULL) ? ERR_MEMORY : 0 )
#define  FREE(x) { if (x) { free(x); x = NULL; } }

//  Conversion macros to be used in place of functions
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
//  Macro to evaluate function x with error checking
//  (Fatal errors are numbered higher than 100)
#define ERRCODE(x) (errcode = ((errcode>100) ? (errcode) : (x)))

// Global Data Structures
//-----------------------

typedef  double        REAL;
typedef  long          INT4;
#define  MAXID         15              // Max. # characters in ID name

typedef struct                         // Holds component ID labels
{
   char id[MAXID+1];
}  IDstring;


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
}  MSXSpattern;


struct Ssource                         // WATER QUALITY SOURCE OBJECT
{
    char      type;                    // sourceType
    int       specie;                  // specie index
    double    c0;                      // base concentration
    int       pat;                     // time pattern index
    double    massRate;                // actual mass flow rate
    struct    Ssource *next;	       // next bulk specie source
};
typedef struct Ssource *Psource;


typedef struct                         // NODE OBJECT
{
   Psource sources;                    // ptr. to WQ source list
   double  *c;                         // current species concentrations
   double  *c0;                        // initial species concentrations
   int     tank;                       // tank index
   char    rpt;                        // reporting flag
}  MSXSnode;


typedef struct                         // LINK OBJECT
{
   int    n1;                          // start node index
   int    n2;                          // end node index
   double diam;                        // diameter
   double len;                         // length
   char   rpt;                         // reporting flag
   double *c0;                         // initial species concentrations
   double *param;                      // kinetic parameter values
}  MSXSlink;


typedef struct                         // TANK OBJECT
{
   int    node;                        // node index of tank
   double hstep;                       // integration time step
   double a;                           // tank area
   double v0;                          // initial volume
   double v;                           // tank volume
   char   mixModel;                    // type of mixing model
   double v1max;                       // mixing compartment size
   double *param;                      // kinetic parameter values
   double *c;                          // current species concentrations
}  MSXStank;


typedef struct                         // CHEMICAL SPECIE OBJECT
{
	char      *id;                     // name
    int       type;                    // BULK or WALL
	int       units;                   // mass units code
    double    aTol;                    // absolute tolerance
    double    rTol;                    // relative tolerance
    int       pipeExprType;            // type of pipe chemistry
    int       tankExprType;            // type of tank chemistry
    int       precision;               // reporting precision
    char      rpt;                     // reporting flag
    MathExpr  *pipeExpr;               // pipe chemistry expression
    MathExpr  *tankExpr;               // tank chemistry expression
}   MSXSspecie;


typedef struct                         // INTERMEDIATE TERM OBJECT
{
    char      *id;                     // name
    MathExpr  *expr;                   // math expression for term
}   MSXSterm;


typedef struct                         // REACTION RATE PARAMETER OBJECT
{
    char       *id;                    // name
    double     value;                  // value
}   MSXSparam;


typedef struct                         // MATH EXPRESSION CONSTANT OBJECT
{
    char       *id;                    // name
    double     value;                  // value
}   MSXSconst;


struct Sseg                            // WATER QUALITY SEGMENT OBJECT
{
    double    hstep;                   // integration time step
    double    v;                       // segment volume
    double    *c;                      // species concentrations
    struct    Sseg *prev;              // ptr. to previous segment
    struct    Sseg *next;              // ptr. to next segment
};
typedef struct Sseg *Pseg;

typedef struct                         // FILE OBJECT
{
   char          name[FILENAME_MAX];   // file name
   char          mode;                 // see FileModeType enumeration below
   FILE*         file;                 // FILE structure pointer
}  MSXTFile;


//  Global Enumeration Variables
//------------------------------

 enum ObjectTypes                      // Object types
                {NODE,
                 TANK,
                 LINK,
                 SPECIE,
                 TERM,
                 PARAMETER,
                 CONSTANT,
                 TIME_PATTERN,
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

 enum SpecieType                       // Types of water quality species
                {BULK,                 //   bulk flow specie
                 WALL};                //   pipe wall attached specie

 enum ExpressionType                   // Types of math expressions
                {NO_EXPR,              //   no expression
                 RATE,                 //   reaction rate
                 FORMULA,              //   simple formula
                 EQUIL};               //   equilibrium expression

 enum SolverType                       // ODE solver options
                 {EUL,                 //   Euler
                  RK5,                 //   5th order Runge-Kutta
                  ROS2};               //   2nd order Rosenbrock

 enum MassUnitsType                    // Concentration mass units
                 {MG,                  //   milligram
                  UG,                  //   microgram
                  MMOL,                //   millimole
                  COUNT};              //   count

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
                  MAX_HYD_VARS};

 enum TstatType                        // Time series statistics
                 {SERIES,              //   full time series
                  AVG,                 //   time-averages
                  MIN,                 //   minimum values
                  MAX,                 //   maximum values
                  RANGE};              //   max - min values

 enum OptionType                       // Analysis options
                 {AREA_UNITS_OPTION,
                  RATE_UNITS_OPTION,
                  SOLVER_OPTION,
                  TIMESTEP_OPTION,
                  RTOL_OPTION,
                  ATOL_OPTION};

 enum FileModeType                     // File modes
                 {SCRATCH_FILE,
                  SAVED_FILE,
                  USED_FILE};

 enum SectionType                      // Input data file sections
                 {s_TITLE,
                  s_SPECIE,
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

 enum ErrorCodeType                    // Error codes (501-520)
          {ERR_FIRST = 500,
           ERR_MEMORY,                 // 501
           ERR_NO_EPANET_FILE,         // 502
           ERR_OPEN_MSX_FILE,          // 503
           ERR_OPEN_HYD_FILE,          // 504
           ERR_READ_HYD_FILE,          // 505
           ERR_MSX_INPUT,              // 506
           ERR_NUM_PIPE_EXPR,          // 507
           ERR_NUM_TANK_EXPR,          // 508
           ERR_ODE_OPEN,               // 509
           ERR_ALG_OPEN,               // 510
           ERR_OPEN_OUT_FILE,          // 511
           ERR_IO_OUT_FILE,            // 512
           ERR_INTEGRATOR,             // 513
           ERR_NEWTON,                 // 514
           ERR_PARAM_CODE,             // 515
           ERR_NO_PROJECT,             // 516
           ERR_PROJECT_OPENED,         // 517
           ERR_TIME_PATTERN,           // 518
           ERR_NUM_VALUE,              // 519
           ERR_INDEX_VALUE,            // 520
           ERR_MAX};
