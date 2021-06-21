
#include <stdio.h>
#include <string.h>

#include "msxgetters.h"
#include "msxtypes.h"

//  External variables
//--------------------
extern MSXproject  MSX;                // MSX project data

int getNobjects(int type, int *Nobjects) {
    *Nobjects = MSX.Nobjects[type];
    return 0;   //Success
}

int getAreaUnits(int *AreaUnits)
{
    *AreaUnits = MSX.AreaUnits;
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

// Node getters

int getNodeTank(int nodeIndex, int *tank)
{
    if (nodeIndex > MSX.Nobjects[NODE]) return 516; //reference made to an illegal object index
    *tank = MSX.Node[nodeIndex].tank;
    return 0;   //Success
}

// int getNodeSources(int nodeIndex, Psource *sources)
// {
//     if (nodeIndex > MSX.Nobjects[NODE]) return 516; //reference made to an illegal object index
//     *sources = MSX.Node[nodeIndex].sources;
//     return 0;   //Success
// }

// Tank getters



// Species getters

int getSpeciesUnits(int speciesIndex, char **units)
{
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    *units = MSX.Species[speciesIndex].units;
    return 0;   //Success
}

int getSpeciesType(int speciesIndex, int *type)
{
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    *type = MSX.Species[speciesIndex].type;
    return 0;   //Success
}

int getSpeciesPipeExpressionType(int speciesIndex, int *type)
{
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    *type = MSX.Species[speciesIndex].pipeExprType;
    return 0;   //Success
}

int getSpeciesTankExpressionType(int speciesIndex, int *type)
{
    if (speciesIndex > MSX.Nobjects[SPECIES]) return 516; //reference made to an illegal object index
    *type = MSX.Species[speciesIndex].tankExprType;
    return 0;   //Success
}

// Term getters

int getTermId(int termIndex, char **id)
{
    if (termIndex > MSX.Nobjects[TERM]) return 516;   //reference made to an illegal object index
    *id = MSX.Term[termIndex].id;
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

// int getPatternFirstMultiplier(int patternIndex, SnumList **first)
// {
//     if (patternIndex > MSX.Nobjects[PATTERN]) return 516;   //reference made to an illegal object index
//     *first = MSX.Pattern[patternIndex].first;
//     return 0;   //Success
// }

// int getPatternCurrentMultiplier(int patternIndex, SnumList **current)
// {
//     if (patternIndex > MSX.Nobjects[PATTERN]) return 516;   //reference made to an illegal object index
//     *current = MSX.Pattern[patternIndex].current;
//     return 0;   //Success
// }