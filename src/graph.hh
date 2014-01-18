#ifndef _GRAPH_HH_
#define _GRAPH_HH_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "config.hh"

class vertexInfo{
        int _vertexID;
        int _inDegree;
        int _outDegree;
    public:
        vertexInfo(){_vertexID=-1;_inDegree=0;_outDegree=0;}
        int setVertexID(int id){_vertexID=id;};
        int getVertexID(){return _vertexID;};
        int inDegreeInc(){_inDegree++;return 0;};
        int inDegreeDec(){_inDegree--;return 0;};
        int outDegreeInc(){_outDegree++;return 0;};
        int outDegreeDec(){_outDegree--;return 0;};
        int getInDegree(){return _inDegree;};
        int getOutDegree(){return _outDegree;};
        int setInDegree(int val){_inDegree=val;return 0;};
        int setOutDegree(int val){_outDegree=val;return 0;};
        int noInEdge(){return _inDegree==0?1:0;};
        int noOutEdge(){return _outDegree==0?1:0;};
        int isFree(){return noInEdge()|noOutEdge();};
};

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

        vertexInfo** _nodeInd;
        vertexInfo** _rackInd;

        /* for calculating max flow */
        int* _vertexColor;
        int* _path;
        int _pathLen;

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
        int pathSearch();
        int getRackPos();
        int dfs(int);
    public:
        graph(config* conf);
        int addEdge(int blockID,int nodeID,int rackID);
        int removeEdge(int blockID,int nodeID,int rackID);
        int maxFlow();
        int showAdjMat();
        int showResMat();
        int initResGraph();
};

#endif

