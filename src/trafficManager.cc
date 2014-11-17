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
  //stream *s=new stream();
  //double prev=0;
  //while(simtime()<360) {
  //  hold(s->exponential(2));
  //  printf("%.1lf\,",simtime()-prev);
  //  prev=simtime();
  //}

  write();
  //bgTraffic();
  hold(30);
  stripe(_conf->_encodingStripeCount);
  hold(370);
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
  //printf("writer starts");
  stream* s=new stream();
  s->reseed(10000);
  if(_conf->getRepPlaPolicy()!=1){
    while(1){
      int* rackInd=(int*)calloc(2,sizeof(int));
      int* loc=(int*)calloc(_conf->getReplicaNum(),sizeof(int));
      _randGen->generateList(_conf->getRackNum(),2,rackInd);
      _randGen->generateList(_conf->getNodePerRack(),1,loc);
      _randGen->generateList(_conf->getNodePerRack(),_conf->getReplicaNum()-1,loc+1);
      for(int k=0;k<_conf->getReplicaNum();k++){
        loc[k]+=k==0?rackInd[0]*_conf->getNodePerRack():rackInd[1]*_conf->getNodePerRack();
      }
      free(rackInd);
      if (s->uniform(0,1)<_conf->_inClusWritePercent){
        inClusWriteOp(loc);
      } else {
        writeOp(loc);
      }
      hold(s->exponential(_conf->_writeInterval));
    }
  } else {
    int** raidTail=(int**)calloc(_conf->getRackNum(),sizeof(int*));
    int* raidTailIdx=(int*)calloc(_conf->getRackNum(),sizeof(int));
    for (int i=0;i<_conf->getRackNum();i++){
      raidTail[i]=(int*)calloc(_ecK,sizeof(int));
      _randGen->generateList(_conf->getRackNum(),_ecK,raidTail[i]);
    }
    while(1) {
      int* loc=(int*)calloc(_conf->getReplicaNum(),sizeof(int));
      loc[0]=_randGen->generateInt(_conf->getRackNum());
      _randGen->generateList(_conf->getNodePerRack(),_conf->getReplicaNum()-1,loc+1);
      for(int i=1;i<_conf->getReplicaNum();i++){
        if(raidTail[loc[0]][raidTailIdx[loc[0]]]==loc[0]){ 
          loc[i]+=raidTail[loc[0]][(raidTailIdx[loc[0]]+1)/_ecK]*_conf->getNodePerRack();
        }else{
          loc[i]+=raidTail[loc[0]][raidTailIdx[loc[0]]]*_conf->getNodePerRack();
        }
      }
      raidTailIdx[loc[0]]=(raidTailIdx[loc[0]]+1)%_ecK;
      if(raidTailIdx[loc[0]]==0) {
        _randGen->generateList(_conf->getRackNum(),_ecK,raidTail[loc[0]]);
      }
      loc[0]=loc[0]*_conf->getNodePerRack()+_randGen->generateInt(_conf->getNodePerRack());
      if (s->uniform(0,1)<_conf->_inClusWritePercent){
        inClusWriteOp(loc);
      } else {
        writeOp(loc);
      }
      hold(s->exponential(_conf->_writeInterval));
    }
  }
  return;
}

void trafficManager::inClusWriteOp(int* loc){
  create("writeOp");
  _writeCounter++;
  double startTime=simtime();
  int* pos=loc;
  _nodeTree->dataTransfer(pos[1],pos[0],_blockSize);
  _nodeTree->dataTransfer(pos[2],pos[1],_blockSize);
  fprintf(stdout,"inClusWriteOp: begins %lf ends %lf\n",startTime,simtime());
  _wholeWriteThpt+=_ecK*_blockSize/(simtime()-startTime);
  _completedWriteCounter++;
  //free(loc);
  return;
}

