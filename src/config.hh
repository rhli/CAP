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
    public:
        config();
        int getRackNum(){return _rackNum;};
        int getNodePerRack(){return _nodePerRack;};
        int getNodeNum(){return _nodeNum;};
        int getEcN(){return _ecN;};
        int getEcK(){return _ecK;};
};

