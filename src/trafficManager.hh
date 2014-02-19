#ifndef _TRAFFICMANAGER_HH_
#define _TRAFFICMANAGER_HH_

#include "config.hh"
#include "stripeManager.hh"
#include "NodeTree.hh"

class trafficManager{
        stripeManager* _strM;
        NodeTree* _nodeTree;
        config* _conf;
        /*
         * We have three data streams here (Not sure whether the word stream is precise here..)
         * 1. A write stream follow some distribution
         * 2. A striping stream stripe a stripe after it is written for some time
         * 3. A background traffic stream
         */
        int write();
        int stripe();
        int bgTraffic();
    public:
        trafficManager(config*);
};

#endif
