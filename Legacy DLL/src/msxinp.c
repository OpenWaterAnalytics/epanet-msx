/*******************************************************************************
**  MODULE:        MSXINP.C
**  PROJECT:       EPANET-MSX
**  DESCRIPTION:   Input data processor for the EPANET Multi-Species Extension
**                 toolkit.
**  COPYRIGHT:     Copyright (C) 2007 Feng Shang, Lewis Rossman, and James Uber.
**                 All Rights Reserved. See license information in LICENSE.TXT.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**  VERSION:       1.1.00
**  LAST UPDATE:   11/01/10
**  BUG FIX:	   BUG ID 09 (add roughness as a hydraulic variable) Feng Shang 01/29/2008	
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "mathexpr.h"
#include "msxutils.h"
#include "msxdict.h"
#include "epanet2.h"

#include "msxsetters.h"
#include "msxgetters.h"

//  Constants
//-----------
#define MAXERRS 100                    // Max. input errors reported
#define MAXTOKS  40                    // Max. items per line of input
#define SEPSTR  " \t\n\r"              // Token separator characters


//  Local variables
//-----------------
static char *Tok[MAXTOKS];             // String tokens from line of input
static int  Ntokens;                   // Number of tokens in line of input
static double **TermArray;             // Incidence array used to check Terms  //1.1.00

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
    "Error 408 (species already assigned an expression)", 
    "Error 409 (illegal math expression)"};

//  Imported functions
//--------------------
int    MSXproj_addObject(int type, char *id, int n);
int    MSXproj_findObject(int type, char *id);
char * MSXproj_findID(int type, char *id);

//  Exported functions
//--------------------
int    MSXinp_countMsxObjects(MSXproject *MSX);
int    MSXinp_countNetObjects(MSXproject *MSX);
int    MSXinp_readNetData(MSXproject *MSX);
int    MSXinp_readMsxData(MSXproject *MSX);
void   MSXinp_getSpeciesUnits(MSXproject *MSX, int m, char *units);

//  Local functions
//-----------------
static int    getLineLength(char *line);
static int    getNewSection(char *tok, char *sectWords[], int *sect);
static int    addSpecies(MSXproject *MSX, char *line);
static int    addCoeff(MSXproject *MSX, char *line);
static int    addTerm(MSXproject *MSX, char *id);
static int    addPattern(MSXproject *MSX, char *id);
static int    checkID(char *id);
static int    parseLine(MSXproject *MSX, int sect, char *line);
static int    parseOption(MSXproject *MSX);
static int    parseSpecies(MSXproject *MSX);
static int    parseCoeff(MSXproject *MSX);
static int    parseTerm(MSXproject *MSX);
static int    parseExpression(MSXproject *MSX, int classType);
static int    parseTankData(void);
static int    parseQuality(MSXproject *MSX);
static int    parseParameter(MSXproject *MSX);
static int    parseSource(MSXproject *MSX);
static int    parsePattern(MSXproject *MSX);
static int    parseReport(MSXproject *MSX);
static int    getVariableCode(MSXproject *MSX, char *id);
static int    getTokens(char *s);
static void   writeInpErrMsg(int errcode, char *sect, char *line, int lineCount);

static int    checkCyclicTerms(MSXproject *MSX);                                          //1.1.00
static int    traceTermPath(int i, int istar, int n);                          //1.1.00

//=============================================================================

int MSXinp_countMsxObjects(MSXproject *MSX)
/**
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
    
    strcpy(MSX->Msg, "Processing MSX input file ");
    strcpy(line, MSX->MsxFile.name);
    strcat(MSX->Msg, line);
    ENwriteline(MSX->Msg);
    ENwriteline("");

// --- make pass through EPANET-MSX data file counting number of each object

    while ( fgets(line, MAXLINE, MSX->MsxFile.file) != NULL )
    {

    // --- skip blank lines & those beginning with a comment

        errcode = 0;
        lineCount++;
        strcpy(wLine, line);           // make working copy of line
        tok = strtok(wLine, SEPSTR);   // get first text token on line
        if ( tok == NULL || *tok == ';' ) continue;
        if ( getNewSection(tok, MsxSectWords, &sect) ) continue;

    // --- read id names from SPECIES, COEFFS, TERMS, & PATTERNS sections

        if ( sect == s_SPECIES ) errcode = addSpecies(MSX, line);
        if ( sect == s_COEFF )   errcode = addCoeff(MSX, line);
        if ( sect == s_TERM )    errcode = addTerm(MSX, tok);
        if ( sect == s_PATTERN ) errcode = addPattern(MSX, tok);

    // --- report any error found

        if ( errcode )
        {
            writeInpErrMsg(errcode, MsxSectWords[sect], line, lineCount);
            errsum++;
            if (errsum >= MAXERRS ) break;
        }
    }

// --- return error code

    if ( errsum > 0 ) return ERR_MSX_INPUT;
    return errcode;
}

//=============================================================================

int  MSXinp_countNetObjects(MSXproject *MSX)
/**
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

    int Nobjects;
    CALL(errcode, ENgetcount(EN_NODECOUNT, &Nobjects));
    CALL(errcode, setNobjects(MSX, NODE, Nobjects));      //Set Nobjects in the MSXpoject struct
    CALL(errcode, ENgetcount(EN_TANKCOUNT, &Nobjects));
    CALL(errcode, setNobjects(MSX, TANK, Nobjects));      //Set Nobjects in the MSXpoject struct
    CALL(errcode, ENgetcount(EN_LINKCOUNT, &Nobjects));
    CALL(errcode, setNobjects(MSX, LINK, Nobjects));      //Set Nobjects in the MSXpoject struct
    return errcode;
}

//=============================================================================

int MSXinp_readNetData(MSXproject *MSX)
/**
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
    int   errcode = 0;
	int   i, k, n, t = 0;
    int   n1 = 0, n2 = 0;
    float diam = 0.0, len = 0.0, v0 = 0.0, xmix = 0.0, vmix = 0.0;

	float roughness = 0.0;   /*Feng Shang, Bug ID 8,  01/29/2008*/
