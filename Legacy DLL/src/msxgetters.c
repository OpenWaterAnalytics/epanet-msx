
#include <stdio.h>
#include <string.h>

#include "msxgetters.h"

//  External variables
//--------------------
extern MSXproject  MSX;                // MSX project data

//TODO add comments for functions

int getNobjects(int type, int *Nobjects)
{
    *Nobjects = MSX.Nobjects[type];
    return 0;   //Success
}

int getFlowUnits(int *Flowflag, int *Unitsflag)
{
    *Flowflag = MSX.Flowflag;
    *Unitsflag = MSX.Unitsflag;
    return 0;   //No room for error
}

int getAreaUnits(int *AreaUnits)
{
    *AreaUnits = MSX.AreaUnits;
    return 0;   //No room for error
}

int getRateUnits(int *RateUnits)
{
    *RateUnits = MSX.RateUnits;
    return 0;   //No room for error
}

int getSolver(int *solver)
{
    *solver = MSX.Solver;
    return 0;   //No room for error
}

int getCoupling(int *coupling)
{
    *coupling = MSX.Coupling;
    return 0;   //No room for error
}

int getCompiler(int *compiler)
{
    *compiler = MSX.Compiler;
    return 0;   //No room for error
}

int getQstep(int *Qstep)
{
    *Qstep = MSX.Qstep;
    return 0;   //No room for error
}

int getRstep(int *Rstep)
{
    *Rstep = MSX.Rstep;
    return 0;   //No room for error
}

int getRstart(int *Rstart)
{
    *Rstart = MSX.Rstart;
    return 0;   //No room for error
}

int getPstep(int *Pstep)
{
    *Pstep = MSX.Pstep;
    return 0;   //No room for error
}

int getPstart(int *Pstart)
{
    *Pstart = MSX.Pstart;
    return 0;   //No room for error
}

int getStatflag(int *Statflag)
{
    *Statflag = MSX.Statflag;
    return 0;   //No room for error
}

int getDefaultRelativeErrorTolerance(double *DefRtol)
{
    *DefRtol = MSX.DefRtol;
    return 0;   //Success
}

int getDefaultAbsoluteErrorTolerance(double *DefAtol)
{
    *DefAtol = MSX.DefAtol;
    return 0;   //Success
}

int getInitialQualityVector(int index, double *value)
{
    //Assuming a valid index
    *value = MSX.C0[index];
    return 0;   //Success
}

// Node getters

int getNodeTank(int nodeIndex, int *tank)
{
    if (nodeIndex > MSX.Nobjects[NODE]) return 516; //reference made to an illegal object index
    *tank = MSX.Node[nodeIndex].tank;
    return 0;   //Success
}

int getNodeInitialSpeciesConcentration(int nodeIndex, int speciesIndex, double *concentration)
{
    if (nodeIndex > MSX.Nobjects[NODE]) return 516; //reference made to an illegal object index
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    *concentration = MSX.Node[nodeIndex].c0[speciesIndex];
    return 0;   //Success
}

int getNodeSources(int nodeIndex, Psource *sources)
{
    if (nodeIndex > MSX.Nobjects[NODE]) return 516; //reference made to an illegal object index
    *sources = MSX.Node[nodeIndex].sources;
    return 0;   //Success
}

int getNodeReport(int nodeIndex, char *report)
{
    if (nodeIndex > MSX.Nobjects[NODE]) return 516; //reference made to an illegal object index
    *report = MSX.Node[nodeIndex].rpt;
    return 0;   //Success
}

// Tank getters

int getTankNode(int tankIndex, int *node)
{
    if (tankIndex > MSX.Nobjects[TANK]) return 516; //reference made to an illegal object index
    *node = MSX.Tank[tankIndex].node;
    return 0;   //Success
}

int getTankArea(int tankIndex, double *area)
{
    if (tankIndex > MSX.Nobjects[TANK]) return 516; //reference made to an illegal object index
    *area = MSX.Tank[tankIndex].a;
    return 0;   //Success
}

