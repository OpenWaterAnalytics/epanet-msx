/******************************************************************************
**  TITLE:         QUALITYMSX.C
**  DESCRIPTION:   water quality routing routines for the EPANET Multi-Species
**                 Extension toolkit.
**  AUTHORS:       L. Rossman, US EPA - NRMRL
**                 F. Shang, University of Cincinnati
**                 J. Uber, University of Cincinnati
**  VERSION:       1.00
**  LAST UPDATE:   11/6/06
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "typesmsx.h"
#define  EXTERN_MSX extern
#include "globals.h"
#include "mempool.h"
#include "utils.h"

// Macros to identify upstream & downstream nodes of a link
// under the current flow and to compute link volume
//
#define   UP_NODE(x)   ( (FlowDir[(x)]=='+') ? Link[(x)].n1 : Link[(x)].n2 )
#define   DOWN_NODE(x) ( (FlowDir[(x)]=='+') ? Link[(x)].n2 : Link[(x)].n1 )
#define   LINKVOL(k)   ( 0.785398*Link[(k)].len*SQR(Link[(k)].diam) )

//  Exported variables
//--------------------
double  *C1;                           // vector of species concentrations
Pseg    *FirstSeg;                     // first (downstream) segment in each pipe

//  Local variables
//-----------------
static Pseg           FreeSeg;         // pointer to unused pipe segment
static Pseg           *LastSeg;        // last (upstream) segment in each pipe
static Pseg           *NewSeg;         // new segment added to each pipe
static char           *FlowDir;        // flow direction for each pipe
static double         *VolIn;          // inflow flow volume to each node
static double         **MassIn;        // mass inflow of each specie to each node
static double         **X;             // work matrix
static char           HasWallSpecies;  // wall species indicator
static char           OutOfMemory;     // out of memory indicator
static alloc_handle_t *QualPool;       // memory pool

//  Imported functions
//--------------------
int    chemistry_open(void);
void   chemistry_close(void);
int    chemistry_react(long dt);
int    chemistry_equil(int zone, double *c);
int    output_open(void);
int    output_saveResults(void);
int    output_saveFinalResults(void);

//  Exported functions
//--------------------
int    quality_open(void);
int    quality_init(void);
int    quality_step(long *t, long *tleft);
int    quality_close(void);
double quality_getNodeQual(int j, int m);
double quality_getLinkQual(int k, int m);

//  Local functions
//-----------------
static int    getHydVars(void);
static int    transport(long tstep);
static void   initSegs(void);
static void   reorientSegs(void);
static void   removeSeg(Pseg seg);
static void   removeAllSegs(int k);
static Pseg   getFreeSeg(double v, double c[]);
static void   addSeg(int k, Pseg seg);
static void   advectSegs(long dt);
static void   getNewSegWallQual(int k, long dt, Pseg seg);
static void   shiftSegWallQual(int k, long dt);
static void   accumulate(long dt);
static void   getIncidentConcen(void);
static void   updateNodes(long dt);
static void   sourceInput(long dt);
static void   addSource(int n, Psource source, double v, long dt);
static void   release(long dt);
static void   tankMix1(int i, long dt);
static double getSourceQual(Psource source);

static void   mixWallQual(int k, Pseg newseg);

//=============================================================================

int  quality_open()
/*
**   Purpose:
**     opens the WQ routing system.
**
**   Returns:
**     an error code (0 if no errors).
*/
{
    int errcode = 0;
    int n;

// --- set flags
    QualityOpened = FALSE;
    OutOfMemory = FALSE;
    HasWallSpecies = FALSE;
    Saveflag = 0;

// --- initialize array pointers to null

    X = NULL;
    C1 = NULL;
    FirstSeg = NULL;
    LastSeg = NULL;
    NewSeg = NULL;
    FlowDir = NULL;
    VolIn = NULL;
    MassIn = NULL;

// --- open the chemistry system

    errcode = chemistry_open();
    if ( errcode > 0 ) return errcode;

// --- allocate a memory pool for pipe segments

    QualPool = AllocInit();
    if ( QualPool == NULL ) return ERR_MEMORY;

// --- allocate memory used for species concentrations

    X  = createMatrix(Nobjects[NODE]+1, Nobjects[SPECIE]+1);
    C1 = (double *) calloc(Nobjects[SPECIE]+1, sizeof(double));

// --- allocate memory used for pointers to the first, last,
//     and new WQ segments in each link and tank

    n        = Nobjects[LINK] + Nobjects[TANK] + 1;
    FirstSeg = (Pseg *) calloc(n, sizeof(Pseg));
    LastSeg  = (Pseg *) calloc(n, sizeof(Pseg));
    NewSeg   = (Pseg *) calloc(n, sizeof(Pseg));

// --- allocate memory used flow direction in each link

    FlowDir  = (char *) calloc(n, sizeof(char));

// --- allocate memory used to accumulate mass and volume
//     inflows to each node

    n        = Nobjects[NODE] + 1;
    VolIn    = (double *) calloc(n, sizeof(double));
    MassIn   = createMatrix(n, Nobjects[SPECIE]+1);

// --- check for successful memory allocation

    ERRCODE(MEMCHECK(X));
    ERRCODE(MEMCHECK(C1));
    ERRCODE(MEMCHECK(FirstSeg));
    ERRCODE(MEMCHECK(LastSeg));
    ERRCODE(MEMCHECK(NewSeg));
    ERRCODE(MEMCHECK(FlowDir));
    ERRCODE(MEMCHECK(VolIn));
    ERRCODE(MEMCHECK(MassIn));

// --- check if wall species are present

    for (n=1; n<=Nobjects[SPECIE]; n++)
    {
        if ( Specie[n].type == WALL ) HasWallSpecies = TRUE;
    }
    if ( !errcode ) QualityOpened = TRUE;
    return(errcode);
}