// --- get flow units & time parameters

    // CALL(errcode, ENgetflowunits(MSX.Flowflag));
    int Flowflag;
    int Unitsflag;
    CALL(errcode, ENgetflowunits(&Flowflag));
    if ( Flowflag >= EN_LPS ) Unitsflag = SI;
    else                          Unitsflag = US;
    CALL(errcode, setFlowUnits(MSX, Flowflag, Unitsflag));     // Sets the Flow and Units in MSX struct
    int Qstep;
    CALL(errcode, ENgettimeparam(EN_QUALSTEP, &Qstep));
    CALL(errcode, setQstep(MSX, Qstep));       // Sets the Qstep in MSX data struct
    int Rstep;
    CALL(errcode, ENgettimeparam(EN_REPORTSTEP, &Rstep));
    CALL(errcode, setRstep(MSX, Rstep));       // Sets the Rstep in MSX data struct
    int Rstart;
    CALL(errcode, ENgettimeparam(EN_REPORTSTART, &Rstart));
    CALL(errcode, setRstart(MSX, Rstart));       // Sets the Rstart in MSX data struct
    int Pstep;
    CALL(errcode, ENgettimeparam(EN_PATTERNSTEP, &Pstep));
    CALL(errcode, setPstep(MSX, Pstep));       // Sets the Pstep in MSX data struct
    int Pstart;
    CALL(errcode, ENgettimeparam(EN_PATTERNSTART, &Pstart));
    CALL(errcode, setPstart(MSX, Pstart));       // Sets the Pstart in MSX data struct
    int Statflag;
    CALL(errcode, ENgettimeparam(EN_STATISTIC, &Statflag));
    CALL(errcode, setStatflag(MSX, Statflag));       // Sets the Statflag in MSX data struct


// --- read tank/reservoir data

    // Get number of node and tank objects
    int numNodeObjects;
    CALL(errcode, getNobjects(MSX, NODE, &numNodeObjects));
    int numTankObjects;
    CALL(errcode, getNobjects(MSX, TANK, &numTankObjects));

    n = numNodeObjects - numTankObjects;
    for (i=1; i<=numNodeObjects; i++)
    {
        k = i - n;
        if ( k > 0 )
        {
            CALL(errcode, ENgetnodetype(i, &t));
            CALL(errcode, ENgetnodevalue(i, EN_INITVOLUME, &v0));
            CALL(errcode, ENgetnodevalue(i, EN_MIXMODEL,   &xmix));
            CALL(errcode, ENgetnodevalue(i, EN_MIXZONEVOL, &vmix));
            if ( !errcode )
            {
                CALL(errcode, setNodeTank(MSX, i, k));
                CALL(errcode, setTankNode(MSX, k, i));
                if ( t == EN_RESERVOIR ) CALL(errcode, setTankArea(MSX, k, 0.0));
                else                     CALL(errcode, setTankArea(MSX, k, 1.0));
                CALL(errcode, setTankInitialVolume(MSX, k, v0));
                CALL(errcode, setTankMixModel(MSX, k, (int)xmix));
                CALL(errcode, setTankMixingSize(MSX, k, vmix));
            }
        }
    }

