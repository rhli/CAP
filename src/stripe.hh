#ifndef _STRIPE_HH_
#define _STRIPE_HH_

#include "config.hh"
#include "striping.hh"

/*
 * Keep the metadata of the stripe and do striping operation.
 *
 * Currently, we do striping operation in stripeManager, although it should
 * be performed in a distributed manner (We just simulate, and this is
 * enough).
 */
class stripe{
        config* _conf;
        int _repFac;

        int _isReplicated;
        int* _locations;
        int _coreRack;
    public:
        stripe(config* c){_isReplicated=1;_conf=c;};
        int setLoc(int* loc){_locations=loc;return 0;};
        int toEC(){_isReplicated=0;return 0;};
        int isReplicated(){return _isReplicated;};
        int* getLoc(){return _locations;};
        int getBlockPos(int blockID,int* pos);
        int setCoreRack(int cr){_coreRack=cr;return 0;};
        int getCoreRack(){return _coreRack;};
};

#endif