//=============================================================================

int  quality_init()
/*
**  Purpose:
**     re-initializes the WQ routing system.
**
**  Input:
**    none.
**
**  Returns:
**    an error code (or 0 if no errors).
*/
{
    int i, n, m;
    int errcode = 0;

// --- initialize node concentrations, tank volumes, & source mass flows

    for (i=1; i<=Nobjects[NODE]; i++)
    {
        for (m=1; m<=Nobjects[SPECIE]; m++) Node[i].c[m] = Node[i].c0[m];
    }
    for (i=1; i<=Nobjects[TANK]; i++)
    {
        Tank[i].hstep = 0.0;
        Tank[i].v = Tank[i].v0;
        n = Tank[i].node;
        for (m=1; m<=Nobjects[SPECIE]; m++) Tank[i].c[m] = Node[n].c0[m];
    }
    for (i=1; i<=Nobjects[TIME_PATTERN]; i++)
    {
        Pattern[i].interval = 0;
        Pattern[i].current = Pattern[i].first;
    }

// --- check if a separate WQ report is required

    Rptflag = 0;
    n = 0;
    for (i=1; i<=Nobjects[NODE]; i++) n += Node[i].rpt;
    for (i=1; i<=Nobjects[LINK]; i++) n += Link[i].rpt;
    if ( n > 0 )
    {
        n = 0;
        for (m=1; m<=Nobjects[SPECIE]; m++) n += Specie[m].rpt;
    }
    if ( n > 0 ) Rptflag = 1;
    if ( Rptflag ) Saveflag = 1;

// --- reset memory pool

    AllocSetPool(QualPool);
    FreeSeg = NULL;
    AllocReset();

// --- re-position hydraulics file

    fseek(HydFile.file, HydOffset, SEEK_SET);

// --- set elapsed times to zero

    Htime = 0;                         //Hydraulic solution time
    Qtime = 0;                         //Quality routing time
    Rtime = Rstart;                    //Reporting time
    Nperiods = 0;                      //Number fo reporting periods

// --- open binary output file if results are to be saved

    if ( Saveflag ) errcode = output_open();
    return errcode;
}

//=============================================================================

int quality_step(long *t, long *tleft)
/*
**  Purpose:
**    updates WQ conditions over a single WQ time step.
**
**  Input:
**    none.
**
**  Output:
**    *t = current simulation time (sec)
**    *tleft = time left in simulation (sec)
**
**  Returns:
**    an error code:
**      0 = no error
**      101 = memory error
**      307 = can't read hydraulics file
**      513 = can't integrate reaction rates
*/
{
    long dt, hstep, tstep;
    int  errcode = 0;

// --- set the shared memory pool to the water quality pool
//     and the overall time step to nominal WQ time step

    AllocSetPool(QualPool);
    tstep = Qstep;

// --- repeat until the end of the time step

    do
    {
    // --- find the time until the next hydraulic event occurs
        dt = tstep;
        hstep = Htime - Qtime;

    // --- check if next hydraulic event occurs within the current time step

        if (hstep <= dt)
        {

        // --- reduce current time step to end at next hydraulic event
            dt = hstep;

        // --- route WQ over this time step
            if ( dt > 0 ) ERRCODE(transport(dt));
            Qtime += dt;

        // --- retrieve new hydraulic solution
            if ( Qtime == Htime ) ERRCODE(getHydVars());

        // --- report results if its time to do so
            if (Saveflag && Qtime == Rtime)
            {
                ERRCODE(output_saveResults());
                Rtime += Rstep;
                Nperiods++;
            }
        }

    // --- otherwise just route WQ over the current time step

        else
        {
            ERRCODE(transport(dt));
            Qtime += dt;
        }

    // --- reduce overall time step by the size of the current time step

        tstep -= dt;
        if (OutOfMemory) errcode = 101;
    } while (!errcode && tstep > 0);

// --- update the current time into the simulation and the amount remaining

    *t = Qtime;
    *tleft = Dur - Qtime;

// --- if there's no time remaining, then save the final records to output file

    if ( *tleft <= 0 && Saveflag )
    {
        ERRCODE(output_saveFinalResults());
    }
    return errcode;
}

