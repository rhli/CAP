#ifndef _STRIPING_HH_
#define _STRIPING_HH_

#include "config.hh"

class striping{
        config* _conf;
        int _ecN;
        int _ecK;
        int _repFac;
    public:
        striping(config* conf);
        /* Given a placement, generate the striping strategy */
        int strOp(int*,int*);
};

#endif


