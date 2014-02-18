#ifndef _CONFIG_HH_
#define _CONFIG_HH_

#include "tinyxml2.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

class config{
        int _rackNum;
        int _nodePerRack;
        int _nodeNum;
        int _ecN;
        int _ecK;
        int _repPlaPolicy;

        int _replicaNum;

    public:
        config();
        int getRackNum(){return _rackNum;};
        int getNodePerRack(){return _nodePerRack;};
        int getNodeNum(){return _nodeNum;};
        int getEcN(){return _ecN;};
        int getEcK(){return _ecK;};
        int getReplicaNum(){return _replicaNum;};
        int getRepPlaPolicy(){return _repPlaPolicy;};
};

#endif


