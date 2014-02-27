#include "NodeTree.hh"

/**
 * NodeTree simulates the network structure of the network. 
 * It has two functionalities:
 * 1. Simulate the network traffic
 * 2. Return tree structured information to help JobStatus assign
 * remote or local tasks. 
 */

NodeTree::NodeTree(int leafNum,int maxChild){
    /** construct tree in a bottom-up fashion */
    _leafNum=leafNum;
    _maxDegree=maxChild;
    tNode** lastLevel=NULL;
    int level=0;
    int levelNodeNum=leafNum;
    int lastLevelNum=0;
    tNode** levelList;
    while(levelNodeNum!=0){
        levelList=(tNode**)calloc(levelNodeNum,sizeof(tNode*));
        tNode** levelList=(tNode**)calloc(levelNodeNum,sizeof(tNode*));
        if(level==0){
            _hostList=levelList;
        }else if(level==1){
            _rackList=levelList;
        }
        for(int i=0;i<levelNodeNum;i++){
            /** the start and end index of this node */
            int startChild=i*_maxDegree;
            int endChild=(i+1)*_maxDegree-1>=lastLevelNum-1?lastLevelNum-1:(i+1)*_maxDegree-1;
            //printf(" startChild:%d endChild %d lastLevelNum:%d\n",
            //        startChild,endChild,lastLevelNum);
            levelList[i]=(tNode*)calloc(1,sizeof(tNode));
            levelList[i]->_parent=NULL;
            if(level==0){
                levelList[i]->_type=1;
                levelList[i]->_tNodeID=i;
                levelList[i]->_childNum=0;
            }else{
                levelList[i]->_type=0;
                levelList[i]->_childNum=endChild-startChild+1;
            }
            if(level==0){
                levelList[i]->_children=NULL;
            }else{
                /** define children and parents */
                levelList[i]->_children=(tNode**)calloc(endChild-startChild+1,sizeof(tNode*));
                for(int j=startChild;j<endChild+1;j++){
                    levelList[i]->_children[j-startChild]=lastLevel[j];
                    lastLevel[j]->_parent=levelList[i];
                }
            }
        }
        if(level==1){
            _rackNum=levelNodeNum;
        }
        lastLevel=levelList;
        if(levelNodeNum==1){
            _treeRoot=levelList[0];
            levelNodeNum=0;
            break;
        }
        lastLevelNum=levelNodeNum;
        levelNodeNum=levelNodeNum%_maxDegree==0?
            levelNodeNum/_maxDegree:levelNodeNum/_maxDegree+1;
        level++;
    }
    _maxLevel=level;
    //puts("NodeTree initialized");
    _dataTransferOnce=0.1;

    nameSwitches();
    setSwitches(_treeRoot);
    setHostRange(_treeRoot);
}

int NodeTree::setSwitches(tNode* node){
    if(node->_type==1){
        return 0;
    }
    if(node==_treeRoot){
        node->_switch=new Switch(node->_childNum,1);
        node->_switch->setStartEnd(node->_children[0]->_tNodeID,
                node->_children[node->_childNum-1]->_tNodeID);
        _switchList[node->_tNodeID]=node->_switch;
    }else{
        node->_switch=new Switch(node->_childNum,0);
        node->_switch->setStartEnd(node->_children[0]->_tNodeID,
                node->_children[node->_childNum-1]->_tNodeID);
        _switchList[node->_tNodeID]=node->_switch;
    }
    for(int i=0;i<node->_childNum;i++){
        setSwitches(node->_children[i]);
    }
    return 0;
}