// --- read link data
    int numLinks;
    getNobjects(MSX, LINK, &numLinks);
    for (i=1; i<=numLinks; i++)
    {
        CALL(errcode, ENgetlinknodes(i, &n1, &n2));
        CALL(errcode, ENgetlinkvalue(i, EN_DIAMETER, &diam));
        CALL(errcode, ENgetlinkvalue(i, EN_LENGTH, &len));
        CALL(errcode, ENgetlinkvalue(i, EN_ROUGHNESS, &roughness));  /*Feng Shang, Bug ID 8,  01/29/2008*/
        if ( !errcode )
        {
            CALL(errcode, setLinkStartNode(MSX, i, n1));
            CALL(errcode, setLinkEndNode(MSX, i, n2));
            CALL(errcode, setLinkDiameter(MSX, i, diam));
            CALL(errcode, setLinkLength(MSX, i, len));
            CALL(errcode, setLinkRoughness(MSX, i, roughness));
        }
    }
    return errcode;
}


//=============================================================================

int  MSXinp_readMsxData(MSXproject *MSX)
/**
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

    int errcode = 0;

// --- create the TermArray for checking circular references in Terms          //1.1.00

    int n;
    CALL(errcode, getNobjects(MSX, TERM, &n));
	TermArray = createMatrix(n+1, n+1);
	if ( TermArray == NULL ) return ERR_MEMORY;                                //1.1.00

// --- read each line from MSX input file

    //TODO do something about the file right here
    rewind(MSX->MsxFile.file);
    while ( fgets(line, MAXLINE, MSX->MsxFile.file) != NULL )
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
            writeInpErrMsg(inperr, MsxSectWords[sect], line, lineCount);
            errsum++;
        }

    // --- check if at start of a new input section

        if ( getNewSection(Tok[0], MsxSectWords, &sect) ) continue;

    // --- parse tokens from input line

        inperr = parseLine(MSX, sect, line);
        if ( inperr > 0 )
        {
            errsum++;
            writeInpErrMsg(inperr, MsxSectWords[sect], line, lineCount);
        }

    // --- stop if reach end of file or max. error count

        if (errsum >= MAXERRS) break;
    }   // End of while

// --- check for errors

    if ( checkCyclicTerms(MSX) ) errsum++;                                        //1.1.00
    freeMatrix(TermArray);                                                     //1.1.00
    if (errsum > 0) return ERR_MSX_INPUT;                                      //1.1.00
    return errcode;
}

//=============================================================================

void   MSXinp_getSpeciesUnits(MSXproject *MSX, int m, char *units)
/**
**  Purpose:
**    constructs the character string for a species concentration units.
**
**  Input:
**    m = species index
**
**  Output:
**    units = character string with species concentration units
*/
{
    char * u;
    getSpeciesUnits(MSX, m, &u);
    strcpy(units, u);
    strcat(units, "/");
    int type;
    getSpeciesType(MSX, m, &type);
    if ( type == BULK ) strcat(units, "L");
    else {
        int AreaUnits;
        getAreaUnits(MSX, &AreaUnits);
        strcat(units, AreaUnitsWords[AreaUnits]);
    }
}

//=============================================================================

