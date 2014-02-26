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

    _writeCounter=0;
    _stripeCounter=0;
    _bgCounter=0;

    create("traM");
    printf("trafficManager::trafficManager() starts\n");
    //test();
    write();
    stripe();
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
        int* loc=_strM->write(simtime())->getLoc();
        writeOp(loc);
        /* TODO: add some distribution */
        hold(5);
    }
    return;
}

void trafficManager::writeOp(int* loc){
    create("writeOp");
    int wID=_writeCounter;
    _writeCounter++;
    double startTime=simtime();
    for(int i=0;i<_ecK;i++){
        //TODO: add write into system
        _nodeTree->dataTransfer(loc[1],loc[0],_blockSize);
        _nodeTree->dataTransfer(loc[2],loc[1],_blockSize);
        loc+=3;
    }
    //fprintf(stdout,"write op %d: starts at %lf\n",wID,startTime);
    //fprintf(stdout,"write op %d: ends at %lf\n",wID,simtime());
    fprintf(stdout,"write op %d: duration %lf\n",wID,simtime()-startTime);
    return;
}

void trafficManager::stripe(){
    create("stripe");
    while(1){
        int* loc=_strM->stripeAStripe();
        /*
         * TODO: add Core-rack support
         */
        //stripeOp(loc,loc+_repFac*_ecK);
        free(loc);
    }
    return;
}

void trafficManager::stripeOp(int* repLoc,int* ecLoc,int opNode){
    create("stripeOp");
    double startTime=simtime();
    fprintf(stdout,"stripe op: starts at %lf\n",startTime);
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
        _nodeTree->dataTransfer(ecLoc[i],opNode,_blockSize);
    }
    return;
    /* 
     * re-allocation, if necessary
     * Feb 26th, 2014 RH: Let us do not consider re-allocation for now.
     */
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

void trafficManager::bgOp(int des,int src,double size){
    create("bgOp");
    _nodeTree->dataTransfer(des,src,size);
    return;
}




