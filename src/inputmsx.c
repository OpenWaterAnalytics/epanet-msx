/*******************************************************************************
**  TITLE:         INPUTMSX.C
**  DESCRIPTION:   Input data processor for the EPANET Multi-Species Extension
**                 toolkit.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**  VERSION:       1.00
**  LAST UPDATE:   10/5/06
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "typesmsx.h"
#define  EXTERN_MSX extern
#include "globals.h"
#include "utils.h"
#include "epanet2.h"

//  Constants
//-----------
#define MAXERRS 100                    // Max. input errors reported
#define MAXTOKS  40                    // Max. items per line of input

//  Local variables
//-----------------
static char *Tok[MAXTOKS];             // String tokens from line of input
static int  Ntokens;                   // Number of tokens in line of input

enum InpErrorCodes {                   // Error codes (401 - 409)
    INP_ERR_FIRST        = 400,
    ERR_LINE_LENGTH,
    ERR_ITEMS,
    ERR_KEYWORD,
    ERR_NUMBER,
    ERR_NAME,
    ERR_RESERVED_NAME,
    ERR_DUP_NAME,
    ERR_DUP_EXPR,
    ERR_MATH_EXPR,
    INP_ERR_LAST};

static char  *InpErrorTxt[INP_ERR_LAST-INP_ERR_FIRST] = {
    "",
    "Error 401 (too many characters)",
    "Error 402 (too few input items)",
    "Error 403 (invalid keyword)",
    "Error 404 (invalid numeric value)",
    "Error 405 (reference to undefined object)",
    "Error 406 (illegal use of a reserved name)",
    "Error 407 (name already used by another object)",
    "Error 408 (specie already assigned an expression)",
    "Error 409 (illegal math expression)"};


// NOTE: the entries in MSsectWords must match the entries in the enumeration
//       variable SectionType defined in typesmsx.h.
static char *MSsectWords[]     = {"[TITLE", "[SPECIE",  "[COEFF",  "[TERM",
                                  "[PIPE",  "[TANK",    "[SOURCE", "[QUALITY",
                                  "[PARAM", "[PATTERN", "[OPTION",
                                  "[REPORT", NULL};
static char *ReportWords[]     = {"NODE", "LINK", "SPECIE", "FILE", "PAGESIZE", NULL};
static char *OptionTypeWords[] = {"AREA_UNITS", "RATE_UNITS", "SOLVER",
                                  "TIMESTEP", "RTOL", "ATOL", NULL};
static char *SourceTypeWords[] = {"CONC", "MASS", "FLOW", "SETPOINT", NULL};
static char *MixingTypeWords[] = {"MIXED", "2COMP", "FIFO", "LIFO", NULL};
static char *MassUnitsWords[]  = {"MG", "UG", "MMOL", "COUNT", NULL};
static char *AreaUnitsWords[]  = {"FT2", "M2", "CM2", NULL};
static char *TimeUnitsWords[]  = {"SEC", "MIN", "HR", "DAY", NULL};
static char *SolverTypeWords[] = {"EUL", "RK5", "ROS2", NULL};
static char *ExprTypeWords[]   = {"", "RATE", "FORMULA", "EQUIL", NULL};
static char *HydVarWords[]     = {"", "D", "Q", "U", "Re",
                                  "Us", "Ff", "Av", NULL};
static char YES[] = "YES";
static char NO[]  = "NO";

//  Imported functions
//--------------------
int    project_addObject(int type, char *id, int n);
int    project_findObject(int type, char *id);
char * project_findID(int type, char *id);

//  Exported functions
//--------------------
int    input_countMSobjects(void);
int    input_countENobjects(void);
int    input_readENdata(void);
int    input_readMSdata(void);
void   input_getSpecieUnits(int m, char *units);
int    getNewSection(char *tok, char *sectWords[], int *sect);
int    addPattern(char *id);

//  Local functions
//-----------------
static int    getLineLength(char *line);
static int    addSpecie(char *line);
static int    addCoeff(char *line);
static int    addTerm(char *id);
static int    checkID(char *id);
static int    parseLine(int sect, char *line);
static int    parseOption(void);
static int    parseSpecie(void);
static int    parseCoeff(void);
static int    parseTerm(void);
static int    parseExpression(int classType);
static int    parseTankData(void);
static int    parseQuality(void);
static int    parseParameter(void);
static int    parseSource(void);
static int    parsePattern(void);
static int    parseReport(void);
static int    getVariableCode(char *id);
static int    getTokens(char *s);
static void   writeInpErrMsg(int errcode, char *sect, char *line, int lineCount);

//=============================================================================

int input_countMSobjects()
/*
**  Purpose:
**    reads multi-species input file to determine number of system objects.
**
**  Input:
**    none
**
**  Returns:
**    an error code (0 if no error)
*/
{
    char  line[MAXLINE+1];             // line from input data file
    char  wLine[MAXLINE+1];            // working copy of input line
    char  *tok;                        // first string token of line
    int   sect = -1;                   // input data sections
    int   errcode = 0;                 // error code
    int   errsum = 0;                  // number of errors found
    long  lineCount = 0;

// --- write name of input file to EPANET report file

    strcpy(Msg, "Processing MSX input file ");
    strcpy(line, InpFile.name);
    strcat(Msg, line);
    ENwriteline(Msg);
    ENwriteline("");

// --- make pass through EPANET-MSX data file counting number of each object

    while ( fgets(line, MAXLINE, InpFile.file) != NULL )
    {

    // --- skip blank lines & those beginning with a comment

        errcode = 0;
        lineCount++;
        strcpy(wLine, line);           // make working copy of line
        tok = strtok(wLine, SEPSTR);   // get first text token on line
        if ( tok == NULL || *tok == ';' ) continue;
        if ( getNewSection(tok, MSsectWords, &sect) ) continue;

    // --- read id names from SPECIES, COEFFS, TERMS, & PATTERNS sections

        if ( sect == s_SPECIE )  errcode = addSpecie(line);
        if ( sect == s_COEFF )   errcode = addCoeff(line);
        if ( sect == s_TERM )    errcode = addTerm(tok);
        if ( sect == s_PATTERN ) errcode = addPattern(tok);

    // --- report any error found

        if ( errcode )
        {
            writeInpErrMsg(errcode, MSsectWords[sect], line, lineCount);
            errsum++;
            if (errsum >= MAXERRS ) break;
        }
    }

// --- return error code

    if ( errsum > 0 ) return 200;
    return errcode;
}

