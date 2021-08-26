/******************************************************************************
**  MODULE:        MSXRPT.C
**  PROJECT:       EPANET-MSX
**  DESCRIPTION:   report writing routines for the EPANET Multi-Species
**                 Extension toolkit.
**  COPYRIGHT:     Copyright (C) 2007 Feng Shang, Lewis Rossman, and James Uber.
**                 All Rights Reserved. See license information in LICENSE.TXT.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**                 K. Arrowood, Xylem intern
**  VERSION:       Refer to git history
**  LAST UPDATE:   2/8/11
**  BUG FIX: Bug ID 08 Feng Shang 01/07/08
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "msxtypes.h"
#include "epanet2.h"
#include "msxdict.h"

// Constants
//----------
#define SERIES_TABLE  0
#define STATS_TABLE   1


//  Local variables
//-----------------
static char *Logo[] =
    {"******************************************************************",
     "*                     MSX Water Quality Engine                   *",
     "*                   Multi-Species Water Quality                  *",
     "*                   Analysis for Pipe  Networks                  *",
     "******************************************************************"};

static char PageHdr[] = "  Page %d                                    ";
static char *StatsHdrs[] =
    {"", "Average Values  ", "Minimum Values  ",
         "Maximum Values  ", "Range of Values "};
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

//  Imported functions
//--------------------
float MSXout_getNodeQual(MSXproject MSX, int k, int j, int m);
float MSXout_getLinkQual(MSXproject MSX, int k, int j, int m);


//  Local functions
//-----------------
static void  createSeriesTables(MSXproject MSX);
static void  createStatsTables(MSXproject MSX);
static void  createTableHdr(MSXproject MSX, int objType, int tableType);
static void  writeTableHdr(MSXproject MSX);
static void  writeNodeTable(MSXproject MSX, int j, int tableType);
static void  writeLinkTable(MSXproject MSX, int j, int tableType);
static void  getHrsMins(MSXproject MSX, int k, int *hrs, int *mins);
static void  newPage(MSXproject MSX);
static void  writeLine(MSXproject MSX, char *line);

static void writemassbalance(MSXproject MSX);

//=============================================================================

int  MSXrpt_write(MSXproject MSX, char *fname)
{
    INT4  magic = 0;
    int  j;
    int recordsize = sizeof(INT4);

    if (strcmp(MSX->RptFile.name, "") == 0) {
        if (fname == NULL) MSX->RptFile.file = stdout;
        else {
            strcpy(MSX->RptFile.name, fname);
            MSX->RptFile.file = fopen(MSX->RptFile.name, "wt");
            if ( MSX->RptFile.file == NULL ) return ERR_OPEN_RPT_FILE;
        }
    }

// --- check that results are available

    if ( MSX->Nperiods < 1 )    return 0;
    if ( MSX->OutFile.file == NULL ) return ERR_OPEN_OUT_FILE;
    fseek(MSX->OutFile.file, -recordsize, SEEK_END);
    fread(&magic, sizeof(INT4), 1, MSX->OutFile.file);
    if ( magic != MAGICNUMBER ) return ERR_IO_OUT_FILE;

// --- write program logo & project title

    PageNum = 1;
    LineNum = 1;
    newPage(MSX);
    for (j=0; j<5; j++) writeLine(MSX, Logo[j]);
    writeLine(MSX, "");
    writeLine(MSX, MSX->Title);

// --- generate the appropriate type of table

    if ( MSX->Statflag == SERIES ) createSeriesTables(MSX);
    else createStatsTables(MSX);

    writemassbalance(MSX);

    writeLine(MSX, "");
    return 0;
}

//=============================================================================

void  MSXrpt_writeLine(MSXproject MSX, char *line)                                             //1.1.00
{                                                                              //1.1.00
    writeLine(MSX, line);                                                           //1.1.00
}                                                                              //1.1.00

//=============================================================================

void createSeriesTables(MSXproject MSX)
{
    int  j;

// --- report on all requested nodes

    for (j=1; j<=MSX->Nobjects[NODE]; j++)
    {
        if ( !(MSX->Node[j].rpt) ) continue;
        if (strcmp(MSX->RptFile.name, "EPA") == 0) ENgetnodeid(j, IDname);
        else strcpy(IDname, MSX->Node[j].id);
        createTableHdr(MSX, NODE, SERIES_TABLE);
        writeNodeTable(MSX, j, SERIES_TABLE);
    }

// --- report on all requested links

    for (j=1; j<=MSX->Nobjects[LINK]; j++)
    {
        if ( !MSX->Link[j].rpt ) continue;
        if (strcmp(MSX->RptFile.name, "EPA") == 0) ENgetlinkid(j, IDname);
        else strcpy(IDname, MSX->Link[j].id);
        createTableHdr(MSX, LINK, SERIES_TABLE);
        writeLinkTable(MSX, j, SERIES_TABLE);
    }
}

//=============================================================================

void createStatsTables(MSXproject MSX)
{
    int  j;
    int  count;

// --- check if any nodes to be reported

    count = 0;
    for (j = 1; j <= MSX->Nobjects[NODE]; j++) count += MSX->Node[j].rpt;

// --- report on all requested nodes

    if ( count > 0 )
    {
        createTableHdr(MSX, NODE, STATS_TABLE);
        for (j = 1; j <= MSX->Nobjects[NODE]; j++)
        {
            if ( MSX->Node[j].rpt ) writeNodeTable(MSX, j, STATS_TABLE);
        }
    }

// --- check if any links to be reported

    count = 0;
    for (j = 1; j <= MSX->Nobjects[LINK]; j++) count += MSX->Link[j].rpt;

// --- report on all requested links

    if ( count > 0 )
    {
        createTableHdr(MSX, LINK, STATS_TABLE);
        for (j = 1; j <= MSX->Nobjects[LINK]; j++)
        {
            if ( MSX->Link[j].rpt ) writeLinkTable(MSX, j, STATS_TABLE);
        }
    }
}

//=============================================================================

void createTableHdr(MSXproject MSX, int objType, int tableType)
{
    int   m;
    char  s1[MAXLINE+1];
    char  s2[MAXLINE+1];

    if ( tableType == SERIES_TABLE )
    {
        if ( objType == NODE )
            sprintf(TableHdr.Line1, "<<< Node %s >>>", IDname);
        else
            sprintf(TableHdr.Line1, "<<< Link %s >>>", IDname);
        strcpy(TableHdr.Line2, "Time   ");
        strcpy(TableHdr.Line3, "hr:min ");
        strcpy(TableHdr.Line4, "-------");
    }
    if ( tableType == STATS_TABLE )
    {
        strcpy(TableHdr.Line1, "");
        sprintf(TableHdr.Line2, "%-16s", StatsHdrs[tableType]);
        if ( objType == NODE ) strcpy(TableHdr.Line3, "for Node        ");
        else                   strcpy(TableHdr.Line3, "for Link        ");
        strcpy(TableHdr.Line4, "----------------");
    }
    for (m=1; m<=MSX->Nobjects[SPECIES]; m++)
    {
        if ( !MSX->Species[m].rpt ) continue;
        if ( objType == NODE && MSX->Species[m].type == WALL ) continue;
        sprintf(s1, "  %10s", MSX->Species[m].id);
        strcat(TableHdr.Line2, s1);
        strcat(TableHdr.Line4, "  ----------");
        strcpy(s1, MSX->Species[m].units);
        strcat(s1, "/");
        if ( MSX->Species[m].type == BULK ) strcat(s1, "L");
        else strcat(s1, AreaUnitsWords[MSX->AreaUnits]);
        sprintf(s2, "  %10s", s1);
        strcat(TableHdr.Line3, s2);
    }
    if ( MSX->PageSize > 0 && MSX->PageSize - LineNum < 8 ) newPage(MSX);
    else writeTableHdr(MSX);
}

//=============================================================================

void  writeTableHdr(MSXproject MSX)
{
    if ( MSX->PageSize > 0 && MSX->PageSize - LineNum < 6 ) newPage(MSX);
    writeLine(MSX, "");
    writeLine(MSX, TableHdr.Line1);
    writeLine(MSX, "");
    writeLine(MSX, TableHdr.Line2);
    writeLine(MSX, TableHdr.Line3);
    writeLine(MSX, TableHdr.Line4);
}

//=============================================================================

void  writeNodeTable(MSXproject MSX, int j, int tableType)
{
    int   k, m, hrs, mins;
    char  s[MAXLINE+1];
    float c;

    for (k=0; k<MSX->Nperiods; k++)
    {
        if ( tableType == SERIES_TABLE )
        {
            getHrsMins(MSX, k, &hrs, &mins);
            sprintf(Line, "%4d:%02d", hrs, mins);
        }
        if ( tableType == STATS_TABLE )
        {
            if (strcmp(MSX->RptFile.name, "EPA") == 0) ENgetnodeid(j, IDname);
            else strcpy(IDname, MSX->Node[j].id);
            sprintf(Line, "%-16s", IDname);
        }
        for (m=1; m<=MSX->Nobjects[SPECIES]; m++)
        {
            if ( !MSX->Species[m].rpt ) continue;
            if ( MSX->Species[m].type == WALL ) continue;
            c = MSXout_getNodeQual(MSX, k, j, m);
            sprintf(s, "  %10.*f", MSX->Species[m].precision, c);
            strcat(Line, s);
        }
        writeLine(MSX, Line);
    }
}

//=============================================================================

void  writeLinkTable(MSXproject MSX, int j, int tableType)
{
    int   k, m, hrs, mins;
    char  s[MAXLINE+1];
    float c;

    for (k=0; k<MSX->Nperiods; k++)
    {
        if ( tableType == SERIES_TABLE )
        {
            getHrsMins(MSX, k, &hrs, &mins);
            sprintf(Line, "%4d:%02d", hrs, mins);
        }
        if ( tableType == STATS_TABLE )
        {
            if (strcmp(MSX->RptFile.name, "EPA") == 0) ENgetlinkid(j, IDname);
            else strcpy(IDname, MSX->Link[j].id);
            sprintf(Line, "%-16s", IDname);
        }
        for (m=1; m<=MSX->Nobjects[SPECIES]; m++)
        {
            if ( !MSX->Species[m].rpt ) continue;
            c = MSXout_getLinkQual(MSX, k, j, m);
            sprintf(s, "  %10.*f", MSX->Species[m].precision, c);
            strcat(Line, s);
        }
        writeLine(MSX, Line);
    }
}

//=============================================================================

void getHrsMins(MSXproject MSX, int k, int *hrs, int *mins)
{
    long m, h;

    m = (MSX->Rstart + k*MSX->Rstep) / 60;
    h = m / 60;
    m = m - 60*h;
    *hrs = h;
    *mins = m;
}

//=============================================================================

void  newPage(MSXproject MSX)
{
    char  s[MAXLINE+1];
    LineNum = 1;
    sprintf(s,
            "\nPage %-3d                                             EPANET-MSX 1.1",   //1.1.00
            PageNum);
    writeLine(MSX, s);
    writeLine(MSX, "");
    if ( PageNum > 1 ) writeTableHdr(MSX);
    PageNum++;
}

//=============================================================================

void  writeLine(MSXproject MSX, char *line)
{
    if ( LineNum == MSX->PageSize ) newPage(MSX);
    if ( MSX->RptFile.file ) fprintf(MSX->RptFile.file, "  %s\n", line);   //(modified, FS-01/07/2008)
    else ENwriteline(line);
    LineNum++;
}



void writemassbalance(MSXproject MSX)
/**
**-------------------------------------------------------------
**   Input:
**      MSX = the underlying MSXproject data struct.
**   Output:  none
**   Purpose: writes water quality mass balance ratio
**            (Outflow + Final Storage) / Inflow + Initial Storage)
**            to report file.
**-------------------------------------------------------------
*/
{

    char s1[MAXMSG + 1];
    int  kunits = 0;

    for (int m = 1; m <= MSX->Nobjects[SPECIES]; m++)
    {
        if (MSX->Species[m].pipeExprType != RATE)
            continue;
        
        snprintf(s1, MAXMSG, "\n");

        snprintf(s1, MAXMSG, "Water Quality Mass Balance: %s (%s)", MSX->Species[m].id, MSX->Species[m].units);
        writeLine(MSX, s1);
        snprintf(s1, MAXMSG, "================================");
        writeLine(MSX, s1);
        snprintf(s1, MAXMSG, "Initial Mass:      %12.5e", MSX->MassBalance.initial[m]);
        writeLine(MSX, s1);
        snprintf(s1, MAXMSG, "Mass Inflow:       %12.5e", MSX->MassBalance.inflow[m]);
        writeLine(MSX, s1);
        snprintf(s1, MAXMSG, "Mass Outflow:      %12.5e", MSX->MassBalance.outflow[m]);
        writeLine(MSX, s1);
        snprintf(s1, MAXMSG, "Mass Reacted:      %12.5e", MSX->MassBalance.reacted[m]);
        writeLine(MSX, s1);
        snprintf(s1, MAXMSG, "Final Mass:        %12.5e", MSX->MassBalance.final[m]);
        writeLine(MSX, s1);
        snprintf(s1, MAXMSG, "Mass Ratio:         %-.5f", MSX->MassBalance.ratio[m]);
        writeLine(MSX, s1);
        snprintf(s1, MAXMSG, "================================\n");
        writeLine(MSX, s1);
    }
}