int NodeTree::nameSwitches(){
    tNode** currentLevel=(tNode**)calloc(1,sizeof(tNode*));
    tNode** nextLevel=NULL;
    currentLevel[0]=_treeRoot;
    int currentLevelNodeNum=1;
    int nextLevelNodeNum;
    int switchInd=0;
    while(1){
        /** set the index of current level */
        for(int i=0;i<currentLevelNodeNum;i++){
            currentLevel[i]->_tNodeID=switchInd;
            switchInd++;
        }
        /** create a list for next level */
        nextLevelNodeNum=0;
        for(int i=0;i<currentLevelNodeNum;i++){
            nextLevelNodeNum+=currentLevel[0]->_childNum;
        }
        nextLevel=(tNode**)calloc(nextLevelNodeNum,sizeof(tNode*));
        int index=0;
        for(int i=0;i<currentLevelNodeNum;i++){
            if(currentLevel[i]!=NULL){
                for(int j=0;j<currentLevel[i]->_childNum;j++){
                    if(currentLevel[i]->_children[j]->_type!=1){
                        nextLevel[index]=currentLevel[i]->_children[j];
                        index++;
                    }
                }
            }
        }
        currentLevelNodeNum=index;
        currentLevel=nextLevel;
        if(currentLevelNodeNum==0){
            break;
        }
    }
    _switchCount=switchInd;
    _switchList=(Switch**)calloc(_switchCount,sizeof(Switch*));
    return 0;
}

int NodeTree::showSwitches(){
    showSubTree(_treeRoot);
    return 0;
}

int NodeTree::showSubTree(tNode* node){
    if(node->_type==0){
        printf("switch %d\n",node->_tNodeID);
    }else{
        printf("host %d\n",node->_tNodeID);
    }
    if(node->_childNum==0){
        return 0;
    }
    for(int i=0;i<node->_childNum;i++){
        showSubTree(node->_children[i]);
    }
    return 0;
}

int* NodeTree::getPath(int des,int src){
    int* fromDes=(int*)calloc(_maxLevel,sizeof(int));
    int* fromSrc=(int*)calloc(_maxLevel,sizeof(int));
    tNode* nodeDes=_hostList[des];
    tNode* nodeSrc=_hostList[src];
    int desID=des;
    int srcID=src;
    int layer=0;
    while(1){
        //printf("NodeTree::getPath(): des:%d src:%d\n",desID,srcID);
        fromDes[layer]=desID;
        fromSrc[layer]=srcID;
        if(desID==srcID){
            break;
        }
        nodeDes=getParent(nodeDes);
        nodeSrc=getParent(nodeSrc);
        desID=nodeDes->_tNodeID;
        srcID=nodeSrc->_tNodeID;
        layer++;
    }
    //printf("NodeTree::getPath(): layer %d \n",layer);
    int* retVal=(int*)calloc(2*layer+2,sizeof(int));
    retVal[0]=2*layer+1;
    for(int i=0;i<=layer;i++){
        retVal[i+1]=fromSrc[i];
        retVal[2*layer+1-i]=fromDes[i];
    }
    return retVal;
}