int  getLineLength(char *line)
/**
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
    int lineLength = (int)strlen(line);
    if ( lineLength >= MAXLINE )
    {
    // --- don't count comment if present
        comment = strchr(line, ';');
        if ( comment ) lineLength = (int)(comment - line);    // Pointer math here
    }
    return lineLength;
}

//=============================================================================

int  getNewSection(char *tok, char *sectWords[], int *sect)
/**
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

        newsect = MSXutils_findmatch(tok, sectWords);
        if ( newsect >= 0 ) *sect = newsect;
        else *sect = -1;
        return 1;
    }
    return 0;
}

//=============================================================================

int addSpecies(MSXproject *MSX, char *line)
/** 
**  Purpose:
**    adds a species ID name to the project.
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
    int numSpecies;
    CALL(errcode, getNobjects(MSX, SPECIES, &numSpecies));
    if ( MSXproj_addObject(SPECIES, Tok[1], numSpecies+1) < 0 )
        errcode = 101;
    else {
        CALL(errcode, setNobjects(MSX, SPECIES, numSpecies+1));
    }
    return errcode;
}

//=============================================================================

int addCoeff(MSXproject *MSX, char *line)
/**
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
    if      (MSXutils_match(Tok[0], "PARAM")) k = PARAMETER;
    else if (MSXutils_match(Tok[0], "CONST")) k = CONSTANT;
    else return ERR_KEYWORD;

// --- check for valid id name

    errcode = checkID(Tok[1]);
    if ( errcode ) return errcode;
    int Nobjects;
    CALL(errcode, getNobjects(MSX, k, &Nobjects));
    if ( MSXproj_addObject(k, Tok[1], Nobjects+1) < 0 )
        errcode = 101;
    else {
        CALL(errcode, setNobjects(MSX, k, Nobjects+1));
    }
    return errcode;
}

//=============================================================================

int addTerm(MSXproject *MSX, char *id)
/**
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
        int numTerms;
        CALL(errcode, getNobjects(MSX, TERM, &numTerms));
        if ( MSXproj_addObject(TERM, id, numTerms+1) < 0 )
            errcode = 101;
        else {
            CALL(errcode, setNobjects(MSX, TERM, numTerms+1));
        }
    }
    return errcode;
}

//=============================================================================

int addPattern(MSXproject *MSX, char *id)
/**
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

    if ( MSXproj_findObject(PATTERN, id) <= 0 )
    {
        int numPatterns;
        CALL(errcode, getNobjects(MSX, PATTERN, &numPatterns));
        if ( MSXproj_addObject(PATTERN, id, numPatterns+1) < 0 )
            errcode = 101;
        else {
            CALL(errcode, setNobjects(MSX, PATTERN, numPatterns+1));
        }
    }
    return errcode;
}

//=============================================================================

int checkID(char *id)
/**
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
      if (MSXutils_strcomp(id, HydVarWords[i])) return ERR_RESERVED_NAME;
      i++;
   }
    
// --- check that id name not used before

    if ( MSXproj_findObject(SPECIES, id)   > 0 ||
         MSXproj_findObject(TERM, id)      > 0 ||
         MSXproj_findObject(PARAMETER, id) > 0 ||
         MSXproj_findObject(CONSTANT, id)  > 0
       ) return ERR_DUP_NAME;
    return 0;
}        

//=============================================================================

int parseLine(MSXproject *MSX, int sect, char *line)
/**
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
        //TODO Does the project even need a title?
        strcpy(MSX->Title, line);
        break;

      case s_OPTION:
        return parseOption(MSX);

      case s_SPECIES:
        return parseSpecies(MSX);

      case s_COEFF:
        return parseCoeff(MSX);

      case s_TERM:
        return parseTerm(MSX);
      
      case s_PIPE:
        return parseExpression(MSX, LINK);

      case s_TANK:
        return parseExpression(MSX, TANK);

      case s_SOURCE:
        return parseSource(MSX);

      case s_QUALITY:
        return parseQuality(MSX);

      case s_PARAMETER:
        return parseParameter(MSX);

      case s_PATTERN:
        return parsePattern(MSX);

      case s_REPORT:
        return parseReport(MSX);
    }
    return 0;
}

//=============================================================================

int parseOption(MSXproject *MSX)
/**
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
    k = MSXutils_findmatch(Tok[0], OptionTypeWords);
    if ( k < 0 ) return ERR_KEYWORD;

// --- parse the value for the given option
    int result; // Only used in the tolerance cases
    switch ( k )
    {
      case AREA_UNITS_OPTION:
          k = MSXutils_findmatch(Tok[1], AreaUnitsWords);
          if ( k < 0 ) return ERR_KEYWORD;
          setAreaUnits(MSX, k);
          break;

      case RATE_UNITS_OPTION:
          k = MSXutils_findmatch(Tok[1], TimeUnitsWords);
          if ( k < 0 ) return ERR_KEYWORD;
          setRateUnits(MSX, k);
          break;

      case SOLVER_OPTION:
          k = MSXutils_findmatch(Tok[1], SolverTypeWords);
          if ( k < 0 ) return ERR_KEYWORD;
          setSolver(MSX, k);
          break;

      case COUPLING_OPTION:
          k = MSXutils_findmatch(Tok[1], CouplingWords);
          if ( k < 0 ) return ERR_KEYWORD;
          setCoupling(MSX, k);
		  break;

      case TIMESTEP_OPTION:
          k = atoi(Tok[1]);
          if ( k <= 0 ) return ERR_NUMBER;
          setQstep(MSX, k);
          break;

      case RTOL_OPTION:
          double DefRtol;
          result = MSXutils_getDouble(Tok[1], &DefRtol);
          setDefaultRelativeErrorTolerance(MSX, DefRtol);
          if ( !result ) return ERR_NUMBER;
          break;

      case ATOL_OPTION:
          double DefAtol;
          result = MSXutils_getDouble(Tok[1], &DefAtol);
          setDefaultAbsoluteErrorTolerance(MSX, DefAtol);
          if ( !result ) return ERR_NUMBER;
          break;

      case COMPILER_OPTION:                                                    //1.1.00
          k = MSXutils_findmatch(Tok[1], CompilerWords);
          if ( k < 0 ) return ERR_KEYWORD;
    	  setCompiler(MSX, k);
	  break;

    }
    return 0;
}

//=============================================================================

int parseSpecies(MSXproject *MSX)
/**
**  Purpose:
**    parses an input line containing a species variable.
**
**  Input:
**    none
**
**  Returns:
**    an error code (0 if no error)
*/
{
    int i;

// --- get Species index

    if ( Ntokens < 3 ) return ERR_ITEMS;
    i = MSXproj_findObject(SPECIES, Tok[1]);
    if ( i <= 0 ) return ERR_NAME;

// --- get pointer to Species name
    setSpeciesId(MSX, i, MSXproj_findID(SPECIES, Tok[1]));

// --- get Species type

    if      ( MSXutils_match(Tok[0], "BULK") ) setSpeciesType(MSX, i, BULK);
    else if ( MSXutils_match(Tok[0], "WALL") ) setSpeciesType(MSX, i, WALL);
    else return ERR_KEYWORD;

// --- get Species units

    setSpeciesUnits(MSX, i, Tok[2]);

// --- get Species error tolerance

    setSpeciesAbsoluteTolerance(MSX, i, 0.0);
    setSpeciesRelativeTolerance(MSX, i, 0.0);

    if ( Ntokens >= 4)
    {
        double aTol;
        int result = MSXutils_getDouble(Tok[3], &aTol);
        setSpeciesAbsoluteTolerance(MSX, i, aTol);
        if ( !result )
            return ERR_NUMBER;
    }
    if ( Ntokens >= 5)
    {
        double rTol;
        int result = MSXutils_getDouble(Tok[4], &rTol);
        setSpeciesRelativeTolerance(MSX, i, rTol);
        if ( !result )
            return ERR_NUMBER;
    }
    return 0;
}

