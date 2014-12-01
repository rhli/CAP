#ifndef _TRAFFICMANAGER_HH_
#define _TRAFFICMANAGER_HH_

#include <cstring>
#include "config.hh"
#include "stripeManager.hh"
#include "NodeTree.hh"
#include "stripe.hh"
#include "randomGen.hh"
#include "layoutGen.hh"

class trafficManager{
        stripeManager* _strM;
        NodeTree* _nodeTree;
        config* _conf;
        randomGen* _randGen;
        
        /*
         * Basic system settings
         */
        int _ecN;
        int _ecK;

        int _blockSize;
        double _bandwidth;

        int _repFac;
        int _repPlaPolicy;

        int _writeCounter;
        int _completedWriteCounter;
        double _wholeWriteThpt;
        int _stripeCounter;
        int _completedStripeCounter;
        double _wholeStripeThpt;
        int _bgInterCounter;
        int _completedBgInterCounter;
        double _wholeBgInterThpt;
        int _bgIntraCounter;
        int _completedBgIntraCounter;
        double _wholeBgIntraThpt;

        /*
         * We have three data streams here (Not sure whether the word stream is precise here..)
         * 1. A write stream follow some distribution
         * 2. A striping stream stripe a stripe after it is written for some time
         * 3. A background traffic stream
         */
        void write();
        void stripe(int);
        void stripeMap(int,int,event*);
        void bgTraffic();

        void writeOp(int*);
        void inClusWriteOp(int*);
        void stripeOp(int*,int*,int);
        void bgOp(int,int,double,int);

        void pipeline(double,int*,event*);
    public:
        trafficManager(config*);
        /* Just for testing purpose on whether NodeTree is performing well */
        void test();
};

#endif
