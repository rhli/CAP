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
    _randGen=new randomGen(10000);

    _writeCounter=0;
    _stripeCounter=0;
    _completedWriteCounter=0;
    _completedStripeCounter=0;
    _wholeWriteTime=0;
    _wholeStripeTime=0;
    _bgCounter=0;

    create("traM");
    printf("trafficManager::trafficManager() starts\n");
    //test();
    stripe();
    hold(1);
    write();
    //bgTraffic();
    hold(10000);
    printf("write Average:%lf\n",_wholeWriteTime/_completedWriteCounter);
    printf("stripe Average:%lf\n",_wholeStripeTime/_completedStripeCounter);
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
        hold(20);
    }
    return;
}

void trafficManager::writeOp(int* loc){
    create("writeOp");
    int wID=_writeCounter;
    _writeCounter++;
    double startTime=simtime();
    int* pos=loc;
    for(int i=0;i<_ecK;i++){
        //TODO: add write into system
        _nodeTree->dataTransfer(pos[1],pos[0],_blockSize);
        _nodeTree->dataTransfer(pos[2],pos[1],_blockSize);
        pos+=3;
    }
    //fprintf(stdout,"write op %d: starts at %lf\n",wID,startTime);
    //fprintf(stdout,"write op %d: ends at %lf\n",wID,simtime());
    //fprintf(stdout,"write op %d: duration %lf\n",wID,simtime()-startTime);
    _wholeWriteTime+=simtime()-startTime;
    _completedWriteCounter++;
    return;
}

void trafficManager::stripe(){
    create("stripe");
    //int* loc=_strM->write(simtime())->getLoc();
    //writeOp(loc);
    //int* loc2=_strM->write(simtime())->getLoc();
    //writeOp(loc2);
    //int* loc3=_strM->write(simtime())->getLoc();
    //writeOp(loc3);
    //int* loc4=_strM->write(simtime())->getLoc();
    //writeOp(loc4);
    //int* loc5=_strM->write(simtime())->getLoc();
    //writeOp(loc5);
    //hold(100);
    while(1){
        double nextTime=_strM->getNextTime();
        //printf("nextTime: %lf\n",nextTime);
        if(nextTime-simtime()>0){
            hold(nextTime-simtime());
        }else if(nextTime<0){
            hold(60);
            continue;
        }
        //printf("trafficManager::strOp time %lf\n",simtime());
        int* loc=(int*)calloc(_repFac*_ecK+_ecN+1,sizeof(int));
        _strM->stripeAStripe(loc);
        if(loc!=NULL){
            /*
             * TODO: add Core-rack support
             */
            //for(int i=0;i<_repFac*_ecK+_ecN+1;i++){
            //    printf((i==_repFac*_ecK+_ecN)?"%3d\n":"%3d",loc[i]);
            //}
            if(_conf->getRepPlaPolicy()==1){
                stripeOp(loc,loc+_repFac*_ecK,
                        loc[_repFac*_ecK+_ecN]*_conf->getNodePerRack()+
                        _randGen->generateInt(_conf->getNodePerRack()));
            }else{
                stripeOp(loc,loc+_repFac*_ecK,
                        _randGen->generateInt(_conf->getNodeNum()));
            }
            //free(loc);
        }
    }
    return;
}

void trafficManager::stripeOp(int* repLoc,int* ecLoc,int opNode){
    create("stripeOp");
    //printf("trafficManager::stripeOp() opNode:%d %lf\n",opNode,simtime());
    double startTime=simtime();
    int sID=_stripeCounter;
    _stripeCounter++;
    //fprintf(stdout,"stripe op: starts at %lf\n",startTime);
    //return;
    /* do download */
    for(int i=0;i<_ecK;i++){
        int* repPos=repLoc+i*_repFac;
        //for(int i=0;i<_repFac;i++){
        //    printf("%d ",repPos[i]);
        //}
        //printf("\n");
        int from=_nodeTree->getNearest(opNode,_repFac,repPos);
        //printf("trafficManager::stripeOp(): %d %d from %d\n",sID,i,from);
        if(from!=-1){
            _nodeTree->dataTransfer(opNode,from,_blockSize);
        }else{
            fprintf(stderr,"trafficManager::stripeOp(): weird thing happened\n");
        }
        //printf("trafficManager::stripeOp(): %d %d-th block transmitted\n",sID,i);
        //if(i==1)return;
        //printf("from:%d\n",from);
    }
    //return;
    /* Computation is much faster compared with download, so we do not consider 
      computation cost. */
    /* write parities */
    //printf(" positions after transformed:\n ");
    //for(int i=0;i<_ecN;i++){
    //    printf(i!=_ecN-1?"%d ":"%d\n",ecLoc[i]);
    //}
    for(int i=_ecK;i<_ecN;i++){
        _nodeTree->dataTransfer(ecLoc[i],opNode,_blockSize);
    }
    //fprintf(stdout,"stripe op %d: duration %lf\n",sID,simtime()-startTime);
    //free(repLoc);
    //return;
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
    //fprintf(stdout,"stripe op: ends at %lf",simtime());
    _wholeStripeTime+=simtime()-startTime;
    _completedStripeCounter++;
    return;
}

void trafficManager::bgOp(int des,int src,double size){
    create("bgOp");
    _nodeTree->dataTransfer(des,src,size);
    return;
}