//=============================================================================

int parseCoeff(MSXproject *MSX)
/**
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
    if ( MSXutils_match(Tok[0], "PARAM") )
    {
    // --- get Parameter's index

        i = MSXproj_findObject(PARAMETER, Tok[1]);
        if ( i <= 0 ) return ERR_NAME;

    // --- get Parameter's value

        setParameterId(MSX, i, MSXproj_findID(PARAMETER, Tok[1]));
        if ( Ntokens >= 3 )
        {
            if ( !MSXutils_getDouble(Tok[2], &x) ) return ERR_NUMBER;
            setParameterValue(MSX, i, x);
            int numLinks;
            getNobjects(MSX, LINK, &numLinks);
            for (j=1; j<=numLinks; j++) setLinkParameter(MSX, j, i, x);
            int numTanks;
            getNobjects(MSX, TANK, &numTanks);
            for (j=1; j<=numTanks; j++) setTankParameter(MSX, j, i, x);
        }
        return 0;
    }

// --- check if variable is a Constant

    else if ( MSXutils_match(Tok[0], "CONST") )
    {
    // --- get Constant's index

        i = MSXproj_findObject(CONSTANT, Tok[1]);
        if ( i <= 0 ) return ERR_NAME;

    // --- get Constant's value

        setConstantId(MSX,i, MSXproj_findID(CONSTANT, Tok[1]));
        double value = 0.0;
        setConstantValue(MSX, i, value);
        if ( Ntokens >= 3 )
        {
            int result = MSXutils_getDouble(Tok[2], &value);
            setConstantValue(MSX, i, value);
            if ( !result )
                return ERR_NUMBER;
        }
        return 0;
    }
    else return  ERR_KEYWORD;
}

//=============================================================================

int parseTerm(MSXproject *MSX)
/**
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
    int k;                                                                     //1.1.00
    char s[MAXLINE+1] = "";
    MathExpr *expr;

// --- get term's name

    if ( Ntokens < 2 ) return 0;
    i = MSXproj_findObject(TERM, Tok[0]);
    setTermId(MSX, i, MSXproj_findID(TERM, Tok[0]));

// --- reconstruct the expression string from its tokens

    for (j=1; j<Ntokens; j++)
    {                                                                          //1.1.00
        strcat(s, Tok[j]);                     
        k = MSXproj_findObject(TERM, Tok[j]);                                  //1.1.00
        if ( k > 0 ) TermArray[i][k] = 1.0;                                    //1.1.00
    }                                                                          //1.1.00
    
// --- convert expression into a postfix stack of op codes

    expr = mathexpr_create(MSX, s, getVariableCode);
    if ( expr == NULL ) return ERR_MATH_EXPR;

// --- assign the expression to a Term object

    setTermExpression(MSX, i, expr);
    return 0;
}

//=============================================================================

int parseExpression(MSXproject *MSX, int classType)
/**
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
    k = MSXutils_findmatch(Tok[0], ExprTypeWords);
    if ( k < 0 ) return ERR_KEYWORD;

// --- determine species associated with expression

    i = MSXproj_findObject(SPECIES, Tok[1]);
    if ( i < 1 ) return ERR_NAME;

// --- check that species does not already have an expression

    if ( classType == LINK )
    {
        int pipeExprType;
        getSpeciesPipeExpressionType(MSX, i, &pipeExprType);
        if ( pipeExprType != NO_EXPR ) return ERR_DUP_EXPR;
    }
    if ( classType == TANK )
    {
        int tankExprType;
        getSpeciesTankExpressionType(MSX, i, &tankExprType);
        if ( tankExprType != NO_EXPR ) return ERR_DUP_EXPR;
    }

// --- reconstruct the expression string from its tokens

    for (j=2; j<Ntokens; j++) strcat(s, Tok[j]);
    
// --- convert expression into a postfix stack of op codes

    expr = mathexpr_create(MSX, s, getVariableCode);
    if ( expr == NULL ) return ERR_MATH_EXPR;

// --- assign the expression to the species

    switch (classType)
    {
    case LINK:
        setSpeciesPipeExpression(MSX, i, expr);
        setSpeciesPipeExpressionType(MSX, i, k);
        break;
    case TANK:
        setSpeciesTankExpression(MSX, i, expr);
        setSpeciesTankExpressionType(MSX, i, k);
        break;
    }
    return 0;    
}

//=============================================================================

int parseQuality(MSXproject *MSX)
/**
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
    if      ( MSXutils_match(Tok[0], "GLOBAL") ) i = 1;
    else if ( MSXutils_match(Tok[0], "NODE") )   i = 2;
    else if ( MSXutils_match(Tok[0], "LINK") )   i = 3;
    else return ERR_KEYWORD;

// --- find species index

    k = 1;
    if ( i >= 2 ) k = 2;
    m = MSXproj_findObject(SPECIES, Tok[k]);
    if ( m <= 0 ) return ERR_NAME;

// --- get quality value

    if ( i >= 2  && Ntokens < 4 ) return ERR_ITEMS;
    k = 2;
    if ( i >= 2 ) k = 3;
    if ( !MSXutils_getDouble(Tok[k], &x) ) return ERR_NUMBER;

// --- for global specification, set initial quality either for
//     all nodes or links depending on type of species

    if ( i == 1)
    {
        setInitialQualityVector(MSX, i, x);
        int type;
        getSpeciesType(MSX, m, &type);
        if ( type == BULK )
        {
            int numNodes;
            getNobjects(MSX, NODE, &numNodes);
            for (j=1; j<=numNodes; j++) setNodeInitialSpeciesConcentration(MSX, j, m, x);
        }
        int numLinks;
        getNobjects(MSX, LINK, &numLinks);
        for (j=1; j<=numLinks; j++) setLinkInitialSpeciesConcentration(MSX, j, m, x);
    }

// --- for a specific node, get its index & set its initial quality

    else if ( i == 2 )
    {
        err = ENgetnodeindex(Tok[1], &j);
        if ( err ) return ERR_NAME;
        int type;
        getSpeciesType(MSX, m, &type);
        if ( type == BULK ) setNodeInitialSpeciesConcentration(MSX, j, m, x);
    }

// --- for a specific link, get its index & set its initial quality

    else if ( i == 3 )
    {
        err = ENgetlinkindex(Tok[1], &j);
        if ( err ) return ERR_NAME;
        setLinkInitialSpeciesConcentration(MSX, j, m, x);
    }
    return 0;
}

//=============================================================================

int parseParameter(MSXproject *MSX)
/**
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
    i = MSXproj_findObject(PARAMETER, Tok[2]);

// --- get parameter value

    if ( !MSXutils_getDouble(Tok[3], &x) ) return ERR_NUMBER;

// --- for pipe parameter, get pipe index and update parameter's value

    if ( MSXutils_match(Tok[0], "PIPE") )
    {
        err = ENgetlinkindex(Tok[1], &j);
        if ( err ) return ERR_NAME;
        setLinkParameter(MSX, j, i, x);
    }

// --- for tank parameter, get tank index and update parameter's value

    else if ( MSXutils_match(Tok[0], "TANK") )
    {
        err = ENgetnodeindex(Tok[1], &j);
        if ( err ) return ERR_NAME;
        int nodeTank;
        getNodeTank(MSX, j, &nodeTank);
        j = nodeTank;

        if ( j > 0 ) setTankParameter(MSX, j, i, x);
    }
    else return ERR_KEYWORD;
    return 0;
}

//=============================================================================

int parseSource(MSXproject *MSX)
/**
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
    k = MSXutils_findmatch(Tok[0], SourceTypeWords);
    if ( k < 0 ) return ERR_KEYWORD;

// --- get node index

    err = ENgetnodeindex(Tok[1], &j);
    if ( err ) return ERR_NAME;

//  --- get species index

    m = MSXproj_findObject(SPECIES, Tok[2]);
    if ( m <= 0 ) return ERR_NAME;

// --- check that species is a BULK species
    int type;
    getSpeciesType(MSX, m, &type);
    if ( type != BULK ) return 0;

// --- get base strength

    if ( !MSXutils_getDouble(Tok[3], &x) ) return ERR_NUMBER;

// --- get time pattern if present

    i = 0;
    if ( Ntokens >= 5 )
    {
        i = MSXproj_findObject(PATTERN, Tok[4]);
        if ( i <= 0 ) return ERR_NAME;
    }

// --- check if a source for this species already exists

    getNodeSources(MSX, j, &source);
    while ( source )
    {
        if ( source->species == m ) break;
        source = source->next;
    }

// --- otherwise create a new source object

    if ( source == NULL )
    {
        source = (struct Ssource *) malloc(sizeof(struct Ssource));
        if ( source == NULL ) return 101;
        getNodeSources(MSX, j, &source->next);
        setNodeSources(MSX, j, source);
    }

// --- save source's properties

    source->type    = (char)k;
    source->species = m;
    source->c0      = x;
    source->pat     = i;
    return 0;
}    

//=============================================================================

int parsePattern(MSXproject *MSX)
/**
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
    i = MSXproj_findObject(PATTERN, Tok[0]);
    if ( i <= 0 ) return ERR_NAME;
    setPatternId(MSX, i, MSXproj_findID(PATTERN, Tok[0]));

// --- begin reading pattern multipliers

    k = 1;
    while ( k < Ntokens )
    {
        if ( !MSXutils_getDouble(Tok[k], &x) ) return ERR_NUMBER;
        listItem = (SnumList *) malloc(sizeof(SnumList));
        if ( listItem == NULL ) return 101;
        listItem->value = x;
        listItem->next = NULL;
        SnumList *first; //TODO
        getPatternFirstMultiplier(MSX, i, &first);
        if ( first == NULL )
        {
            setPatternCurrentMultiplier(MSX, i, listItem);
            setPatternFirstMultiplier(MSX, i, listItem);
        }
        else
        {
            setPatternNextMultiplier(MSX, i, listItem);
            setPatternCurrentMultiplier(MSX, i, listItem);
        }
        int length;
        getPatternLength(MSX, i, &length);
        setPatternLength(MSX, i, length+1);
        k++;
    }
    return 0;
}

//=============================================================================

int parseReport(MSXproject *MSX)
{
    int  i, j, k, err;

// --- get keyword

    if ( Ntokens < 2 ) return 0;
    k = MSXutils_findmatch(Tok[0], ReportWords);
    if ( k < 0 ) return ERR_KEYWORD;
    switch(k)
    {

    // --- keyword is NODE; parse ID names of reported nodes
    
        case 0:
            int numNodes;
            getNobjects(MSX, NODE, &numNodes);
            if ( MSXutils_strcomp(Tok[1], ALL) )
            {
                
                for (j=1; j<=numNodes; j++) setNodeReport(MSX, j, 1);
            }
            else if ( MSXutils_strcomp(Tok[1], NONE) )
            {
                for (j=1; j<=numNodes; j++) setNodeReport(MSX, j, 0);
            }
            else for (i=1; i<Ntokens; i++)
            {
                err = ENgetnodeindex(Tok[i], &j);
                if ( err ) return ERR_NAME;
                setNodeReport(MSX, j, 1);
            }
            break;

    // --- keyword is LINK: parse ID names of reported links
        case 1:
            int numLinks;
            getNobjects(MSX, LINK, &numLinks);
            if ( MSXutils_strcomp(Tok[1], ALL) )
            {
                for (j=1; j<=numLinks; j++) setLinkReport(MSX, j, 1);
            }
            else if ( MSXutils_strcomp(Tok[1], NONE) )
            {
                for (j=1; j<=numLinks; j++) setLinkReport(MSX, j, 0);
            }
            else for (i=1; i<Ntokens; i++)
            {
                err = ENgetlinkindex(Tok[i], &j);
                if ( err ) return ERR_NAME;
                setLinkReport(MSX, j, 1);
            }
            break;

    // --- keyword is SPECIES; get YES/NO & precision

        case 2:
            j = MSXproj_findObject(SPECIES, Tok[1]);
            if ( j <= 0 ) return ERR_NAME;
            if ( Ntokens >= 3 )
            {
                if ( MSXutils_strcomp(Tok[2], YES) ) setSpeciesReport(MSX, j, 1);
                else if ( MSXutils_strcomp(Tok[2], NO)  ) setSpeciesReport(MSX, j, 0);
                else return ERR_KEYWORD;
            }
            if ( Ntokens >= 4 )
            {
                int precision;
                int result = MSXutils_getInt(Tok[3], &precision);
                setSpeciesPrecision(MSX, j, precision);
                if ( !result )
                    return ERR_NUMBER;
            }
            break;

    // --- keyword is FILE: get name of report file

        case 3:
            strcpy(MSX->RptFile.name, Tok[1]); //TODO
            break;

    // --- keyword is PAGESIZE;

        case 4:
            if ( !MSXutils_getInt(Tok[1], &MSX->PageSize) ) return ERR_NUMBER; //TODO
            break;
    }
    return 0;
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
    int j = MSXproj_findObject(SPECIES, id);
    if ( j >= 1 ) return j;
    j = MSXproj_findObject(TERM, id);
    if ( j >= 1 ) {
        int numSpecies;
        getNobjects(MSX, SPECIES, &numSpecies);
        return numSpecies + j;
    }
    j = MSXproj_findObject(PARAMETER, id);
    if ( j >= 1 ){
        int numSpecies;
        int numTerms;
        getNobjects(MSX, SPECIES, &numSpecies);
        getNobjects(MSX, TERM, &numTerms);
        return numSpecies + numTerms + j;
    }
    j = MSXproj_findObject(CONSTANT, id);
    if ( j >= 1 ){
        int numSpecies;
        int numTerms;
        int numParams;
        getNobjects(MSX, SPECIES, &numSpecies);
        getNobjects(MSX, TERM, &numTerms);
        getNobjects(MSX, PARAMETER, &numParams);
        return numSpecies + numTerms + numParams + j;
    }
    j = MSXutils_findmatch(id, HydVarWords);
    if ( j >= 1 ) {
        int numSpecies;
        int numTerms;
        int numParams;
        int numConstants;
        getNobjects(MSX, SPECIES, &numSpecies);
        getNobjects(MSX, TERM, &numTerms);
        getNobjects(MSX, PARAMETER, &numParams);
        getNobjects(MSX, CONSTANT, &numConstants);
        return numSpecies + numTerms + numParams + numConstants + j;
    }
    return -1;
}

//=============================================================================

int  getTokens(char *s)
/**
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
**    (spaces, tabs, newline, carriage return) which is defined in
**    MSXGLOBALS.H. Text between quotes is treated as a single token.
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
    len = (int)strlen(s);

    // --- scan s for tokens until nothing left

    while (len > 0 && n < MAXTOKS)
    {
        m = (int)strcspn(s,SEPSTR);              // find token length 
        if (m == 0) s++;                    // no token found
        else
        {
            if (*s == '"')                  // token begins with quote
            {
                s++;                        // start token after quote
                len--;                      // reduce length of s
                m = (int)strcspn(s,"\"\n"); // find end quote or new line
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

//=============================================================================

int checkCyclicTerms(MSXproject *MSX)                                                         //1.1.00
/**
**  Purpose:
**    checks for cyclic references in Term expressions (e.g., T1 = T2 + T3
**    and T3 = T2/T1)
**
**  Input:
**    none
**
**  Returns:
**    1 if cyclic reference found or 0 if none found.
*/
{
    int i, j, n;
    char msg[MAXMSG+1];

    getNobjects(MSX, TERM, &n);
    for (i=1; i<n; i++)
    {
        for (j=1; j<=n; j++) TermArray[0][j] = 0.0;
        if ( traceTermPath(i, i, n) )
        {
            char * id;
            getTermId(MSX, i, &id);
            sprintf(msg, "Error 410 - term %s contains a cyclic reference.",
                         id);
            ENwriteline(msg);
            return 1;
        }
    }
    return 0;
}

//=============================================================================

int traceTermPath(int i, int istar, int n)                                     //1.1.00
/**
**  Purpose:
**    checks if Term[istar] is in the path of terms that appear when evaluating
**    Term[i]
**
**  Input:
**    i = index of term whose expressions are to be traced
**    istar = index of term being searched for
**    n = total number of terms
**
**  Returns:
**    1 if term istar found; 0 if not found.
*/
{
    int j;
    if ( TermArray[0][i] == 1.0 ) return 0;
    TermArray[0][i] = 1.0;
    for (j=1; j<=n; j++)
    {
        if ( TermArray[i][j] == 0.0 ) continue;
        if ( j == istar ) return 1;
        else if ( traceTermPath(j, istar, n) ) return 1;
    }
    return 0;
}
