#include "TrafficManager.hh"

/**
 * Constructor
 *
 * @param c config* configuration of simulator
 */
TrafficManager::TrafficManager(config* c){
  _conf=c;
  _bandwidth=_conf->getBandwidth();
  _blockSize=_conf->getBlockSize();
  _ecN=_conf->getEcN();
  _ecK=_conf->getEcK();
  _repFac=_conf->getReplicaNum();
  _repPlaPolicy=_conf->getRepPlaPolicy();
  _nodeTree=new Topology(c->getNodeNum(),c->getNodePerRack());
  _nodeTree->setBandwidth(_bandwidth);
  _randGen=new randomGen(_conf->_randSeed);
  _stream = new stream();
  _stream->reseed(54321);

  _completedWriteCounter=0;
  _completedStripeCounter=0;
  _completedBgInterCounter=0;
  _completedBgIntraCounter=0;
  _wholeWriteThpt=0;
  _wholeStripeThpt=0;
  _wholeBgInterThpt=0;
  _wholeBgIntraThpt=0;

  _writeOverhead=0.33;
  _stripeOverhead=4;
  _stripeSwitchingOverhead=3.5;

  create("traM");

  write();
  hold(30);
  encode(_conf->_encodingStripeCount);
}

/**
 * write stream
 */
