#include "layoutGen.hh"

layoutGen::layoutGen(int blockNum,config* conf){
    _conf=conf;
    _repFac=_conf->getReplicaNum();
    _blockNum=blockNum;
    _blockNum=_conf->getEcK();
    _stripeNum=_blockNum%_conf->getEcK()==0?_blockNum%_conf->getEcK():_blockNum%_conf->getEcK()+1;
    _graph=new graph(_conf);
}

/* Currently, we use this constructor */
layoutGen::layoutGen(config* conf){
    _conf=conf;
    _blockNum=_conf->getEcK();
    _repFac=_conf->getReplicaNum();
    _stripeNum=_blockNum%_conf->getEcK()==0?_blockNum%_conf->getEcK():_blockNum%_conf->getEcK()+1;
    _graph=new graph(_conf);
}

/*
 * We implement the default random placement by HDFS
 * The return value is a matrix containing where to place a block
 *
 * TODO: varify how HDFS does for more than three replicas.
 *
 * We place 1 block in a random rack and other blocks in another
 * random rack, but in different nodes.
 */
int layoutGen::randomPla(int* output){
    //int* retVal=(int*)calloc(_blockNum*_repFac,sizeof(int));
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
    //showPlacement(retVal);
    return 0;
}

int layoutGen::coreRackOnly(int* output){
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

/*
 * The return value is a matrix containing the block placement.
 * We benefit from stripe-oriented placement in two perspective:
 *  1. Reliability;
 *  2. Striping performance;
 *
 *  return the index of Core-rack
 *  TODO: add core-rack support
 */
int layoutGen::SOP(int* output){
    //int* retVal=(int*)calloc(_blockNum*_repFac,sizeof(int));
    int* rackInd=(int*)calloc(2,sizeof(int));
    int retVal=_randGen->generateInt(_conf->getRackNum());
    //graph* gra=new graph(_conf);
    _graph->graphInit();
    for(int i=0;i<_blockNum;i++){
        /* Every time, we **try** to generate a placement for ONE block. */
        int index=0;
        while(1){
            index++;
            //if(index>=11){
            //    printf("give up\n");
            //    return retVal;
            //}
            _graph->backGraph();
            int* pos=output+i*_repFac;
            /*
             * TODO: We can give higher probability to the nodes we prefer
             * (e.g., nodes with higher storage capacity, and/or better network link condition).
             * Currently, we just use a random placement.
             */
            //_randGen->generateList(_conf->getRackNum(),2,rackInd);
            rackInd[0]=retVal;
            while((rackInd[1]=_randGen->generateInt(_conf->getRackNum()))==rackInd[0]);
            _randGen->generateList(_conf->getNodePerRack(),1,pos);
            _randGen->generateList(_conf->getNodePerRack(),_repFac-1,pos+1);
            for(int j=0;j<_conf->getReplicaNum();j++){
                pos[j]+=j==0?rackInd[0]*_conf->getNodePerRack():rackInd[1]*_conf->getNodePerRack();
                //printf("add %d %d %d\n",i,pos[j],pos[j]/_conf->getNodePerRack());
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
        //printf("iteration:%d\n",index);
    }
    free(rackInd);
    return retVal;
}

int layoutGen::showPlacement(int* placement){
    for(int i=0;i<_blockNum;i++){
        printf("Block %4d:",i);
        for(int j=0;j<_conf->getReplicaNum();j++){
            printf("%4d ",placement[i*_conf->getReplicaNum()+j]);
        }
        printf("\n");
    }
    return 0;
}

int layoutGen::examinePla(int* pla){
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




