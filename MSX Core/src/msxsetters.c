
#include <stdio.h>
#include <string.h>

#include "msxutils.h"
#include "msxsetters.h"


//TODO add comments for functions

int setDefaults(MSXproject *MSX)
/**
**  Purpose:
**    assigns default values to project variables.
**
**  Input:
**    MSX data struct.
*/
{
    int i;
    MSX->RptFile.file = NULL;                                                   //(LR-11/20/07)
    MSX->HydFile.file = NULL;
    MSX->HydFile.mode = USED_FILE;
    MSX->OutFile.file = NULL;
    MSX->OutFile.mode = SCRATCH_FILE;
    MSX->TmpOutFile.file = NULL;
    MSXutils_getTempName(MSX->OutFile.name);                                    //1.1.00
    MSXutils_getTempName(MSX->TmpOutFile.name);                                 //1.1.00
    strcpy(MSX->RptFile.name, "");
    strcpy(MSX->Title, "");
    MSX->Rptflag = 0;
    for (i=0; i<MAX_OBJECTS; i++) MSX->Nobjects[i] = 0;
    MSX->Unitsflag = US;
    MSX->Flowflag = GPM;
    MSX->Statflag = SERIES;
    MSX->DefRtol = 0.001;
    MSX->DefAtol = 0.01;
    MSX->Solver = EUL;
    MSX->Coupling = NO_COUPLING;
    MSX->Compiler = NO_COMPILER;                                                //1.1.00
    MSX->AreaUnits = FT2;
    MSX->RateUnits = DAYS;
    MSX->Qstep = 300;
    MSX->Rstep = 3600;
    MSX->Rstart = 0;
    MSX->Dur = 0;
    MSX->Node = NULL;
    MSX->NodesCapacity = 0;
    MSX->Link = NULL;
    MSX->Tank = NULL;
    MSX->D = NULL;
    MSX->Q = NULL;
    MSX->H = NULL;
    MSX->Species = NULL;
    MSX->Term = NULL;
    MSX->Const = NULL;
    MSX->Pattern = NULL;
    MSX->K = NULL;                                                              //1.1.00
    MSX->Adjlist = NULL;
    return 0;
}

int setNobjects(MSXproject *MSX, int type, int Nobjects)
{
    MSX->Nobjects[type] = Nobjects;
    return 0;   //No room for error
}

int setFlowUnits(MSXproject *MSX, int Flowflag, int Unitsflag)
{
    MSX->Flowflag = Flowflag;
    MSX->Unitsflag = Unitsflag;
    return 0;   //No room for error
}

int setAreaUnits(MSXproject *MSX, int AreaUnits)
{
    MSX->AreaUnits = AreaUnits;
    return 0;   //No room for error
}

int setRateUnits(MSXproject *MSX, int RateUnits)
{
    MSX->RateUnits = RateUnits;
    return 0;   //No room for error
}

int setSolver(MSXproject *MSX, int solver)
{
    MSX->Solver = solver;
    return 0;   //No room for error
}

int setCoupling(MSXproject *MSX, int coupling)
{
    MSX->Coupling = coupling;
    return 0;   //No room for error
}

int setCompiler(MSXproject *MSX, int compiler)
{
    MSX->Compiler = compiler;
    return 0;   //No room for error
}

int setQstep(MSXproject *MSX, int Qstep)
{
    MSX->Qstep = Qstep;
    return 0;   //No room for error
}

int setRstep(MSXproject *MSX, int Rstep)
{
    MSX->Rstep = Rstep;
    return 0;   //No room for error
}

int setRstart(MSXproject *MSX, int Rstart)
{
    MSX->Rstart = Rstart;
    return 0;   //No room for error
}

int setPstep(MSXproject *MSX, int Pstep)
{
    MSX->Pstep = Pstep;
    return 0;   //No room for error
}

int setPstart(MSXproject *MSX, int Pstart)
{
    MSX->Pstart = Pstart;
    return 0;   //No room for error
}

