#ifndef _STRIPING_HH_
#define _STRIPING_HH_

#include "config.hh"

class striping{
        config* _conf;
    public:
        striping(config* conf);
        /* Given a placement, generate the striping strategy */
        int* strOp(int*);
};

#endif


