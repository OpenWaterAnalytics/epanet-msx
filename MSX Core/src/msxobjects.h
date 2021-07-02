
#ifndef MAGICNUMBER
#include "msxtypes.h"
#endif

int addObject(int type, char *id, int n);
int findObject(int type, char *id);
char * findID(int type, char *id);
int createHashTables();
void deleteHashTables();
int getVariableCode(MSXproject *MSX, char *id);
