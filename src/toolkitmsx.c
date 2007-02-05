/*******************************************************************************
**  TITLE:         TOOLKITMSX.C
**  DESCRIPTION:   contains the exportable set of functions that comprise the
**                 EPANET Multi-Species Extension toolkit.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**  VERSION:       1.00
**
**  These functions can be used in conjunction with the original EPANET
**  toolkit functions to model water quality fate and transport of
**  multiple interacting chemcial species within piping networks. See the
**  EPANETMSX.C module for an example of how these functions were used to
**  extend the original command line version of EPANET to include multiple
**  chemical species. Consult the EPANET and EPANET-MSX Users Manuals for
**  detailed descriptions of the input data file formats required by both
**  the original EPANET and its multi-species extension.
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <float.h>
#include <malloc.h>

#include "typesmsx.h"
#define  EXTERN_MSX
#include "globals.h"
#include "epanet2.h"
#include "epanetmsx.h"

//  Imported functions
//--------------------
int    project_open(char *fname);
int    project_close(void);
int    project_findObject(int type, char *id);
char * project_getErrmsg(int errcode);
int    quality_open(void);
int    quality_init(void);
int    quality_step(long *t, long *tleft);
int    quality_close(void);
double quality_getNodeQual(int j, int m);
double quality_getLinkQual(int k, int m);
int    report_write(void);
int    savemsxfile(char *fname);
int    addPattern(char *id);
char * project_findID(int type, char *id);

//=============================================================================
//   Functions for opening and closing the EPANET-MSX system
//=============================================================================

int  DLLEXPORT  ENMSXopen(char *fname)
/*
**  Purpose:
**    opens an EPANET-MSX input file.
**
**  Input:
**    fname = name of file.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    int errcode = 0;
    if (ProjectOpened) return(ERR_PROJECT_OPENED);
    ERRCODE(project_open(fname));
    ERRCODE(quality_open());
    return errcode;
}

//=============================================================================

int  DLLEXPORT  ENMSXclose()
/*
**  Purpose:
**    closes the EPANET-MSX system.
**
**  Input:
**    none
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    if (!ProjectOpened) return(ERR_NO_PROJECT);
    quality_close();
    project_close();
    return 0;
}

//=============================================================================

int DLLEXPORT ENMSXsavemsxfile(char *filename)
/*
**  Purpose: 
**    saves current data base to file
**
**  Input:   
**    filename = name of MSX file
**
**  Output:  
**    none
**
**  Returns: 
**    error code
*/
{
   if (!ProjectOpened) return(ERR_NO_PROJECT);
   return(savemsxfile(filename));
}

//=============================================================================
//   Functions for running a hydraulic and MSX water quality analysis 
//=============================================================================

