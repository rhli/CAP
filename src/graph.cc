/*
 * We use a depth-first search to search for a path in residual graph.
 */
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
    _vertexColor=(int*)calloc(_vertexNum,sizeof(int));
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
    //printf(" marker: %d\n",marker);
    if(marker==-1){
        fprintf(stderr,"graph::addEdge(): some weird thing has happened..\n");
        return -1;
    }
    _nodeInd[marker]->setVertexID(nodeID);
    _nodeInd[marker]->inDegreeInc();
    _veGraph[(blockID+_blockOffset)*_vertexNum+marker+_nodeOffset]=1;

    int rackMarker=-1;
    int firstRackFree=-1;
    for(int i=0;i<_rackNum;i++){
        if((_rackInd[i]->noInEdge()==1)&&(firstRackFree==-1)){
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
    _rackInd[rackMarker]->inDegreeInc();
    _nodeInd[marker]->outDegreeInc();
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

int graph::showResMat(){
    for(int i=0;i<_vertexNum;i++){
        for(int j=0;j<_vertexNum;j++){
            printf("%2d",_resGraph[i*_vertexNum+j]);
        }
        printf("\n");
    }
    printf("\n");
    return 0;
}

int graph::pathSearch(){
    /* this function operates on residual graphs, it gets a path from source to sink */
    /* We use depth first search */
    memset(_vertexColor,0,_vertexNum*sizeof(int));
    _pathLen=-1;
    dfs(0);
}

int graph::dfs(int vID){
    //printf("graph::dfs(%d)\n",vID);
    /* return value 0 means there is no path to sink and 1 means yes */
    int retVal=0;
    _pathLen++;
    _vertexColor[vID]=1;
    if(vID==_vertexNum-1){
        retVal=1;
        _path=(int*)calloc(_pathLen+1,sizeof(int));
        _path[_pathLen]=vID;
        _pathLen--;
        return retVal;
    }
    for(int i=0;i<_vertexNum;i++){
        if((_resGraph[vID*_vertexNum+i]!=0)&&
                (_vertexColor[i]==0)){
            if(dfs(i)==1){
                retVal=1;
                break;
            }
        }
    }
    if(retVal!=0){
        _path[_pathLen]=vID;
    }
    _vertexColor[vID]=2;
    _pathLen--;
    return retVal;
}

int graph::maxFlow(){
    int retVal=0;
    initResGraph();
    while(pathSearch()!=0){
        /* Find a path, update _resGraph and search for the next round */
        int edgeCount=0;
        //printf("path:\n");
        int minCap=INT_MAX;
        /*
         * Rules for updating residual graph:
         *  1. find minimal capacity of the path
         *  2. add reverse edges
         */
        while(_path[edgeCount]!=_vertexNum-1){
            //printf(" %2d %2d\n",_path[edgeCount],_path[edgeCount+1]);
            if(_resGraph[_path[edgeCount]*_vertexNum+_path[edgeCount+1]]<minCap){
                minCap=_resGraph[_path[edgeCount]*_vertexNum+_path[edgeCount+1]];
            }
            edgeCount++;
        }
        edgeCount=0;
        while(_path[edgeCount]!=_vertexNum-1){
            _resGraph[_path[edgeCount]*_vertexNum+_path[edgeCount+1]]-=minCap;
            _resGraph[_path[edgeCount+1]*_vertexNum+_path[edgeCount]]+=minCap;
            edgeCount++;
        }
        retVal+=minCap;
        //break;
        //showResMat();
    }
    return retVal;
}

int graph::initResGraph(){
    _resGraph=(int*)calloc(_vertexNum*_vertexNum,sizeof(int));
    memcpy((char*)_resGraph,(char*)_veGraph,_vertexNum*_vertexNum*sizeof(int));
    return 0;
}

