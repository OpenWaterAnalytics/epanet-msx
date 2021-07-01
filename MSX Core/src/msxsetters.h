
#ifndef MAGICNUMBER
#include "msxtypes.h"
#endif

int setDefaults(MSXproject *MSX);

int setNobjects(MSXproject *MSX, int type, int Nobjects);

int setFlowUnits(MSXproject *MSX, int Flowflag, int Unitsflag);

int setAreaUnits(MSXproject *MSX, int AreaUnits);

int setRateUnits(MSXproject *MSX, int RateUnits);

int setSolver(MSXproject *MSX, int solver);

int setCoupling(MSXproject *MSX, int coupling);

int setCompiler(MSXproject *MSX, int compiler);

int setQstep(MSXproject *MSX, int Qstep);

int setRstep(MSXproject *MSX, int Rstep);

int setRstart(MSXproject *MSX, int Rstart);

int setPstep(MSXproject *MSX, int Pstep);

int setPstart(MSXproject *MSX, int Pstart);

int setStatflag(MSXproject *MSX, int Statflag);

int setDefaultRelativeErrorTolerance(MSXproject *MSX, double DefRtol);

int setDefaultAbsoluteErrorTolerance(MSXproject *MSX, double DefAtol);

int setInitialQualityVector(MSXproject *MSX, int index, double value);

// Node setters

int setNodeTank(MSXproject *MSX, int nodeIndex, int tank);

int setNodeInitialSpeciesConcentration(MSXproject *MSX, int nodeIndex, int speciesIndex, double concentration);

int setNodeSources(MSXproject *MSX, int nodeIndex, Psource sources);

int setNodeReport(MSXproject *MSX, int nodeIndex, char report);

// Tank setters

int setTankNode(MSXproject *MSX, int tankIndex, int node);

int setTankArea(MSXproject *MSX, int tankIndex, double area);

int setTankInitialVolume(MSXproject *MSX, int tankIndex, double initialV);

int setTankMixModel(MSXproject *MSX, int tankIndex, int mix);

int setTankMixingSize(MSXproject *MSX, int tankIndex, float vMix);

int setTankParameter(MSXproject *MSX, int tankIndex, int paramIndex, double val);

// Link setters

int setLinkStartNode(MSXproject *MSX, int linkIndex, int startNode);

int setLinkEndNode(MSXproject *MSX, int linkIndex, int endNode);

int setLinkDiameter(MSXproject *MSX, int linkIndex, double diameter);

int setLinkLength(MSXproject *MSX, int linkIndex, double length);

int setLinkRoughness(MSXproject *MSX, int linkIndex, double roughness);

int setLinkParameter(MSXproject *MSX, int linkIndex, int paramIndex, double val);

int setLinkInitialSpeciesConcentration(MSXproject *MSX, int linkIndex, int speciesIndex, double concentration);

int setLinkReport(MSXproject *MSX, int linkIndex, char report);

// Species setters

int setSpeciesId(MSXproject *MSX, int speciesIndex, char *id);

int setSpeciesType(MSXproject *MSX, int speciesIndex, int type);

int setSpeciesUnits(MSXproject *MSX, int speciesIndex, char * units);

int setSpeciesAbsoluteTolerance(MSXproject *MSX, int speciesIndex, double aTol);

int setSpeciesRelativeTolerance(MSXproject *MSX, int speciesIndex, double rTol);

int setSpeciesPipeExpressionType(MSXproject *MSX, int speciesIndex, int type);

int setSpeciesPipeExpression(MSXproject *MSX, int speciesIndex, MathExpr *expr);

int setSpeciesTankExpressionType(MSXproject *MSX, int speciesIndex, int type);

int setSpeciesTankExpression(MSXproject *MSX, int speciesIndex, MathExpr *expr);

int setSpeciesReport(MSXproject *MSX, int speciesIndex, char report);

int setSpeciesPrecision(MSXproject *MSX, int speciesIndex, int precision);

// Parameter setters

int setParameterId(MSXproject *MSX, int paramIndex, char *id);

int setParameterValue(MSXproject *MSX, int paramIndex, double value);

// Constant setters

int setConstantId(MSXproject *MSX, int constantIndex, char *id);

int setConstantValue(MSXproject *MSX, int constantIndex, double value);

// Term setters

int setTermId(MSXproject *MSX, int termIndex, char *id);

int setTermExpression(MSXproject *MSX, int termIndex, MathExpr *expr);

// Pattern setters

int setPatternId(MSXproject *MSX, int patternIndex, char *id);

int setPatternLength(MSXproject *MSX, int patternIndex, int length);

int setPatternFirstMultiplier(MSXproject *MSX, int patternIndex, SnumList *first);

int setPatternCurrentMultiplier(MSXproject *MSX, int patternIndex, SnumList *current);

int setPatternNextMultiplier(MSXproject *MSX, int patternIndex, SnumList *next);

