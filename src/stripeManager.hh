#ifndef _STRIPEMANAGER_HH_
#define _STRIPEMANAGER_HH_

#include "config.hh"
#include "stripe.hh"
#include "striping.hh"
#include "layoutGen.hh"

typedef struct stripeNode{
    stripe* _str;
    int _stime;
    stripeNode* _next;
}stripeNode;

class stripeManager{
        config* _conf;
        stripeNode* _stripeQueue;
        stripeNode* _sQueueTail;
        int _stripeCount;
        layoutGen* _lGen;
        striping* _striping;

        int* _blockCountInNode;
        int* _blockCountInRack;

        /*
         * Replication placement policy, get from config
         * 0 for conventional random placement
         * 1 for SOP
         */
        int _repPlaPolicy;
        int _nodeNum;
        int _rackNum;
        int _repFac;
        int _ecN;
        int _ecK;

        /*
         * We do in this way:
         * after a stripe is created for some time, it is striped in to
         * erasure coded format.
         * This function serves as a background process.
         */
        int strOp();
        /* 
         * Works as a queue, however, we do dequeue operation when the stripe is
         * "striped".
         */
        int pushStripe(stripe* str);
        stripe* popStripe();

        /* These two updates storage capacity status of node and rack */
        int addRepStripe(int* loc);
        int addECStripe(int*,int*);
    public:
        stripeManager(config* c);
        stripe* write();
        int showNodeCapacity();
        int showRackCapacity();
};

#endif