//=============================================================================

double  quality_getNodeQual(int j, int m)
/*
**   Purpose:
**     retrieves WQ for specie m at node n.
**
**   Input:
**     j = node index
**     m = species index.
**
**   Returns:
**     WQ value of node.
*/
{
    int k;

// --- return 0 for WALL species

    if ( Specie[m].type == WALL ) return 0.0;

// --- if node is a tank, return its internal concentration

    k = Node[j].tank;
    if (k > 0 && Tank[k].a > 0.0)
    {
        return Tank[k].c[m];
    }

// --- otherwise return node's concentration (which includes
//     any contribution from external sources)

    return Node[j].c[m];
}

//=============================================================================

double  quality_getLinkQual(int k, int m)
/*
**   Purpose:
**     computes average quality in link k.
**
**   Input:
**     k = link index
**     m = species index.
**
**   Returns:
**     WQ value of link.
*/
{
    double  vsum = 0.0,
            msum = 0.0;
    Pseg    seg;

    seg = FirstSeg[k];
    while (seg != NULL)
    {
        vsum += seg->v;
        msum += (seg->c[m])*(seg->v);
        seg = seg->prev;
    }
    if (vsum > 0.0) return(msum/vsum);
    else
    {
        return (quality_getNodeQual(Link[k].n1, m) +
                quality_getNodeQual(Link[k].n2, m)) / 2.0;
    }
}

//=============================================================================

int quality_close()
/*
**   Purpose:
**     closes the WQ routing system.
**
**   Input:
**     none.
**
**   Returns:
**     error code (0 if no error).
*/
{
    int errcode = 0;
    if ( !ProjectOpened ) return 0;
    chemistry_close();
    FREE(FirstSeg);
    FREE(LastSeg);
    FREE(NewSeg);
    FREE(FlowDir);
    FREE(VolIn);
    FREE(C1);
    freeMatrix(MassIn);
    freeMatrix(X);
    if ( QualPool)
    {
        AllocSetPool(QualPool);
        AllocFreePool();
    }
    QualityOpened = FALSE;
    return errcode;
}

//=============================================================================

int  getHydVars()
/*
**   Purpose:
**     retrieves hydraulic solution and time step for next hydraulic event
**     from a hydraulics file.
**
**   Input:
**     none.
**
**   Returns:
**     error code (0 if no error).
**
**   NOTE:
**     A hydraulic solution consists of the current time
**     (hydtime), nodal demands (D) and heads (H), link
**     flows (Q), and link status values and settings (which are not used).
*/
{
    int n, errcode = 0;
    long hydtime, hydstep;

// --- read hydraulic time, demands, heads, and flows from the file

    if (fread(&hydtime, sizeof(long), 1, HydFile.file) < 1)  return 307;
    n = Nobjects[NODE];
    if (fread(D+1, sizeof(float), n, HydFile.file) < (unsigned)n) return 307;
    if (fread(H+1, sizeof(float), n, HydFile.file) < (unsigned)n) return 307;
    n = Nobjects[LINK];
    if (fread(Q+1, sizeof(float), n, HydFile.file) < (unsigned)n) return 307;

// --- skip over link status and settings

    fseek(HydFile.file, 2*n*sizeof(float), SEEK_CUR);

// --- read time step until next hydraulic event

    if (fread(&hydstep, sizeof(long), 1, HydFile.file) < 1) return 307;

// --- update elapsed time until next hydraulic event

    Htime = hydtime + hydstep;

// --- initialize pipe segments (at time 0) or else re-orient segments
//     to accommodate any flow reversals

    if (Qtime < Dur)
    {
        if (Qtime == 0) initSegs();
        else reorientSegs();
    }
    return(errcode);
}

//=============================================================================

int  transport(long tstep)
/*
**  Purpose:
**    transports constituent mass through pipe network
**    under a period of constant hydraulic conditions.
**
**  Input:
**    tstep = length of current time step (sec).
**
**  Returns:
**    an error code or 0 if no error.
*/
{
    long qtime, dt;
    int  errcode = 0;

// --- repeat until time step is exhausted

    qtime = 0;
    while (!OutOfMemory &&
           !errcode &&
           qtime < tstep)
    {                                  // Qstep is nominal quality time step
        dt = MIN(Qstep, tstep-qtime);  // get actual time step
        qtime += dt;                   // update amount of input tstep taken
        errcode = chemistry_react(dt); // react species in each pipe & tank
        if ( errcode ) return errcode;
        advectSegs(dt);                // advect segments in each pipe
        accumulate(dt);                // accumulate all inflows at nodes
        updateNodes(dt);               // update nodal quality
        sourceInput(dt);               // compute nodal inputs from sources
        release(dt);                   // release new outflows from nodes
   }
   return errcode;
}

//=============================================================================