int NodeTree::dataTransfer(int des,int src,double amount){
    //printf("NodeTree::dataTransfer(): %d %d %lf\n",des,src,amount);
    if(des==src){
        return 0;
    }
    if(des<0||des>=_leafNum){
        fprintf(stderr,"NodeTree::dataTransfer(): out of index\n");
        return 0;
    }
    if(src<0||src>=_leafNum){
        fprintf(stderr,"NodeTree::dataTransfer(): out of index\n");
        return 0;
    }
    if(amount==0){
        return 0;
    }
    int* path=getPath(des,src);
    double specialBandwidth=10*1000000/1024/1024/8;
    //for(int i=0;i<path[0];i++){
    //    printf(i==0?"Path: %d":" %d",path[i+1]);
    //}
    //printf("\n");
    double startTime=simtime();
    double transferedAmount=0;
    while(transferedAmount<amount){
        //printf("NodeTree::dataTransfer():%lf transferred at %lf\n",transferedAmount,simtime());
        /** Do transfer */
        double tAmount=_dataTransferOnce<amount-transferedAmount?
            _dataTransferOnce:amount-transferedAmount;
        //printf("NodeTree::dataTransfer(): transfered:%lf amount:%lf tAmount %lf\n",
        //        transferedAmount,amount,tAmount);
        int switchNum=path[0]-2;
        int toTop=(switchNum-1)/2;
        int fromTop=(switchNum-1)/2;
        _switchList[path[2+toTop]]->reservePath(path[3+toTop],path[1+toTop]);
        for(int i=0;i<toTop;i++){
            //_switchList[path[2+i]]->transferData(-1,path[1+i],tAmount);
            _switchList[path[2+i]]->reservePath(-1,path[1+i]);
        }
        //_switchList[path[2+toTop]]->transferData(path[3+toTop],path[1+toTop],tAmount);
        for(int i=0;i<fromTop;i++){
            //_switchList[path[path[0]-1-i]]->transferData(path[path[0]-i],-1,tAmount);
            _switchList[path[path[0]-1-i]]->reservePath(path[path[0]-i],-1);
        }
        //TODO: add some randomness here
        //if(path[path[0]]==1){
        //    hold(tAmount/specialBandwidth);
        //}else{
        //    hold(tAmount/_bandwidth);
        //}
        hold(tAmount/_bandwidth);
        _switchList[path[2+toTop]]->releasePath(path[3+toTop],path[1+toTop]);
        for(int i=0;i<toTop;i++){
            _switchList[path[2+i]]->releasePath(-1,path[1+i]);
        }
        for(int i=0;i<fromTop;i++){
            _switchList[path[path[0]-1-i]]->releasePath(path[path[0]-i],-1);
        }
        transferedAmount+=tAmount;
    }
    //printf("NodeTree::dataTransfer(): finished %d %d %lf\n",des,src,amount);
    //printf("from: %d to: %d amount %lf time %lf\n",src,des,amount,simtime()-startTime);
    free(path);
    return 0;
}

int NodeTree::setHostRange(tNode* node){
    if(node->_type==1){
        node->_startHost=node->_tNodeID;
        node->_endHost=node->_tNodeID;
        return 0;
    }
    for(int i=0;i<node->_childNum;i++){
        setHostRange(node->_children[i]);
    }
    node->_startHost=node->_children[0]->_startHost;
    node->_endHost=node->_children[node->_childNum-1]->_endHost;
    return 0;
}

int* NodeTree::getNodeList(int node,int level){
    int* retVal=(int*)calloc(2,sizeof(int));
    tNode* topNode=_hostList[node];
    for(int i=0;i<level;i++){
        topNode=getParent(topNode);
    }
    retVal[0]=topNode->_startHost;
    retVal[1]=topNode->_endHost;
    return retVal;
}

int* NodeTree::getRackRange(int rackID){
    int* retVal=(int*)calloc(2,sizeof(int));
    retVal[0]=_rackList[rackID]->_startHost;
    retVal[1]=_rackList[rackID]->_endHost;
    return retVal;
}

/*
 * get the nearest node from list
 */
int NodeTree::getNearest(int node,int len,int* list){
    int retVal;
    tNode* t=_hostList[node];
    //printf("%d\n",node);
    //for(int i=0;i<len;i++){
    //    printf(i==(len-1)?"%4d\n":"%4d",list[i]);
    //}
    while(1){
        for(int i=0;i<len;i++){
            if(isInSwitch(list[i],t)==1){
                return list[i];
            }
        }
        if(t==_treeRoot){
            fprintf(stderr,"NodeTree::getNearest(): no nodes\n");
            return -1;
        }
        t=t->_parent;
    }
    /*
     * Dummy code, while not get executed
     */
    return -1;
}

int NodeTree::isInSwitch(int node,tNode* tn){
    //printf("NodeTree::isInSwitch():%d %d %d\n",node,tn->_startHost,tn->_endHost);
    return (node>=tn->_startHost)&&(node<=tn->_endHost)?1:0;
}





