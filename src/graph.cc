#include "graph.hh"

graph::graph(config* conf){
    _conf=conf;
    _blockNum=_conf->getEcK();
    _maxInRack=_conf->getEcN()-_blockNum;
    _replicaNum=_conf->getReplicaNum();
    _nodeNum=_conf->getNodeNum()<_replicaNum*_blockNum?
        _conf->getNodeNum():_replicaNum*_blockNum;
    _rackNum=_conf->getRackNum()<_replicaNum*_blockNum?
        _conf->getRackNum():_replicaNum*_blockNum;
    _vertexNum=2+_rackNum+_nodeNum+_blockNum;
    _blockOffset=1;
    _nodeOffset=1+_blockNum;
    _rackOffset=_nodeNum+_nodeOffset;
    _sinkOffset=_rackNum+_rackOffset;
    _nodeInd=(vertexInfo**)calloc(_nodeNum,sizeof(vertexInfo*));
    _rackInd=(vertexInfo**)calloc(_rackNum,sizeof(vertexInfo*));
    for(int i=0;i<_nodeNum;i++){
        _nodeInd[i]=new vertexInfo();
    }
    for(int i=0;i<_rackNum;i++){
        _rackInd[i]=new vertexInfo();
    }
    graphInit();
}

int graph::graphInit(){
    printf("_rackNum: %d\n",_rackNum);
    printf("_blockNum: %d\n",_blockNum);
    _veGraph=(int*)calloc(_vertexNum*_vertexNum,sizeof(int));
    /* We add two types of edges: source to block (capacity=1) and rack to sink (capacity=n-k) */
    for(int i=0;i<_blockNum;i++){
        _veGraph[i+_blockOffset]=1;
    }
    for(int i=0;i<_nodeNum;i++){
        _nodeInd[i]=new vertexInfo();
    }
    for(int i=0;i<_rackNum;i++){
        _rackInd[i]=new vertexInfo();
        _veGraph[(i+_rackOffset)*_vertexNum+_vertexNum-1]=_maxInRack;
    }
    return 0;
}

int graph::addEdge(int blockID,int nodeID,int rackID){
    /* The capacity of every edge is 1 */
    /* check whether the node is already in nodeInd */
    int marker=-1;
    int firstFree=-1;
    for(int i=0;i<_nodeNum;i++){
        if((_nodeInd[i]->isFree()==1)&&(firstFree==-1)){
            firstFree=i;
            continue;
        }else if(_nodeInd[i]->getVertexID()==nodeID){
            marker=i;
            break;
        }
    }
    if(marker==-1){
        marker=firstFree;
    }
    if(marker==-1){
        fprintf(stderr,"graph::addEdge(): some weird thing has happened..\n");
        return -1;
    }
    _nodeInd[marker]->setVertexID(nodeID);
    _veGraph[(blockID+_blockOffset)*_vertexNum+marker+_nodeOffset]=1;

    int rackMarker=-1;
    int firstRackFree=-1;
    for(int i=0;i<_rackNum;i++){
        if((_rackInd[i]->isFree()==1)&&(firstRackFree==-1)){
            firstRackFree=i;
            continue;
        }else if(_rackInd[i]->getVertexID()==rackID){
            rackMarker=i;
            break;
        }
    }
    if(rackMarker==-1){
        rackMarker=firstRackFree;
    }
    if(rackMarker==-1){
        fprintf(stderr,"graph::addEdge(): some weird thing has happened..\n");
        return -1;
    }
    _rackInd[rackMarker]->setVertexID(rackID);
    _veGraph[(marker+_nodeOffset)*_vertexNum+rackMarker+_rackOffset]=1;
    return 0;
}

int graph::removeEdge(int blockID,int nodeID,int rackID){
    if(blockID>=_blockNum||blockID<0){
        return -1;
    }
    int nodeMarker=-1;
    int rackMarker=-1;
    for(int i=0;i<_nodeNum;i++){
        if(_nodeInd[i]->getVertexID()==nodeID){
            nodeMarker=i;
        }
    }
    for(int i=0;i<_rackNum;i++){
        if(_rackInd[i]->getVertexID()==rackID){
            rackMarker=i;
        }
    }
    if((nodeMarker!=-1)&&
            (rackMarker!=-1)&&
            (_veGraph[(blockID+_blockOffset)*_vertexNum+nodeMarker+_nodeOffset]==1)){
        _veGraph[(blockID+_blockOffset)*_vertexNum+nodeMarker+_nodeOffset]=0;
        _nodeInd[nodeMarker]->inDegreeDec();
    }else{
        fprintf(stderr,"no such edge!");
    }
    /* mark node/rack as free vertex if necessary */
    if(_nodeInd[nodeMarker]->noInEdge()==1){
        _nodeInd[nodeMarker]->setVertexID(-1);
        _veGraph[(_nodeOffset+nodeMarker)*_vertexNum+_rackOffset+rackMarker]=0;
        _rackInd[rackMarker]->inDegreeDec();
        if(_rackInd[rackMarker]->noInEdge()){
            _rackInd[rackMarker]->setVertexID(-1);
        }
    }
    return 0;
}

int graph::showAdjMat(){
    for(int i=0;i<_vertexNum;i++){
        for(int j=0;j<_vertexNum;j++){
            printf("%2d",_veGraph[i*_vertexNum+j]);
        }
        printf("\n");
    }
    printf("\n");
    return 0;
}

