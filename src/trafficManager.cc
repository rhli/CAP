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

    //_writeCounter=0;
    //_stripeCounter=0;
    //_bgInterCounter=0;
    //_bgIntraCounter=0;
    _completedWriteCounter=0;
    _completedStripeCounter=0;
    _completedBgInterCounter=0;
    _completedBgIntraCounter=0;
    _wholeWriteThpt=0;
    _wholeStripeThpt=0;
    _wholeBgInterThpt=0;
    _wholeBgIntraThpt=0;

    create("traM");
    printf("trafficManager::trafficManager() starts\n");
    //test();
    stripe();
    //hold(1);
    write();
    bgTraffic();
    hold(3600);
    printf("write Average:%lf\n",_wholeWriteThpt/_completedWriteCounter);
    printf("stripe Average:%lf\n",_wholeStripeThpt/_completedStripeCounter);
    printf("bgInter Average:%lf\n",_wholeBgInterThpt/_completedBgInterCounter);
    printf("bgIntra Average:%lf\n",_wholeBgIntraThpt/_completedBgIntraCounter);
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
    stream* s=new stream();
    s->reseed(10000);
    while(1){
        int* loc=_strM->write(simtime())->getLoc();
        writeOp(loc);
        /* TODO: add some distribution */
        hold(s->exponential(5));
    }
    return;
}

void trafficManager::writeOp(int* loc){
    create("writeOp");
    int wID=_writeCounter;
    _writeCounter++;
    double startTime=simtime();
    int* pos=loc;
    //for(int i=0;i<_ecK*_repFac;i++){
    //    printf(i==_ecK*_repFac-1?"%d\n":"%d ",loc[i]);
    //}
    for(int i=0;i<_ecK;i++){
        _nodeTree->dataTransfer(pos[0],-1,_blockSize);
        _nodeTree->dataTransfer(pos[1],pos[0],_blockSize);
        _nodeTree->dataTransfer(pos[2],pos[1],_blockSize);
        pos+=3;
    }
    //fprintf(stdout,"write op %d: starts at %lf\n",wID,startTime);
    //fprintf(stdout,"write op %d: ends at %lf\n",wID,simtime());
    fprintf(stdout,"write op %d: thpt %lf\n",wID,simtime()-startTime);
    _wholeWriteThpt+=_ecK*_blockSize/(simtime()-startTime);
    _completedWriteCounter++;
    return;
}

void trafficManager::stripe(){
    create("stripe");
    stream* s=new stream();
    s->reseed(12345);
    while(1){
        double nextTime=_strM->getNextTime();
        //printf("nextTime: %lf\n",nextTime);
        if(nextTime-simtime()>0){
            //hold(nextTime-simtime());
            hold(s->exponential(5));
        }else if(nextTime<0){
            hold(1);
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
            //    printf((i==_repFac*_ecK+_ecN)?"%d\n":"%d ",loc[i]);
            //}
            if(_conf->getRepPlaPolicy()==1){
                stripeOp(loc,loc+_repFac*_ecK,
                        loc[_repFac*_ecK+_ecN]*_conf->getNodePerRack()+
                        _randGen->generateInt(_conf->getNodePerRack()));
            }else{
                stripeOp(loc,loc+_repFac*_ecK,
                        _randGen->generateInt(_conf->getNodeNum()));
            }
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
            //printf("stripeOp: %d %d\n",opNode,from);
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
        //printf("2stripeOp: %d %d\n",ecLoc[i],opNode);
        _nodeTree->dataTransfer(ecLoc[i],opNode,_blockSize);
    }
    //fprintf(stdout,"stripe op %d: duration %lf\n",sID,simtime()-startTime);
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
                //printf("3stripeOp: %d %d\n",ecLoc[i],opNode);
                _nodeTree->dataTransfer(ecLoc[i],
                        _nodeTree->getNearest(ecLoc[i],_repFac,repPos),
                        _blockSize);
            }
        }
    }
    //fprintf(stdout,"stripe op: ends at %lf",simtime());
    fprintf(stdout,"stripe op %d: thpt %lf\n",sID,simtime()-startTime);
    _wholeStripeThpt+=_ecK*_blockSize/(simtime()-startTime);
    _completedStripeCounter++;
    free(repLoc);
    return;
}

void trafficManager::bgOp(int des,int src,double size,int flag){
    create("bgOp");
    double startTime=simtime();
    _nodeTree->dataTransfer(des,src,size);
    fprintf(stdout,flag==0?"bg inter thpt %lf\n":"bg intra thpt %lf\n",
            size/(simtime()-startTime));
    if(flag==0){
        _completedBgInterCounter++;
        _wholeBgInterThpt+=size/(simtime()-startTime);
    }else{
        _completedBgIntraCounter++;
        _wholeBgIntraThpt+=size/(simtime()-startTime);
    }
    return;
}

void trafficManager::bgTraffic(){
    create("bgTraffic");
    stream* s=new stream();
    s->reseed(11111);
    while(1){
        if(s->uniform_int(0,1)==1){
            /* Generate a cross rack one */
            int* rackInd=(int*)calloc(2,sizeof(int));
            _randGen->generateList(_conf->getRackNum(),2,rackInd);
            rackInd[0]*=_conf->getNodePerRack();
            rackInd[0]+=_randGen->generateInt(_conf->getNodePerRack());
            rackInd[1]*=_conf->getNodePerRack();
            rackInd[1]+=_randGen->generateInt(_conf->getNodePerRack());
            bgOp(rackInd[0],rackInd[1],s->exponential(_blockSize),0);
        }else{
            /* Generate a intra rack one */
            int rackID=_randGen->generateInt(_conf->getRackNum());
            int* rackInd=(int*)calloc(2,sizeof(int));
            _randGen->generateList(_conf->getNodePerRack(),2,rackInd);
            rackInd[0]+=rackID*_conf->getNodePerRack();
            rackInd[1]+=rackID*_conf->getNodePerRack();
            bgOp(rackInd[0],rackInd[1],s->exponential(_blockSize),1);
        }
        hold(s->exponential(1));
    }
}





