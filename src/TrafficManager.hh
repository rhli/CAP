#ifndef _TRAFFICMANAGER_HH_
#define _TRAFFICMANAGER_HH_

#include <cstring>
#include "config.hh"
#include "Topology.hh"
#include "randomGen.hh"
#include "PlacementManager.hh"

class TrafficManager{
        Topology* _nodeTree;
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

        /* operational overheads */
        double _writeOverhead;
        double _stripeOverhead;
        double _stripeSwitchingOverhead;
        stream* _stream;

        /*
         * We have three data streams here (Not sure whether the word stream is precise here..)
         * 1. A write stream follow some distribution
         * 2. A striping stream stripe a stripe after it is written for some time
         * 3. A background traffic stream
         */
        void write();
        void encode(int);
        void encodeMap(int,int,event*);
        //void bgTraffic();

        void writeOp(int*);
        void inClusWriteOp(int*);
        void encodeOp(int*,int*,int);
        void bgOp(int,int,double,int);

        void pipeline(double,int*,event*);
    public:
        TrafficManager(config*);
};

#endif