void trafficManager::writeOp(int* loc){
  create("writeOp");
  _writeCounter++;
  double startTime=simtime();
  int* pos=loc;
  _nodeTree->dataTransfer(pos[0],-1,_blockSize);
  _nodeTree->dataTransfer(pos[1],pos[0],_blockSize);
  _nodeTree->dataTransfer(pos[2],pos[1],_blockSize);
  fprintf(stdout,"writeOp: begins %lf ends %lf\n",startTime,simtime());
  _wholeWriteThpt+=_ecK*_blockSize/(simtime()-startTime);
  _completedWriteCounter++;
  //free(loc);
  return;
}

void trafficManager::stripe(int stripeCount){
  create("stripe");
  int* rackStripeCount=(int*)calloc(_conf->getRackNum(),sizeof(int));
  for(int i=0;i<stripeCount*_ecK;i++){
    rackStripeCount[_randGen->generateInt(_conf->getRackNum())]++;
  }
  for(int i=0;i<_conf->getRackNum();i++){
    rackStripeCount[i]=
      rackStripeCount[i]%_ecK==0?rackStripeCount[i]/_ecK:rackStripeCount[i]/_ecK+1;
  }
  for(int i=0;i<_conf->getRackNum();i++){
    if(_conf->getRepPlaPolicy()==1){
      stripeMap(i*_conf->getNodePerRack()+_randGen->generateInt(_conf->getNodePerRack()),
          rackStripeCount[i]);
      //stripeMap(i*_conf->getNodePerRack()+_randGen->generateInt(_conf->getNodePerRack()),
      //    stripeCount/_conf->getRackNum());
    }else{
      stripeMap(_randGen->generateInt(_conf->getNodeNum()),stripeCount/_conf->getRackNum());
    }
    //fprintf(stdout,"stripe count:%d %d\n",i,rackStripeCount[i]);
    hold(0.5);
  }
  hold(3600);
}

// the argument is the list of stripeID to process
void trafficManager::stripeMap(int id,int size){
  create("stripeMap");
  //fprintf(stdout,"stripeMap starts!\n");
  double startTime;
  int coreRack=id/_conf->getNodePerRack();
  for(int i=0;i<size;i++){
    // generate a stripe to process
    startTime=simtime();
    int* repLocs=(int*)calloc(_ecK*_repFac,sizeof(int));
    if(_conf->getRepPlaPolicy()==1){
      int* list=(int*)calloc(_ecK,sizeof(int));
      _randGen->generateList(_conf->getRackNum(),_ecK,list);
      int* rackInd=(int*)calloc(2,sizeof(int));
      for(int j=0;j<_ecK;j++){
        rackInd[0]=coreRack;
        rackInd[1]=list[j];
        if(rackInd[1]==coreRack){
          rackInd[1]=list[(j+1)%_ecK];
        }
        int* pos=repLocs+j*_conf->getReplicaNum();
        _randGen->generateList(_conf->getNodePerRack(),1,pos);
        _randGen->generateList(_conf->getNodePerRack(),_conf->getReplicaNum()-1,pos+1);
        for(int k=0;k<_conf->getReplicaNum();k++){
          pos[k]+=k==0?rackInd[0]*_conf->getNodePerRack():rackInd[1]*_conf->getNodePerRack();
        }
      }
      free(rackInd);
      free(list);
    }else{
      int* rackInd=(int*)calloc(2,sizeof(int));
      for(int j=0;j<_ecK;j++){
        int* pos=repLocs+j*_conf->getReplicaNum();
        _randGen->generateList(_conf->getRackNum(),2,rackInd);
        _randGen->generateList(_conf->getNodePerRack(),1,pos);
        _randGen->generateList(_conf->getNodePerRack(),_conf->getReplicaNum()-1,pos+1);
        for(int k=0;k<_conf->getReplicaNum();k++){
          pos[k]+=k==0?rackInd[0]*_conf->getNodePerRack():rackInd[1]*_conf->getNodePerRack();
        }
      }
      free(rackInd);
    }
    for(int j=0;j<_ecK;j++){
      int* repPos=repLocs+j*_repFac;
      int from=_nodeTree->getNearest(id,_repFac,repPos);
      if(from!=-1){
        _nodeTree->dataTransfer(id,from,_blockSize);
      }else{
        fprintf(stderr,"trafficManager::stripeMap(): weird thing happened\n");
      }
    }
    /* hold some time for computation */
    hold(1);
    int* ecLoc=(int*)calloc(_ecN-_ecK,sizeof(int));
    _randGen->generateList(_conf->getRackNum(),_ecN-_ecK,ecLoc);
    for(int j=_ecK;j<_ecN;j++){
      _nodeTree->dataTransfer(ecLoc[j-_ecK]*_conf->getNodePerRack()+
          _randGen->generateInt(_conf->getNodePerRack()),
          id,_blockSize);
    }
    _wholeStripeThpt+=simtime()-startTime;
    fprintf(stdout,"stripeOp: begins %lf ends %lf\n",startTime,simtime());
    /* hold some time for other overhead */
    hold(2);
    free(repLocs);
  }
}


