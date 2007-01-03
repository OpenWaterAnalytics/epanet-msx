//-----------------------------------------------------------------------------
//   hash.h
//
//   Header file for Hash Table module hash.c.
//-----------------------------------------------------------------------------

#define HTMAXSIZE 1999
#define NOTFOUND  -1

// Hash table entry
struct HTentry
{
    char * key;
    int    data;
    struct HTentry * next;
};
typedef struct HTentry *HTtable;

// Creates a hash table
HTtable * HTcreate(void);

// Inserts a new entry into a hash table
int     HTinsert(HTtable *, char *, int);

// Finds the data entry in a hash table given its key
int     HTfind(HTtable *, char *);

// Checks if key value is in a hash table
char * HTfindKey(HTtable *, char *);

// Deletes a hash table
void   HTfree(HTtable *);
