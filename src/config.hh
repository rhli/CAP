#ifndef _CONFIG_HH_
#define _CONFIG_HH_

#include "tinyxml2.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

class config{
    public:
        int _rackNum;
        int _nodePerRack;
        int _nodeNum;
        int _ecN;
        int _ecK;
        int _repPlaPolicy;

        int _replicaNum;
        double _bandwidth;
        int _blockSize;
        int _randSeed;

        // workload related
        double _writeInterval;
        double _inClusWritePercent;
        double _bgTrafficInterval;
        double _inRackTrafficPercent;
        int _encodingStripeCount;
        int _maxInRack;

        config();
        int getRackNum(){return _rackNum;};
        int getNodePerRack(){return _nodePerRack;};
        int getNodeNum(){return _nodeNum;};
        int getEcN(){return _ecN;};
        int getEcK(){return _ecK;};
        int getReplicaNum(){return _replicaNum;};
        int getRepPlaPolicy(){return _repPlaPolicy;};
        int getBlockSize(){return _blockSize;};
        double getBandwidth(){return _bandwidth;};
};

#endif