int   DLLEXPORT  ENMSXsolveH()
/*
**  Purpose:
**    solves for system hydraulics which are written to a temporary file.
**
**  Input:
**    none.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    int errcode = 0;

    if (!ProjectOpened) return(ERR_NO_PROJECT);

// --- close & remove any existing hydraulics file

    if ( HydFile.file )
    {
        fclose(HydFile.file);
        HydFile.file = NULL;
    }
    if ( HydFile.mode == SCRATCH_FILE ) remove(HydFile.name);

// --- create a temporary hydraulics file

    tmpnam(HydFile.name);

// --- use EPANET to solve for & save hydraulics results

    ERRCODE(ENsolveH());
    ERRCODE(ENsavehydfile(HydFile.name));
    ERRCODE(ENMSXusehydfile(HydFile.name));

// --- make sure file is declared temporary

    HydFile.mode = SCRATCH_FILE;
    return errcode;
}

//=============================================================================

int   DLLEXPORT  ENMSXusehydfile(char *fname)
/*
**  Purpose:
**    registers a hydraulics solution file with the MSX system.
**
**  Input:
**    fname = name of binary hydraulics results file.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    int magic;
    int version;
    int n;

    if (!ProjectOpened) return(ERR_NO_PROJECT);

// --- open hydraulics file

    if ( HydFile.file ) fclose(HydFile.file);
    HydFile.mode = USED_FILE;
    HydFile.file = fopen(fname, "rb");
    if (!HydFile.file) return 504;

// --- check that file is really a hydraulics file for current project

    fread(&magic, sizeof(int), 1, HydFile.file);
    if ( magic != MAGICNUMBER ) return 505;
    fread(&version, sizeof(int), 1, HydFile.file);
    //if ( version != VERSION ) return 505;
    fread(&n, sizeof(int), 1, HydFile.file);
    if ( n != Nobjects[NODE] ) return 505;
    fread(&n, sizeof(int), 1, HydFile.file);
    if ( n != Nobjects[LINK] ) return 505;
    fseek(HydFile.file, 3*sizeof(int), SEEK_CUR);

// --- read length of simulation period covered by file

    fread(&Dur, sizeof(int), 1, HydFile.file);
    HydOffset = ftell(HydFile.file);
    return 0;
}

//=============================================================================

int  DLLEXPORT  ENMSXinit(int saveFlag)
/*
**  Purpose:
**    initializes a MSX water quality analysis.
**
**  Input:
**    saveFlag = 1 if results saved to binary file, 0 if not.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    int errcode = 0;
    if (!ProjectOpened) return(ERR_NO_PROJECT);
    Saveflag = saveFlag;
    ERRCODE(quality_init());
    return errcode;
}

//=============================================================================

int  DLLEXPORT  ENMSXstep(long *t, long *tleft)
/*
**  Purpose:
**    updates the WQ simulation over a single time step.
**
**  Input:
**    none
**
**  Output:
**    *t = current simulation time at the end of the step (sec)
**    *tleft = time left in the simulation (sec)
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    if (!ProjectOpened) return(ERR_NO_PROJECT);
    return quality_step(t, tleft);
}

//=============================================================================
//   Functions for generating an output report
//=============================================================================

int  DLLEXPORT  ENMSXreport()
/*
**  Purpose:
**    writes requested WQ simulation results to a text file.
**
**  Input:
**    none
**
**  Returns:
**    an error code (or 0 for no error).
**
**  Notes:
**    Results are written to the EPANET report file unless a specific
**    water quality report file is named in the [REPORT] section of
**    the MSX input file.
*/
{
    if (!ProjectOpened) return(ERR_NO_PROJECT);
    if ( Rptflag ) return report_write();
    else return 0;
}

//=============================================================================

int  DLLEXPORT  ENMSXsaveoutfile(char *fname)
/*
**  Purpose:
**    saves all results of the WQ simulation to a binary file.
**
**  Input:
**    fname = name of the binary results file.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    FILE *f;
    int   c;

    if (!ProjectOpened) return(ERR_NO_PROJECT);
    if ( !OutFile.file ) return 511;
    if ( (f = fopen(fname,"w+b") ) == NULL) return 511;
    fseek(OutFile.file, 0, SEEK_SET);
    while ( (c = fgetc(OutFile.file)) != EOF) fputc(c, f);
    fclose(f);
    return 0;
}

//=============================================================================
//   Functions for retrieving network information
//=============================================================================

int  DLLEXPORT  ENMSXgetcount(int code, int *count)
/*
**  Purpose: retrieves the number of components of a
**           given type in the network
**
**  Input:   code = component code (see EPANETMSX.H)
**
**  Output:  *count = number of components in network
**
**  Returns: error code
*/
{
    *count = 0;
    if (!ProjectOpened) return(ERR_NO_PROJECT);
    switch (code)
    {
    case ENMSX_SPECIESCOUNT:    *count = Nobjects[SPECIE];    break;
    case ENMSX_TERMCOUNT:       *count = Nobjects[TERM];    break;
    case ENMSX_PARAMCOUNT:      *count = Nobjects[PARAMETER];    break;
    case ENMSX_CONSTCOUNT:      *count = Nobjects[CONSTANT];    break;
    case ENMSX_PATCOUNT:        *count = Nobjects[TIME_PATTERN];    break;
    default: return(ERR_PARAM_CODE);
    }
    return(0);
}

//=============================================================================