void  initSegs()
/*
**   Purpose:
**     initializes water quality in pipe segments.
**
**   Input:
**     none.
*/
{
    int     j, k, m;
    double  v;

// --- examine each link

    for (k=1; k<=Nobjects[LINK]; k++)
    {
    // --- establish flow direction

        FlowDir[k] = '+';
        if (Q[k] < 0.) FlowDir[k] = '-';

    // --- start with no segments

        LastSeg[k] = NULL;
        FirstSeg[k] = NULL;
        NewSeg[k] = NULL;

    // --- use quality of downstream node for BULK species
    //     if no initial link quality supplied

        j = DOWN_NODE(k);
        for (m=1; m<=Nobjects[SPECIE]; m++)
        {
            if ( Link[k].c0[m] != MISSING )    C1[m] = Link[k].c0[m];
            else if ( Specie[m].type == BULK ) C1[m] = Node[j].c0[m];
            else                               C1[m] = 0.0;
        }

    // --- fill link with a single segment of this quality

        v = LINKVOL(k);
        if ( v > 0.0 ) addSeg(k, getFreeSeg(LINKVOL(k), C1));
    }

// --- initialize segments in tanks that use them

    for (j=1; j<=Nobjects[TANK]; j++)
    {
    // --- skip reservoirs & complete mix tanks

        if (Tank[j].a == 0.0
        ||  Tank[j].mixModel == MIX1) continue;

    // --- tank segment pointers are stored after those for links

        k = Tank[j].node;
        for (m=1; m<=Nobjects[SPECIE]; m++) C1[m] = Node[k].c0[m];
        k = Nobjects[LINK] + j;
        LastSeg[k] = NULL;
        FirstSeg[k] = NULL;

    // --- add 2 segments for 2-compartment model

        if (Tank[j].mixModel == MIX2)
        {
            v = MAX(0, Tank[j].v-Tank[j].v1max);
            addSeg(k, getFreeSeg(v, C1));
            v = Tank[j].v - v;
            addSeg(k, getFreeSeg(v, C1));
        }

    // --- add one segment for FIFO & LIFO models

        else
        {
            v = Tank[j].v;
            addSeg(k, getFreeSeg(v, C1));
        }
    }
}

//=============================================================================

void  reorientSegs()
/*
**   Purpose:
**     re-orients pipe segments (if flow reverses).
**
**   Input:
**     none.
*/
{
    Pseg   seg, nseg, pseg;
    int    k;
    char   newdir;

// --- examine each link

    for (k=1; k<=Nobjects[LINK]; k++)
    {
    // --- find new flow direction

        newdir = '+';
        if (Q[k] == 0.0)     newdir = FlowDir[k];
        else if (Q[k] < 0.0) newdir = '-';

    // --- if direction changes, then reverse the order of segments
    //     (first to last) and save new direction

        if (newdir != FlowDir[k])
        {
            seg = FirstSeg[k];
            FirstSeg[k] = LastSeg[k];
            LastSeg[k] = seg;
            pseg = NULL;
            while (seg != NULL)
            {
                nseg = seg->prev;
                seg->prev = pseg;
                seg->next = nseg;
                pseg = seg;
                seg = nseg;
            }
            FlowDir[k] = newdir;
        }
    }
}

//=============================================================================

void removeSeg(Pseg seg)
/*
**   Purpose:
**     places a WQ segment back into the memory pool of segments.
**
**   Input:
**     seg = pointer to a WQ segment.
*/
{
    if ( seg == NULL ) return;
    seg->prev = FreeSeg;
    seg->next = NULL;
    FreeSeg = seg;
}

//=============================================================================

void  removeAllSegs(int k)
/*
**   Purpose:
**     removes all segments in a pipe link.
**
**   Input:
**     k = link index.
*/
{
    Pseg seg;
    seg = FirstSeg[k];
    while (seg != NULL)
    {
        FirstSeg[k] = seg->prev;
        removeSeg(seg);
        seg = FirstSeg[k];
    }
    LastSeg[k] = NULL;
}

//=============================================================================

Pseg getFreeSeg(double v, double c[])
/*
**   Purpose:
**     retrieves an unused water quality volume segment from the memory pool.
**
**   Input:
**     v = segment volume (ft3)
**     c[] = segment quality
**
**   Returns:
**     a pointer to an unused water quality segment.
*/
{
    Pseg seg;
    int  m;

// --- try using the last discarded segment if one is available

    if (FreeSeg != NULL)
    {
        seg = FreeSeg;
        FreeSeg = seg->prev;
    }

// --- otherwise create a new segment from the memory pool

    else
    {
        seg = (struct Sseg *) Alloc(sizeof(struct Sseg));
        if (seg == NULL)
        {
            OutOfMemory = TRUE;
            return NULL;
        }
        seg->c = (double *) Alloc((Nobjects[SPECIE]+1)*sizeof(double));
        if ( seg->c == NULL )
        {
            OutOfMemory = TRUE;
            return NULL;
        }
    }

// --- assign volume, WQ, & integration time step to the new segment

    seg->v = v;
    for (m=1; m<=Nobjects[SPECIE]; m++) seg->c[m] = c[m];
    seg->hstep = 0.0;
    return seg;
}

