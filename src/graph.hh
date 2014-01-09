#ifndef _GRAPH_HH_
#define _GRAPH_HH_

#include <cstdio>
#include <cstdlib>
#include "config.hh"

/*
 * We deal with a flow graph here, since the scale of the
 * graph is not large, we use a adjacent graph to represent
 * the graph
 */

class graph{
        config* _conf;
        /* the value of the matrix represent capacity of some edge */
        int* _veGraph;
        int* _resGraph;

        int* _nodeInd;
        int* _rackInd;

        int _blockOffset;
        int _nodeOffset;
        int _rackOffset;
        int _sinkOffset;

        /* parameters of the graph */
        int _vertexNum;
        int _maxFlow;

        /* Information of the system */
        int _blockNum;/* k in erasure coding */
        int _replicaNum;
        int _nodeNum;/* replica number times k */
        int _rackNum;
        int _maxInRack;/* the value of (n-k) */

        int graphInit();
    public:
        graph(config* conf);
        int addEdge(int blockID,int nodeID,int rackID);
        int removeEdge(int blockID,int nodeID,int rackID);
        int maxFlow();
};

#endif