int  DLLEXPORT  ENMSXgeterror(int errcode, char *errmsg, int n)
/*
**  Purpose:
**    retrieves text of an error message.
**
**  Input:
**    errcode = error code number
**    n = maximum length of string errmsg.
**
**  Output:
**    errmsg  = text of error message.
**
**  Returns:
**    an error code which is always 0.
*/
{
    strncpy(errmsg, project_getErrmsg(errcode), n);
    return 0;
}

//=============================================================================

int  DLLEXPORT  ENMSXgetspecieindex(char *id, int *index)
/*
**  Purpose:
**    retrieves the index of a named WQ species.
**
**  Input:
**    id = name of water quality species.
**
**  Output:
**    index = index (base 1) of the species in the list of all species.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    int i;
    if (!ProjectOpened) return(ERR_NO_PROJECT);
    i = project_findObject(SPECIE, id);
    *index = 0;
    if (i >= 1) *index = i;
    return 0;
}

//=============================================================================

int  DLLEXPORT  ENMSXgetspecieID(int index, char *id)
/*
**  Purpose:
**    retrieves the name of a WQ species given its index.
**
**  Input:
**    index = index (base 1) of the species in the list of all species.
**
**  Output:
**    id = name of the species.
**
**  Returns:
**    an error code (or 0 for no error).
**
**  Notes:
**    The character array id must be dimensioned to accept a string of
**    up to 16 characters.
*/
{
    if (!ProjectOpened) return(ERR_NO_PROJECT);
    if ( index >= 1 && index <= Nobjects[SPECIE] )
    {
        strcpy(id, Specie[index].id);
    }
    else strcpy(id, "");
    return 0;
}

//=============================================================================

int DLLEXPORT  ENMSXgetspecietype(int index, int *code)
/*
**  Purpose:
**    retrieves the type code of a WQ species.
**
**  Input:
**    index = index (base 1) of the species in the list of all species.
**
**  Output:
**    code = 0 (BULK) for a bulk flow specie, 1 (WALL) for an attached specie.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    if (!ProjectOpened) return(ERR_NO_PROJECT);
    if ( index >= 1 && index <= Nobjects[SPECIE] )
    {
        *code = Specie[index].type;
    }
    else *code = 0;
    return 0;
}

//=============================================================================

int  DLLEXPORT  ENMSXgetpatternindex(char *id, int *index)
/*----------------------------------------------------------------
**  Input:   id     = time pattern ID
**  Output:  *index = index of time pattern in list of patterns
**  Returns: error code
**  Purpose: retrieves index of time pattern with specific ID
**----------------------------------------------------------------
*/
{
    int i;
    *index = 0;
    if (!ProjectOpened) return(ERR_NO_PROJECT);
    for (i=1; i<=Nobjects[TIME_PATTERN]; i++)
    {
        if (strcmp(id, Pattern[i].id) == 0)
        {
            *index = i;
            return(0);
        }
    }
    *index = 0;
    return(ERR_TIME_PATTERN);
}

//=============================================================================

int DLLEXPORT ENMSXgetpatternid(int index, char *id)
/*----------------------------------------------------------------
**  Input:   index = index of time pattern
**  Output:  id    = pattern ID
**  Returns: error code
**  Purpose: retrieves ID of a time pattern with specific index
**
**  NOTE: 'id' must be able to hold MAXID characters
**----------------------------------------------------------------
*/
{
    strcpy(id,"");
    if (!ProjectOpened) return(ERR_NO_PROJECT);
    if (index < 1 || index > Nobjects[TIME_PATTERN]) return(ERR_TIME_PATTERN);
    strcpy(id,Pattern[index].id);
    return(0);
}

//=============================================================================

int DLLEXPORT ENMSXgetpatternlen(int index, int *len)
/*----------------------------------------------------------------
**  Input:   index = index of time pattern
**  Output:  *len  = pattern length (number of multipliers)
**  Returns: error code
**  Purpose: retrieves number of multipliers in a time pattern
**----------------------------------------------------------------
*/
{
    if (!ProjectOpened) return(ERR_NO_PROJECT);
    if (index < 1 || index > Nobjects[TIME_PATTERN]) return(ERR_TIME_PATTERN);
    *len = Pattern[index].length;
    return(0);
}

