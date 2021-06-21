
#include <stdio.h>
#include <string.h>

#include "msxsetters.h"
#include "msxtypes.h"
// #include "mathexpr.h"

//  External variables
//--------------------
extern MSXproject  MSX;                // MSX project data

//TODO add comments for functions

int setNobjects(int type, int Nobjects)
{
    MSX.Nobjects[type] = Nobjects;
    return 0;   //No room for error
}

int setFlowUnits(int Flowflag, int Unitsflag)
{
    MSX.Flowflag = Flowflag;
    MSX.Unitsflag = Unitsflag;
    return 0;   //No room for error
}

int setAreaUnits(int AreaUnits)
{
    MSX.AreaUnits = AreaUnits;
    return 0;   //No room for error
}

int setRateUnits(int RateUnits)
{
    MSX.RateUnits = RateUnits;
    return 0;   //No room for error
}

int setSolver(int solver)
{
    MSX.Solver = solver;
    return 0;   //No room for error
}

int setCoupling(int coupling)
{
    MSX.Coupling = coupling;
    return 0;   //No room for error
}

int setCompiler(int compiler)
{
    MSX.Compiler = compiler;
    return 0;   //No room for error
}

int setQstep(int Qstep)
{
    MSX.Qstep = Qstep;
    return 0;   //No room for error
}

int setRstep(int Rstep)
{
    MSX.Rstep = Rstep;
    return 0;   //No room for error
}

int setRstart(int Rstart)
{
    MSX.Rstart = Rstart;
    return 0;   //No room for error
}

int setPstep(int Pstep)
{
    MSX.Pstep = Pstep;
    return 0;   //No room for error
}

int setPstart(int Pstart)
{
    MSX.Pstart = Pstart;
    return 0;   //No room for error
}

int setStatflag(int Statflag)
{
    MSX.Statflag = Statflag;
    return 0;   //No room for error
}

int setDefaultRelativeErrorTolerance(double DefRtol)
{
    MSX.DefRtol = DefRtol;
    return 0;   //Success
}

int setDefaultAbsoluteErrorTolerance(double DefAtol)
{
    MSX.DefAtol = DefAtol;
    return 0;   //Success
}

int setInitialQualityVector(int index, double value)
{
    //Assuming a valid index
    MSX.C0[index] = value;
    return 0;   //Success
}

// Node setters

int setNodeTank(int nodeIndex, int tank)
{
    if (nodeIndex > MSX.Nobjects[NODE]) return 516; //reference made to an illegal object index
    MSX.Node[nodeIndex].tank = tank;
    return 0;   //Success
}

int setNodeInitialSpeciesConcentration(int nodeIndex, int speciesIndex, double concentration)
{
    if (nodeIndex > MSX.Nobjects[NODE]) return 516; //reference made to an illegal object index
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    MSX.Node[nodeIndex].c0[speciesIndex] = concentration;
    return 0;   //Success
}

// int setNodeSources(int nodeIndex, Psource sources)
// {
//     if (nodeIndex > MSX.Nobjects[NODE]) return 516; //reference made to an illegal object index
//     MSX.Node[nodeIndex].sources = sources;
//     return 0;   //Success
// }

int setNodeReport(int nodeIndex, char report)
{
    if (nodeIndex > MSX.Nobjects[NODE]) return 516; //reference made to an illegal object index
    MSX.Node[nodeIndex].rpt = report;
    return 0;   //Success
}

// Tank setters

int setTankNode(int tankIndex, int node)
{
    if (tankIndex > MSX.Nobjects[TANK]) return 516; //reference made to an illegal object index
    MSX.Tank[tankIndex].node = node;
    return 0;   //Success
}

int setTankArea(int tankIndex, double area)
{
    if (tankIndex > MSX.Nobjects[TANK]) return 516; //reference made to an illegal object index
    MSX.Tank[tankIndex].a = area;
    return 0;   //Success
}

int setTankInitialVolume(int tankIndex, double v0) 
{
    if (tankIndex > MSX.Nobjects[TANK]) return 516; //reference made to an illegal object index
    MSX.Tank[tankIndex].v0 = v0;
    return 0;   //Success
}

