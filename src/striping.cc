#include "striping.hh"

striping::striping(config* conf){
    _conf=conf;
    _ecN=_conf->getEcN();
    _ecK=_conf->getEcK();
    _rackOcp=NULL;
    _repFac=_conf->getReplicaNum();
    _maxInRack=_ecN-_ecK;
    _graph=new graph(_conf);
}

int striping::strOp(int* input,int* output){
    int rackNum=_conf->getRackNum();
    _rackOcp=(int*)calloc(rackNum,sizeof(int));
    _graph->graphInit();
    //printf("replication placement: %d\n",_repFac);
    //for(int i=0;i<_repFac*_ecK;i++){
    //    printf(i==_ecK*_repFac-1?"%4d\n":"%4d",input[i]);
    //}
    _graph->initFromPla(input);
    /*
     * RH: Jan 23th, 2014
     * TODO: we can manipulate the location of the reserved copy.
     * However, currently we use a most naive approach.
     */
    _graph->getMaxMatch(output);
    for(int i=0;i<_ecK;i++){
        if(output[i]!=-1){
            _rackOcp[output[i]/_conf->getNodePerRack()]++;
        }
    }
    //for(int i=0;i<_conf->getRackNum();i++){
    //    printf("%d\n",_rackOcp[i]);
    //}
    //printf("_maxInRack:%d\n",_maxInRack);
    /*
     * determine the location of parities or re-allocated native blocks
     */
    for(int i=0;i<_ecN;i++){
        if(output[i]!=-1){
            continue;
        }
        /* first determine a rack */
        int rackPos=-1;
        int nodePos=-1;
        while(1){
            rackPos=_randomGen->generateInt(rackNum);
            //printf("%d: _rackOcp[%d]=%d\n",i,rackPos,_rackOcp[rackPos]);
            if(_rackOcp[rackPos]<_maxInRack){
                break;
            }
        }
        while(1){
            nodePos=_randomGen->generateInt(rackNum);
            int duplicated=0;
            for(int j=0;j<i;j++){
                if(nodePos+rackPos*_conf->getNodePerRack()==output[j]){
                    duplicated=1;
                    break;
                }
            }
            if(duplicated==0){
                break;
            }
        }
        output[i]=nodePos+rackPos*_conf->getNodePerRack();
        _rackOcp[rackPos]++;
    }
    for(int i=0;i<_ecN;i++){
        //printf(i==_ecN-1?"%4d\n":"%4d",output[i]);
        _rackOcp[output[i]/_maxInRack]++;
    }
}