//=============================================================================

int DLLEXPORT ENMSXgetpatternvalue(int index, int period, double *value)
/*----------------------------------------------------------------
**  Input:   index  = index of time pattern
**           period = pattern time period
**  Output:  *value = pattern multiplier
**  Returns: error code
**  Purpose: retrieves multiplier for a specific time period
**           and pattern
**----------------------------------------------------------------
*/
{
    SnumList *p;
    int j;
    if (!ProjectOpened) return(ERR_NO_PROJECT);
    if (index < 1 || index > Nobjects[TIME_PATTERN]) return(ERR_TIME_PATTERN);
    if (period < 1 || period > Pattern[index].length) return(ERR_PARAM_CODE);
    p = Pattern[index].first;
    for ( j = 1; j < period; j++) {p=p->next;}
    *value = p->value;
    return(0);
}

//=============================================================================
//   Functions for retrieving node and link data
//=============================================================================

int  DLLEXPORT  ENMSXgetnodequal(int node, int specie, double *c)
/*
**  Purpose:
**    retrieves the current concentration of a species at a particular node
**    of the pipe network.
**
**  Input:
**    node = node index (base 1).
**    specie = specie index (base 1).
**
**  Output:
**    c = specie concentration.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    *c = 0.0;
    if (!ProjectOpened) return(ERR_NO_PROJECT);
    if ( node > 0 && node <= Nobjects[NODE] &&
         specie > 0 && specie <= Nobjects[SPECIE] )
    {
        *c = quality_getNodeQual(node, specie);
    }
    return 0;
}

//=============================================================================

int  DLLEXPORT  ENMSXgetlinkqual(int link, int specie, double *c)
/*
**  Purpose:
**    retrieves the current average concentration of a species in a particular
**    link of the pipe network.
**
**  Input:
**    link = link index (base 1).
**    specie = specie index (base 1).
**
**  Output:
**    c = specie concentration.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    *c = 0.0;
    if (!ProjectOpened) return(ERR_NO_PROJECT);
    if ( link > 0 && link <= Nobjects[LINK] &&
        specie > 0 && specie <= Nobjects[SPECIE] )
    {
        *c = quality_getLinkQual(link, specie);
    }
    return 0;
}

//=============================================================================

int  DLLEXPORT  ENMSXgetsourcequal(int node, int specie, int code, double *value)
/*
**  Purpose:
**    retrieves source properties associated with a water quality analysis.
**
**  Input:
**    node = node index (base 1).
**    specie = index of BULK water quality specie (base 1).
**    code = parameter code (see EPANETMSX.H)
**    value = value to set associated with parameter code
**      (see EPANETMSX.H for valid source type values)
**
**  Output:
**    Value associated with parameter code for the specified source.
**
**  Returns:
**    an error code (or 0 for no error).
**
**  Note:
**    If no source exists for node and specie, a new CONCEN source is created
**    with no pattern and zero strength.
*/
{
    Psource source=NULL;

    if (!ProjectOpened) return(ERR_NO_PROJECT);
    if ( node <= 0 || node > Nobjects[NODE] ||
        specie <= 0 || specie > Nobjects[SPECIE] )
        return(ERR_INDEX_VALUE);
    if ( Specie[specie].type != BULK ) return(ERR_INDEX_VALUE);

    switch (code)
    {
    case ENMSX_SOURCEQUAL:
    case ENMSX_SOURCEPAT:
    case ENMSX_SOURCETYPE:
        source = Node[node].sources;

        // --- find the source for this specie

        while ( source )
        {
            if ( source->specie == specie ) break;
            source = source->next;
        }
        if ( source == NULL ) return(ERR_INDEX_VALUE);

        // --- get source's properties

        if ( code == ENMSX_SOURCEQUAL ) *value = source->c0;
        else if ( code == ENMSX_SOURCEPAT ) *value = source->pat;
        else if ( code == ENMSX_SOURCETYPE ) *value = source->type;
        break;
    default: return(ERR_PARAM_CODE);
    }
    return 0;
}

