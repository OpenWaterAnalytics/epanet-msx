#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>

#include <sqlite3.h>

#include "epanet2.h"
#include "time.h"

#include "msxtypes.h"

//  External variables
//--------------------
extern MSXproject  MSX;                // MSX project data


//  Imported functions
//--------------------
double MSXqual_getNodeQual(int j, int m);
double MSXqual_getLinkQual(int k, int m);

//  Exported functions
//--------------------
int   MSXout_open(void);
int   MSXout_saveInitialResults(void);
int   MSXout_saveResults(void);
int   MSXout_saveFinalResults(void);
float MSXout_getNodeQual(int k, int j, int m);
float MSXout_getLinkQual(int k, int j, int m);

//  Local functions
//-----------------
static int   saveStatResults(void);
static void  getStatResults(int objType, int m, double* stats1,
             double* stats2, REAL4* x);


static int initSchema(sqlite3 *db);
int addSpecies(const Sspecies *species);
int addTimeSeries(const char *id, const char* units);
void getTsNameForNode(int iNode, const Sspecies *species, char *tsNameOut);
void getTsNameForLink(int iLink, const Sspecies *species, char *tsNameOut);
void saveValue(const char* tsName, double value, time_t time);
double fetchValue(const char* tsName, time_t time);

//=============================================================================

