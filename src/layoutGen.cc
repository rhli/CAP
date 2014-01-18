#include "layoutGen.hh"

layoutGen::layoutGen(int blockNum,config* conf){
    _conf=conf;
    _blockNum=blockNum;
    _stripeNum=_blockNum%_conf->getEcK()==0?_blockNum%_conf->getEcK():_blockNum%_conf->getEcK()+1;
}

/*
 * We implement the default random placement by HDFS
 * The return value is a matrix containing where to place a block
 */
int* layoutGen::randomPla(){
    int* retVal=(int*)calloc(_blockNum*_repFac,sizeof(int));
    int* rackInd=(int*)calloc(,sizeof(int));
    int* nodeInd=(int*)calloc(_conf->getReplicaNum(),sizeof(int));
    for(int i=0;i<_blockNum;i++){
        ;
    }
    return 0;
}

/*
 * The return value is a matrix containing the block placement.
 * We benefit from stripe-oriented placement in two perspective:
 *  1. Reliability;
 *  2. Striping performance;
 */
int* layoutGen::SOP(){
    int* retVal=(int*)calloc(_blockNum*_repFac,sizeof(int));
    return 0;
}


