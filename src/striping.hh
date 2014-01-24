#ifndef _STRIPING_HH_
#define _STRIPING_HH_

#include "config.hh"
#include "graph.hh"
#include "randomGen.hh"

class striping{
        config* _conf;
        int _ecN;
        int _ecK;
        int _repFac;
        int _maxInRack;
        /* Count how many racks are occupying in a rack */
        int* _rackOcp;

        graph* _graph;

        randomGen* _randomGen;
    public:
        striping(config* conf);
        int setRandomGen(randomGen* rg){_randomGen=rg;return 0;};
        /* Given a placement, generate the striping strategy */
        int strOp(int*,int*);
};

#endif