//=============================================================================

int  input_countENobjects()
/*
**  Purpose:
**    queries EPANET data base to determine number of network objects.
**
**  Input:
**    none
**
**  Returns:
**    an error code (0 if no error)
*/
{
    int errcode = 0;

// --- retrieve number of network elements

    ERRCODE(ENgetcount(EN_NODECOUNT, &Nobjects[NODE]));
    ERRCODE(ENgetcount(EN_TANKCOUNT, &Nobjects[TANK]));
    ERRCODE(ENgetcount(EN_LINKCOUNT, &Nobjects[LINK]));
    return errcode;
}

//=============================================================================

int input_readENdata()
/*
**  Purpose:
**    retrieves required input data from the EPANET project data.
**
**  Input:
**    none
**
**  Returns:
**    an error code (0 if no error)
*/
{
    int errcode = 0;
	int i, k, n, t = 0;
    int n1 = 0, n2 = 0;
    float diam = 0.0, len = 0.0, v0 = 0.0;

// --- get flow units & time parameters

    ERRCODE(ENgetflowunits(&Flowflag));
    if ( Flowflag >= EN_LPS ) Unitsflag = SI;
    else                      Unitsflag = US;
    ERRCODE(ENgettimeparam(EN_QUALSTEP, &Qstep));
    ERRCODE(ENgettimeparam(EN_REPORTSTEP, &Rstep));
    ERRCODE(ENgettimeparam(EN_REPORTSTART, &Rstart));
    ERRCODE(ENgettimeparam(EN_PATTERNSTEP, &Pstep));
    ERRCODE(ENgettimeparam(EN_PATTERNSTART, &Pstart));

// --- read tank/reservoir data

    n = Nobjects[NODE] - Nobjects[TANK];
    for (i=1; i<=Nobjects[NODE]; i++)
    {
        k = i - n;
        if ( k > 0 )
        {
            ERRCODE(ENgetnodetype(i, &t));
            ERRCODE(ENgetnodevalue(i, EN_INITVOLUME, &v0));
            if ( !errcode )
            {
                Node[i].tank = k;
                Tank[k].node = i;
                if ( t == EN_RESERVOIR ) Tank[k].a = 0.0;
                else                     Tank[k].a = 1.0;
                Tank[k].mixModel = MIX1;
                Tank[k].v0 = v0;
            }
        }
    }

// --- read link data

    for (i=1; i<=Nobjects[LINK]; i++)
    {
        ERRCODE(ENgetlinknodes(i, &n1, &n2));
        ERRCODE(ENgetlinkvalue(i, EN_DIAMETER, &diam));
        ERRCODE(ENgetlinkvalue(i, EN_LENGTH, &len));
        if ( !errcode )
        {
            Link[i].n1 = n1;
            Link[i].n2 = n2;
            Link[i].diam = diam;
            Link[i].len = len;
        }
    }
    return errcode;
}