int setStatflag(MSXproject *MSX, int Statflag)
{
    MSX->Statflag = Statflag;
    return 0;   //No room for error
}

int setDefaultRelativeErrorTolerance(MSXproject *MSX, double DefRtol)
{
    MSX->DefRtol = DefRtol;
    return 0;   //Success
}

int setDefaultAbsoluteErrorTolerance(MSXproject *MSX, double DefAtol)
{
    MSX->DefAtol = DefAtol;
    return 0;   //Success
}

int setInitialQualityVector(MSXproject *MSX, int index, double value)
{
    //Assuming a valid index
    MSX->C0[index] = value;
    return 0;   //Success
}

// Node setters

int setNodeTank(MSXproject *MSX, int nodeIndex, int tank)
{
    if (nodeIndex > MSX->Nobjects[NODE]) return 516; //reference made to an illegal object index
    MSX->Node[nodeIndex].tank = tank;
    return 0;   //Success
}

int setNodeInitialSpeciesConcentration(MSXproject *MSX, int nodeIndex, int speciesIndex, double concentration)
{
    if (nodeIndex > MSX->Nobjects[NODE]) return 516; //reference made to an illegal object index
    if (speciesIndex > MSX->Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    MSX->Node[nodeIndex].c0[speciesIndex] = concentration;
    return 0;   //Success
}

int setNodeSources(MSXproject *MSX, int nodeIndex, Psource sources)
{
    if (nodeIndex > MSX->Nobjects[NODE]) return 516; //reference made to an illegal object index
    MSX->Node[nodeIndex].sources = sources;
    return 0;   //Success
}

int setNodeReport(MSXproject *MSX, int nodeIndex, char report)
{
    if (nodeIndex > MSX->Nobjects[NODE]) return 516; //reference made to an illegal object index
    MSX->Node[nodeIndex].rpt = report;
    return 0;   //Success
}

// Tank setters

int setTankNode(MSXproject *MSX, int tankIndex, int node)
{
    if (tankIndex > MSX->Nobjects[TANK]) return 516; //reference made to an illegal object index
    MSX->Tank[tankIndex].node = node;
    return 0;   //Success
}

int setTankArea(MSXproject *MSX, int tankIndex, double area)
{
    if (tankIndex > MSX->Nobjects[TANK]) return 516; //reference made to an illegal object index
    MSX->Tank[tankIndex].a = area;
    return 0;   //Success
}

int setTankInitialVolume(MSXproject *MSX, int tankIndex, double v0) 
{
    if (tankIndex > MSX->Nobjects[TANK]) return 516; //reference made to an illegal object index
    MSX->Tank[tankIndex].v0 = v0;
    return 0;   //Success
}

int setTankMixModel(MSXproject *MSX, int tankIndex, int mix)
{
    if (tankIndex > MSX->Nobjects[TANK]) return 516; //reference made to an illegal object index
    MSX->Tank[tankIndex].mixModel = mix;
    return 0;   //Success
}

int setTankMixingSize(MSXproject *MSX, int tankIndex, float vMix)
{
    if (tankIndex > MSX->Nobjects[TANK]) return 516; //reference made to an illegal object index
    MSX->Tank[tankIndex].vMix = vMix;
    return 0;   //Success
}

int setTankParameter(MSXproject *MSX, int tankIndex, int paramIndex, double val)
{
    if (tankIndex > MSX->Nobjects[TANK]) return 516; //reference made to an illegal object index
    if (paramIndex > MSX->Nobjects[PARAMETER]) return 516;   //reference made to an illegal object index
    MSX->Tank[tankIndex].param[paramIndex] = val;
    return 0;   //Success
}

// Link setters

int setLinkStartNode(MSXproject *MSX, int linkIndex, int startNode)
{
    if (linkIndex > MSX->Nobjects[LINK]) return 516; //reference made to an illegal object index
    MSX->Link[linkIndex].n1 = startNode;
    return 0;   //Success
}

int setLinkEndNode(MSXproject *MSX, int linkIndex, int endNode)
{
    if (linkIndex > MSX->Nobjects[LINK]) return 516; //reference made to an illegal object index
    MSX->Link[linkIndex].n2 = endNode;
    return 0;   //Success
}

int setLinkDiameter(MSXproject *MSX, int linkIndex, double diameter)
{
    if (linkIndex > MSX->Nobjects[LINK]) return 516; //reference made to an illegal object index
    MSX->Link[linkIndex].diam = diameter;
    return 0;   //Success
}

int setLinkLength(MSXproject *MSX, int linkIndex, double length)
{
    if (linkIndex > MSX->Nobjects[LINK]) return 516; //reference made to an illegal object index
    MSX->Link[linkIndex].len = length;
    return 0;   //Success
}

int setLinkRoughness(MSXproject *MSX, int linkIndex, double roughness)
{
    if (linkIndex > MSX->Nobjects[LINK]) return 516; //reference made to an illegal object index
    MSX->Link[linkIndex].roughness = roughness;
    return 0;   //Success
}

int setLinkParameter(MSXproject *MSX, int linkIndex, int paramIndex, double val)
{
    if (linkIndex > MSX->Nobjects[LINK]) return 516; //reference made to an illegal object index
    if (paramIndex > MSX->Nobjects[PARAMETER]) return 516;   //reference made to an illegal object index
    MSX->Link[linkIndex].param[paramIndex] = val;
    return 0;   //Success
}

int setLinkInitialSpeciesConcentration(MSXproject *MSX, int linkIndex, int speciesIndex, double concentration)
{
    if (linkIndex > MSX->Nobjects[LINK]) return 516; //reference made to an illegal object index
    if (speciesIndex > MSX->Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    MSX->Node[linkIndex].c0[speciesIndex] = concentration;
    return 0;   //Success
}

int setLinkReport(MSXproject *MSX, int linkIndex, char report)
{
    if (linkIndex > MSX->Nobjects[LINK]) return 516; //reference made to an illegal object index
    MSX->Link[linkIndex].rpt = report;
    return 0;   //Success
}

// Species setters

int setSpeciesId(MSXproject *MSX, int speciesIndex, char *id)
{
    if (speciesIndex > MSX->Nobjects[SPECIES]) return 516;   //reference made to an illegal object index
    MSX->Species[speciesIndex].id = id;
    return 0;   //Success
}

int setSpeciesType(MSXproject *MSX, int speciesIndex, int type)
{
    if (speciesIndex > MSX->Nobjects[SPECIES]) return 516;   //reference made to an illegal object index
    MSX->Species[speciesIndex].type = type;
    return 0;   //Success
}

int setSpeciesUnits(MSXproject *MSX, int speciesIndex, char * units)
{
    if (speciesIndex > MSX->Nobjects[SPECIES]) return 516;   //reference made to an illegal object index
    strncpy(MSX->Species[speciesIndex].units, units, MAXUNITS);
    return 0;   //Success
}

int setSpeciesAbsoluteTolerance(MSXproject *MSX, int speciesIndex, double aTol)
{
    if (speciesIndex > MSX->Nobjects[SPECIES]) return 516;   //reference made to an illegal object index
    MSX->Species[speciesIndex].aTol = aTol;
    return 0;   //Success
}

int setSpeciesRelativeTolerance(MSXproject *MSX, int speciesIndex, double rTol)
{
    if (speciesIndex > MSX->Nobjects[SPECIES]) return 516;   //reference made to an illegal object index
    MSX->Species[speciesIndex].rTol = rTol;
    return 0;   //Success
}

int setSpeciesPipeExpressionType(MSXproject *MSX, int speciesIndex, int type)
{
    if (speciesIndex > MSX->Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    MSX->Species[speciesIndex].pipeExprType = type;
    return 0;   //Success
}

int setSpeciesPipeExpression(MSXproject *MSX, int speciesIndex, MathExpr *expr)
{
    if (speciesIndex > MSX->Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    MSX->Species[speciesIndex].pipeExpr = expr;
    return 0;   //Success
}

int setSpeciesTankExpressionType(MSXproject *MSX, int speciesIndex, int type)
{
    if (speciesIndex > MSX->Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    MSX->Species[speciesIndex].tankExprType = type;
    return 0;   //Success
}

int setSpeciesTankExpression(MSXproject *MSX, int speciesIndex, MathExpr *expr)
{
    if (speciesIndex > MSX->Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    MSX->Species[speciesIndex].tankExpr = expr;
    return 0;   //Success
}

int setSpeciesReport(MSXproject *MSX, int speciesIndex, char report)
{
    if (speciesIndex > MSX->Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    MSX->Species[speciesIndex].rpt = report;
    return 0;   //Success
}

int setSpeciesPrecision(MSXproject *MSX, int speciesIndex, int precision)
{
    if (speciesIndex > MSX->Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    MSX->Species[speciesIndex].precision = precision;
    return 0;   //Success
}

// Parameter setters

int setParameterId(MSXproject *MSX, int paramIndex, char *id)
{
    if (paramIndex > MSX->Nobjects[PARAMETER]) return 516;   //reference made to an illegal object index
    MSX->Param[paramIndex].id = id;
    return 0;   //Success
}

int setParameterValue(MSXproject *MSX, int paramIndex, double value)
{
    if (paramIndex > MSX->Nobjects[PARAMETER]) return 516;   //reference made to an illegal object index
    MSX->Param[paramIndex].value = value;
    return 0;   //Success
}

// Constant setters

int setConstantId(MSXproject *MSX, int constantIndex, char *id)
{
    if (constantIndex > MSX->Nobjects[CONSTANT]) return 516;   //reference made to an illegal object index
    MSX->Const[constantIndex].id = id;
    return 0;   //Success
}

int setConstantValue(MSXproject *MSX, int constantIndex, double value)
{
    if (constantIndex > MSX->Nobjects[CONSTANT]) return 516;   //reference made to an illegal object index
    MSX->Const[constantIndex].value = value;
    return 0;   //Success
}

// Term setters

int setTermId(MSXproject *MSX, int termIndex, char *id)
{
    if (termIndex > MSX->Nobjects[TERM]) return 516;   //reference made to an illegal object index
    MSX->Term[termIndex].id = id;
    return 0;   //Success
}

int setTermExpression(MSXproject *MSX, int termIndex, MathExpr *expr)
{
    if (termIndex > MSX->Nobjects[TERM]) return 516;   //reference made to an illegal object index
    MSX->Term[termIndex].expr = expr;
    return 0;   //Success
}

// Pattern setters

int setPatternId(MSXproject *MSX, int patternIndex, char *id)
{
    if (patternIndex > MSX->Nobjects[PATTERN]) return 516;   //reference made to an illegal object index
    MSX->Pattern[patternIndex].id = id;
    return 0;   //Success
}

int setPatternLength(MSXproject *MSX, int patternIndex, int length)
{
    if (patternIndex > MSX->Nobjects[PATTERN]) return 516;   //reference made to an illegal object index
    MSX->Pattern[patternIndex].length = length;
    return 0;   //Success
}

int setPatternFirstMultiplier(MSXproject *MSX, int patternIndex, SnumList *first)
{
    if (patternIndex > MSX->Nobjects[PATTERN]) return 516;   //reference made to an illegal object index
    MSX->Pattern[patternIndex].first = first;
    return 0;   //Success
}

int setPatternCurrentMultiplier(MSXproject *MSX, int patternIndex, SnumList *current)
{
    if (patternIndex > MSX->Nobjects[PATTERN]) return 516;   //reference made to an illegal object index
    MSX->Pattern[patternIndex].current = current;
    return 0;   //Success
}

int setPatternNextMultiplier(MSXproject *MSX, int patternIndex, SnumList *next)
{
    if (patternIndex > MSX->Nobjects[PATTERN]) return 516;   //reference made to an illegal object index
    MSX->Pattern[patternIndex].current->next = next;
    return 0;   //Success
}