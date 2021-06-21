


int setNobjects(int type, int Nobjects);

int setFlowUnits(int Flowflag, int Unitsflag);

int setAreaUnits(int AreaUnits);

int setRateUnits(int RateUnits);

int setSolver(int solver);

int setCoupling(int coupling);

int setCompiler(int compiler);

int setQstep(int Qstep);

int setRstep(int Rstep);

int setRstart(int Rstart);

int setPstep(int Pstep);

int setPstart(int Pstart);

int setStatflag(int Statflag);

int setDefaultRelativeErrorTolerance(double DefRtol);

int setDefaultAbsoluteErrorTolerance(double DefAtol);

int setInitialQualityVector(int index, double value);

// Node setters

int setNodeTank(int nodeIndex, int tank);

int setNodeInitialSpeciesConcentration(int nodeIndex, int speciesIndex, double concentration);

// int setNodeSources(int nodeIndex, Psource sources);

int setNodeReport(int nodeIndex, char report);

// Tank setters

int setTankNode(int tankIndex, int node);

int setTankArea(int tankIndex, double area);

int setTankInitialVolume(int tankIndex, double initialV);

int setTankMixModel(int tankIndex, int mix);

int setTankMixingSize(int tankIndex, float vMix);

int setTankParameter(int tankIndex, int paramIndex, double val);

// Link setters

int setLinkStartNode(int linkIndex, int startNode);

int setLinkEndNode(int linkIndex, int endNode);

int setLinkDiameter(int linkIndex, double diameter);

int setLinkLength(int linkIndex, double length);

int setLinkRoughness(int linkIndex, double roughness);

int setLinkParameter(int linkIndex, int paramIndex, double val);

int setLinkInitialSpeciesConcentration(int linkIndex, int speciesIndex, double concentration);

int setLinkReport(int linkIndex, char report);

// Species setters

int setSpeciesId(int speciesIndex, char *id);

int setSpeciesType(int speciesIndex, int type);

int setSpeciesUnits(int speciesIndex, char * units);

int setSpeciesAbsoluteTolerance(int speciesIndex, double aTol);

int setSpeciesRelativeTolerance(int speciesIndex, double rTol);

int setSpeciesPipeExpressionType(int speciesIndex, int type);

// int setSpeciesPipeExpression(int speciesIndex, MathExpr *expr);

int setSpeciesTankExpressionType(int speciesIndex, int type);

// int setSpeciesTankExpression(int speciesIndex, MathExpr *expr);

int setSpeciesReport(int speciesIndex, char report);

int setSpeciesPrecision(int speciesIndex, int precision);

// Parameter setters

int setParameterId(int paramIndex, char *id);

int setParameterValue(int paramIndex, double value);

// Constant setters

int setConstantId(int constantIndex, char *id);

int setConstantValue(int constantIndex, double value);

// Term setters

int setTermId(int termIndex, char *id);

// int setTermExpression(int termIndex, MathExpr *expr);

// Pattern setters

int setPatternId(int patternIndex, char *id);

int setPatternLength(int patternIndex, int length);

// int setPatternFirstMultiplier(int patternIndex, SnumList *first);

// int setPatternCurrentMultiplier(int patternIndex, SnumList *current);

// int setPatternNextMultiplier(int patternIndex, SnumList *next);