//=============================================================================

int  input_readMSdata()
/*
**  Purpose:
**    reads multi-species data from the EPANET-MSX input file.
**
**  Input:
**    none
**
**  Returns:
**    an error code (0 if no error)
*/
{
    char  line[MAXLINE+1];             // line from input data file
    char  wLine[MAXLINE+1];            // working copy of input line
    int   sect = -1;                   // input data sections
    int   errsum = 0;                  // number of errors found
    int   inperr = 0;                  // input error code
    long  lineCount = 0;               // line count

// --- read each line from MSX input file

    rewind(InpFile.file);
    while ( fgets(line, MAXLINE, InpFile.file) != NULL )
    {
    // --- make copy of line and scan for tokens

        lineCount++;
        strcpy(wLine, line);
        Ntokens = getTokens(wLine);

    // --- skip blank lines and comments

        if ( Ntokens == 0 || *Tok[0] == ';' ) continue;

    // --- check if max. line length exceeded

        if ( getLineLength(line) >= MAXLINE )
        {
            inperr = ERR_LINE_LENGTH;
            writeInpErrMsg(inperr, MSsectWords[sect], line, lineCount);
            errsum++;
        }

    // --- check if at start of a new input section

        if ( getNewSection(Tok[0], MSsectWords, &sect) ) continue;

    // --- parse tokens from input line

        inperr = parseLine(sect, line);
        if ( inperr > 0 )
        {
            errsum++;
            writeInpErrMsg(inperr, MSsectWords[sect], line, lineCount);
        }

    // --- stop if reach end of file or max. error count

        if (errsum >= MAXERRS) break;
    }   // End of while

// --- check for errors

    if (errsum > 0) return 200;
    return 0;
}

//=============================================================================

void   input_getSpecieUnits(int m, char *units)
/*
**  Purpose:
**    constructs the character string for a specie's concentration units.
**
**  Input:
**    m = specie index
**
**  Output:
**    units = character string with specie's concentration units
*/
{
    strcpy(units, MassUnitsWords[Specie[m].units]);
    strcat(units, "/");
    if ( Specie[m].type == BULK ) strcat(units, "L");
    else strcat(units, AreaUnitsWords[AreaUnits]);
}

//=============================================================================

int  getLineLength(char *line)
/*
**  Purpose:
**    determines number of characters of data in a line of input.
**
**  Input:
**    line = line of text from an input file
**
**  Returns:
**    number of characters in the line (with comment ignored).
*/
{
    char *comment;
    int lineLength = strlen(line);
    if ( lineLength >= MAXLINE )
    {
    // --- don't count comment if present
        comment = strchr(line, ';');
        if ( comment ) lineLength = comment - line;    // Pointer math here
    }
    return lineLength;
}

//=============================================================================

int  getNewSection(char *tok, char *sectWords[], int *sect)
/*
**  Purpose:
**    checks if a line begins a new section in the input file.
**
**  Input:
**    tok = a string token
**    sectWords = array of input file section keywords
**
**  Output:
**    sect = index code of section matching tok (or -1 if no match)
**
**  Returns:
**    1 if a section is found, 0 if not
*/
{
	int newsect;

// --- check if line begins with a new section heading

    if ( *tok == '[' )
    {
    // --- look for section heading in list of section keywords

        newsect = findmatch(tok, sectWords);
        if ( newsect >= 0 ) *sect = newsect;
        else *sect = -1;
        return 1;
    }
    return 0;
}

//=============================================================================

