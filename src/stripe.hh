#ifndef _STRIPE_HH_
#define _STRIPE_HH_

#include "blockInfo.hh"
#include "config.hh"
#include "striping.hh"

/*
 * Keep the metadata of the stripe and do striping operation.
 */
class stripe{
        int _isReplicated;
        config* _conf;
        int* _locations;
        int _repFac;
    public:
        stripe(config* c){_isReplicated=1;_conf=c;};
        int setLoc(int* loc){_locations=loc;return 0;};
        int toEC(){_isReplicated=0;return 0;};
        int isReplicated(){return _isReplicated;};
        int getPos(int* loc);
        int getBlockPos(int blockID,int* pos);
};

#endif
