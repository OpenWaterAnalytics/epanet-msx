/******************************************************************************
**  TITLE:         REPORTMSX.C
**  DESCRIPTION:   report writing routines for the EPANET Multi-Species
**                 Extension toolkit.
**  AUTHORS  :     L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**  VERSION:       1.00
**  LAST UPDATE:   10/5/06
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "typesmsx.h"
#define  EXTERN_MSX extern
#include "globals.h"
#include "epanet2.h"

// Constants
//----------
#define RECORDSIZE 4

//  Local variables
//-----------------
static char *Logo[] =
    {"******************************************************************",
     "*                      E P A N E T  -  M S X                     *",
     "*                   Multi-Species Water Quality                  *",
     "*                   Analysis for Pipe  Networks                  *",
     "*                           Version 1.0                          *",
     "******************************************************************"};

static char PageHdr[] = "  Page %d                                    ";
static char Line[MAXLINE+1];
static long LineNum;
static long PageNum;
static int  *RptdSpecies;
static struct TableHdrStruct
{
    char Line1[MAXLINE+1];
    char Line2[MAXLINE+1];
    char Line3[MAXLINE+1];
    char Line4[MAXLINE+1];
    char Line5[MAXLINE+1];
} TableHdr;
static char IDname[MAXLINE+1];
static int  InTable;

//  Imported functions
//--------------------
void  input_getSpecieUnits(int m, char *units);
float output_getNodeQual(int k, int j, int m);
float output_getLinkQual(int k, int j, int m);

//  Exported functions
//--------------------
int   report_write(void);

//  Local functions
//-----------------
static void  createTableHdr(int tableType);
static void  writeTableHdr(void);
static void  writeNodeTable(int j);
static void  writeLinkTable(int j);
static void  getHrsMins(int k, int *hrs, int *mins);
static void  newPage(void);
static void  writeLine(char *line);

//=============================================================================

int  report_write()
{
    INT4 magic = 0;
    int  j;
    int  recordsize=sizeof(INT4);

// --- check that results are available

    if ( MSXNperiods < 1 )    return 0;
    if ( MSXOutFile.file == NULL ) return ERR_OPEN_OUT_FILE;
    fseek(MSXOutFile.file, -recordsize, SEEK_END);
    fread(&magic, sizeof(INT4), 1, MSXOutFile.file);
    if ( magic != MAGICNUMBER ) return ERR_IO_OUT_FILE;

// --- write program logo & project title

    InTable = 0;
    PageNum = 1;
    LineNum = 1;
    newPage();
    for (j=0; j<=5; j++) writeLine(Logo[j]);
    writeLine("");
    writeLine(MSXTitle);

// --- report on all requested nodes

    for (j=1; j<=MSXNobjects[NODE]; j++)
    {
        if ( !MSXNode[j].rpt ) continue;
        ENgetnodeid(j, IDname);
        createTableHdr(NODE);
        writeNodeTable(j);
    }

// --- report on all requested links

    for (j=1; j<=MSXNobjects[LINK]; j++)
    {
        if ( !MSXLink[j].rpt ) continue;
        ENgetlinkid(j, IDname);
        createTableHdr(LINK);
        writeLinkTable(j);
    }
    writeLine("");
    return 0;
}

//=============================================================================

void createTableHdr(int tableType)
{
    int   m;
    char  s1[MAXLINE+1];
    char  s2[MAXLINE+1];

    if ( tableType == NODE )
        sprintf(TableHdr.Line1, "<<< Node %s >>>", IDname);
    else
        sprintf(TableHdr.Line1, "<<< Link %s >>>", IDname);
    strcpy(TableHdr.Line2, "Time   ");
    strcpy(TableHdr.Line3, "hr:min ");
    strcpy(TableHdr.Line4, "-------");
    for (m=1; m<=MSXNobjects[SPECIE]; m++)
    {
        if ( !MSXSpecie[m].rpt ) continue;
        if ( tableType == NODE && MSXSpecie[m].type == WALL ) continue;
        sprintf(s1, "  %10s", MSXSpecie[m].id);
        strcat(TableHdr.Line2, s1);
        strcat(TableHdr.Line4, "  ----------");
        input_getSpecieUnits(m, s1);
        sprintf(s2, "  %10s", s1);
        strcat(TableHdr.Line3, s2);
    }
    if ( MSXPageSize > 0 && MSXPageSize - LineNum < 8 ) newPage();
    else writeTableHdr();
}

//=============================================================================

void  writeTableHdr()
{
    if ( MSXPageSize > 0 && MSXPageSize - LineNum < 6 ) newPage();
    writeLine("");
    writeLine(TableHdr.Line1);
    writeLine("");
    writeLine(TableHdr.Line2);
    writeLine(TableHdr.Line3);
    writeLine(TableHdr.Line4);
}

//=============================================================================

void  writeNodeTable(int j)
{
    int   k, m, hrs, mins;
    char  s[MAXLINE+1];
    float c;

    for (k=0; k<MSXNperiods; k++)
    {
        getHrsMins(k, &hrs, &mins);
        sprintf(Line, "%4d:%02d", hrs, mins);
        for (m=1; m<=MSXNobjects[SPECIE]; m++)
        {
            if ( !MSXSpecie[m].rpt ) continue;
            if ( MSXSpecie[m].type == WALL ) continue;
            c = output_getNodeQual(k, j, m);
            sprintf(s, "  %10.*f", MSXSpecie[m].precision, c);
            strcat(Line, s);
        }
        writeLine(Line);
    }
}

//=============================================================================

void  writeLinkTable(int j)
{
    int   k, m, hrs, mins;
    char  s[MAXLINE+1];
    float c;

    for (k=0; k<MSXNperiods; k++)
    {
        getHrsMins(k, &hrs, &mins);
        sprintf(Line, "%4d:%02d", hrs, mins);
        for (m=1; m<=MSXNobjects[SPECIE]; m++)
        {
            if ( !MSXSpecie[m].rpt ) continue;
            c = output_getLinkQual(k, j, m);
            sprintf(s, "  %10.*f", MSXSpecie[m].precision, c);
            strcat(Line, s);
        }
        writeLine(Line);
    }
}

//=============================================================================

void getHrsMins(int k, int *hrs, int *mins)
{
    long m, h;

    m = (MSXRstart + k*MSXRstep) / 60;
    h = m / 60;
    m = m - 60*h;
    *hrs = h;
    *mins = m;
}

//=============================================================================

void  newPage()
{
    char  s[MAXLINE+1];
    LineNum = 1;
    sprintf(s,
            "\n\fPage %-3d                                             EPANET-MSX 1.0",
            PageNum);
    writeLine(s);
    writeLine("");
    if ( PageNum > 1 ) writeTableHdr();
    PageNum++;
}

//=============================================================================

void  writeLine(char *line)
{
    if ( LineNum == MSXPageSize ) newPage();
    if ( MSXRptFile.file ) fprintf(MSXRptFile.file, "  %s", line);
    else ENwriteline(line);
    LineNum++;
}