int addSpecie(char *line)
/*
**  Purpose:
**    adds a specie ID name to the project.
**
**  Input:
**    line = line of input data
**
**  Returns:
**    an error code (0 if no error)
*/
{
    int errcode = 0;
    Ntokens = getTokens(line);
    if ( Ntokens < 2 ) return ERR_ITEMS;
    errcode = checkID(Tok[1]);
    if ( errcode ) return errcode;
    if ( project_addObject(SPECIE, Tok[1], Nobjects[SPECIE]+1) < 0 )
        errcode = 101;
    else Nobjects[SPECIE]++;
    return errcode;
}

//=============================================================================

int addCoeff(char *line)
/*
**  Purpose:
**    adds a coefficient ID name to the project.
**
**  Input:
**    line = line of input data
**
**  Returns:
**    an error code (0 if no error)
*/
{
    int k;
    int errcode = 0;

// --- determine the type of coeff.

    Ntokens = getTokens(line);
    if ( Ntokens < 2 ) return ERR_ITEMS;
    if      (match(Tok[0], "PARAM")) k = PARAMETER;
    else if (match(Tok[0], "CONST")) k = CONSTANT;
    else return ERR_KEYWORD;

// --- check for valid id name

    errcode = checkID(Tok[1]);
    if ( errcode ) return errcode;
    if ( project_addObject(k, Tok[1], Nobjects[k]+1) < 0 )
        errcode = 101;
    else Nobjects[k]++;
    return errcode;
}

//=============================================================================

int addTerm(char *id)
/*
**  Purpose:
**    adds an intermediate expression term ID name to the project.
**
**  Input:
**    id = name of an intermediate expression term
**
**  Returns:
**    an error code (0 if no error)
*/
{
    int errcode = checkID(id);
    if ( !errcode )
    {
        if ( project_addObject(TERM, id, Nobjects[TERM]+1) < 0 )
            errcode = 101;
        else Nobjects[TERM]++;
    }
    return errcode;
}

//=============================================================================

int addPattern(char *id)
/*
**  Purpose:
**    adds a time pattern ID name to the project.
**
**  Input:
**    id = name of a time pattern
**
**  Returns:
**    an error code (0 if no error)
*/
{
    int errcode = 0;

    // --- a time pattern can span several lines

    if ( project_findObject(TIME_PATTERN, id) <= 0 )
    {
        if ( project_addObject(TIME_PATTERN, id, Nobjects[TIME_PATTERN]+1) < 0 )
            errcode = 101;
        else Nobjects[TIME_PATTERN]++;
    }
    return errcode;
}

//=============================================================================

int checkID(char *id)
/*
**  Purpose:
**    checks that an object's name is unique
**
**  Input:
**    id = name of an object
**
**  Returns:
**    an error code (0 if successful)
*/
{
// --- check that id name is not a reserved word
   int i = 1;
   while (HydVarWords[i] != NULL)
   {
      if (strcomp(id, HydVarWords[i])) return ERR_RESERVED_NAME;
      i++;
   }

// --- check that id name not used before

    if ( project_findObject(SPECIE, id) > 0 ||
         project_findObject(TERM, id)   > 0 ||
         project_findObject(PARAMETER, id)  > 0 ||
         project_findObject(CONSTANT, id)  > 0
       ) return ERR_DUP_NAME;
    return 0;
}

//=============================================================================

int parseLine(int sect, char *line)
/*
**  Purpose:
**    parses the contents of a line of input data.
**
**  Input:
**    sect = index of current input data section
**    line = contents of current line of input data
**
**  Returns:
**    an error code (0 if no error)
*/
{
    switch(sect)
    {
      case s_TITLE:
        strcpy(Title, line);
        break;

      case s_OPTION:
        return parseOption();

      case s_SPECIE:
        return parseSpecie();

      case s_COEFF:
        return parseCoeff();

      case s_TERM:
        return parseTerm();

      case s_PIPE:
        return parseExpression(LINK);

      case s_TANK:
        return parseExpression(TANK);

      case s_SOURCE:
        return parseSource();

      case s_QUALITY:
        return parseQuality();

      case s_PARAMETER:
        return parseParameter();

      case s_PATTERN:
        return parsePattern();

      case s_REPORT:
        return parseReport();
    }
    return 0;
}

//=============================================================================

