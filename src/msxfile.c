/*******************************************************************************
**  TITLE:         MSXFILE.C
**  DESCRIPTION:   Save input function for the EPANET-MSX
**                 Multi-Species Extension toolkit.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**  VERSION:       1.00
**  LAST UPDATE:   1/15/07
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "typesmsx.h"    
#define  EXTERN_MSX extern
#include "globals.h"
#include "utils.h"
#include "epanet2.h"

// NOTE: the entries in MSsectWords must match the entries in the enumeration
//       variable SectionType defined in typesmsx.h.
static char *MSsectWords[] = {
        "[TITLE", "[SPECIE",  "[COEFF",  "[TERM",
        "[PIPE",  "[TANK",    "[SOURCE", "[QUALITY",
        "[PARAM", "[PATTERN", "[OPTION",
        "[REPORT", NULL};
static char *SourceTypeWords[] = {"CONC", "MASS", "FLOW", "SETPOINT", NULL};

// Exported Functions
// ------------------
int  savemsxfile(char *fname);

// Imported Functions
// ------------------
int  getNewSection(char *tok, char *sectWords[], int *sect);

// Local Functions
// ---------------
static void copymsxsections(FILE *ftmp);

void  copymsxsections(FILE *ftmp)
/*
-------------------------------------------------
Writes network data sections to text file.
All data sections are copied except PATTERN,
SOURCE, and TITLE
-------------------------------------------------
*/
{
    int   sect=-1;
    char  *tok; 
    char  line[MAXLINE+1];
    char  s[MAXLINE+1];

    while (fgets(line,MAXLINE,InpFile.file) != NULL)
    {
        strcpy(s,line);
        tok = strtok(line,SEPSTR);
        if (tok == NULL || *tok == ';') continue;

        /* Check if line begins with a new section heading */
        if (*tok == '[')
        {
            if ( getNewSection(tok, MSsectWords, &sect) )
            {
                switch(sect)
                {
                case s_SPECIE:
                case s_COEFF:
                case s_TERM:
                case s_PIPE:
                case s_TANK:
                case s_QUALITY:
                case s_PARAMETER:
                case s_OPTION:
                case s_REPORT: fwrite(s, sizeof(char), strlen(s), ftmp);
                }
                continue;
            }
            else continue;
        }

        /* Write lines appearing in the section to file */
        switch(sect)
        {
        case s_SPECIE:
        case s_COEFF:
        case s_TERM:
        case s_PIPE:
        case s_TANK:
        case s_QUALITY:
        case s_PARAMETER:
        case s_OPTION:
        case s_REPORT: fwrite(s, sizeof(char), strlen(s), ftmp);
        }
    }
}

int  savemsxfile(char *fname)
/*
-------------------------------------------------
Writes network data to text file.
-------------------------------------------------
*/
{
    int   i,j,err;
    char  s[MAXLINE+1], s1[MAXLINE+1], nid[MAXID+1];

    FILE  *f;
    FILE  *ftmp;

    /* Copy sections from original input file to new input file */

    if ((InpFile.file = fopen(InpFile.name,"rt")) == NULL) return 302;
    ftmp = tmpfile();
    if (ftmp) copymsxsections(ftmp);
    fclose(InpFile.file);
    InpFile.file = NULL;

    /* Open text file */

    if ((f = fopen(fname,"wt")) == NULL)
    {
        if (ftmp) fclose(ftmp);
        return(102);
    }

    /* Write [TITLE] section */

    fprintf(f,"[TITLE]");
    if (strlen(Title) > 0) fprintf(f,"\n%s",Title);

    /* Write [PATTERNS] section */
    /* (Use 6 pattern factors per line) */

    fprintf(f, "\n\n[PATTERNS]");
    for (i=1; i<=Nobjects[TIME_PATTERN]; i++)
    {
        SnumList *p=Pattern[i].first;
        int j=0;
        while ( p )
        {
            if (j % 6 == 0) fprintf(f,"\n %-15s",Pattern[i].id);
            fprintf(f," %12.4f",p->value);

            j++;
            p=p->next;
        }
    }

    /* Write [SOURCES] section */

    fprintf(f, "\n\n[SOURCES]");
    for (i=1; i<=Nobjects[NODE]; i++)
    {
        Psource source = Node[i].sources;
        while ( source )
        {
            err = ENgetnodeid(i,nid);
            if(err) return(err);
            sprintf(s," %-8s %-15s %-15s %12.2f",
                SourceTypeWords[source->type],
                nid,
                Specie[source->specie].id,
                source->c0);
            if ((j = source->pat) > 0)
                sprintf(s1,"%s",Pattern[j].id);
            else strcpy(s1,"");
            fprintf(f,"\n%s %s",s,s1);

            source = source->next;
        }
    }

    /* Copy data from scratch file to new input file */
    fprintf(f,"\n");
    if (ftmp != NULL)
    {
        fseek(ftmp, 0, SEEK_SET);
        do
        {
            j = fgetc(ftmp);
            fputc(j, f);
        }  while (j != EOF);
        fclose(ftmp);
    } 

    fclose(f);
    return(0);
}

