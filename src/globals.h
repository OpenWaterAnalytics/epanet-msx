/************************************************************************
**  TITLE:         GLOBALS.H
**  DESCRIPTION:   Global constants and variables used by the
**                 EPANET Multi-Species Extension toolkit.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**  VERSION:       1.00                                               
**  LAST UPDATE:   10/5/06
***********************************************************************/

//  Global Constants
//------------------
#define   MAGICNUMBER  516114521
#define   VERSION      100000
#define   MAXMSG       1024            // Max. # characters in message text
#define   MAXLINE      1024            // Max. # characters in input line
#define   MAXFNAME     259             // Max. # characters in file name
#define   TRUE         1
#define   FALSE        0
#define   BIG          1.E10
#define   TINY         1.E-6
#define   MISSING      -1.E10
#define   PI           3.141592654
#define   VISCOS       1.1E-5          // Kinematic viscosity of water
                                       // @ 20 deg C (sq ft/sec)

//  Various conversion factors
//----------------------------
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
 
#define   SEPSTR    " \t\n\r"          // Token separator characters

EXTERN_MSX TFile  HydFile,             // EPANET hydraulics file
                  InpFile,             // MSX input file
                  OutFile,             // MSX binary output file
                  RptFile;             // MSX report file

EXTERN_MSX char   Title[MAXLINE+1],    // Project title
                  Msg[MAXLINE+1];      // Message string

EXTERN_MSX int    Nobjects[MAX_OBJECTS],  // Numbers of objects
                  Unitsflag,           // Unit system flag
                  Flowflag,            // Flow units flag
                  Saveflag,            // Save results flag
                  Rptflag,             // Report results flag
                  AreaUnits,           // Surface area units
                  RateUnits,           // Reaction rate time units
                  Solver,              // Choice of ODE solver
                  PageSize,            // Lines per page in report
                  Nperiods,            // Number of reporting periods
                  ErrCode,             // Error code
                  ProjectOpened,       // Project opened flag
                  QualityOpened;       // Water quality system opened flag

EXTERN_MSX long   HydOffset,           // Hydraulics file byte offset
                  Qstep,               // Quality time step (sec)
                  Pstep,               // Time pattern time step (sec)
                  Pstart,              // Starting pattern time (sec)
                  Rstep,               // Reporting time step (sec)
                  Rstart,              // Time when reporting starts
                  Rtime,               // Next reporting time (sec)
                  Htime,               // Current hydraulic time (sec)
                  Qtime,               // Current quality time (sec)
                  Dur;                 // Duration of simulation (sec)

EXTERN_MSX float  *D,                  // Node demands
                  *H,                  // Node heads
                  *Q;                  // Link flows

EXTERN_MSX double Ucf[MAX_UNIT_TYPES], // Unit conversion factors
                  DefRtol,             // Default relative error tolerance
                  DefAtol;             // Default absolute error tolerance

EXTERN_MSX Sspecie  *Specie;           // WQ species data
EXTERN_MSX Sparam   *Param;            // Expression parameters
EXTERN_MSX Sconst   *Const;            // Expression constants
EXTERN_MSX Sterm    *Term;             // Intermediate terms
EXTERN_MSX Snode    *Node;             // Node data
EXTERN_MSX Slink    *Link;             // Link data
EXTERN_MSX Stank    *Tank;             // Tank data
EXTERN_MSX Spattern *Pattern;          // Pattern data
