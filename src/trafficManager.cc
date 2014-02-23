#include "trafficManager.hh"

trafficManager::trafficManager(config* c){
    _conf=c;
    _bandwidth=_conf->getBandwidth();
    _blockSize=_conf->getBlockSize();
    _ecN=_conf->getEcN();
    _ecK=_conf->getEcK();
    _repFac=_conf->getReplicaNum();
    _repPlaPolicy=_conf->getRepPlaPolicy();
    _nodeTree=new NodeTree(c->getNodeNum(),c->getNodePerRack());
    _nodeTree->setBandwidth(_bandwidth);
    _strM=new stripeManager(c);

    create("traM");
    printf("trafficManager::trafficManager() starts\n");
    test();
    //write();
    //stripe();
    //bgTraffic();
    hold(10000);
}

void trafficManager::test(){
    create("test1");
    printf("trafficManager::test() starts at %lf\n",simtime());
    _nodeTree->dataTransfer(10,1,_blockSize);
    printf("trafficManager::test() ends at %lf\n",simtime());
    return;
}

void trafficManager::write(){
    create("write");
    while(1){
        int* loc=_strM->write()->getLoc();
        writeOp(loc);
        /* TODO: add some distribution */
        hold(100);
    }
    return;
}

void trafficManager::writeOp(int* loc){
    create("writeOp");
    double startTime=simtime();
    fprintf(stdout,"write op: starts at %lf",startTime);
    for(int i=0;i<_ecK;i++){
        //TODO: add write into system
        _nodeTree->dataTransfer(loc[1],loc[0],_blockSize);
        _nodeTree->dataTransfer(loc[2],loc[1],_blockSize);
        loc+=3;
    }
    fprintf(stdout,"write op: ends at %lf",simtime());
    return;
}

void trafficManager::stripeOp(int* repLoc,int* ecLoc,int opNode){
    create("stripeOp");
    fprintf(stdout,"stripe op: starts at %lf",startTime);
    /* do download */
    for(int i=0;i<_ecK;i++){
        int* repPos=repLoc+i*_repFac;
        int from=_nodeTree->getNearest(opNode,_repFac,repPos);
        if(from!=-1){
            _nodeTree->dataTransfer(opNode,from,_blockSize);
        }else{
            fprintf(stderr,"trafficManager::stripeOp(): weird thing happened\n");
        }
    }
    /* Computation is much faster compared with download, so we do not consider 
      computation cost. */
    /* write parities */
    for(int i=_ecK;i<_ecN;i++){
        _nodeTree->dataTransfer(,opNode,_blockSize);
    }
    /* re-allocation, if necessary */
    if(_repPlaPolicy==0){
        for(int i=0;i<_ecK;i++){
            int* repPos=repLoc+i*_repFac;
            int needRA=1;
            for(int j=0;j<_repFac;j++){
                if(ecLoc[i]==repPos[j]){
                    needRA=0;
                    break;
                }
            }
            if(needRA==1){
                _nodeTree->dataTransfer(ecLoc[i],
                        _nodeTree->getNearest(ecLoc[i],_repFac,repPos),
                        _blockSize);
            }
        }
    }
    fprintf(stdout,"stripe op: ends at %lf",simtime());
    return;
}



