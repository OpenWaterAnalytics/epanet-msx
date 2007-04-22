/*******************************************************************************
**  MODULE:        MSXFILE.C
**  PROJECT:       EPANET-MSX
**  DESCRIPTION:   writes MSX project data to a MSX input file.
**  COPYRIGHT:     Copyright (C) 2007 Feng Shang, Lewis Rossman, and James Uber.
**                 All Rights Reserved. See license information in LICENSE.TXT.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**  VERSION:       1.00
**  LAST UPDATE:   3/13/07
*******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "msxtypes.h"
#include "msxutils.h"
#include "msxdict.h"
#include "epanet2.h"

//  External variables
//--------------------
extern MSXproject  MSX;                // MSX project data

//  Exported functions
//--------------------
void MSXfile_save(FILE *f);

//  Local functions
//-----------------
static void  saveSpecies(FILE *f);
static void  saveCoeffs(FILE *f);
static void  saveInpSections(FILE *f);
static void  saveParams(FILE *f);
static void  saveQuality(FILE *f);
static void  saveSources(FILE *f);
static void  savePatterns(FILE *f);

//=============================================================================

void MSXfile_save(FILE *f)
/*
**  Purpose:
**    saves current MSX project data to file.
**
**  Input:
**    f = pointer to MSX file where data are saved.
*/
{
    fprintf(f, "[TITLE]");
    fprintf(f, "\n%s\n", MSX.Title);
    saveSpecies(f);
    saveCoeffs(f);
    saveInpSections(f);
    saveParams(f);
    saveQuality(f);
    saveSources(f);
    savePatterns(f);
}

//=============================================================================

void  saveSpecies(FILE *f)
{
    int  i, n;
    fprintf(f, "\n[SPECIES]");
    n = MSX.Nobjects[SPECIE];
    for (i=1; i<=n; i++)
    {
        if ( MSX.Specie[i].type == BULK ) fprintf(f, "\nBULK    "); 
        else                          fprintf(f, "\nWALL    ");
        fprintf(f, "%-32s %-15s %.6f %.6f",
            MSX.Specie[i].id, MSX.Specie[i].units,
            MSX.Specie[i].aTol, MSX.Specie[i].rTol);
    }
}

//=============================================================================

void  saveCoeffs(FILE *f)
{
    int  i, n;
    fprintf(f, "\n\n[COEFFICIENTS]");
    n = MSX.Nobjects[CONSTANT];
    for (i=1; i<=n; i++)
    {
        fprintf(f, "\nCONSTANT    %-32s  %.6f",
            MSX.Const[i].id, MSX.Const[i].value);
    }
    n = MSX.Nobjects[PARAMETER];
    for (i=1; i<=n; i++)
    {
        fprintf(f, "\nPARAMETER   %-32s  %.6f",
            MSX.Param[i].id, MSX.Param[i].value);
    }
}

//=============================================================================

void  saveInpSections(FILE *f)
{
    char   line[MAXLINE+1];
    char   writeLine;
    int    newsect;

    rewind(MSX.MsxFile.file);
    writeLine = FALSE;
    while ( fgets(line, MAXLINE, MSX.MsxFile.file) != NULL )
    {
        if (*line == '[' )
        {
            writeLine = TRUE;
            newsect = MSXutils_findmatch(line, MsxSectWords);
            if ( newsect >= 0 ) switch(newsect)
            {
              case s_OPTION:
              case s_TERM:
              case s_PIPE:
              case s_TANK:
              case s_REPORT:
                fprintf(f, "\n");
                break;
              default:
                writeLine = FALSE;
            }
        }
        if ( writeLine) fprintf(f, "\n%s", line);
    }
}

//=============================================================================

void  saveParams(FILE *f)
{
    int    i, j, k;
    double x;
    char   id[MAXLINE+1];

    if ( MSX.Nobjects[PARAMETER] > 0 )
    {
        fprintf(f, "\n\n[PARAMETERS]");
        for (j=1; j<=MSX.Nobjects[PARAMETER]; j++)
        {
            x = MSX.Param[j].value;
            for (i=1; i<=MSX.Nobjects[LINK]; i++)
            {
                if ( MSX.Link[i].param[j] != x )
                {
                    ENgetlinkid(i, id);
                    fprintf(f, "\nPIPE    %-32s  %-32s  %.6f",
                        id, MSX.Param[j].id, MSX.Link[i].param[j]);
                }
            }
            for (i=1; i<=MSX.Nobjects[TANK]; i++)
            {
                if ( MSX.Tank[i].param[j] != x )
                {
                    k = MSX.Tank[i].node;
                    ENgetnodeid(k, id);
                    fprintf(f, "\nTANK    %-32s  %-32s  %.6f",
                        MSX.Param[j].id, MSX.Tank[i].param[j]);
                }
            }
        }
    }
}

//=============================================================================

void  saveQuality(FILE *f)
{
    int    i, j;
    char   id[MAXLINE+1];

    fprintf(f, "\n\n[QUALITY]");
    for (j=1; j<=MSX.Nobjects[SPECIE]; j++)
    {
        for (i=1; i<=MSX.Nobjects[NODE]; i++)
        {
            if ( MSX.Node[i].c0[j] > 0.0 )
            {
                ENgetnodeid(i, id);
                fprintf(f, "\nNODE    %-32s  %-32s  %.6f",
                    id, MSX.Specie[j].id, MSX.Node[i].c0[j]);
            }
        }
        for (i=1; i<=MSX.Nobjects[LINK]; i++)
        {
            if ( MSX.Link[i].c0[j] > 0.0 )
            {
                ENgetlinkid(i, id);
                fprintf(f, "\nLINK    %-32s  %-32s  %.6f",
                    id, MSX.Specie[j].id, MSX.Link[i].c0[j]);
            }
        }
    }
}

//=============================================================================

void  saveSources(FILE *f)
{
    int     i;
    Psource source;
    char    id[MAXLINE+1];

    fprintf(f, "\n\n[SOURCES]");
    for (i=1; i<=MSX.Nobjects[NODE]; i++)
    {
        source = MSX.Node[i].sources;
        while ( source )
        {
            if ( source->c0 > 0.0 )
            {
                ENgetnodeid(i, id);
                fprintf(f, "\n%-10s  %-32s  %-32s  %.6f",
                    SourceTypeWords[source->type], id,
                    MSX.Specie[source->specie].id, source->c0);
                if ( source->pat > 0 )
                    fprintf(f, "  %-32s", MSX.Pattern[source->pat]);
            }
            source = source->next;
        }
    }
}

//=============================================================================

void  savePatterns(FILE *f)
{
    int  i, count;
    SnumList *listItem;

    if ( MSX.Nobjects[PATTERN] > 0 ) fprintf(f, "\n\n[PATTERNS]");
    for (i=1; i<=MSX.Nobjects[PATTERN]; i++)
    {
        count = 0;
        listItem = MSX.Pattern[i].first;
        while (listItem)
        {
            if ( count % 6 == 0 )
            {
                fprintf(f, "\n%-32s", MSX.Pattern[i].id);
            }
            fprintf(f, "  %.6f", listItem->value);
            count++;
            listItem = listItem->next;
        }
    }
}