//=============================================================================
//   Functions for modifying network data
//=============================================================================

int  DLLEXPORT  ENMSXsetnodequal(int node, int specie, int code, double value)
/*
**  Purpose:
**    sets node properties associated with a water quality analysis.
**
**  Input:
**    node = node index (base 1).
**    specie = index of BULK water quality specie (base 1).
**    code = parameter code (see EPANETMSX.H)
**    value = value to set associated with parameter code
**      (see EPANETMSX.H for valid source type values)
**
**  Output:
**    None.
**
**  Returns:
**    an error code (or 0 for no error).
**
*/
{
    if (!ProjectOpened) return(ERR_NO_PROJECT);
    if ( node <= 0 || node > Nobjects[NODE] ||
        specie <= 0 || specie > Nobjects[SPECIE] )
        return(ERR_INDEX_VALUE);
    if ( Specie[specie].type != BULK ) return(ERR_INDEX_VALUE);

    switch (code)
    {
    case ENMSX_INITQUAL:       
        Node[node].c0[specie] = value;    
        break;
    default: return(ERR_PARAM_CODE);
    }
    return 0;
}

//=============================================================================

int  DLLEXPORT  ENMSXsetlinkqual(int link, int specie, int code, double value)
/*
**  Purpose:
**    sets link properties associated with a water quality analysis.
**
**  Input:
**    link = link index (base 1).
**    specie = index of WALL water quality specie (base 1).
**    code = parameter code (see EPANETMSX.H)
**    value = value to set associated with parameter code
**      (see EPANETMSX.H for valid source type values)
**
**  Output:
**    None.
**
**  Returns:
**    an error code (or 0 for no error).
*/
{
    if (!ProjectOpened) return(ERR_NO_PROJECT);
    if ( link <= 0 || link > Nobjects[LINK] ||
        specie <= 0 || specie > Nobjects[SPECIE] )
        return(ERR_INDEX_VALUE);
    if ( Specie[specie].type != WALL ) return(ERR_INDEX_VALUE);

    switch (code)
    {
        // --- Should allow for setting link-specific parameter values
        //     Requires ENMSXgetparamid(), ENMSXgetparamindex()
    case ENMSX_INITQUAL:       
        Link[link].c0[specie] = value;    
        break;
    default: return(ERR_PARAM_CODE);
    }
    return 0;
}

//=============================================================================
int  DLLEXPORT  ENMSXsetsourcequal(int node, int specie, int code, double value)
/*
**  Purpose:
**    sets source properties associated with a water quality analysis.
**
**  Input:
**    node = node index (base 1).
**    specie = index of BULK water quality specie (base 1).
**    code = parameter code (see EPANETMSX.H)
**    value = value to set associated with parameter code
**      (see EPANETMSX.H for valid source type values)
**
**  Output:
**    None.
**
**  Returns:
**    an error code (or 0 for no error).
**
**  Note:
**    If no source exists for node and specie, a new CONCEN source is created
**    with no pattern and zero strength.
*/
{
    Psource source=NULL;

    if (!ProjectOpened) return(ERR_NO_PROJECT);
    if ( node <= 0 || node > Nobjects[NODE] ||
        specie <= 0 || specie > Nobjects[SPECIE] )
        return(ERR_INDEX_VALUE);
    if ( Specie[specie].type != BULK ) return(ERR_INDEX_VALUE);

    switch (code)
    {
    case ENMSX_SOURCEQUAL:
    case ENMSX_SOURCEPAT:
    case ENMSX_SOURCETYPE:
        source = Node[node].sources;

        // --- check if a source for this specie already exists

        while ( source )
        {
            if ( source->specie == specie ) break;
            source = source->next;
        }

        // --- otherwise create a new source object
        //     default is concen source with zero strength and no pattern

        if ( source == NULL )
        {
            source = (struct Ssource *) malloc(sizeof(struct Ssource));
            if ( source == NULL ) return 101;
            source->next = Node[node].sources;
            Node[node].sources = source;
            
            source->c0 = 0.0;
            source->pat = 0;
            source->specie = specie;
            source->type = CONCEN;
        }

        // --- save source's properties

        if ( code == ENMSX_SOURCEQUAL ) source->c0 = value;
        else if ( code == ENMSX_SOURCEPAT )  {
            if ( (int)value < 1 || (int)value > Nobjects[TIME_PATTERN] ) return(ERR_INDEX_VALUE);
            source->pat = (int)value;
        }
        else if ( code == ENMSX_SOURCETYPE ) {
            if ( (int)value < CONCEN || (int)value > FLOWPACED ) return(ERR_PARAM_CODE);
            source->type = (int)value;
        }
        break;
    default: return(ERR_PARAM_CODE);
    }
    return 0;
}