int parseOption()
/*
**  Purpose:
**    parses an input line containing a project option.
**
**  Input:
**    none
**
**  Returns:
**    an error code (0 if no error)
*/
{
    int k;

// --- determine which option is being read

    if ( Ntokens < 2 ) return 0;
    k = findmatch(Tok[0], OptionTypeWords);
    if ( k < 0 ) return ERR_KEYWORD;

// --- parse the value for the given option

    switch ( k )
    {
      case AREA_UNITS_OPTION:
          k = findmatch(Tok[1], AreaUnitsWords);
          if ( k < 0 ) return ERR_KEYWORD;
          AreaUnits = k;
          break;

      case RATE_UNITS_OPTION:
          k = findmatch(Tok[1], TimeUnitsWords);
          if ( k < 0 ) return ERR_KEYWORD;
          RateUnits = k;
          break;

      case SOLVER_OPTION:
          k = findmatch(Tok[1], SolverTypeWords);
          if ( k < 0 ) return ERR_KEYWORD;
          Solver = k;
          break;

      case TIMESTEP_OPTION:
          k = atoi(Tok[1]);
          if ( k <= 0 ) return ERR_NUMBER;
          Qstep = k;
          break;

      case RTOL_OPTION:
          if ( !getDouble(Tok[1], &DefRtol) ) return ERR_NUMBER;
          break;

      case ATOL_OPTION:
          if ( !getDouble(Tok[1], &DefAtol) ) return ERR_NUMBER;
          break;
    }
    return 0;
}

//=============================================================================

int parseSpecie()
/*
**  Purpose:
**    parses an input line containing a specie variable.
**
**  Input:
**    none
**
**  Returns:
**    an error code (0 if no error)
*/
{
    int i, m;

// --- get Specie's index

    if ( Ntokens < 3 ) return ERR_ITEMS;
    i = project_findObject(SPECIE, Tok[1]);
    if ( i <= 0 ) return ERR_NAME;

// --- get pointer to Specie's name

    Specie[i].id = project_findID(SPECIE, Tok[1]);

// --- get Specie's type

    if      ( match(Tok[0], "BULK") ) Specie[i].type = BULK;
    else if ( match(Tok[0], "WALL") ) Specie[i].type = WALL;
    else return ERR_KEYWORD;

// --- get Specie's units

    m = findmatch(Tok[2], MassUnitsWords);
    if ( m < 0 ) return ERR_KEYWORD;
    Specie[i].units = m;

// --- get Specie's error tolerance

    Specie[i].aTol = 0.0;
    Specie[i].rTol = 0.0;
    if ( Ntokens >= 4)
    {
        if ( !getDouble(Tok[3], &Specie[i].aTol) ) return ERR_NUMBER;
    }
    if ( Ntokens >= 5)
    {
        if ( !getDouble(Tok[4], &Specie[i].rTol) ) return ERR_NUMBER;
    }
    return 0;
}

//=============================================================================

int parseCoeff()
/*
**  Purpose:
**    parses an input line containing a coefficient definition.
**
**  Input:
**    none
**
**  Returns:
**    an error code (0 if no error)
*/
{
    int i, j;
    double x;

// --- check if variable is a Parameter

    if ( Ntokens < 2 ) return 0;
    if ( match(Tok[0], "PARAM") )
    {
    // --- get Parameter's index

        i = project_findObject(PARAMETER, Tok[1]);
        if ( i <= 0 ) return ERR_NAME;

    // --- get Parameter's value

        Param[i].id = project_findID(PARAMETER, Tok[1]);
        if ( Ntokens >= 3 )
        {
            if ( !getDouble(Tok[2], &x) ) return ERR_NUMBER;
            for (j=1; j<=Nobjects[LINK]; j++) Link[j].param[i] = x;
            for (j=1; j<=Nobjects[TANK]; j++) Tank[j].param[i] = x;
        }
        return 0;
    }

// --- check if variable is a Constant

    else if ( match(Tok[0], "CONST") )
    {
    // --- get Constant's index

        i = project_findObject(CONSTANT, Tok[1]);
        if ( i <= 0 ) return ERR_NAME;

    // --- get Constant's value

        Const[i].id = project_findID(CONSTANT, Tok[1]);
        Const[i].value = 0.0;
        if ( Ntokens >= 3 )
        {
            if ( !getDouble(Tok[2], &Const[i].value) ) return ERR_NUMBER;
        }
        return 0;
    }
    else return  ERR_KEYWORD;
}

//=============================================================================