/* Commented on Nov 5th, we need a MapReduce like stripe function begins */
//void trafficManager::stripe(){
//    create("stripe");
//    stream* s=new stream();
//    s->reseed(12345);
//    while(1){
//        double nextTime=_strM->getNextTime();
//        //printf("nextTime: %lf\n",nextTime);
//        if(nextTime-simtime()>0){
//            //hold(nextTime-simtime());
//            hold(s->exponential(5));
//        }else if(nextTime<0){
//            hold(1);
//            continue;
//        }
//        //printf("trafficManager::strOp time %lf\n",simtime());
//        int* loc=(int*)calloc(_repFac*_ecK+_ecN+1,sizeof(int));
//        _strM->stripeAStripe(loc);
//        if(loc!=NULL){
//            /*
//             * TODO: add Core-rack support
//             */
//            //for(int i=0;i<_repFac*_ecK+_ecN+1;i++){
//            //    printf((i==_repFac*_ecK+_ecN)?"%d\n":"%d ",loc[i]);
//            //}
//            if(_conf->getRepPlaPolicy()==1){
//                stripeOp(loc,loc+_repFac*_ecK,
//                        loc[_repFac*_ecK+_ecN]*_conf->getNodePerRack()+
//                        _randGen->generateInt(_conf->getNodePerRack()));
//            }else{
//                stripeOp(loc,loc+_repFac*_ecK,
//                        _randGen->generateInt(_conf->getNodeNum()));
//            }
//        }
//    }
//    return;
//}
/* Commented on Nov 5th, we need a MapReduce like stripe function ends */

void trafficManager::stripeOp(int* repLoc,int* ecLoc,int opNode){
  create("stripeOp");
  //printf("trafficManager::stripeOp() opNode:%d %lf\n",opNode,simtime());
  double startTime=simtime();
  //int sID=_stripeCounter;
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
  //if(_repPlaPolicy==0){
  //    for(int i=0;i<_ecK;i++){
  //        int* repPos=repLoc+i*_repFac;
  //        int needRA=1;
  //        for(int j=0;j<_repFac;j++){
  //            if(ecLoc[i]==repPos[j]){
  //                needRA=0;
  //                break;
  //            }
  //        }
  //        if(needRA==1){
  //            //printf("3stripeOp: %d %d\n",ecLoc[i],opNode);
  //            _nodeTree->dataTransfer(ecLoc[i],
  //                    _nodeTree->getNearest(ecLoc[i],_repFac,repPos),
  //                    _blockSize);
  //        }
  //    }
  //}
  //fprintf(stdout,"stripe op: ends at %lf",simtime());
  //fprintf(stdout,"stripe op %d: thpt %lf\n",sID,_ecK*_blockSize/(simtime()-startTime));
  _wholeStripeThpt+=_ecK*_blockSize/(simtime()-startTime);
  _completedStripeCounter++;
  free(repLoc);
  return;
}

void trafficManager::bgOp(int des,int src,double size,int flag){
  create("bgOp");
  double startTime=simtime();
  _nodeTree->dataTransfer(des,src,size);
  //fprintf(stdout,flag==0?"bg inter thpt %lf\n":"bg intra thpt %lf\n",size/(simtime()-startTime));
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
    if(s->uniform(0,1)>=_conf->_inRackTrafficPercent){
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
    hold(s->exponential(_conf->_bgTrafficInterval));
  }
}