int getTankInitialVolume(int tankIndex, double *v0) 
{
    if (tankIndex > MSX.Nobjects[TANK]) return 516; //reference made to an illegal object index
    *v0 = MSX.Tank[tankIndex].v0;
    return 0;   //Success
}

int getTankMixModel(int tankIndex, int *mix)
{
    if (tankIndex > MSX.Nobjects[TANK]) return 516; //reference made to an illegal object index
    *mix = MSX.Tank[tankIndex].mixModel;
    return 0;   //Success
}

int getTankMixingSize(int tankIndex, double *vMix)
{
    if (tankIndex > MSX.Nobjects[TANK]) return 516; //reference made to an illegal object index
    *vMix = MSX.Tank[tankIndex].vMix;
    return 0;   //Success
}

int getTankParameter(int tankIndex, int paramIndex, double *val)
{
    if (tankIndex > MSX.Nobjects[TANK]) return 516; //reference made to an illegal object index
    if (paramIndex > MSX.Nobjects[PARAMETER]) return 516;   //reference made to an illegal object index
    *val = MSX.Tank[tankIndex].param[paramIndex];
    return 0;   //Success
}

// Link getters

int getLinkStartNode(int linkIndex, int *startNode)
{
    if (linkIndex > MSX.Nobjects[LINK]) return 516; //reference made to an illegal object index
    *startNode = MSX.Link[linkIndex].n1;
    return 0;   //Success
}

int getLinkEndNode(int linkIndex, int *endNode)
{
    if (linkIndex > MSX.Nobjects[LINK]) return 516; //reference made to an illegal object index
    *endNode = MSX.Link[linkIndex].n2;
    return 0;   //Success
}

int getLinkDiameter(int linkIndex, double *diameter)
{
    if (linkIndex > MSX.Nobjects[LINK]) return 516; //reference made to an illegal object index
    *diameter = MSX.Link[linkIndex].diam;
    return 0;   //Success
}

int getLinkLength(int linkIndex, double *length)
{
    if (linkIndex > MSX.Nobjects[LINK]) return 516; //reference made to an illegal object index
    *length = MSX.Link[linkIndex].len;
    return 0;   //Success
}

int getLinkRoughness(int linkIndex, double *roughness)
{
    if (linkIndex > MSX.Nobjects[LINK]) return 516; //reference made to an illegal object index
    *roughness = MSX.Link[linkIndex].roughness;
    return 0;   //Success
}

int getLinkParameter(int linkIndex, int paramIndex, double *val)
{
    if (linkIndex > MSX.Nobjects[LINK]) return 516; //reference made to an illegal object index
    if (paramIndex > MSX.Nobjects[PARAMETER]) return 516;   //reference made to an illegal object index
    *val = MSX.Link[linkIndex].param[paramIndex];
    return 0;   //Success
}

int getLinkInitialSpeciesConcentration(int linkIndex, int speciesIndex, double *concentration)
{
    if (linkIndex > MSX.Nobjects[LINK]) return 516; //reference made to an illegal object index
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    *concentration = MSX.Node[linkIndex].c0[speciesIndex];
    return 0;   //Success
}

int getLinkReport(int linkIndex, char *report)
{
    if (linkIndex > MSX.Nobjects[LINK]) return 516; //reference made to an illegal object index
    *report = MSX.Link[linkIndex].rpt;
    return 0;   //Success
}

// Species getters

int getSpeciesId(int speciesIndex, char **id)
{
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516;   //reference made to an illegal object index
    *id = MSX.Species[speciesIndex].id;
    return 0;   //Success
}

int getSpeciesType(int speciesIndex, int *type)
{
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    *type = MSX.Species[speciesIndex].type;
    return 0;   //Success
}

int getSpeciesUnits(int speciesIndex, char **units)
{
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    *units = MSX.Species[speciesIndex].units;
    return 0;   //Success
}

