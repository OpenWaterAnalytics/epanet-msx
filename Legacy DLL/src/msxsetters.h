
/*
#include "msxtypes.h"

int setNobjects(MSXproject *MSX, int type, int Nobjects);

int setFlowUnits(MSXproject *MSX, int Flowflag, int Unitsflag);

int setQstep(MSXproject *MSX, int Qstep);

int setRstep(MSXproject *MSX, int Rstep);

int setRstart(MSXproject *MSX, int Rstart);

int setPstep(MSXproject *MSX, int Pstep);

int setPstart(MSXproject *MSX, int Pstart);

int setStatflag(MSXproject *MSX, int Statflag);

// Node setters

int setNodeTank(MSXproject *MSX,int nodeIndex, int tank);

// Tank setters

int setTankArea(MSXproject *MSX, int tankIndex, double area);

int setTankInitialVolume(MSXproject *MSX, int tankIndex, double initialV);

int setTankMixModel(MSXproject *MSX, int tankIndex, int mix);

int setTankMixingSize(MSXproject *MSX, int tankIndex, float vMix);

// Link setters

int setLinkStartNode(MSXproject *MSX, int linkIndex, int startNode);

int setLinkEndNode(MSXproject *MSX, int linkIndex, int endNode);

int setLinkDiameter(MSXproject *MSX, int linkIndex, double diameter);

int setLinkLength(MSXproject *MSX, int linkIndex, double length);

int setLinkRoughness(MSXproject *MSX, int linkIndex, double roughness);

*/


int setNobjects(int type, int Nobjects);

int setFlowUnits(int Flowflag, int Unitsflag);

int setQstep(int Qstep);

int setRstep(int Rstep);

int setRstart(int Rstart);

int setPstep(int Pstep);

int setPstart(int Pstart);

int setStatflag(int Statflag);

// Node setters

int setNodeTank(int nodeIndex, int tank);

// Tank setters

int setTankNode(int tankIndex, int node);

int setTankArea(int tankIndex, double area);

int setTankInitialVolume(int tankIndex, double initialV);

int setTankMixModel(int tankIndex, int mix);

int setTankMixingSize(int tankIndex, float vMix);

// Link setters

int setLinkStartNode(int linkIndex, int startNode);

int setLinkEndNode(int linkIndex, int endNode);

int setLinkDiameter(int linkIndex, double diameter);

int setLinkLength(int linkIndex, double length);

int setLinkRoughness(int linkIndex, double roughness);