//=============================================================================

void  addSeg(int k, Pseg seg)
/*
**   Purpose:
**     adds a new segment to the upstream end of a link.
**
**   Input:
**     k = link index
**     seg = pointer to a free WQ segment.
*/
{
    seg->prev = NULL;
    seg->next = NULL;
    if (FirstSeg[k] == NULL) FirstSeg[k] = seg;
    if (LastSeg[k] != NULL)
    {
        LastSeg[k]->prev = seg;
        seg->next = LastSeg[k];
    }
    LastSeg[k] = seg;
}

//=============================================================================

void advectSegs(long dt)
/*
**   Purpose:
**     advects WQ segments within each pipe.
**
**   Input:
**     dt = current WQ time step (sec).
*/
{
    int k, m;

// --- examine each link

    for (k=1; k<=Nobjects[LINK]; k++)
    {
    // --- zero out WQ in new segment to be added at entrance of link

        for (m=1; m<=Nobjects[SPECIE]; m++) C1[m] = 0.0;

    // --- get a free segment to add to entrance of link

        NewSeg[k] = getFreeSeg(0.0, C1);

    // --- skip zero-length links (pumps & valves) & no-flow links

        if ( NewSeg[k] == NULL ||
             Link[(k)].len == 0.0 || Q[k] == 0.0 ) continue;

    // --- find conc. of wall species in new segment to be added
    //     and adjust conc. of wall species to reflect shifted
    //     positions of existing segments

        if ( HasWallSpecies )
        {
            getNewSegWallQual(k, dt, NewSeg[k]);
            shiftSegWallQual(k, dt);
        //      mixWallQual(k, NewSeg[k]);
        }
    }
}

void mixWallQual(int k, Pseg newseg)
{
    int m;
    Pseg  seg;

    for (m=1; m<=Nobjects[SPECIE]; m++)
    {
        if ( Specie[m].type == WALL ) newseg->c[m] = quality_getLinkQual(k, m);
    }
    seg = FirstSeg[k];
    while (seg != NULL)
    {
        for (m=1; m<=Nobjects[SPECIE]; m++)
        {
            if ( Specie[m].type == WALL ) seg->c[m] = newseg->c[m];
        }
        seg = seg->prev;
    }
}

//=============================================================================

void getNewSegWallQual(int k, long dt, Pseg newseg)
/*
**  Purpose:
**     computes wall specie concentrations for a new WQ segment that
**     enters a pipe from its upstream node.
**
**  Input:
**    k = link index
**    dt = current WQ time step (sec)
**    newseg = pointer to a new, unused WQ segment
**
**  Output:
**    newseg->c[] = wall species concentrations in the new WQ segment
*/
{
    Pseg  seg;
    int   m;
    double v, vin, vsum, vadded, vleft;

// --- get volume of inflow to link

    if ( newseg == NULL ) return;
    v = LINKVOL(k);
	vin = ABS(Q[k])*dt;
    if (vin > v) vin = v;

// --- start at last (most upstream) existing WQ segment

	seg = LastSeg[k];
	vsum = 0.0;
    vleft = vin;
    for (m = 1; m <= Nobjects[SPECIE]; m++)
    {
        if ( Specie[m].type == WALL ) newseg->c[m] = 0.0;
    }

// --- repeat while some inflow volume still remains

    while ( vleft > 0.0 && seg != NULL )
    {

    // --- find volume added by this segment

        vadded = seg->v;
        if ( vadded > vleft ) vadded = vleft;

    // --- update total volume added and inflow volume remaining

        vsum += vadded;
        vleft -= vadded;

    // --- add wall species mass contributed by this segment to new segment

        for (m = 1; m <= Nobjects[SPECIE]; m++)
        {
            if ( Specie[m].type == WALL ) newseg->c[m] += vadded*seg->c[m];
        }

    // --- move to next downstream WQ segment

        seg = seg->next;
    }

// --- convert mass of wall species in new segment to concentration

    if ( vsum > 0.0 )
    {
        for (m = 1; m <= Nobjects[SPECIE]; m++)
        {
            if ( Specie[m].type == WALL ) newseg->c[m] /= vsum;
        }
    }
}

//=============================================================================