int MSXout_open()
/*
**  Purpose:
**    opens an sqlite file for writing results.
**
**  Input:
**    none.
**
**  Returns:
**    an error code (or 0 if no error).
*/
{
// --- close output file if already opened

  if (MSX.ResultsDb.dbHandle != NULL) {
    sqlite3_close(MSX.ResultsDb.dbHandle);
    MSX.ResultsDb.dbHandle = NULL;
  }
  

// --- try to open the file

  int returnCode = sqlite3_open_v2(MSX.ResultsDb.name, &MSX.ResultsDb.dbHandle, SQLITE_OPEN_READWRITE, NULL);
  if (returnCode == SQLITE_CANTOPEN) {
    returnCode = sqlite3_open_v2(MSX.ResultsDb.name, &MSX.ResultsDb.dbHandle, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if (returnCode == SQLITE_OK) {
      returnCode = initSchema(MSX.ResultsDb.dbHandle);
    }
  }
  if (returnCode != SQLITE_OK) {
    return ERR_OPEN_OUT_FILE;
  }
  
  
  initSchema(MSX.ResultsDb.dbHandle);
  
  
  // set up prepared statements
  
  
  const char *selectStr = "SELECT time, value, quality, confidence FROM points INNER JOIN meta USING (series_id) WHERE name = ? AND time = ? order by time asc";
  returnCode = sqlite3_prepare_v2(MSX.ResultsDb.dbHandle, selectStr, -1, &MSX.ResultsDb.selectStmt, NULL);
  
  if (returnCode != SQLITE_OK) {
    const char *msg = sqlite3_errmsg(MSX.ResultsDb.dbHandle);
    fprintf(stderr, "%s\n", msg);
  }
  
  const char *insertStr = "INSERT INTO points (time, series_id, value, quality, confidence) SELECT ?,series_id,?,?,? FROM meta WHERE name = ?";
  returnCode = sqlite3_prepare(MSX.ResultsDb.dbHandle, insertStr, -1, &MSX.ResultsDb.insertStmt, NULL);
  
  if (returnCode != SQLITE_OK) {
    const char *msg = sqlite3_errmsg(MSX.ResultsDb.dbHandle);
    fprintf(stderr, "%s\n", msg);
  }
  
// --- open a scratch output file for statistics
//
//    if ( MSX.Statflag == SERIES ) MSX.TmpOutFile.file = MSX.OutFile.file;
//    else if ( (MSX.TmpOutFile.file = fopen(MSX.TmpOutFile.name, "w+b")) == NULL)
//    {
//        return ERR_OPEN_OUT_FILE;
//    }

// --- write initial results to file

    MSX.Nperiods = 0;
    MSXout_saveInitialResults();
    return 0;
}

//=============================================================================

int MSXout_saveInitialResults()
/*
**  Purpose:
**    saves general information to db.
**
**  Input:
**    none.
**
**  Returns:
**    an error code (or 0 if no error).
*/
{
  int   m;
  
  for (m=1; m<=MSX.Nobjects[SPECIES]; m++)
  {
    addSpecies(&MSX.Species[m]);
  }
  
  return 0;
}


//=============================================================================

int MSXout_saveResults()
/*
**  Purpose:
**    saves computed species concentrations for each node and link at the
**    current time period to the db
**
**  Input:
**    none.
**
**  Returns:
**    an error code (or 0 if no error).
*/
{
  int   iSpecies, iElement;
  char tsName[MAXLINE];
  REAL4 qual;
  
  time_t now = (time_t)MSX.Qtime;
  
  for (iSpecies=1; iSpecies<=MSX.Nobjects[SPECIES]; iSpecies++) {
    
    for (iElement=1; iElement<=MSX.Nobjects[NODE]; iElement++) {
      qual = (REAL4)MSXqual_getNodeQual(iElement, iSpecies);
      getTsNameForNode(iElement, &MSX.Species[iSpecies], tsName);
      saveValue(tsName, qual, now);
    }
    
    for (iElement=1; iElement<=MSX.Nobjects[LINK]; iElement++) {
      qual = (REAL4)MSXqual_getLinkQual(iElement, iSpecies);
      getTsNameForLink(iElement, &MSX.Species[iSpecies], tsName);
      saveValue(tsName, qual, now);
    }
    
  }
  return 0;
}

//=============================================================================

int MSXout_saveFinalResults()
/*
**  Purpose:
**    saves any statistical results plus the following information to the end
**    of the MSX binary output file:
**    - byte offset into file where WQ results for each time period begins,
**    - total number of time periods written to the file,
**    - any error code generated by the analysis (0 if there were no errors),
**    - the Magic Number to indicate that the file is complete.
**
**  Input:
**    none.
**
**  Returns:
**    an error code (or 0 if no error).
*/
{
    INT4  n;
    INT4  magic = MAGICNUMBER;
    int   err = 0;

// --- save statistical results to the file

    if ( MSX.Statflag != SERIES ) err = saveStatResults();
    if ( err > 0 ) return err;

// --- write closing records to the file

//    n = (INT4)ResultsOffset;
//    fwrite(&n, sizeof(INT4), 1, MSX.OutFile.file);
//    n = (INT4)MSX.Nperiods;
//    fwrite(&n, sizeof(INT4), 1, MSX.OutFile.file);
//    n = (INT4)MSX.ErrCode;
//    fwrite(&n, sizeof(INT4), 1, MSX.OutFile.file);
//    fwrite(&magic, sizeof(INT4), 1, MSX.OutFile.file);
  
  
  sqlite3_close(MSX.ResultsDb.dbHandle);
  
    return 0;
}

//=============================================================================

float MSXout_getNodeQual(int k, int j, int m)
/*
**  Purpose:
**    retrieves a result for a specific node from the MSX binary output file.
**
**  Input:
**    k = time period index
**    j = node index
**    m = species index.
**
**  Returns:
**    the requested species concentration. 
*/
{
  REAL4 c;
  char tsName[MAXLINE];
  getTsNameForNode(j, &MSX.Species[m], tsName);
  c = (REAL4)fetchValue(tsName, k);
  return (float)c;
}

//=============================================================================

float MSXout_getLinkQual(int k, int j, int m)
/*
**  Purpose:
**    retrieves a result for a specific link from the MSX binary output file.
**
**  Input:
**    k = time period index
**    j = link index
**    m = species index.
**
**  Returns:
**    the requested species concentration. 
*/
{
  REAL4 c;
  char tsName[MAXLINE];
  getTsNameForLink(j, &MSX.Species[m], tsName);
  c = (REAL4)fetchValue(tsName, k);
  return (float)c;
}

//=============================================================================

int  saveStatResults()
/*
**  Purpose:
**    saves time statistic results (average, min., max., or range) for each
**    node and link to the permanent binary output file.
**
**  Input:
**    none.
**
**  Returns:
**    an error code (or 0 if no error).
*/
{
    int     m, err = 0;
    REAL4*  x = NULL;
    double* stats1 = NULL;
    double* stats2 = NULL;

// --- create arrays used to store statistics results

//    if ( MSX.Nperiods <= 0 ) return err;
//    m = MAX(MSX.Nobjects[NODE], MSX.Nobjects[LINK]);
//    x = (REAL4 *) calloc(m+1, sizeof(REAL4));
//    stats1 = (double *) calloc(m+1, sizeof(double));
//    stats2 = (double *) calloc(m+1, sizeof(double));
//
//// --- get desired statistic for each node & link and save to binary file
//
//    if ( x && stats1 && stats2 )
//    {
//        for (m = 1; m <= MSX.Nobjects[SPECIES]; m++ )
//        {
//            getStatResults(NODE, m, stats1, stats2, x);
//            fwrite(x+1, sizeof(REAL4), MSX.Nobjects[NODE], MSX.OutFile.file);
//        }
//        for (m = 1; m <= MSX.Nobjects[SPECIES]; m++)
//        {
//            getStatResults(LINK, m, stats1, stats2, x);    
//            fwrite(x+1, sizeof(REAL4), MSX.Nobjects[LINK], MSX.OutFile.file);
//        }
//        MSX.Nperiods = 1;
//    }
//    else err = ERR_MEMORY;
//
//// --- free allocated arrays
//
//    FREE(x);
//    FREE(stats1);
//    FREE(stats2);
    return err;
}

//=============================================================================

void getStatResults(int objType, int m, double * stats1, double * stats2,
                    REAL4 * x)
/*
**  Purpose:
**    reads all results for a given type of object from the temporary
**    binary output file and computes the required statistic (average,
**    min., max., or range) for each object.
**
**  Input:
**    objType = type of object (nodes or links)
**    m = species index
**    stats1, stats2 = work arrays used to hold intermediate values
**    x = array used to store results read from file.
**
**  Output:
**    x = array that contains computed statistic for each object.
*/
{
    int  j, k;
    int  n = MSX.Nobjects[objType];
    long bp;

// --- initialize work arrays
//    
//    for (j = 1; j <= n; j++)
//    {
//        stats1[j] = 0.0;
//        stats2[j] = 0.0; 
//    }
//
//// --- for all time periods
//
//    for (k = 0; k < MSX.Nperiods; k++)
//    {
//
//    // --- position file at start of time period
//
//        bp = k*(NodeBytesPerPeriod + LinkBytesPerPeriod);
//        if ( objType == NODE )
//        {
//            bp += (m-1) * MSX.Nobjects[NODE] * sizeof(REAL4);
//        }
//        if ( objType == LINK)
//        {
//            bp += NodeBytesPerPeriod + 
//                  (m-1) * MSX.Nobjects[LINK] * sizeof(REAL4);
//        }
//        fseek(MSX.TmpOutFile.file, bp, SEEK_SET);
//
//    // --- read concentrations and update stats for all objects
//
//        fread(x+1, sizeof(REAL4), n, MSX.TmpOutFile.file);
//        if ( MSX.Statflag == AVGERAGE )
//        { 
//            for (j = 1; j <= n; j++) stats1[j] += x[j];
//        }
//        else for (j = 1; j <= n; j++)
//        {
//            stats1[j] = MIN(stats1[j], x[j]); 
//            stats2[j] = MAX(stats2[j], x[j]); 
//        }
//    }
//
//// --- place final stat value for each object in x
//
//    if ( MSX.Statflag == AVGERAGE )
//    {
//        for ( j = 1; j <= n; j++) stats1[j] /= (double)MSX.Nperiods;
//    }
//    if ( MSX.Statflag == RANGE )
//    {
//        for ( j = 1; j <= n; j++)
//            stats1[j] = fabs(stats2[j] - stats1[j]);
//    }
//    if ( MSX.Statflag == MAXIMUM)
//    {
//        for ( j = 1; j <= MSX.Nobjects[NODE]; j++) stats1[j] = stats2[j]; 
//    }
//    for (j = 1; j <= n; j++) x[j] = (REAL4)stats1[j];
}


int initSchema(sqlite3 *db)
{
  
  const char *initTablesStr = "CREATE TABLE 'meta' ('series_id' INTEGER PRIMARY KEY ASC AUTOINCREMENT, 'name' TEXT UNIQUE ON CONFLICT ABORT, 'units' TEXT, 'regular_period' INTEGER, 'regular_offset' INTEGER); CREATE TABLE 'points' ('time' INTEGER, 'series_id' INTEGER REFERENCES 'meta'('series_id'), 'value' REAL, 'confidence' REAL, 'quality' INTEGER, UNIQUE (series_id, time) ON CONFLICT IGNORE)";
  
  int errCode;
  char *errmsg;
  errCode = sqlite3_exec(MSX.ResultsDb.dbHandle, initTablesStr, NULL, NULL, &errmsg);
  return errCode;
  
}


int addSpecies(const Sspecies *species)
{
  
  char tsName[MAXLINE];
  
  // for each node and link, add a time series for the concentration of this species
  
  for (int iNode = 1; iNode <= MSX.Nobjects[NODE]; ++iNode) {
    getTsNameForNode(iNode, species, tsName);
    addTimeSeries(tsName, species->units);
  }
  
  for (int iLink = 1; iLink <= MSX.Nobjects[LINK]; ++iLink) {
    getTsNameForLink(iLink, species, tsName);
    addTimeSeries(tsName, species->units);
  }
  
  return SQLITE_OK;
}

int addTimeSeries(const char *id, const char *units)
{
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(MSX.ResultsDb.dbHandle, "insert or ignore into meta (name,units) values (?,?)", -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, id, -1, NULL);
  sqlite3_bind_text(stmt, 2, units, -1, NULL);
  
  int ret = sqlite3_step(stmt);
  if (ret != SQLITE_DONE) {
    return ret;
  }
  sqlite3_reset(stmt);
  return SQLITE_OK;
}


void getTsNameForNode(int iNode, const Sspecies *species, char *tsNameOut)
{
  char nodeName[MAXLINE];
  ENgetnodeid(iNode, nodeName);
  sprintf(tsNameOut, "N %s %s", nodeName, species->id);
}

void getTsNameForLink(int iLink, const Sspecies *species, char *tsNameOut)
{
  char linkName[MAXLINE];
  ENgetlinkid(iLink, linkName);
  sprintf(tsNameOut, "L %s %s", linkName, species->id);
}

void saveValue(const char* tsName, double value, time_t time)
{
  int ret;
  
  // INSERT INTO points (time, series_id, value, quality, confidence) SELECT ?,series_id,?,?,? FROM meta WHERE name = ?
  
  ret = sqlite3_bind_int(    MSX.ResultsDb.insertStmt, 1, (int)time         );
  ret = sqlite3_bind_double( MSX.ResultsDb.insertStmt, 2, value             );
  ret = sqlite3_bind_int(    MSX.ResultsDb.insertStmt, 3, 0                 );
  ret = sqlite3_bind_double( MSX.ResultsDb.insertStmt, 4, 0                 );
  ret = sqlite3_bind_text(   MSX.ResultsDb.insertStmt, 5, tsName, -1, NULL  );
  
  ret = sqlite3_step(MSX.ResultsDb.insertStmt);
  if (ret != SQLITE_DONE) {
    // do something?
  }
  sqlite3_reset(MSX.ResultsDb.insertStmt);
}

double fetchValue(const char* tsName, time_t time)
{
  double value = 0;
  // SELECT time, value, quality, confidence FROM points INNER JOIN meta USING (series_id) WHERE name = ? AND time = ? order by time asc
  
  int ret;
  ret = sqlite3_bind_text(MSX.ResultsDb.selectStmt, 1, tsName, -1, NULL);
  ret = sqlite3_bind_int(MSX.ResultsDb.selectStmt, 2, (int)time);
  
  while (sqlite3_step(MSX.ResultsDb.selectStmt) == SQLITE_ROW) {
    value = sqlite3_column_double(MSX.ResultsDb.selectStmt, 1);
  }
  sqlite3_reset(MSX.ResultsDb.selectStmt);
  
  return value;
}




