
#ifndef MAGICNUMBER
#include "msxtypes.h"
#endif

int addObject(int type, char *id, int n);
int findObject(int type, char *id);
char * findID(int type, char *id);
int createHashTables();
void deleteHashTables();
int setDefaults(MSXproject *MSX);
int getVariableCode(MSXproject *MSX, char *id);
int  buildadjlists(MSXproject *MSX);
void  freeadjlists(MSXproject *MSX);
int convertUnits(MSXproject *MSX);
void deleteObjects(MSXproject *MSX);
int checkCyclicTerms(MSXproject *MSX, double **TermArray);
int traceTermPath(int i, int istar, int n, double **TermArray);
int finishInit(MSXproject *MSX);