int setTankMixModel(int tankIndex, int mix)
{
    if (tankIndex > MSX.Nobjects[TANK]) return 516; //reference made to an illegal object index
    MSX.Tank[tankIndex].mixModel = mix;
    return 0;   //Success
}

int setTankMixingSize(int tankIndex, float vMix)
{
    if (tankIndex > MSX.Nobjects[TANK]) return 516; //reference made to an illegal object index
    MSX.Tank[tankIndex].vMix = vMix;
    return 0;   //Success
}

int setTankParameter(int tankIndex, int paramIndex, double val)
{
    if (tankIndex > MSX.Nobjects[TANK]) return 516; //reference made to an illegal object index
    if (paramIndex > MSX.Nobjects[PARAMETER]) return 516;   //reference made to an illegal object index
    MSX.Tank[tankIndex].param[paramIndex] = val;
    return 0;   //Success
}

// Link setters

int setLinkStartNode(int linkIndex, int startNode)
{
    if (linkIndex > MSX.Nobjects[LINK]) return 516; //reference made to an illegal object index
    MSX.Link[linkIndex].n1 = startNode;
    return 0;   //Success
}

int setLinkEndNode(int linkIndex, int endNode)
{
    if (linkIndex > MSX.Nobjects[LINK]) return 516; //reference made to an illegal object index
    MSX.Link[linkIndex].n2 = endNode;
    return 0;   //Success
}

int setLinkDiameter(int linkIndex, double diameter)
{
    if (linkIndex > MSX.Nobjects[LINK]) return 516; //reference made to an illegal object index
    MSX.Link[linkIndex].diam = diameter;
    return 0;   //Success
}

int setLinkLength(int linkIndex, double length)
{
    if (linkIndex > MSX.Nobjects[LINK]) return 516; //reference made to an illegal object index
    MSX.Link[linkIndex].len = length;
    return 0;   //Success
}

int setLinkRoughness(int linkIndex, double roughness)
{
    if (linkIndex > MSX.Nobjects[LINK]) return 516; //reference made to an illegal object index
    MSX.Link[linkIndex].roughness = roughness;
    return 0;   //Success
}

int setLinkParameter(int linkIndex, int paramIndex, double val)
{
    if (linkIndex > MSX.Nobjects[LINK]) return 516; //reference made to an illegal object index
    if (paramIndex > MSX.Nobjects[PARAMETER]) return 516;   //reference made to an illegal object index
    MSX.Link[linkIndex].param[paramIndex] = val;
    return 0;   //Success
}

int setLinkInitialSpeciesConcentration(int linkIndex, int speciesIndex, double concentration)
{
    if (linkIndex > MSX.Nobjects[LINK]) return 516; //reference made to an illegal object index
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    MSX.Node[linkIndex].c0[speciesIndex] = concentration;
    return 0;   //Success
}

int setLinkReport(int linkIndex, char report)
{
    if (linkIndex > MSX.Nobjects[LINK]) return 516; //reference made to an illegal object index
    MSX.Link[linkIndex].rpt = report;
    return 0;   //Success
}

// Species setters

int setSpeciesId(int speciesIndex, char *id)
{
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516;   //reference made to an illegal object index
    MSX.Species[speciesIndex].id = id;
    return 0;   //Success
}

int setSpeciesType(int speciesIndex, int type)
{
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516;   //reference made to an illegal object index
    MSX.Species[speciesIndex].type = type;
    return 0;   //Success
}

int setSpeciesUnits(int speciesIndex, char * units)
{
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516;   //reference made to an illegal object index
    strncpy(MSX.Species[speciesIndex].units, units, MAXUNITS);
    return 0;   //Success
}

int setSpeciesAbsoluteTolerance(int speciesIndex, double aTol)
{
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516;   //reference made to an illegal object index
    MSX.Species[speciesIndex].aTol = aTol;
    return 0;   //Success
}

int setSpeciesRelativeTolerance(int speciesIndex, double rTol)
{
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516;   //reference made to an illegal object index
    MSX.Species[speciesIndex].rTol = rTol;
    return 0;   //Success
}

