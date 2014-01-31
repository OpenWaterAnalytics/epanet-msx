/* HASH.H
**
** Header file for Hash Table module HASH.C
**
*/

#define MSXHASHTABLEMAXSIZE 128000
#define NOTFOUND  0

typedef struct MsxHashEntryStruct
{
	char 	*key;
	int 	data;
	struct MsxHashEntryStruct *next;
} MsxHashEntry;

typedef MsxHashEntry *MsxHashTable;

MsxHashTable *MsxHashTableCreate(void);
int     MsxHashTableInsert(MsxHashTable *, char *, int);
int 	MsxHashTableFind(MsxHashTable *, char *);
char    *MsxHashTableFindKey(MsxHashTable *, char *);
void	MsxHashTableFree(MsxHashTable *);
	
