#include "blockInfo.hh"

blockInfo::blockInfo(config* conf,int fileId,int blockId){
    _conf=conf;
    _ecN=_conf->getEcN();
    _ecK=_conf->getEcK();
    _repFac=_conf->getReplicaNum();
    _isReplicated=1;
    _isParity=0;
}

int blockInfo::toErasureCoded(int pos){
    int inList=0;
    for(int i=0;i<_repFac;i++){
        if(_repPos[i]==pos){
            inList=1;
            break;
        }
    }
    _isReplicated=0;
    return 0;
}
