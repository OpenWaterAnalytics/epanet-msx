
#ifndef MAGICNUMBER
#include "msxtypes.h"
#endif

int getNobjects(MSXproject *MSX, int type, int *Nobjects) ;

int getFlowUnits(MSXproject *MSX, int *Flowflag, int *Unitsflag);

int getAreaUnits(MSXproject *MSX, int *AreaUnits);

int getRateUnits(MSXproject *MSX, int *RateUnits);

int getSolver(MSXproject *MSX, int *solver);

int getCoupling(MSXproject *MSX, int *coupling);

int getCompiler(MSXproject *MSX, int *compiler);

int getQstep(MSXproject *MSX, int *Qstep);

int getRstep(MSXproject *MSX, int *Rstep);

int getRstart(MSXproject *MSX, int *Rstart);

int getPstep(MSXproject *MSX, int *Pstep);

int getPstart(MSXproject *MSX, int *Pstart);

int getStatflag(MSXproject *MSX, int *Statflag);

int getDefaultRelativeErrorTolerance(MSXproject *MSX, double *DefRtol);

int getDefaultAbsoluteErrorTolerance(MSXproject *MSX, double *DefAtol);

int getInitialQualityVector(MSXproject *MSX, int index, double *value);

// Node getters

int getNodeTank(MSXproject *MSX, int nodeIndex, int *tank);

int getNodeInitialSpeciesConcentration(MSXproject *MSX, int nodeIndex, int speciesIndex, double *concentration);

int getNodeSources(MSXproject *MSX, int nodeIndex, Psource *sources);

int getNodeReport(MSXproject *MSX, int nodeIndex, char *report);

// Tank getters

int getTankNode(MSXproject *MSX, int tankIndex, int *node);

int getTankArea(MSXproject *MSX, int tankIndex, double *area);

int getTankInitialVolume(MSXproject *MSX, int tankIndex, double *v0);

int getTankMixModel(MSXproject *MSX, int tankIndex, int *mix);

int getTankMixingSize(MSXproject *MSX, int tankIndex, double *vMix);

int getTankParameter(MSXproject *MSX, int tankIndex, int paramIndex, double *val);

// Link getters

int getLinkStartNode(MSXproject *MSX, int linkIndex, int *startNode);

int getLinkEndNode(MSXproject *MSX, int linkIndex, int *endNode);

int getLinkDiameter(MSXproject *MSX, int linkIndex, double *diameter);

int getLinkLength(MSXproject *MSX, int linkIndex, double *length);

int getLinkRoughness(MSXproject *MSX, int linkIndex, double *roughness);

int getLinkParameter(MSXproject *MSX, int linkIndex, int paramIndex, double *val);

int getLinkInitialSpeciesConcentration(MSXproject *MSX, int linkIndex, int speciesIndex, double *concentration);

int getLinkReport(MSXproject *MSX, int linkIndex, char *report);

// Species getters

int getSpeciesId(MSXproject *MSX, int speciesIndex, char **id);

int getSpeciesType(MSXproject *MSX, int speciesIndex, int *type);

int getSpeciesUnits(MSXproject *MSX, int speciesIndex, char **units);

int getSpeciesAbsoluteTolerance(MSXproject *MSX, int speciesIndex, double *aTol);

int getSpeciesRelativeTolerance(MSXproject *MSX, int speciesIndex, double *rTol);

int getSpeciesPipeExpressionType(MSXproject *MSX, int speciesIndex, int *type);

int getSpeciesPipeExpression(MSXproject *MSX, int speciesIndex, MathExpr **expr);

int getSpeciesTankExpressionType(MSXproject *MSX, int speciesIndex, int *type);

int getSpeciesTankExpression(MSXproject *MSX, int speciesIndex, MathExpr **expr);

int getSpeciesReport(MSXproject *MSX, int speciesIndex, char *report);

int getSpeciesPrecision(MSXproject *MSX, int speciesIndex, int *precision);

// Parameter getters

int getParameterId(MSXproject *MSX, int paramIndex, char **id);

int getParameterValue(MSXproject *MSX, int paramIndex, double *value);

// Constant getters

int getConstantId(MSXproject *MSX, int constantIndex, char **id);

int getConstantValue(MSXproject *MSX, int constantIndex, double *value);

// Term getters

int getTermId(MSXproject *MSX, int termIndex, char **id);

int getTermExpression(MSXproject *MSX, int termIndex, MathExpr **expr);

// Pattern getters

int getPatternId(MSXproject *MSX, int patternIndex, char **id);

int getPatternLength(MSXproject *MSX, int patternIndex, int *length);

int getPatternFirstMultiplier(MSXproject *MSX, int patternIndex, SnumList **first);

int getPatternCurrentMultiplier(MSXproject *MSX, int patternIndex, SnumList **current);