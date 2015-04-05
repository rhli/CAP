/*
 * We use this to generate placements
 *
 * The core of encoding-aware replication. 
 * We benefit from encoding-aware replication for two aspects.
 * We explain with the example of single stripe file, meaning that we need to place k blocks.
 * We first randomly pick a core-rack. EAR follows the following rules:
 *  1. For every block, at least one copy is placed in core rack
 *  2. By using the flow graph, we make sure the k blocks can achieve fault-/partition-tolerance
 *     without reallocating the placed blocks.
 * We can benefit from the following two aspects:
 *  1. Performance: The downloading traffic of striping operation are all within a single rack
 *      while intra-rack traffic is more sufficient than inter-rack traffic.
 *  2. Reliability: After striping, we can guarantee that the stripe can tolerate node failure
 *      rack failure.
 */
#ifndef _PLACEMENTMANAGER_HH_
#define _PLACEMENTMANAGER_HH_

#include <cstdio>
#include <cstdlib>
#include "randomGen.hh"
#include "FlowGraph.hh"

class PlacementManager{
        config* _conf;
        int _blockNum;
        /* Num of replicas of a block */
        int _repFac;

        /* Erasure coding parameters, for stripe-oriented placement */
        //int _stripeNum;
        randomGen* _randGen;
        FlowGraph* _graph;
    public:
        PlacementManager(config* conf);
        /* conventional placement */
        int randomPla(int*);
        /* stripe-oriented placement */
        int SOP(int*);
        /* with appointed core rack */
        int SOP(int,int*);
        int coreRackOnly(int*);
        int showPlacement(int*);
        int examinePla(int*);
        int setRandomGen(randomGen* rg){_randGen=rg;return 0;};

        int getParityLoc(int* input,int* output,int);
};

#endif