void shiftSegWallQual(int k, long dt)
/*
**  Purpose:
**    recomputes wall specie concentrations in segments that remain
**    within a pipe after flow is advected over current time step.
**
**  Input:
**    k = link index
**    dt = current WQ time step (sec)
*/
{
    Pseg  seg1, seg2;
    int   m;
    double v, vin, vstart, vend, vcur, vsum;

// --- find volume of water displaced in pipe

    v = LINKVOL(k);
	vin = ABS(Q[k])*dt;
    if (vin > v) vin = v;

// --- set future start position (measured by pipe volume) of original last segment

    vstart = vin;

// --- examine each segment, from upstream to downstream

    for( seg1 = LastSeg[k]; seg1 != NULL; seg1 = seg1->next )
    {
    // --- initialize a "mixture" WQ

        for (m = 1; m <= Nobjects[SPECIE]; m++) C1[m] = 0.0;

    // --- find the future end position of this segment

        vend = vstart + seg1->v;
        if (vend > v) vend = v;
        vcur = vstart;
        vsum = 0;

    // --- find volume taken up by the segment after it moves down the pipe

        for (seg2 = LastSeg[k]; seg2 != NULL; seg2 = seg2->next)
        {
            if ( seg2->v == 0.0 ) continue;
            vsum += seg2->v;
            if ( vsum >= vstart && vsum <= vend )
            {
                for (m = 1; m <= Nobjects[SPECIE]; m++)
                {
                    if ( Specie[m].type == WALL )
                        C1[m] += (vsum - vcur) * seg2->c[m];
                }
                vcur = vsum;
            }
            if ( vsum >= vend ) break;
        }

    // --- update the wall specie concentrations in the segment

        for (m = 1; m <= Nobjects[SPECIE]; m++)
        {
            if ( Specie[m].type != WALL ) continue;
            if (seg2 != NULL) C1[m] += (vend - vcur) * seg2->c[m];
            seg1->c[m] = C1[m] / (vend - vstart);
            if ( seg1->c[m] < 0.0 ) seg1->c[m] = 0.0;
        }

    // --- re-start at the current end location

        vstart = vend;
        if ( vstart >= v ) break;
    }
}

//=============================================================================

void accumulate(long dt)
/*
**  Purpose:
**    accumulates mass inflow at downstream node of each link.
**
**  Input:
**    dt = current WQ time step (sec).
*/
{
    int    i, j, k, m, n;
    double cseg, v, vseg;
    Pseg   seg;

// --- compute average conc. of segments incident on each node
//     (for use if there is no transport through the node)

    getIncidentConcen();

// --- reset cumlulative inflow to each node to zero

    memset(VolIn, 0, (Nobjects[NODE]+1)*sizeof(double));
    n = (Nobjects[NODE]+1)*(Nobjects[SPECIE]+1);
    memset(MassIn[0], 0, n*sizeof(double));

// --- move mass from first segment of each link into link's downstream node

    for (k=1; k<=Nobjects[LINK]; k++)
    {
        i = UP_NODE(k);               // upstream node
        j = DOWN_NODE(k);             // downstream node
        v = ABS(Q[k])*dt;             // flow volume

    // --- if link volume < flow volume, then transport upstream node's
    //     quality to downstream node and remove all link segments

        if (LINKVOL(k) < v)
        {
            VolIn[j] += v;
            seg = FirstSeg[k];
            for (m=1; m<=Nobjects[SPECIE]; m++)
            {
                if ( Specie[m].type != BULK ) continue;
                cseg = Node[i].c[m];
                if (seg != NULL) cseg = seg->c[m];
                MassIn[j][m] += v*cseg;
             }
             removeAllSegs(k);
        }

    // --- otherwise remove flow volume from leading segments
    //     and accumulate flow mass at downstream node

        else while (v > 0.0)
        {
        // --- identify leading segment in pipe

            seg = FirstSeg[k];
            if (seg == NULL) break;

        // --- volume transported from this segment is
        //     minimum of remaining flow volume & segment volume
        //     (unless leading segment is also last segment)

            vseg = seg->v;
            vseg = MIN(vseg, v);
            if (seg == LastSeg[k]) vseg = v;

        // --- update volume & mass entering downstream node

            for (m=1; m<=Nobjects[SPECIE]; m++)
            {
                if ( Specie[m].type != BULK ) continue;
                cseg = seg->c[m];
                MassIn[j][m] += vseg*cseg;
            }
            VolIn[j] += vseg;

        // --- reduce flow volume by amount transported

            v -= vseg;

        // --- if all of segment's volume was transferred, then
        //     replace leading segment with the one behind it
        //     (Note that the current seg is recycled for later use.)

            if (v >= 0.0 && vseg >= seg->v)
            {
                FirstSeg[k] = seg->prev;
                if (FirstSeg[k] == NULL) LastSeg[k] = NULL;
                removeSeg(seg);
            }

        // --- otherwise reduce segment's volume

            else
            {
                seg->v -= vseg;
            }

       } // End while

    } // Next link
}

//=============================================================================