int  DLLEXPORT  ENMSXsetpattern(int index, double *f, int n)
/*----------------------------------------------------------------
**   Input:   index = time pattern index
**            *f    = array of pattern multipliers
**            n     = number of time periods in pattern
**   Output:  none
**   Returns: error code
**   Purpose: sets multipliers for a specific time pattern
**----------------------------------------------------------------
*/
{
    int k = 0;
    SnumList *listItem;

    /* Check for valid arguments */
    if (!ProjectOpened) return(ERR_NO_PROJECT);
    if (index <= 0 || index > Nobjects[TIME_PATTERN]) return(ERR_TIME_PATTERN);
    if (n <= 0) return(ERR_NUM_VALUE);

    /* Free memory used by pattern values */
    listItem = Pattern[index].first;
    while (listItem)
    {
        Pattern[index].first = listItem->next;
        free(listItem);
        listItem = Pattern[index].first;
    }

    /* Re-set number of time periods & load new pattern values */
    Pattern[index].length = n;
    listItem = (SnumList *) malloc(sizeof(SnumList));
    if ( listItem == NULL ) return 101;
    listItem->value = f[k++];
    listItem->next = NULL;
    Pattern[index].current = listItem;
    Pattern[index].first = listItem;
    while ( k < n )
    {
        listItem = (SnumList *) malloc(sizeof(SnumList));
        if ( listItem == NULL ) return 101;
        listItem->value = f[k++];
        listItem->next = NULL;
        Pattern[index].current->next = listItem;
        Pattern[index].current = listItem;
    }
    return(0);
}

//=============================================================================

int  DLLEXPORT  ENMSXsetpatternvalue(int index, int period, double value)
/*----------------------------------------------------------------
**  Input:   index  = time pattern index
**           period = time pattern period
**           value  = pattern multiplier
**  Output:  none
**  Returns: error code
**  Purpose: sets multiplier for a specific time period and pattern
**----------------------------------------------------------------
*/
{
    SnumList *p;
    int j;

    if (!ProjectOpened) return(ERR_NO_PROJECT);
    if (index < 1 || index > Nobjects[TIME_PATTERN]) return(ERR_TIME_PATTERN);
    if (period < 1 || period > Pattern[index].length) return(ERR_PARAM_CODE);
    p = Pattern[index].first;
    for ( j = 1; j < period; j++) {p=p->next;}
    p->value = value;
    return(0);
}

//=============================================================================
//   Functions for creating new network data
//=============================================================================

int  DLLEXPORT  ENMSXcreatepattern(char *id)
/*----------------------------------------------------------------
**  Input:   id     = new time pattern ID
**  Output:  none
**  Returns: error code                              
**  Purpose: Creates new time pattern with PatternID *id.
**           New patterns are indexed sequentially with length
**           0.
**----------------------------------------------------------------
*/
{
    int i;
    if (!ProjectOpened) return(ERR_NO_PROJECT);
    if ( addPattern( id ) ) return(ERR_PARAM_CODE);  // Increments Nobjects[TIME_PATTERN]
    Pattern = (Spattern *) realloc(Pattern, (Nobjects[TIME_PATTERN]+1)*sizeof(Spattern));
    if (Pattern == NULL) return(101);
    i = Nobjects[TIME_PATTERN];
    Pattern[i].current = NULL;
    Pattern[i].first = NULL;
    Pattern[i].length = 0;
	Pattern[i].id = project_findID(TIME_PATTERN, id);
    return(0);
}
