
#ifndef MAGICNUMBER
#include "msxtypes.h"
#endif

int getNobjects(int type, int *Nobjects) ;

int getFlowUnits(int *Flowflag, int *Unitsflag);

int getAreaUnits(int *AreaUnits);

int getRateUnits(int *RateUnits);

int getSolver(int *solver);

int getCoupling(int *coupling);

int getCompiler(int *compiler);

int getQstep(int *Qstep);

int getRstep(int *Rstep);

int getRstart(int *Rstart);

int getPstep(int *Pstep);

int getPstart(int *Pstart);

int getStatflag(int *Statflag);

int getDefaultRelativeErrorTolerance(double *DefRtol);

int getDefaultAbsoluteErrorTolerance(double *DefAtol);

int getInitialQualityVector(int index, double *value);

// Node getters

int getNodeTank(int nodeIndex, int *tank);

int getNodeInitialSpeciesConcentration(int nodeIndex, int speciesIndex, double *concentration);

int getNodeSources(int nodeIndex, Psource *sources);

int getNodeReport(int nodeIndex, char *report);

// Tank getters

int getTankNode(int tankIndex, int *node);

int getTankArea(int tankIndex, double *area);

int getTankInitialVolume(int tankIndex, double *v0);

int getTankMixModel(int tankIndex, int *mix);

int getTankMixingSize(int tankIndex, double *vMix);

int getTankParameter(int tankIndex, int paramIndex, double *val);

// Link getters

int getLinkStartNode(int linkIndex, int *startNode);

int getLinkEndNode(int linkIndex, int *endNode);

int getLinkDiameter(int linkIndex, double *diameter);

int getLinkLength(int linkIndex, double *length);

int getLinkRoughness(int linkIndex, double *roughness);

int getLinkParameter(int linkIndex, int paramIndex, double *val);

int getLinkInitialSpeciesConcentration(int linkIndex, int speciesIndex, double *concentration);

int getLinkReport(int linkIndex, char *report);

// Species getters

int getSpeciesId(int speciesIndex, char **id);

int getSpeciesType(int speciesIndex, int *type);

int getSpeciesUnits(int speciesIndex, char **units);

int getSpeciesAbsoluteTolerance(int speciesIndex, double *aTol);

int getSpeciesRelativeTolerance(int speciesIndex, double *rTol);

int getSpeciesPipeExpressionType(int speciesIndex, int *type);

int getSpeciesPipeExpression(int speciesIndex, MathExpr **expr);

int getSpeciesTankExpressionType(int speciesIndex, int *type);

int getSpeciesTankExpression(int speciesIndex, MathExpr **expr);

int getSpeciesReport(int speciesIndex, char *report);

int getSpeciesPrecision(int speciesIndex, int *precision);

// Parameter getters

int getParameterId(int paramIndex, char **id);

int getParameterValue(int paramIndex, double *value);

// Constant getters

int getConstantId(int constantIndex, char **id);

int getConstantValue(int constantIndex, double *value);

// Term getters

int getTermId(int termIndex, char **id);

int getTermExpression(int termIndex, MathExpr **expr);

// Pattern getters

int getPatternId(int patternIndex, char **id);

int getPatternLength(int patternIndex, int *length);

int getPatternFirstMultiplier(int patternIndex, SnumList **first);

int getPatternCurrentMultiplier(int patternIndex, SnumList **current);