/************************************************************************
**  MODULE:        MSXDICT.H
**  PROJECT:       EPANET-MSX
**  DESCRIPTION:   Dictionary of key words used by the
**                 EPANET Multi-Species Extension toolkit.
**  COPYRIGHT:     Copyright (C) 2007 Feng Shang, Lewis Rossman, and James Uber.
**                 All Rights Reserved. See license information in LICENSE.TXT.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**  VERSION:       1.00                                               
**  LAST UPDATE:   3/1/07
***********************************************************************/

// NOTE: the entries in MsxsectWords must match the entries in the enumeration
//       variable SectionType defined in msxtypes.h.
static char *MsxSectWords[] = {"[TITLE", "[SPECIE",  "[COEFF",  "[TERM",
                               "[PIPE",  "[TANK",    "[SOURCE", "[QUALITY",
                               "[PARAM", "[PATTERN", "[OPTION", 
                               "[REPORT", NULL};
static char *ReportWords[]  = {"NODE", "LINK", "SPECIE", "FILE", "PAGESIZE", NULL};
static char *OptionTypeWords[] = {"AREA_UNITS", "RATE_UNITS", "SOLVER", "COUPLING",
                                  "TIMESTEP", "RTOL", "ATOL", NULL}; 
static char *SourceTypeWords[] = {"CONC", "MASS", "FLOW", "SETPOINT", NULL};
static char *MixingTypeWords[] = {"MIXED", "2COMP", "FIFO", "LIFO", NULL};
static char *MassUnitsWords[]  = {"MG", "UG", "MOLE", "MMOL", NULL};
static char *AreaUnitsWords[]  = {"FT2", "M2", "CM2", NULL};
static char *TimeUnitsWords[]  = {"SEC", "MIN", "HR", "DAY", NULL};
static char *SolverTypeWords[] = {"EUL", "RK5", "ROS2", NULL};
static char *CouplingWords[]   = {"NONE", "FULL", NULL};
static char *ExprTypeWords[]   = {"", "RATE", "FORMULA", "EQUIL", NULL};
static char *HydVarWords[]     = {"", "D", "Q", "U", "Re",
                                  "Us", "Ff", "Av", NULL};
static char YES[]  = "YES";
static char NO[]   = "NO";
static char ALL[]  = "ALL";
static char NONE[] = "NONE";
