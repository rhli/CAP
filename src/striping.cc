#include "striping.hh"

striping::striping(config* conf){
    _conf=conf;
    _ecN=_conf->getEcN();
    _ecK=_conf->getEcK();
    _rackOcp=NULL;
    _maxInRack=_ecN-_ecK;
    _graph=new graph(_conf);
}

int striping::strOp(int* input,int* output){
    int rackNum=_conf->getRackNum();
    _rackOcp=(int*)calloc(rackNum,sizeof(int));
    _graph->initFromPla(input);
    /*
     * RH: Jan 23th, 2014
     * TODO: we can manipulate the location of the reserved copy.
     * However, currently we use a most naive approach.
     */
    _graph->getMaxMatch(output);
    for(int i=0;i<_ecK;i++){
        printf(i==_ecK-1?"%4d\n":"%4d",output[i]);
        _rackOcp[output[i]/_maxInRack]++;
    }
    /*
     * TODO: determine the location of parities
     */
    for(int i=_ecK;i<_ecN;i++){
        /* first determine a rack */
        int rackPos=-1;
        int nodePos=-1;
        while(1){
            rackPos=_randomGen->generateInt(rackNum);
            if(_rackOcp[rackPos]!=_maxInRack){
                break;
            }
        }
        while(1){
            nodePos=_randomGen->generateInt(rackNum);
            int duplicated=0;
            for(int j=0;j<_i;j++){
                if(nodePos==output[j]){
                    duplicated=1;
                    break;
                }
            }
            if(duplicated==0){
                break;
            }
        }
        output[i]=nodePos;
        _rackOcp[rackPos]++;
    }
}


