#ifndef _TRAFFICMANAGER_HH_
#define _TRAFFICMANAGER_HH_

#include "config.hh"
#include "stripeManager.hh"
#include "NodeTree.hh"
#include "stripe.hh"

class trafficManager{
        stripeManager* _strM;
        NodeTree* _nodeTree;
        config* _conf;
        
        /*
         * Basic system settings
         */
        int _ecN;
        int _ecK;

        int _blockSize;
        double _bandwidth;

        int _repFac;
        int _repPlaPolicy;

        /*
         * We have three data streams here (Not sure whether the word stream is precise here..)
         * 1. A write stream follow some distribution
         * 2. A striping stream stripe a stripe after it is written for some time
         * 3. A background traffic stream
         */
        void write();
        void stripe();
        void bgTraffic();

        void writeOp(int*);
        void stripeOp(int*,int*,int);
    public:
        trafficManager(config*);
        /* Just for testing purpose on whether NodeTree is performing well */
        void test();
};

#endif
