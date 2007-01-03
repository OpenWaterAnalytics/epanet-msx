/*
**   EPANET2.H
**
** C/C++ header file for EPANET Programmers Toolkit
**
** Last updated on 10/5/06
*/

#ifndef EPANET2_H
#define EPANET2_H

/* These are codes used by the DLL functions */
#define EN_ELEVATION    0    /* Node parameters */
#define EN_BASEDEMAND   1
#define EN_PATTERN      2
#define EN_EMITTER      3
#define EN_INITQUAL     4
#define EN_SOURCEQUAL   5
#define EN_SOURCEPAT    6
#define EN_SOURCETYPE   7
#define EN_TANKLEVEL    8
#define EN_DEMAND       9
#define EN_HEAD         10
#define EN_PRESSURE     11
#define EN_QUALITY      12
#define EN_SOURCEMASS   13

#define EN_INITVOLUME   14

#define EN_DIAMETER     0    /* Link parameters */
#define EN_LENGTH       1
#define EN_ROUGHNESS    2
#define EN_MINORLOSS    3
#define EN_INITSTATUS   4
#define EN_INITSETTING  5
#define EN_KBULK        6
#define EN_KWALL        7
#define EN_FLOW         8
#define EN_VELOCITY     9
#define EN_HEADLOSS     10
#define EN_STATUS       11
#define EN_SETTING      12
#define EN_ENERGY       13

#define EN_DURATION     0    /* Time parameters */
#define EN_HYDSTEP      1
#define EN_QUALSTEP     2
#define EN_PATTERNSTEP  3
#define EN_PATTERNSTART 4
#define EN_REPORTSTEP   5
#define EN_REPORTSTART  6
#define EN_RULESTEP     7
#define EN_STATISTIC    8
#define EN_PERIODS      9

#define EN_NODECOUNT    0    /* Component counts */
#define EN_TANKCOUNT    1
#define EN_LINKCOUNT    2
#define EN_PATCOUNT     3
#define EN_CURVECOUNT   4
#define EN_CONTROLCOUNT 5

#define EN_JUNCTION     0    /* Node types */
#define EN_RESERVOIR    1
#define EN_TANK         2

#define EN_CVPIPE       0    /* Link types */
#define EN_PIPE         1
#define EN_PUMP         2
#define EN_PRV          3
#define EN_PSV          4
#define EN_PBV          5
#define EN_FCV          6
#define EN_TCV          7
#define EN_GPV          8

#define EN_NONE         0    /* Quality analysis types */
#define EN_CHEM         1
#define EN_AGE          2
#define EN_TRACE        3

#define EN_CONCEN       0    /* Source quality types */
#define EN_MASS         1
#define EN_SETPOINT     2
#define EN_FLOWPACED    3

#define EN_CFS          0    /* Flow units types */
#define EN_GPM          1
#define EN_MGD          2
#define EN_IMGD         3
#define EN_AFD          4
#define EN_LPS          5
#define EN_LPM          6
#define EN_MLD          7
#define EN_CMH          8
#define EN_CMD          9

#define EN_TRIALS       0   /* Misc. options */
#define EN_ACCURACY     1
#define EN_TOLERANCE    2
#define EN_EMITEXPON    3
#define EN_DEMANDMULT   4

#define EN_LOWLEVEL     0   /* Control types */
#define EN_HILEVEL      1
#define EN_TIMER        2
#define EN_TIMEOFDAY    3

#define EN_AVERAGE      1   /* Time statistic types.    */
#define EN_MINIMUM      2 
#define EN_MAXIMUM      3
#define EN_RANGE        4

#define EN_NOSAVE       0   /* Save-results-to-file flag */
#define EN_SAVE         1
#define EN_INITFLOW     10  /* Re-initialize flow flag   */


/* These are the external functions that comprise the DLL */

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern 
#endif
#define CALLTYPE __stdcall