int parseTerm()
/*
**  Purpose:
**    parses an input line containing an intermediate expression term .
**
**  Input:
**    none
**
**  Returns:
**    an error code (0 if no error)
*/
{
    int i, j;
    char s[MAXLINE+1] = "";
    MathExpr *expr;

// --- get term's name

    if ( Ntokens < 2 ) return 0;
    i = project_findObject(TERM, Tok[0]);

// --- reconstruct the expression string from its tokens

    for (j=1; j<Ntokens; j++) strcat(s, Tok[j]);

// --- convert expression into a postfix stack of op codes

    expr = mathexpr_create(s, getVariableCode);
    if ( expr == NULL ) return ERR_MATH_EXPR;

// --- assign the expression to a Term object

    Term[i].expr = expr;
    return 0;
}

//=============================================================================

int parseExpression(int classType)
/*
**  Purpose:
**    parses an input line containing a math expression.
**
**  Input:
**    classType = either LINK or TANK
**
**  Returns:
**    an error code (0 if  no error)
*/
{
    int i, j, k;
    char s[MAXLINE+1] = "";
    MathExpr *expr;

// --- determine expression type

    if ( Ntokens < 3 ) return ERR_ITEMS;
    k = findmatch(Tok[0], ExprTypeWords);
    if ( k < 0 ) return ERR_KEYWORD;

// --- determine specie associated with expression

    i = project_findObject(SPECIE, Tok[1]);
    if ( i < 1 ) return ERR_NAME;

// --- check that specie does not already have an expression

    if ( classType == LINK )
    {
        if ( Specie[i].pipeExprType != NO_EXPR ) return ERR_DUP_EXPR;
    }
    if ( classType == TANK )
    {
        if ( Specie[i].tankExprType != NO_EXPR ) return ERR_DUP_EXPR;
    }

// --- reconstruct the expression string from its tokens

    for (j=2; j<Ntokens; j++) strcat(s, Tok[j]);

// --- convert expression into a postfix stack of op codes

    expr = mathexpr_create(s, getVariableCode);
    if ( expr == NULL ) return ERR_MATH_EXPR;

// --- assign the expression to the specie

    switch (classType)
    {
    case LINK:
        Specie[i].pipeExpr = expr;
        Specie[i].pipeExprType = k;
        break;
    case TANK:
        Specie[i].tankExpr = expr;
        Specie[i].tankExprType = k;
        break;
    }
    return 0;
}

//=============================================================================

int parseQuality()
/*
**  Purpose:
**    parses an input line containing initial species concentrations.
**
**  Input:
**    none
**
**  Returns:
**    an error code (0 if no error)
*/
{
    int err, i, j, k, m;
    double x;

// --- determine if quality value is global or object-specific

    if ( Ntokens < 3 ) return ERR_ITEMS;
    if      ( match(Tok[0], "GLOBAL") ) i = 1;
    else if ( match(Tok[0], "NODE") )   i = 2;
    else if ( match(Tok[0], "LINK") )   i = 3;
    else return ERR_KEYWORD;

// --- find species index

    k = 1;
    if ( i >= 2 ) k = 2;
    m = project_findObject(SPECIE, Tok[k]);
    if ( m <= 0 ) return ERR_NAME;

// --- get quality value

    if ( i >= 2  && Ntokens < 4 ) return ERR_ITEMS;
    k = 2;
    if ( i >= 2 ) k = 3;
    if ( !getDouble(Tok[k], &x) ) return ERR_NUMBER;

// --- for global specification, set initial quality either for
//     all nodes or links depending on type of specie

    if ( i == 1)
    {
        if ( Specie[m].type == BULK )
        {
            for (j=1; j<=Nobjects[NODE]; j++) Node[j].c0[m] = x;
        }
        for (j=1; j<=Nobjects[LINK]; j++) Link[j].c0[m] = x;
    }

// --- for a specific node, get its index & set its initial quality

    else if ( i == 2 )
    {
        err = ENgetnodeindex(Tok[1], &j);
        if ( err ) return ERR_NAME;
        if ( Specie[m].type == BULK ) Node[j].c0[m] = x;
    }

// --- for a specific link, get its index & set its initial quality

    else if ( i == 3 )
    {
        err = ENgetlinkindex(Tok[1], &j);
        if ( err ) return ERR_NAME;
        Link[j].c0[m] = x;
    }
    return 0;
}

//=============================================================================