void TrafficManager::write(){
  create("write");
  if(_conf->_writeInterval==0){
    return;
  }
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
    int ** layout = (int**)calloc(_conf->getRackNum(),sizeof(int*));
    PlacementManager* lGen = new PlacementManager(_conf);
    int* raidTailIdx=(int*)calloc(_conf->getRackNum(),sizeof(int));
    for (int i=0;i<_conf->getRackNum();i++){
      layout[i]=(int*)calloc(_ecK*_conf->getReplicaNum(),sizeof(int));
      lGen->SOP(i,layout[i]);
    }
    while(1) {
      int* loc=(int*)calloc(_conf->getReplicaNum(),sizeof(int));
      int coreRack=_randGen->generateInt(_conf->getRackNum());
      memcpy((char*)loc,(char*)layout[coreRack]+raidTailIdx[coreRack]*_repFac*sizeof(int),
          _repFac*sizeof(int));
      raidTailIdx[coreRack]=(raidTailIdx[coreRack]+1)%_ecK;
      if(raidTailIdx[coreRack]==0) {
        lGen->SOP(coreRack,layout[coreRack]);
      }
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

/**
 * Pipiling writer
 *
 * @param size double size of written data
 * @param loc int* list of nodeID to write data
 * @param eve event* event to turn on after write operation finishes
 */
void TrafficManager::pipeline(double size,int* loc,event* eve){
  create("pipeline");
  for (int i=0;i<_repFac-1;i++){
    _nodeTree->dataTransferTD(loc[i+1],loc[i],size);
  }
  eve->set();
}

/**
 * an incluster-write operation
 *
 * @param loc int* list of locations to write data
 */
void TrafficManager::inClusWriteOp(int* loc){
  create("writeOp");
  _writeCounter++;
  double startTime=simtime();
  int* pos=loc;
  int packetSize=1;
  if (_conf->_bandwidth<=20){
    packetSize=32;
  }
  // checksum operations and other overheads.
  hold(_writeOverhead);
  event** doneEvent=(event**)calloc(_blockSize/packetSize,sizeof(event*));
  for(int i=0;i<_blockSize/packetSize;i++){
    doneEvent[i]=new event("doneFlag");
    _nodeTree->dataTransfer(pos[0],pos[0],packetSize);
    pipeline(packetSize,loc,doneEvent[i]);
  }
  for(int i=0;i<_blockSize/packetSize;i++){
    doneEvent[i]->wait();
    delete doneEvent[i];
  }
  free(doneEvent);
  fprintf(stdout,"inClusWriteOp: begins %lf ends %lf\n",startTime,simtime());
  _wholeWriteThpt+=_ecK*_blockSize/(simtime()-startTime);
  _completedWriteCounter++;
  free(loc);
  return;
}

/**
 * write operation whose writer is from off-cluster
 *
 * @param loc int* locations storing data.
 */
void TrafficManager::writeOp(int* loc){
  create("writeOp");
  _writeCounter++;
  double startTime=simtime();
  int* pos=loc;
  int packetSize=1;
  event** doneEvent=(event**)calloc(_blockSize/packetSize,sizeof(event*));
  for(int i=0;i<_blockSize/packetSize;i++){
    doneEvent[i]=new event("doneFlag");
    _nodeTree->dataTransferTD(pos[0],-1,_blockSize);
    pipeline(packetSize,loc,doneEvent[i]);
  }
  for(int i=0;i<_blockSize/packetSize;i++){
    doneEvent[i]->wait();
    delete doneEvent[i];
  }
  free(doneEvent);
  fprintf(stdout,"writeOp: begins %lf ends %lf\n",startTime,simtime());
  _wholeWriteThpt+=_ecK*_blockSize/(simtime()-startTime);
  _completedWriteCounter++;
  free(loc);
  return;
}

/**
 * stripe stream
 *
 * @param stripeCount int number of stripe to stripe.
 */ 
void TrafficManager::encode(int stripeCount){
  create("encode");
  int* rackStripeCount=(int*)calloc(_conf->getRackNum(),sizeof(int));
  for(int i=0;i<stripeCount*_ecK;i++){
    rackStripeCount[_randGen->generateInt(_conf->getRackNum())]++;
  }
  for(int i=0;i<_conf->getRackNum();i++){
    rackStripeCount[i]=
      rackStripeCount[i]%_ecK==0?rackStripeCount[i]/_ecK:rackStripeCount[i]/_ecK+1;
  }
  event** doneEnc=(event**)calloc(_conf->getRackNum(),sizeof(event*));
  for(int i=0;i<_conf->getRackNum();i++){
    doneEnc[i]=new event("doneEnc");
    if(_conf->getRepPlaPolicy()==1){
      encodeMap(i*_conf->getNodePerRack()+_randGen->generateInt(_conf->getNodePerRack()),
          stripeCount/_conf->getRackNum(),doneEnc[i]);
    }else{
      encodeMap(_randGen->generateInt(_conf->getNodeNum()),stripeCount/_conf->getRackNum(),doneEnc[i]);
    }
    hold(1);
  }
  for(int i=0;i<_conf->getRackNum();i++){
    doneEnc[i]->wait();
    free(doneEnc[i]);
  }
  free(doneEnc);
  hold(100);
}

/**
 * map task of a striping job
 *
 * @param id int nodeID of the TaskTracker
 * @param size int number of stripes to encode
 * @param eve event* event to trigger after striping.
 */
void TrafficManager::encodeMap(int id,int size,event* eve){
  create("encodeMap");
  double startTime;
  int coreRack=id/_conf->getNodePerRack();
  PlacementManager* lGen = new PlacementManager(_conf);
  int* repLocs=(int*)calloc(_ecK*_repFac,sizeof(int));
  for(int i=0;i<size;i++){
    startTime=simtime();
    // generate a stripe to process
    if(_conf->getRepPlaPolicy()==1){
      lGen->SOP(coreRack,repLocs);
    }else{
      int* rackInd=(int*)calloc(2,sizeof(int));
      for(int j=0;j<_ecK;j++){
        int* pos=repLocs+j*_conf->getReplicaNum();
        for(int idx=0;idx<_repFac;idx++){
          pos[idx]=pos[idx]/_conf->getNodePerRack()*_conf->getNodePerRack();
        }
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
        //printf("from %d\n",from);
        _nodeTree->dataTransfer(id,from,_blockSize);
      }else{
        fprintf(stderr,"TrafficManager::encodeMap(): weird thing happened\n");
      }
    }
    /* hold some time for computation */
    hold(_stripeOverhead);
    int* ecLoc=(int*)calloc(_ecN,sizeof(int));
    lGen->getParityLoc(repLocs,ecLoc,coreRack);
    for(int j=_ecK;j<_ecN;j++){
      _nodeTree->dataTransferTD(ecLoc[j],id,_blockSize);
    }
    fprintf(stdout,"encodeOp: begins %lf ends %lf\n",startTime,simtime());
    /* hold some time for switching stripes */
    hold(_stripeSwitchingOverhead);
  }
  eve->set();
  free(repLocs);
}


/**
 * stripe operation of single stripe
 *
 * @param repLoc int* locations of replicated data blocks
 * @param ecLoc int* locations of erasure-coded data and parity blocks
 * @param opNode int nodeID carrying out the operation
 */
void TrafficManager::encodeOp(int* repLoc,int* ecLoc,int opNode){
  create("encodeOp");
  //double startTime=simtime();
  _stripeCounter++;
  /* do download */
  for(int i=0;i<_ecK;i++){
    int* repPos=repLoc+i*_repFac;
    int from=_nodeTree->getNearest(opNode,_repFac,repPos);
    if(from!=-1){
      _nodeTree->dataTransfer(opNode,from,_blockSize);
    }else{
      fprintf(stderr,"TrafficManager::encodeOp(): weird thing happened\n");
    }
  }
  /* write parities */
  for(int i=_ecK;i<_ecN;i++){
    //printf("2stripeOp: %d %d\n",ecLoc[i],opNode);
    _nodeTree->dataTransfer(ecLoc[i],opNode,_blockSize);
  }
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
  _completedStripeCounter++;
  free(repLoc);
  return;
}