EXTERN int   CALLTYPE ENepanet(char *, char *, char *, void (*) (char *));
EXTERN int   CALLTYPE ENopen(char *, char *, char *);
EXTERN int   CALLTYPE ENsaveinpfile(char *);
EXTERN int   CALLTYPE ENclose(void);

EXTERN int   CALLTYPE ENsolveH(void);
EXTERN int   CALLTYPE ENsaveH(void);
EXTERN int   CALLTYPE ENopenH(void);
EXTERN int   CALLTYPE ENinitH(int);
EXTERN int   CALLTYPE ENrunH(long *);
EXTERN int   CALLTYPE ENnextH(long *);
EXTERN int   CALLTYPE ENcloseH(void);
EXTERN int   CALLTYPE ENsavehydfile(char *);
EXTERN int   CALLTYPE ENusehydfile(char *);

EXTERN int   CALLTYPE ENsolveQ(void);
EXTERN int   CALLTYPE ENopenQ(void);
EXTERN int   CALLTYPE ENinitQ(int);
EXTERN int   CALLTYPE ENrunQ(long *);
EXTERN int   CALLTYPE ENnextQ(long *);
EXTERN int   CALLTYPE ENstepQ(long *);
EXTERN int   CALLTYPE ENcloseQ(void);

EXTERN int   CALLTYPE ENwriteline(char *);
EXTERN int   CALLTYPE ENreport(void);
EXTERN int   CALLTYPE ENresetreport(void);
EXTERN int   CALLTYPE ENsetreport(char *);

EXTERN int   CALLTYPE ENgetcontrol(int, int *, int *, float *,
                      int *, float *);
EXTERN int   CALLTYPE ENgetcount(int, int *);
EXTERN int   CALLTYPE ENgetoption(int, float *);
EXTERN int   CALLTYPE ENgettimeparam(int, long *);
EXTERN int   CALLTYPE ENgetflowunits(int *);
EXTERN int   CALLTYPE ENgetpatternindex(char *, int *);
EXTERN int   CALLTYPE ENgetpatternid(int, char *);
EXTERN int   CALLTYPE ENgetpatternlen(int, int *);
EXTERN int   CALLTYPE ENgetpatternvalue(int, int, float *);
EXTERN int   CALLTYPE ENgetqualtype(int *, int *);
EXTERN int   CALLTYPE ENgeterror(int, char *, int);

EXTERN int   CALLTYPE ENgetnodeindex(char *, int *);
EXTERN int   CALLTYPE ENgetnodeid(int, char *);
EXTERN int   CALLTYPE ENgetnodetype(int, int *);
EXTERN int   CALLTYPE ENgetnodevalue(int, int, float *);

EXTERN int   CALLTYPE ENgetlinkindex(char *, int *);
EXTERN int   CALLTYPE ENgetlinkid(int, char *);
EXTERN int   CALLTYPE ENgetlinktype(int, int *);
EXTERN int   CALLTYPE ENgetlinknodes(int, int *, int *);
EXTERN int   CALLTYPE ENgetlinkvalue(int, int, float *);

EXTERN int   CALLTYPE ENgetversion(int *);

EXTERN int   CALLTYPE ENsetcontrol(int, int, int, float, int, float);
EXTERN int   CALLTYPE ENsetnodevalue(int, int, float);
EXTERN int   CALLTYPE ENsetlinkvalue(int, int, float);
EXTERN int   CALLTYPE ENsetpattern(int, float *, int);
EXTERN int   CALLTYPE ENsetpatternvalue(int, int, float);
EXTERN int   CALLTYPE ENsettimeparam(int, long);
EXTERN int   CALLTYPE ENsetoption(int, float);
EXTERN int   CALLTYPE ENsetstatusreport(int);
EXTERN int   CALLTYPE ENsetqualtype(int, char *, char *, char *);

#undef EXTERN
#undef CALLTYPE

#endif