void getIncidentConcen()
/*
**  Purpose:
**    determines average WQ for bulk species in link end segments that are
**    incident on each node.
**
**  Input:
**    none
*/
{
    int j, k, m, n;

// --- zero-out memory used to store accumulated totals

    memset(VolIn, 0, (Nobjects[NODE]+1)*sizeof(double));
    n = (Nobjects[NODE]+1)*(Nobjects[SPECIE]+1);
    memset(MassIn[0], 0, n*sizeof(double));
    memset(X[0], 0, n*sizeof(double));

// --- examine each link

    for (k=1; k<=Nobjects[LINK]; k++)
    {
        j = DOWN_NODE(k);             // downstream node
        if (FirstSeg[k] != NULL)      // accumulate concentrations
        {
            for (m=1; m<=Nobjects[SPECIE]; m++)
            {
                if ( Specie[m].type == BULK )
                  MassIn[j][m] += FirstSeg[k]->c[m];
            }
            VolIn[j]++;
        }
        j = UP_NODE(k);              // upstream node
        if (LastSeg[k] != NULL)      // accumulate concentrations
        {
            for (m=1; m<=Nobjects[SPECIE]; m++)
            {
                if ( Specie[m].type == BULK )
                    MassIn[j][m] += LastSeg[k]->c[m];
            }
            VolIn[j]++;
        }
    }

// --- compute avg. incident concen. at each node

    for (k=1; k<=Nobjects[NODE]; k++)
    {
        if (VolIn[k] > 0.0)
        {
            for (m=1; m<=Nobjects[SPECIE]; m++)
                X[k][m] = MassIn[k][m]/VolIn[k];
        }
    }
}

//=============================================================================

void updateNodes(long dt)
/*
**  Purpose:
**    updates the concentration at each node to the mixture
**    concentration of the accumulated inflow from connecting pipes.
**
**  Input:
**    dt = current WQ time step (sec)
**
**  Note:
**    Does not account for source flow effects. X[i][] contains
**    average concen. of segments adjacent to node i, used in case
**    there was no inflow into i.
*/
{
    int i, j, m;

// --- examine each node

    for (i=1; i<=Nobjects[NODE]; i++)
    {
    // --- node is a junction

        j = Node[i].tank;
        if (j <= 0)
        {
        // --- add any external inflow (i.e., negative demand)
        //     to total inflow volume

            if (D[i] < 0.0) VolIn[i] -= D[i]*dt;

        // --- if inflow volume is non-zero, then compute the mixture
        //     concentration resulting at the node

            if (VolIn[i] > 0.0)
            {
                for (m=1; m<=Nobjects[SPECIE]; m++)
                    Node[i].c[m] = MassIn[i][m]/VolIn[i];
            }

        // --- otherwise use the avg. of the concentrations in the
        //     links incident on the node

            else
            {
                for (m=1; m<=Nobjects[SPECIE]; m++)
                    Node[i].c[m] = X[i][m];
            }

        // --- compute new equilibrium mixture

            chemistry_equil(NODE, Node[i].c);
        }

    // --- node is a tank

        else
        {
        // --- use initial quality for reservoirs

            if (Tank[j].a == 0.0)
            {
                for (m=1; m<=Nobjects[SPECIE]; m++)
                    Node[i].c[m] = Node[i].c0[m];
            }

        // --- otherwise update tank WQ based on mixing model

            else switch(Tank[j].mixModel)
            {
            //case MIX2: tankmix2(i,dt); break;
            //case FIFO: tankmix3(i,dt); break;
            //case LIFO: tankmix4(i,dt); break;
                default:   tankMix1(j, dt); break;
            }
        }
    }
}

//=============================================================================

void sourceInput(long dt)
/*
**  Purpose:
**    computes contribution (if any) of mass additions from WQ
**    sources at each node.
**
**  Input:
**    dt = current WQ time step (sec)
*/
{
    int     n;
    double  qout, qcutoff, volout;
    Psource source;

// --- establish a flow cutoff which indicates no outflow from a node

    qcutoff = 10.0*TINY;

// --- consider each node

    for (n=1; n<=Nobjects[NODE]; n++)
    {
    // --- skip node if no WQ source

        source = Node[n].sources;
        if (source == NULL) continue;

    // --- find total flow volume leaving node

        if (Node[n].tank == 0) volout = VolIn[n];  // Junctions
        else volout = VolIn[n] - D[n]*dt;          // Tanks
        qout = volout / (double) dt;

    // --- evaluate source input only if node outflow > cutoff flow

        if (qout <= qcutoff) continue;

    // --- add contribution of each source specie

        while (source)
        {
            addSource(n, source, volout, dt);
            source = source->next;
        }

    // --- compute a new chemical equilibrium at the source node

        chemistry_equil(NODE, Node[n].c);
    }
}

//=============================================================================

