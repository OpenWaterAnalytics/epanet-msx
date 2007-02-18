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

EXTERN_MSX MSXTFile  MSXHydFile,             // EPANET hydraulics file
                     MSXInpFile,             // MSX input file
                     MSXOutFile,             // MSX binary output file
                     MSXRptFile;             // MSX report file

EXTERN_MSX char   MSXTitle[MAXLINE+1],    // Project title
                  MSXMsg[MAXLINE+1];      // Message string

EXTERN_MSX int    MSXNobjects[MAX_OBJECTS],  // Numbers of objects
                  MSXUnitsflag,           // Unit system flag
                  MSXFlowflag,            // Flow units flag
                  MSXSaveflag,            // Save results flag
                  MSXRptflag,             // Report results flag
                  MSXAreaUnits,           // Surface area units
                  MSXRateUnits,           // Reaction rate time units
                  MSXSolver,              // Choice of ODE solver
                  MSXPageSize,            // Lines per page in report
                  MSXNperiods,            // Number of reporting periods
                  MSXErrCode,             // Error code
                  MSXProjectOpened,       // Project opened flag
                  MSXQualityOpened;       // Water quality system opened flag

EXTERN_MSX long   MSXHydOffset,           // Hydraulics file byte offset
                  MSXQstep,               // Quality time step (sec)
                  MSXPstep,               // Time pattern time step (sec)
                  MSXPstart,              // Starting pattern time (sec)
                  MSXRstep,               // Reporting time step (sec)
                  MSXRstart,              // Time when reporting starts
                  MSXRtime,               // Next reporting time (sec)
                  MSXHtime,               // Current hydraulic time (sec)
                  MSXQtime,               // Current quality time (sec)
                  MSXDur;                 // Duration of simulation (sec)

EXTERN_MSX float  *MSXD,                  // Node demands
                  *MSXH,                  // Node heads
                  *MSXQ;                  // Link flows

EXTERN_MSX double MSXUcf[MAX_UNIT_TYPES], // Unit conversion factors
                  MSXDefRtol,             // Default relative error tolerance
                  MSXDefAtol;             // Default absolute error tolerance

EXTERN_MSX MSXSspecie  *MSXSpecie;           // WQ species data
EXTERN_MSX MSXSparam   *MSXParam;            // Expression parameters
EXTERN_MSX MSXSconst   *MSXConst;            // Expression constants
EXTERN_MSX MSXSterm    *MSXTerm;             // Intermediate terms
EXTERN_MSX MSXSnode    *MSXNode;             // Node data
EXTERN_MSX MSXSlink    *MSXLink;             // Link data
EXTERN_MSX MSXStank    *MSXTank;             // Tank data
EXTERN_MSX MSXSpattern *MSXPattern;          // Pattern data
