
#include <stdio.h>
#include <string.h>

#include "hash.h"
#include "msxobjects.h"
#include "msxdict.h"

//  Local variables
//-----------------
static alloc_handle_t  *HashPool;           // Memory pool for hash tables
static HTtable  *Htable[MAX_OBJECTS];       // Hash tables for object ID names

int addObject(int type, char *id, int n)
/**
**  Purpose:
**    adds an object ID to the project's hash tables.
**
**  Input:
**    type = object type
**    id   = object ID string
**    n    = object index.
**
**  Returns:
**    0 if object already added, 1 if not, -1 if hashing fails.
*/
{
    int  result;
    int  len;
    char *newID;

// --- do nothing if object already exists in a hash table

    if ( findObject(type, id) > 0 ) return 0;

// --- use memory from the hash tables' common memory pool to store
//     a copy of the object's ID string

    len = (int) strlen(id) + 1;
    newID = (char *) Alloc(len*sizeof(char));
    strcpy(newID, id);

// --- insert object's ID into the hash table for that type of object

    result = (int) HTinsert(Htable[type], newID, n);
    if ( result == 0 ) result = -1;
    return result;
}

int findObject(int type, char *id)
/**
**  Purpose:
**    uses hash table to find index of an object with a given ID.
**
**  Input:
**    type = object type
**    id   = object ID.
**
**  Returns:
**    index of object with given ID, or -1 if ID not found.
*/
{
    return HTfind(Htable[type], id);
}

char * findID(int type, char *id)
/**
**  Purpose:
**    uses hash table to find address of given string entry.
**
**  Input:
**    type = object type
**    id   = ID name being sought.
**
**  Returns:
**    pointer to location where object's ID string is stored.
*/
{
    return HTfindKey(Htable[type], id);
}

int createHashTables()
/**
**  Purpose:
**    allocates memory for object ID hash tables.
**
**  Input:
**    none.
**
**  Returns:
**    an error code (0 if no error).
*/
{   int j;

// --- create a hash table for each type of object

    for (j = 0; j < MAX_OBJECTS ; j++)
    {
         Htable[j] = HTcreate();
         if ( Htable[j] == NULL ) return ERR_MEMORY;
    }

// --- initialize the memory pool used to store object ID's

    HashPool = AllocInit();
    if ( HashPool == NULL ) return ERR_MEMORY;
    return 0;
}


void deleteHashTables()
/**
**  Purpose:
**    frees memory allocated for object ID hash tables.
**
**  Input:
**    none.
*/
{
    int j;

// --- free the hash tables

    for (j = 0; j < MAX_OBJECTS; j++)
    {
        if ( Htable[j] != NULL ) HTfree(Htable[j]);
    }

// --- free the object ID memory pool

    if ( HashPool )
    {
        AllocSetPool(HashPool);
        AllocFreePool();
    }
}

//=============================================================================

int getVariableCode(MSXproject *MSX, char *id)
/**
**  Purpose:
**    finds the index assigned to a species, intermediate term,
**    parameter, or constant that appears in a math expression.
**
**  Input:
**    id = ID name being sought
**
**  Returns:
**    index of the symbolic variable or term named id.
**
**  Note:
**    Variables are assigned consecutive code numbers starting from 1 
**    and proceeding through each Species, Term, Parameter and Constant.
*/
{
    int j = findObject(SPECIES, id);
    if ( j >= 1 ) return j;
    j = findObject(TERM, id);
    if ( j >= 1 ) return MSX->Nobjects[SPECIES] + j;
    j = findObject(PARAMETER, id);
    if ( j >= 1 ) return MSX->Nobjects[SPECIES] + MSX->Nobjects[TERM] + j;
    j = findObject(CONSTANT, id);
    if ( j >= 1 ) return MSX->Nobjects[SPECIES] + MSX->Nobjects[TERM] + 
                         MSX->Nobjects[PARAMETER] + j;
    j = MSXutils_findmatch(id, HydVarWords);
    if ( j >= 1 ) return MSX->Nobjects[SPECIES] + MSX->Nobjects[TERM] + 
                         MSX->Nobjects[PARAMETER] + MSX->Nobjects[CONSTANT] + j;
    return -1;
}

//=============================================================================

int  buildadjlists(MSXproject *MSX)   //from epanet2.2 for node sorting in WQ routing
/**
**--------------------------------------------------------------
** Input:   none
** Output:  returns error code
** Purpose: builds linked list of links adjacent to each node
**--------------------------------------------------------------
*/
{
    int       i, j, k;
    int       errcode = 0;
    Padjlist  alink;

    // Create an array of adjacency lists
    freeadjlists(MSX);
    MSX->Adjlist = (Padjlist*)calloc(MSX->Nobjects[NODE]+1, sizeof(Padjlist));
    if (MSX->Adjlist == NULL) return 101;
    for (i = 0; i <= MSX->Nobjects[NODE]; i++) 
        MSX->Adjlist[i] = NULL;

    // For each link, update adjacency lists of its end nodes
    for (k = 1; k <= MSX->Nobjects[LINK]; k++)
    {
        i = MSX->Link[k].n1;
        j = MSX->Link[k].n2;

        // Include link in start node i's list
        alink = (struct Sadjlist*) malloc(sizeof(struct Sadjlist));
        if (alink == NULL)
        {
            errcode = 101;
            break;
        }
        alink->node = j;
        alink->link = k;
        alink->next = MSX->Adjlist[i];
        MSX->Adjlist[i] = alink;

        // Include link in end node j's list
        alink = (struct Sadjlist*) malloc(sizeof(struct Sadjlist));
        if (alink == NULL)
        {
            errcode = 101;
            break;
        }
        alink->node = i;
        alink->link = k;
        alink->next = MSX->Adjlist[j];
        MSX->Adjlist[j] = alink;
    }
    if (errcode) freeadjlists(MSX);
    return errcode;
}


void  freeadjlists(MSXproject *MSX)            //from epanet2.2 for node sorting in WQ routing
/**
**--------------------------------------------------------------
** Input:   none
** Output:  none
** Purpose: frees memory used for nodal adjacency lists
**--------------------------------------------------------------
*/
{
    int   i;
    Padjlist alink;

    if (MSX->Adjlist == NULL) return;
    for (i = 0; i <= MSX->Nobjects[NODE]; i++)
    {
        for (alink = MSX->Adjlist[i]; alink != NULL; alink = MSX->Adjlist[i])
        {
            MSX->Adjlist[i] = alink->next;
            free(alink);
        }
    }
    free(MSX->Adjlist);
}