#ifndef _LAYOUTGEN_HH_
#define _LAYOUTGEN_HH_

class layoutGen{
        config* _conf;
        int _blockNum;
        /* Num of replicas of a block */
        int _repFac;

        /* Erasure coding parameters, for stripe-oriented placement */
        int _ecN;
        int _ecK;
    public:
        layoutGen(int blockNum,config* conf);
};

#endif

