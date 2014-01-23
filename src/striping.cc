#include "striping.hh"

striping::striping(config* conf){
    _conf=conf;
    _ecN=_conf->getEcN();
    _ecK=_conf->getEcK();
    _graph=new graph(_conf);
}

int striping::strOp(int* input,int* output){
    _graph->initFromPla(input);
    /*
     * RH: Jan 23th, 2014
     * TODO: we can manipulate the location of the reserved copy.
     * However, currently we use a most naive approach.
     */
    _graph->getMaxMatch(output);
    for(int i=0;i<_ecK;i++){
        printf(i==_ecK-1?"%4d\n":"%4d",output[i]);
    }
    /*
     * TODO: determine the location of parities
     */
    for(int i=_ecK;i<_ecN;i++){
        ;
    }
}


