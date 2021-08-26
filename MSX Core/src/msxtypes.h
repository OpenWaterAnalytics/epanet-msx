/************************************************************************
**  MODULE:        MSXTYPES.H
**  PROJECT:       EPANET-MSX
**  DESCRIPTION:   Global constants and data types used by the EPANET
**                 Multi-Species Extension toolkit.
**  COPYRIGHT:     Copyright (C) 2007 Feng Shang, Lewis Rossman, and James Uber.
**                 All Rights Reserved. See license information in LICENSE.TXT.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**                 K. Arrowood, Xylem intern
**  VERSION:       1.1.00
**  LAST UPDATE:   Refer to git history
**  Bug Fix:       Bug ID 08, Feng Shang, 01/07/08 
**                 Bug ID 09 (add roughness as hydraulic variable) Feng Shang 01/29/2008
***********************************************************************/

#include "msxenums.h"
#include "mempool.h"
#include "mathexpr.h"
#include <stdio.h>


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
#define   LPSperCFS    28.3168466 //28.317
#define   LPMperCFS    1699.0
#define   CMHperCFS    101.94
#define   CMDperCFS    2446.6
#define   MLDperCFS    2.4466
#define   M3perFT3     0.028317
#define   LperFT3      28.317//28.3168466 //28.317
#define   MperFT       0.3048
#define   PSIperFT     0.4333
#define   KPAperPSI    6.895
#define   KWperHP      0.7457
#define   SECperDAY    86400


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
   char    *id;                        // id
}  Snode;


typedef struct                         // LINK OBJECT
{
   int    n1;                          // start node index
   int    n2;                          // end node index
   double diam;                        // diameter
   double len;                         // length
   char   rpt;                         // reporting flag
   double *c0;                         // initial species concentrations
   double *reacted;
   double *param;                      // kinetic parameter values
   double roughness;		           // roughness  /*Feng Shang, Bug ID 8,  01/29/2008*/
   char   *id;                         // id
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
   char   *id;                         // id
   double *reacted;
}  Stank;


struct Sseg                            // PIPE SEGMENT OBJECT
{
    double    hstep;                   // integration time step
    double    v;                       // segment volume
    double    *c;                      // species concentrations
    double    * lastc;                 // species concentrations of previous step 
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
    char      *equation;               // String of the equation
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



struct Sadjlist           // Node Adjacency List Item
{
    int    node;           // index of connecting node
    int    link;           // index of connecting link
    struct Sadjlist* next; // next item in list
};

typedef struct Sadjlist* Padjlist; // Pointer to adjacency list

typedef enum {
    NEGATIVE = -1,  // flow in reverse of pre-assigned direction
    ZERO_FLOW = 0,   // zero flow
    POSITIVE = 1    // flow in pre-assigned direction
} FlowDirection;

typedef struct                 // Mass Balance Components
{
    double   * initial;         // initial mass in system
    double   * inflow;          // mass inflow to system
    double   * outflow;         // mass outflow from system
    double   * reacted;         // mass reacted in system
    double   * final;           // final mass in system
    double   * ratio;           // ratio of mass added to mass lost
} SmassBalance;

typedef struct Project                 // MSX PROJECT VARIABLES
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
          QualityOpened,               // Water quality system opened flag
          Sizes[MAX_OBJECTS];          // Capacities for the dynamic arrays
   

   long   HydOffset,                   // Hydraulics file byte offset
          Qstep,                       // Quality time step (sec)
          Pstep,                       // Time pattern time step (sec)
          Pstart,                      // Starting pattern time (sec)
          Rstep,                       // Reporting time step (sec)
          Rstart,                      // Time when reporting starts
          Rtime,                       // Next reporting time (sec)
          Htime,                       // Current hydraulic time (sec)
          Hstep,                       // Hydrualic step (sec)
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
   
   char      HasWallSpecies;  // wall species indicator
   char      OutOfMemory;     // out of memory indicator
   Padjlist* Adjlist;                   // Node adjacency lists
   Pseg* NewSeg;         // new segment added to each pipe
   Pseg  FreeSeg;        // pointer to unused segment
   FlowDirection *FlowDir;        // flow direction for each pipe
   SmassBalance MassBalance;
   alloc_handle_t* QualPool;       // memory pool

   double* MassIn;        // mass inflow of each species to each node
   double* SourceIn;      // external mass inflow of each species from WQ source;
   int* SortedNodes;

} *MSXproject;