int setSpeciesPipeExpressionType(int speciesIndex, int type)
{
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    MSX.Species[speciesIndex].pipeExprType = type;
    return 0;   //Success
}

// int setSpeciesPipeExpression(int speciesIndex, MathExpr *expr)
// {
//     if (speciesIndex > MSX.Nobjects[SPECIES]) return 516; //reference made to an illegal object index
//     MSX.Species[speciesIndex].pipeExpr = expr;
//     return 0;   //Success
// }

int setSpeciesTankExpressionType(int speciesIndex, int type)
{
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    MSX.Species[speciesIndex].tankExprType = type;
    return 0;   //Success
}

// int setSpeciesTankExpression(int speciesIndex, MathExpr *expr)
// {
//     if (speciesIndex > MSX.Nobjects[SPECIES]) return 516; //reference made to an illegal object index
//     MSX.Species[speciesIndex].tankExpr = expr;
//     return 0;   //Success
// }

int setSpeciesReport(int speciesIndex, char report)
{
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    MSX.Species[speciesIndex].rpt = report;
    return 0;   //Success
}

int setSpeciesPrecision(int speciesIndex, int precision)
{
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    MSX.Species[speciesIndex].precision = precision;
    return 0;   //Success
}

// Parameter setters

int setParameterId(int paramIndex, char *id)
{
    if (paramIndex > MSX.Nobjects[PARAMETER]) return 516;   //reference made to an illegal object index
    MSX.Param[paramIndex].id = id;
    return 0;   //Success
}

int setParameterValue(int paramIndex, double value)
{
    if (paramIndex > MSX.Nobjects[PARAMETER]) return 516;   //reference made to an illegal object index
    MSX.Param[paramIndex].value = value;
    return 0;   //Success
}

// Constant setters

int setConstantId(int constantIndex, char *id)
{
    if (constantIndex > MSX.Nobjects[CONSTANT]) return 516;   //reference made to an illegal object index
    MSX.Const[constantIndex].id = id;
    return 0;   //Success
}

int setConstantValue(int constantIndex, double value)
{
    if (constantIndex > MSX.Nobjects[CONSTANT]) return 516;   //reference made to an illegal object index
    MSX.Const[constantIndex].value = value;
    return 0;   //Success
}

// Term setters

int setTermId(int termIndex, char *id)
{
    if (termIndex > MSX.Nobjects[TERM]) return 516;   //reference made to an illegal object index
    MSX.Term[termIndex].id = id;
    return 0;   //Success
}

// int setTermExpression(int termIndex, MathExpr *expr)
// {
//     if (termIndex > MSX.Nobjects[TERM]) return 516;   //reference made to an illegal object index
//     MSX.Term[termIndex].expr = expr;
//     return 0;   //Success
// }

// Pattern setters

int setPatternId(int patternIndex, char *id)
{
    if (patternIndex > MSX.Nobjects[PATTERN]) return 516;   //reference made to an illegal object index
    MSX.Pattern[patternIndex].id = id;
    return 0;   //Success
}

int setPatternLength(int patternIndex, int length)
{
    if (patternIndex > MSX.Nobjects[PATTERN]) return 516;   //reference made to an illegal object index
    MSX.Pattern[patternIndex].length = length;
    return 0;   //Success
}

// int setPatternFirstMultiplier(int patternIndex, SnumList *first)
// {
//     if (patternIndex > MSX.Nobjects[PATTERN]) return 516;   //reference made to an illegal object index
//     MSX.Pattern[patternIndex].first = first;
//     return 0;   //Success
// }

// int setPatternCurrentMultiplier(int patternIndex, SnumList *current)
// {
//     if (patternIndex > MSX.Nobjects[PATTERN]) return 516;   //reference made to an illegal object index
//     MSX.Pattern[patternIndex].current = current;
//     return 0;   //Success
// }

// int setPatternNextMultiplier(int patternIndex, SnumList *next)
// {
//     if (patternIndex > MSX.Nobjects[PATTERN]) return 516;   //reference made to an illegal object index
//     MSX.Pattern[patternIndex].current->next = next;
//     return 0;   //Success
// }