int parseParameter()
/*
**  Purpose:
**    parses an input line containing a parameter data.
**
**  Input:
**    none
**
**  Returns:
**    an error code (0 if no error)
*/
{
    int err, i, j;
    double x;

// --- get parameter name

    if ( Ntokens < 4 ) return 0;
    i = project_findObject(PARAMETER, Tok[2]);

// --- get parameter value

    if ( !getDouble(Tok[3], &x) ) return ERR_NUMBER;

// --- for pipe parameter, get pipe index and update parameter's value

    if ( match(Tok[0], "PIPE") )
    {
        err = ENgetlinkindex(Tok[1], &j);
        if ( err ) return ERR_NAME;
        Link[j].param[i] = x;
    }

// --- for tank parameter, get tank index and update parameter's value

    else if ( match(Tok[0], "TANK") )
    {
        err = ENgetnodeindex(Tok[1], &j);
        if ( err ) return ERR_NAME;
        j = Node[j].tank;
        if ( j > 0 ) Tank[j].param[i] = x;
    }
    else return ERR_KEYWORD;
    return 0;
}

//=============================================================================

int parseSource()
/*
**  Purpose:
**    parses an input line containing a source input data.
**
**  Input:
**    none
**
**  Returns:
**    an error code (0 if no error)
*/
{
    int err, i, j, k, m;
    double  x;
    Psource source;

// --- get source type

    if ( Ntokens < 4 ) return ERR_ITEMS;
    k = findmatch(Tok[0], SourceTypeWords);
    if ( k < 0 ) return ERR_KEYWORD;

// --- get node index

    err = ENgetnodeindex(Tok[1], &j);
    if ( err ) return ERR_NAME;

//  --- get specie index

    m = project_findObject(SPECIE, Tok[2]);
    if ( m <= 0 ) return ERR_NAME;

// --- check that specie is a BULK specie

    if ( Specie[m].type != BULK ) return 0;

// --- get base strength

    if ( !getDouble(Tok[3], &x) ) return ERR_NUMBER;

// --- get time pattern if present

    i = 0;
    if ( Ntokens >= 5 )
    {
        i = project_findObject(TIME_PATTERN, Tok[4]);
        if ( i <= 0 ) return ERR_NAME;
    }

// --- check if a source for this specie already exists

    source = Node[j].sources;
    while ( source )
    {
        if ( source->specie == m ) break;
        source = source->next;
    }

// --- otherwise create a new source object

    if ( source == NULL )
    {
        source = (struct Ssource *) malloc(sizeof(struct Ssource));
        if ( source == NULL ) return 101;
        source->next = Node[j].sources;
        Node[j].sources = source;
    }

// --- save source's properties

    source->type   = k;
    source->specie = m;
    source->c0     = x;
    source->pat    = i;
    return 0;
}

//=============================================================================

int parsePattern()
/*
**  Purpose:
**    parses an input line containing a time pattern data.
**
**  Input:
**    none
**
**  Returns:
**    an error code (0 if no error)
*/
{
	int i, k;
	double x;
    SnumList *listItem;

// --- get time pattern index

    if ( Ntokens < 2 ) return ERR_ITEMS;
    i = project_findObject(TIME_PATTERN, Tok[0]);
    if ( i <= 0 ) return ERR_NAME;
	Pattern[i].id = project_findID(TIME_PATTERN, Tok[0]);

// --- begin reading pattern multipliers

    k = 1;
    while ( k < Ntokens )
    {
        if ( !getDouble(Tok[k], &x) ) return ERR_NUMBER;
        listItem = (SnumList *) malloc(sizeof(SnumList));
        if ( listItem == NULL ) return 101;
        listItem->value = x;
        listItem->next = NULL;
        if ( Pattern[i].first == NULL )
        {
            Pattern[i].current = listItem;
            Pattern[i].first = listItem;
        }
        else
        {
            Pattern[i].current->next = listItem;
            Pattern[i].current = listItem;
        }
        Pattern[i].length++;
        k++;
    }
    return 0;
}

//=============================================================================