int getSpeciesAbsoluteTolerance(int speciesIndex, double *aTol)
{
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516;   //reference made to an illegal object index
    *aTol = MSX.Species[speciesIndex].aTol;
    return 0;   //Success
}

int getSpeciesRelativeTolerance(int speciesIndex, double *rTol)
{
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516;   //reference made to an illegal object index
    *rTol = MSX.Species[speciesIndex].rTol;
    return 0;   //Success
}

int getSpeciesPipeExpressionType(int speciesIndex, int *type)
{
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    *type = MSX.Species[speciesIndex].pipeExprType;
    return 0;   //Success
}

int getSpeciesPipeExpression(int speciesIndex, MathExpr **expr)
{
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    *expr = MSX.Species[speciesIndex].pipeExpr;
    return 0;   //Success
}

int getSpeciesTankExpressionType(int speciesIndex, int *type)
{
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    *type = MSX.Species[speciesIndex].tankExprType;
    return 0;   //Success
}

int getSpeciesTankExpression(int speciesIndex, MathExpr **expr)
{
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    *expr = MSX.Species[speciesIndex].tankExpr;
    return 0;   //Success
}

int getSpeciesReport(int speciesIndex, char *report)
{
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    *report = MSX.Species[speciesIndex].rpt;
    return 0;   //Success
}

int getSpeciesPrecision(int speciesIndex, int *precision)
{
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    *precision = MSX.Species[speciesIndex].precision;
    return 0;   //Success
}

// Parameter getters

int getParameterId(int paramIndex, char **id)
{
    if (paramIndex > MSX.Nobjects[PARAMETER]) return 516;   //reference made to an illegal object index
    *id = MSX.Param[paramIndex].id;
    return 0;   //Success
}

int getParameterValue(int paramIndex, double *value)
{
    if (paramIndex > MSX.Nobjects[PARAMETER]) return 516;   //reference made to an illegal object index
    *value = MSX.Param[paramIndex].value;
    return 0;   //Success
}

// Constant getters

int getConstantId(int constantIndex, char **id)
{
    if (constantIndex > MSX.Nobjects[CONSTANT]) return 516;   //reference made to an illegal object index
    *id = MSX.Const[constantIndex].id;
    return 0;   //Success
}

int getConstantValue(int constantIndex, double *value)
{
    if (constantIndex > MSX.Nobjects[CONSTANT]) return 516;   //reference made to an illegal object index
    *value = MSX.Const[constantIndex].value;
    return 0;   //Success
}

// Term getters

int getTermId(int termIndex, char **id)
{
    if (termIndex > MSX.Nobjects[TERM]) return 516;   //reference made to an illegal object index
    *id = MSX.Term[termIndex].id;
    return 0;   //Success
}

int getTermExpression(int termIndex, MathExpr **expr)
{
    if (termIndex > MSX.Nobjects[TERM]) return 516;   //reference made to an illegal object index
    *expr = MSX.Term[termIndex].expr;
    return 0;   //Success
}

// Pattern getters

int getPatternId(int patternIndex, char **id)
{
    if (patternIndex > MSX.Nobjects[PATTERN]) return 516;   //reference made to an illegal object index
    *id = MSX.Pattern[patternIndex].id;
    return 0;   //Success
}

int getPatternLength(int patternIndex, int *length)
{
    if (patternIndex > MSX.Nobjects[PATTERN]) return 516;   //reference made to an illegal object index
    *length = MSX.Pattern[patternIndex].length;
    return 0;   //Success
}

int getPatternFirstMultiplier(int patternIndex, SnumList **first)
{
    if (patternIndex > MSX.Nobjects[PATTERN]) return 516;   //reference made to an illegal object index
    *first = MSX.Pattern[patternIndex].first;
    return 0;   //Success
}

int getPatternCurrentMultiplier(int patternIndex, SnumList **current)
{
    if (patternIndex > MSX.Nobjects[PATTERN]) return 516;   //reference made to an illegal object index
    *current = MSX.Pattern[patternIndex].current;
    return 0;   //Success
}