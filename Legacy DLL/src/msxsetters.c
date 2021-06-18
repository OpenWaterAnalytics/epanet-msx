
#include <stdio.h>

#include "msxsetters.h"
#include "msxtypes.h"

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

// Node setters

int setNodeTank(int nodeIndex, int tank)
{
    if (nodeIndex > MSX.Nobjects[NODE]) return -1; //TODO send error
    MSX.Node[nodeIndex].tank = tank;
    return 0;   //Success
}

// Tank setters

int setTankNode(int tankIndex, int node)
{
    if (tankIndex > MSX.Nobjects[TANK]) return -1; //TODO send error
    MSX.Tank[tankIndex].node = node;
    return 0;   //Success
}

int setTankArea(int tankIndex, double area)
{
    if (tankIndex > MSX.Nobjects[TANK]) return -1; //TODO send error
    MSX.Tank[tankIndex].a = area;
    return 0;   //Success
}

int setTankInitialVolume(int tankIndex, double v0) 
{
    if (tankIndex > MSX.Nobjects[TANK]) return -1; //TODO send error
    MSX.Tank[tankIndex].v0 = v0;
    return 0;   //Success
}

int setTankMixModel(int tankIndex, int mix)
{
    if (tankIndex > MSX.Nobjects[TANK]) return -1; //TODO send error
    MSX.Tank[tankIndex].mixModel = mix;
    return 0;   //Success
}

int setTankMixingSize(int tankIndex, float vMix)
{
    if (tankIndex > MSX.Nobjects[TANK]) return -1; //TODO send error
    MSX.Tank[tankIndex].vMix = vMix;
    return 0;   //Success
}

// Link setters

int setLinkStartNode(int linkIndex, int startNode)
{
    if (linkIndex > MSX.Nobjects[LINK]) return -1; //TODO send error
    MSX.Link[linkIndex].n1 = startNode;
    return 0;   //Success
}

int setLinkEndNode(int linkIndex, int endNode)
{
    if (linkIndex > MSX.Nobjects[LINK]) return -1; //TODO send error
    MSX.Link[linkIndex].n2 = endNode;
    return 0;   //Success
}

int setLinkDiameter(int linkIndex, double diameter)
{
    if (linkIndex > MSX.Nobjects[LINK]) return -1; //TODO send error
    MSX.Link[linkIndex].diam = diameter;
    return 0;   //Success
}

int setLinkLength(int linkIndex, double length)
{
    if (linkIndex > MSX.Nobjects[LINK]) return -1; //TODO send error
    MSX.Link[linkIndex].len = length;
    return 0;   //Success
}

int setLinkRoughness(int linkIndex, double roughness)
{
    if (linkIndex > MSX.Nobjects[LINK]) return -1; //TODO send error
    MSX.Link[linkIndex].roughness = roughness;
    return 0;   //Success
}



