//-----------------------------------------------------------------------------
//   hash.c
//
//   Implementation of a simple Hash Table for string storage & retrieval
//
//   Written by L. Rossman
//   Last Updated on 6/19/03
//
//   The hash table data structure (HTable) is defined in "hash.h".
//   Interface Functions:
//      HTcreate() - creates a hash table
//      HTinsert() - inserts a string & its index value into a hash table
//      HTfind()   - retrieves the index value of a string from a table
//      HTfree()   - frees a hash table

//    subsequently modified and improved by sam hatchett via open water analytics 1.2014
//-----------------------------------------------------------------------------

#include <malloc.h>
#include <string.h>
#include "hash.h"

unsigned int _hash(char *str);
unsigned int _hash(char *str)
{
  unsigned int hash = 5381;
  int c;
  
  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }
  unsigned int retHash = hash % MSXHASHTABLEMAXSIZE;
  return retHash;
}

MsxHashTable *MsxHashTableCreate(void)
{
  int i;
  MsxHashTable *ht = (MsxHashTable *) calloc(MSXHASHTABLEMAXSIZE, sizeof(MsxHashTable));
  if (ht != NULL) {
    for (i=0; i<MSXHASHTABLEMAXSIZE; i++) {
      ht[i] = NULL;
    }
  }
  return(ht);
}

int     MsxHashTableInsert(MsxHashTable *ht, char *key, int data)
{
  unsigned int i = _hash(key);
  MsxHashEntry *entry;
  if ( i >= MSXHASHTABLEMAXSIZE ) {
    return(0);
  }
  entry = (MsxHashEntry *) malloc(sizeof(MsxHashEntry));
  if (entry == NULL) {
    return(0);
  }
  entry->key = key;
  entry->data = data;
  entry->next = ht[i];
  ht[i] = entry;
  return(1);
}

int 	MsxHashTableFind(MsxHashTable *ht, char *key)
{
  unsigned int i = _hash(key);
  MsxHashEntry *entry;
  if ( i >= MSXHASHTABLEMAXSIZE ) {
    return(NOTFOUND);
  }
  entry = ht[i];
  while (entry != NULL)
  {
    if ( strcmp(entry->key,key) == 0 ) {
      return(entry->data);
    }
    entry = entry->next;
  }
  return(NOTFOUND);
}

char    *MsxHashTableFindKey(MsxHashTable *ht, char *key)
{
  unsigned int i = _hash(key);
  MsxHashEntry *entry;
  if ( i >= MSXHASHTABLEMAXSIZE ) {
    return(NULL);
  }
  entry = ht[i];
  while (entry != NULL)
  {
    if ( strcmp(entry->key,key) == 0 ) return(entry->key);
    entry = entry->next;
  }
  return(NULL);
}

void	MsxHashTableFree(MsxHashTable *ht)
{
  MsxHashEntry *entry, *nextentry;
  int i;
  for (i=0; i<MSXHASHTABLEMAXSIZE; i++)
  {
    entry = ht[i];
    while (entry != NULL)
    {
      nextentry = entry->next;
      free(entry);
      entry = nextentry;
    }
  }
  free(ht);
}
