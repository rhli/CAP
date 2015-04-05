#include "PlacementManager.hh"

/**
 * Constructor
 * @param conf config* 
 */
PlacementManager::PlacementManager(config* conf){
    _conf=conf;
    _blockNum=_conf->getEcK();
    _repFac=_conf->getReplicaNum();
    //_stripeNum=_blockNum%_conf->getEcK()==0?_blockNum%_conf->getEcK():_blockNum%_conf->getEcK()+1;
    _graph=new FlowGraph(_conf);
}

/**
 * Generate random replication for ecK blocks
 *
 * @param output int* list storing the locations
 */
int PlacementManager::randomPla(int* output){
    int* rackInd=(int*)calloc(2,sizeof(int));
    for(int i=0;i<_blockNum;i++){
        int* pos=output+i*_conf->getReplicaNum();
        _randGen->generateList(_conf->getRackNum(),2,rackInd);
        _randGen->generateList(_conf->getNodePerRack(),1,pos);
        _randGen->generateList(_conf->getNodePerRack(),_conf->getReplicaNum()-1,pos+1);
        for(int j=0;j<_conf->getReplicaNum();j++){
            pos[j]+=j==0?rackInd[0]*_conf->getNodePerRack():rackInd[1]*_conf->getNodePerRack();
        }
    }
    free(rackInd);
    return 0;
}

/**
 * Priliminary EAR, where we do not consider availability
 *
 * @param output int* list storing the locations
 */
int PlacementManager::coreRackOnly(int* output){
    int* rackInd=(int*)calloc(2,sizeof(int));
    rackInd[0]=_randGen->generateInt(_conf->getRackNum());
    for(int i=0;i<_blockNum;i++){
        int* pos=output+i*_conf->getReplicaNum();
        while((rackInd[1]=_randGen->generateInt(_conf->getRackNum()))==rackInd[0]);
        _randGen->generateList(_conf->getNodePerRack(),1,pos);
        _randGen->generateList(_conf->getNodePerRack(),_conf->getReplicaNum()-1,pos+1);
        for(int j=0;j<_conf->getReplicaNum();j++){
            pos[j]+=j==0?rackInd[0]*_conf->getNodePerRack():rackInd[1]*_conf->getNodePerRack();
        }
    }
    free(rackInd);
    //showPlacement(retVal);
    return 0;
}

/**
 * Complete implementation of EAR with appointed core rack
 *
 * @param coreRack int rackID of coreRack
 * @param output int* list of blocks to be placed
 */
int PlacementManager::SOP(int coreRack,int* output){
    int* rackInd=(int*)calloc(2,sizeof(int));
    int retVal=coreRack;
    _graph->graphInit();
    for(int i=0;i<_blockNum;i++){
        /* Every time, we **try** to generate a placement for ONE block. */
        int index=0;
        while(1){
            index++;
            _graph->backGraph();
            int* pos=output+i*_repFac;
            rackInd[0]=retVal;
            while((rackInd[1]=_randGen->generateInt(_conf->getRackNum()))==rackInd[0]);
            _randGen->generateList(_conf->getNodePerRack(),1,pos);
            _randGen->generateList(_conf->getNodePerRack(),_repFac-1,pos+1);
            for(int j=0;j<_conf->getReplicaNum();j++){
                pos[j]+=j==0?rackInd[0]*_conf->getNodePerRack():rackInd[1]*_conf->getNodePerRack();
                _graph->addEdge(i,pos[j],pos[j]/_conf->getNodePerRack());
            }
            if(_graph->incrementalMaxFlow()==0){
                for(int j=0;j<_conf->getReplicaNum();j++){
                    _graph->removeEdge(i,pos[j],pos[j]/_conf->getNodePerRack());
                }
                _graph->restoreGraph();
            }else{
                break;
            }
        }
    }
    free(rackInd);
    return retVal;
}

/**
 * Show the placement
 *
 * @param placement int* the placement to show
 */
int PlacementManager::showPlacement(int* placement){
    for(int i=0;i<_blockNum;i++){
        printf("Block %4d:",i);
        for(int j=0;j<_conf->getReplicaNum();j++){
            printf("%4d ",placement[i*_conf->getReplicaNum()+j]);
        }
        printf("\n");
    }
    return 0;
}

/**
 * Examine the placement to see whether meet the max flow requirement
 *
 * @param pla int* the placement to examine
 */
int PlacementManager::examinePla(int* pla){
    _graph->graphInit();
    int repFac=_conf->getReplicaNum();
    for(int i=0;i<_conf->getEcK();i++){
        for(int j=0;j<repFac;j++){
            _graph->addEdge(i,pla[i*repFac+j],pla[i*repFac+j]/_conf->getNodePerRack());
        }
    }
    printf("max flow: %d\n",_graph->maxFlow());
    return _graph->maxFlow()==_conf->getEcK()?1:0;
}

/**
 * get locations of parity
 *
 * @param input int*
 * @param output int*
 * @param coreRack int the rack id of core rack
 */
int PlacementManager::getParityLoc(int* input,int* output,int coreRack){
  _graph->initFromPla(input);
  _graph->getMaxMatch(output);
  int* rackCount=(int*)calloc(_conf->getRackNum(),sizeof(int));
  for(int i=0;i<_blockNum;i++){
    if(output[i]!=-1){
      rackCount[output[i]/_conf->getNodePerRack()]++;
    }
  }
  for(int i=_conf->getEcK();i<_conf->getEcN();i++){
    int rackPos=-1;
    int nodePos=-1;
    while(1){
      if(rackCount[coreRack]<_conf->_maxInRack){
        rackPos=coreRack;
        break;
      }else{
        rackPos=_randGen->generateInt(_conf->getRackNum());
        if(rackCount[rackPos]<_conf->_maxInRack){
          break;
        }
      }
    }
    while(1){
      nodePos=_randGen->generateInt(_conf->getRackNum());
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
    rackCount[rackPos]++;
  }
  return 0;
}




