/******************************************************************************
**  MODULE:        MSXBJECTS.H
**  PROJECT:       EPANET-MSX
**  DESCRIPTION:   C/C++ header file for the MSXOBJECTS functions.         
**  COPYRIGHT:     Copyright (C) 2007 Feng Shang, Lewis Rossman, and James Uber.
**                 All Rights Reserved. See license information in LICENSE.TXT.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**                 K. Arrowood, Xylem intern
**  VERSION:       1.1.00
**  LAST UPDATE:   Refer to git history
******************************************************************************/

// Opaque Project pointer
typedef struct Project *MSXproject;

int addObject(int type, char *id, int n);
int findObject(int type, char *id);
char * findID(int type, char *id);
int createHashTables();
void deleteHashTables();
int setDefaults(MSXproject MSX);
int getVariableCode(MSXproject MSX, char *id);
int  buildadjlists(MSXproject MSX);
void  freeadjlists(MSXproject MSX);
int convertUnits(MSXproject MSX);
void deleteObjects(MSXproject MSX);
void freeIDs(MSXproject MSX);
int checkCyclicTerms(MSXproject MSX, double **TermArray);
int traceTermPath(int i, int istar, int n, double **TermArray);
int finishInit(MSXproject MSX);

