
int getNobjects(int type, int *Nobjects);

int getAreaUnits(int *AreaUnits);

int getDefaultRelativeErrorTolerance(double *DefRtol);

int getDefaultAbsoluteErrorTolerance(double *DefAtol);

// Node getters

int getNodeTank(int nodeIndex, int *tank);

// int getNodeSources(int nodeIndex, Psource *sources);

// Tank getters


// Species getters

int getSpeciesUnits(int speciesIndex, char **units);

int getSpeciesType(int speciesIndex, int *type);

int getSpeciesPipeExpressionType(int speciesIndex, int *type);

int getSpeciesTankExpressionType(int speciesIndex, int *type);

// Term getters

int getTermId(int termIndex, char **id);

// Pattern getters

int getPatternId(int patternIndex, char **id);

int getPatternLength(int patternIndex, int *length);

// int getPatternFirstMultiplier(int patternIndex, SnumList **first);

// int getPatternCurrentMultiplier(int patternIndex, SnumList **current);