void addSource(int n, Psource source, double volout, long dt)
/*
**  Purpose:
**    updates concentration of particular species leaving a node
**    that receives external source input.
**
**  Input:
**    n = index of source node
**    source = pointer to WQ source data
**    volout = volume of water leaving node during current time step
*/
{
    int     m;
    double  massadded, s;

// --- only analyze bulk species

    m = source->specie;
    massadded = 0.0;
    if (source->c0 > 0.0 && Specie[m].type == BULK)
    {

    // --- mass added depends on type of source

        s = getSourceQual(source);
        switch(source->type)
        {
        // Concen. Source:
        // Mass added = source concen. * -(demand)

          case CONCEN:

          // Only add source mass if demand is negative

              if (D[n] < 0.0) massadded = -s*D[n]*dt;

          // If node is a tank then set concen. to 0.
          // (It will be re-set to true value later on)

              if (Node[n].tank > 0) Node[n].c[m] = 0.0;
              break;

        // Mass Inflow Booster Source:

          case MASS:
              massadded = s*dt/LperFT3;
              break;

        // Setpoint Booster Source:
        // Mass added is difference between source
        // & node concen. times outflow volume

          case SETPOINT:
              if (s > Node[n].c[m]) massadded = (s - Node[n].c[m])*volout;
              break;

        // Flow-Paced Booster Source:
        // Mass added = source concen. times outflow volume

          case FLOWPACED:
              massadded = s*volout;
              break;
        }

    // --- adjust nodal concentration to reflect source addition

        Node[n].c[m] += massadded/volout;
    }
}

//=============================================================================

void release(long dt)
/*
**  Purpose:
**    releases outflow from nodes into incident links.
**
**  Input:
**    dt = current WQ time step
*/
{
    int    k, n, m;
    int    useNewSeg;
    double q, v;
    Pseg   seg;

// --- examine each link

    for (k=1; k<=Nobjects[LINK]; k++)
    {
    // --- ignore links with no flow

        if (Q[k] == 0.0)
        {
            removeSeg(NewSeg[k]);
            continue;
        }

    // --- find flow volume released to link from upstream node
    //     (NOTE: Flow volume is allowed to be > link volume.)

        n = UP_NODE(k);
        q = ABS(Q[k]);
        v = q*dt;

    // --- place bulk WQ at upstream node in new segment identified for link

        for (m=1; m<=Nobjects[SPECIE]; m++)
        {
            if ( Specie[m].type == BULK ) NewSeg[k]->c[m] = Node[n].c[m];
        }

    // --- if link has no last segment, then we must add a new one

        useNewSeg = 0;
        seg = LastSeg[k];
        if ( seg == NULL ) useNewSeg = 1;

    // --- otherwise check if quality in last segment
    //     differs from that of the new segment

        else for (m=1; m<=Nobjects[SPECIE]; m++)
        {
            if ( ABS(seg->c[m] - NewSeg[k]->c[m]) >= Specie[m].aTol )
            {
                useNewSeg = 1;
                break;
            }
        }

    // --- quality of last seg & new seg are close;
    //     simply increase volume of last seg

        if (!useNewSeg)
        {
            seg->v += v;
            removeSeg(NewSeg[k]);
        }

    // --- otherwise add the new seg to the end of the link

        else
        {
            NewSeg[k]->v = v;
            addSeg(k, NewSeg[k]);
        }

    }   //next link
}

//=============================================================================

void  tankMix1(int i, long dt)
/*
**  Purpose:
**    computes new WQ at end of time step in a completely mixed tank
**    (after contents have been reacted).
**
**  Input:
**    i = tank index
**    dt = current WQ time step
*/
{
    int    m, n;
    double c, cin;

// --- blend inflow with contents

    n = Tank[i].node;
    for (m=1; m<=Nobjects[SPECIE]; m++)
    {
        if ( Specie[m].type != BULK ) continue;
        c = Tank[i].c[m];
        cin = 0.0;
        if (VolIn[n] > 0.0) cin = MassIn[n][m]/VolIn[n];
        if (Tank[i].v > 0.0) c = c + (cin - c)*VolIn[n]/Tank[i].v;
        else c = cin;
        c = MAX(0.0, c);
        Tank[i].c[m] = c;
    }

// --- update species equilibrium

    if ( VolIn[n] > 0.0 ) chemistry_equil(NODE, Tank[i].c);
    for (m=1; m<=Nobjects[SPECIE]; m++) Node[n].c[m] = Tank[i].c[m];

// --- update tank volume

    Tank[i].v += D[n]*dt;
}

//=============================================================================

double  getSourceQual(Psource source)
/*
**   Input:   j = source index
**   Output:  returns source WQ value
**   Purpose: determines source concentration in current time period
*/
{
    int    i;
    long   k;
    double c, f = 1.0;

// --- get source concentration (or mass flow) in original units
    c = source->c0;

// --- convert mass flow rate from min. to sec.
    if (source->type == MASS) c /= 60.0;

// --- apply time pattern if assigned
    i = source->pat;
    if (i == 0) return(c);
    k = ((Qtime+Pstart)/Pstep) % Pattern[i].length;
    if (k != Pattern[i].interval)
    {
        if ( k < Pattern[i].interval )
        {
            Pattern[i].current = Pattern[i].first;
            Pattern[i].interval = 0;
        }
        while (Pattern[i].current && Pattern[i].interval < k)
        {
             Pattern[i].current = Pattern[i].current->next;
             Pattern[i].interval++;
        }
    }
    if (Pattern[i].current) f = Pattern[i].current->value;
    return c*f;
}
