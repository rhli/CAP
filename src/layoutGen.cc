#include "layoutGen.hh"

layoutGen::layoutGen(int blockNum,config* conf){
    _conf=conf;
    _blockNum=blockNum;
    _blockNum=_conf->getEcK();
    _stripeNum=_blockNum%_conf->getEcK()==0?_blockNum%_conf->getEcK():_blockNum%_conf->getEcK()+1;
    _graph=new graph(_conf);
}

/*
 * We implement the default random placement by HDFS
 * The return value is a matrix containing where to place a block
 *
 * TODO: varify
 * We place 1 block in a random rack and other blocks in another
 * random rack, but in different nodes.
 */
int* layoutGen::randomPla(){
    int* retVal=(int*)calloc(_blockNum*_repFac,sizeof(int));
    int* rackInd=(int*)calloc(2,sizeof(int));
    for(int i=0;i<_blockNum;i++){
        int* pos=retVal+i*_conf->getReplicaNum();
        _randGen->generateList(_conf->getRackNum(),2,rackInd);
        _randGen->generateList(_conf->getNodePerRack(),1,pos);
        _randGen->generateList(_conf->getNodePerRack(),_conf->getReplicaNum()-1,pos+1);
        for(int j=0;j<_conf->getReplicaNum();j++){
            pos[j]+=j==0?rackInd[0]*_conf->getNodePerRack():rackInd[1]*_conf->getNodePerRack();
        }
    }
    free(rackInd);
    //showPlacement(retVal);
    return retVal;
}

/*
 * The return value is a matrix containing the block placement.
 * We benefit from stripe-oriented placement in two perspective:
 *  1. Reliability;
 *  2. Striping performance;
 */
int* layoutGen::SOP(){
    int* retVal=(int*)calloc(_blockNum*_repFac,sizeof(int));
    int* rackInd=(int*)calloc(2,sizeof(int));
    for(int i=0;i<_blockNum;i++){
        /* Every time, we **try** to generate a placement for ONE block. */
        while(1){
            _graph->backGraph();
            int* pos=retVal+i*_conf->getReplicaNum();
            _randGen->generateList(_conf->getRackNum(),2,rackInd);
            _randGen->generateList(_conf->getNodePerRack(),1,pos);
            _randGen->generateList(_conf->getNodePerRack(),_conf->getReplicaNum()-1,pos+1);
            for(int j=0;j<_conf->getReplicaNum();j++){
                pos[j]+=j==0?rackInd[0]*_conf->getNodePerRack():rackInd[1]*_conf->getNodePerRack();
            }
            if(_graph->incrementalMaxFlow()==0){
                _graph->restoreGraph();
            }else{
                break;
            }
        }
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
    //printf("max flow: %d\n",_graph->maxFlow());
    return _graph->maxFlow()==_conf->getEcK()?1:0;
}