int parseReport()
{
    int  i, j, k, err;

// --- get keyword

    if ( Ntokens < 2 ) return 0;
    k = findmatch(Tok[0], ReportWords);
    if ( k < 0 ) return ERR_KEYWORD;
    switch(k)
    {

    // --- keyword is NODE; parse ID names of reported nodes

        case 0:
        for (i=1; i<Ntokens; i++)
        {
            err = ENgetnodeindex(Tok[i], &j);
            if ( err ) return ERR_NAME;
            Node[j].rpt = 1;
        }
        break;

    // --- keyword is LINK: parse ID names of reported links
        case 1:
        for (i=1; i<Ntokens; i++)
        {
            err = ENgetlinkindex(Tok[i], &j);
            if ( err ) return ERR_NAME;
            Link[j].rpt = 1;
        }
        break;

    // --- keyword is SPECIE; get YES/NO & precision

        case 2:
        j = project_findObject(SPECIE, Tok[1]);
        if ( j <= 0 ) return ERR_NAME;
        if ( Ntokens >= 3 )
        {
            if ( strcomp(Tok[2], YES) ) Specie[j].rpt = 1;
            else if ( strcomp(Tok[2], NO)  ) Specie[j].rpt = 0;
            else return ERR_KEYWORD;
        }
        if ( Ntokens >= 4 )
        {
            if ( !getInt(Tok[3], &Specie[j].precision) ) return ERR_NUMBER;
        }
        break;

    // --- keyword is FILE: get name of report file

        case 3:
        strcpy(RptFile.name, Tok[1]);
        break;

    // --- keyword is PAGESIZE;

        case 4:
        if ( !getInt(Tok[1], &PageSize) ) return ERR_NUMBER;
        break;
    }
    return 0;
}

//=============================================================================

int getVariableCode(char *id)
/*
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
**    and proceeding through each Specie, Term, Parameter and Constant.
*/
{
    int j = project_findObject(SPECIE, id);
    if ( j >= 1 ) return j;
    j = project_findObject(TERM, id);
    if ( j >= 1 ) return Nobjects[SPECIE] + j;
    j = project_findObject(PARAMETER, id);
    if ( j >= 1 ) return Nobjects[SPECIE] + Nobjects[TERM] + j;
    j = project_findObject(CONSTANT, id);
    if ( j >= 1 ) return Nobjects[SPECIE] + Nobjects[TERM] + Nobjects[PARAMETER] + j;
    j = findmatch(id, HydVarWords);
    if ( j >= 1 ) return Nobjects[SPECIE] + Nobjects[TERM] + Nobjects[PARAMETER] +
                         Nobjects[CONSTANT] + j;
    return -1;
}

//=============================================================================

int  getTokens(char *s)
/*
**  Purpose:
**    scans a string for tokens, saving pointers to them
**    in shared variable Tok[].
**
**  Input:
**    s = a character string
**
**  Returns:
**    number of tokens found in s
**
**  Notes:
**    Tokens can be separated by the characters listed in SEPSTR
**    (spaces, tabs, newline, carriage return) which is defined
**    in CONSTS.H. Text between quotes is treated as a single token.
*/
{
    int  len, m, n;
    char *c;

    // --- begin with no tokens

    for (n = 0; n < MAXTOKS; n++) Tok[n] = NULL;
    n = 0;

    // --- truncate s at start of comment

    c = strchr(s,';');
    if (c) *c = '\0';
    len = strlen(s);

    // --- scan s for tokens until nothing left

    while (len > 0 && n < MAXTOKS)
    {
        m = strcspn(s,SEPSTR);              // find token length
        if (m == 0) s++;                    // no token found
        else
        {
            if (*s == '"')                  // token begins with quote
            {
                s++;                        // start token after quote
                len--;                      // reduce length of s
                m = strcspn(s,"\"\n");      // find end quote or new line
            }
            s[m] = '\0';                    // null-terminate the token
            Tok[n] = s;                     // save pointer to token
            n++;                            // update token count
            s += m+1;                       // begin next token
        }
        len -= m+1;                         // update length of s
    }
    return(n);
}

//=============================================================================

void writeInpErrMsg(int errcode, char *sect, char *line, int lineCount)
{
    char msg[MAXMSG+1];
    if ( errcode >= INP_ERR_LAST  || errcode <= INP_ERR_FIRST )
    {
        sprintf(msg, "Error Code = %d", errcode);
    }
    else
    {
        sprintf(msg, "%s at line %d of %s] section:",
             InpErrorTxt[errcode-INP_ERR_FIRST], lineCount, sect);
    }
    ENwriteline("");
    ENwriteline(msg);
    ENwriteline(line);
}
