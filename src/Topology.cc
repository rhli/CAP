#include "Topology.hh"

/**
 * Topology simulates the network structure of the cluster. 
 * It has two functionalities:
 * 1. Simulate the network traffic
 * 2. Return tree structured information to help JobStatus assign
 * remote or local tasks. 
 *
 * @param leafNum int number of hosts
 * @param maxChild int number of hosts in one rack.
 */
Topology::Topology(int leafNum,int maxChild){
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
        if(level==0){
            _hostList=levelList;
        }else if(level==1){
            _rackList=levelList;
        }else{
          ;
        }
        for(int i=0;i<levelNodeNum;i++){
            /** the start and end index of this node */
            int startChild=i*_maxDegree;
            int endChild=(i+1)*_maxDegree-1>=lastLevelNum-1?lastLevelNum-1:(i+1)*_maxDegree-1;
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
    _dataTransferOnce=0.1;
    _nodeDisk=(facility**)calloc(_leafNum,sizeof(facility*));
    for(int i=0;i<_leafNum;i++){
      _nodeDisk[i]=new facility("disk");
    }

    nameSwitches();
    setSwitches(_treeRoot);
    setHostRange(_treeRoot);
}

/**
 * Create corresponding switches for non-leaf nodes.
 *
 * @param node tNode* we do in a recursive manner, input root node.
 */
int Topology::setSwitches(tNode* node){
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

int Topology::nameSwitches(){
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

int Topology::showSwitches(){
    showSubTree(_treeRoot);
    return 0;
}

int Topology::showSubTree(tNode* node){
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

/**
 * Get path from src to destination
 *
 * @param des int nodeID of destination
 * @param src int nodeID of source
 */
int* Topology::getPath(int des,int src){
    int* fromDes=(int*)calloc(_maxLevel,sizeof(int));
    int* fromSrc=(int*)calloc(_maxLevel,sizeof(int));
    tNode* nodeDes=_hostList[des];
    tNode* nodeSrc=_hostList[src];
    int desID=des;
    int srcID=src;
    int layer=0;
    while(1){
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
    int* retVal=(int*)calloc(2*layer+2,sizeof(int));
    retVal[0]=2*layer+1;
    for(int i=0;i<=layer;i++){
        retVal[i+1]=fromSrc[i];
        retVal[2*layer+1-i]=fromDes[i];
    }
    free(fromSrc);
    free(fromDes);
    return retVal;
}

/**
 * Get the path from root node to a leaf node.
 *
 * @param des int nodeID of the destination.
 *
 * @return path of nodes from root to a leaf node.
 */
int* Topology::getPathToLeaf(int des){
    int* retVal=(int*)calloc(_maxLevel+1,sizeof(int));
    tNode* nodeDes=_hostList[des];
    int layer=0;
    while(1){
        retVal[layer]=des;
        if(nodeDes==_treeRoot){
            break;
        }
        nodeDes=getParent(nodeDes);
        des=nodeDes->_tNodeID;
        layer++;
    }
    return retVal;
}

/**
 * Transfer data from disk to network
 *
 * @param des int nodeID of destination
 * @param src int nodeID of source
 * @param amount double amount of transmitted data
 */
int Topology::dataTransfer(int des,int src,double amount){
  double packet=1;
  int round=static_cast<int>(ceil(amount/packet));
  event** doneTrans=(event**)calloc(round,sizeof(event*));
  for(int i=0;i<round;i++){
    doneTrans[i]=new event("doneTrans");
    double transferedAmount=0;
    /** Do transfer */
    if(src!=-1){
      _nodeDisk[src]->reserve();
      hold(packet/_bandwidth/10*6.5/90*_bandwidth);
      _nodeDisk[src]->release();
    }
    transferedAmount+=packet;
    dataTransferNetwork(des,src,packet,doneTrans[i]);
  }
  for(int i=0;i<round;i++){
    doneTrans[i]->wait();
    delete doneTrans[i];
  }
  free(doneTrans);
  return 0;
}

/**
 * Transfer to disk of the destination node.
 *
 * @param des int nodeID of destination
 * @param src int nodeID of source
 * @param amount double amount of transmitted data
 */
int Topology::dataTransferTD(int des,int src,double amount){
  double packet=1;
  int round=static_cast<int>(ceil(amount/packet));
  event** doneTrans=(event**)calloc(round,sizeof(event*));
  for(int i=0;i<round;i++){
    doneTrans[i]=new event("doneTrans");
    if(src!=-1){
      _nodeDisk[src]->reserve();
      hold(packet/_bandwidth/10*6.5/90*_bandwidth);
      _nodeDisk[src]->release();
    }
    dataTransferNDisk(des,src,packet,doneTrans[i]);
  }
  for(int i=0;i<round;i++){
    doneTrans[i]->wait();
    delete doneTrans[i];
  }
  free(doneTrans);
  return 0;
}

/**
 * Transfer to disk of the destination node.
 *
 * @param des int nodeID of destination
 * @param src int nodeID of source
 * @param amount double amount of transmitted data
 */
void Topology::dataTransferNDisk(int des,int src,double amount,event* eve){
    create("transND");
    if(des==src){
        eve->set();
        return;
    }
    if(des<0||des>=_leafNum){
        fprintf(stderr,"Topology::dataTransfer(): des %d out of index\n",des);
        exit(0);
        eve->set();
        return;
    }
    if(src==-1){
        int* path=getPathToLeaf(des);
        double transferedAmount=0;
        while(transferedAmount<amount){
            /** Do transfer */
            double tAmount=_dataTransferOnce<amount-transferedAmount?
                _dataTransferOnce:amount-transferedAmount;
            for(int i=_maxLevel;i>0;i--){
                _switchList[path[i]]->reservePath(path[i-1],-1);
            }
            hold(tAmount/_bandwidth);
            for(int i=_maxLevel;i>0;i--){
                _switchList[path[i]]->releasePath(path[i-1],-1);
            }
            transferedAmount+=tAmount;
        }
        free(path);
        _nodeDisk[des]->reserve();
        hold(amount*1.2/_bandwidth/90*_bandwidth);
        _nodeDisk[des]->release();
        eve->set();
        return;
    }
    if(src<0||src>=_leafNum){
        fprintf(stderr,"Topology::dataTransfer(): out of index\n");
        eve->set();
        return;
    }
    if(amount==0){
        _nodeDisk[des]->reserve();
        hold(amount*1.2/_bandwidth/90*_bandwidth);
        _nodeDisk[des]->release();
        eve->set();
        return;
    }
    int* path=getPath(des,src);
    double transferedAmount=0;
    while(transferedAmount<amount){
        /** Do transfer */
        double tAmount=_dataTransferOnce<amount-transferedAmount?
            _dataTransferOnce:amount-transferedAmount;
        int switchNum=path[0]-2;
        int toTop=(switchNum-1)/2;
        int fromTop=(switchNum-1)/2;
        _switchList[path[2+toTop]]->reservePath(path[3+toTop],path[1+toTop]);
        for(int i=0;i<toTop;i++){
            _switchList[path[2+i]]->reservePath(-1,path[1+i]);
        }
        for(int i=0;i<fromTop;i++){
            _switchList[path[path[0]-1-i]]->reservePath(path[path[0]-i],-1);
        }
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
    _nodeDisk[des]->reserve();
    hold(amount*1.2/_bandwidth/90*_bandwidth);
    _nodeDisk[des]->release();
    free(path);
    eve->set();
    return;
}

/**
 * Transfer through network.
 *
 * @param des int nodeID of destination
 * @param src int nodeID of source
 * @param amount double amount of transmitted data
 */
void Topology::dataTransferNetwork(int des,int src,double amount,event* eve){
    create("transNet");
    if(des==src){
        eve->set();
        return;
    }
    if(des<0||des>=_leafNum){
        fprintf(stderr,"Topology::dataTransfer(): des %d out of index\n",des);
        exit(0);
        eve->set();
        return;
    }
    if(src==-1){
        int* path=getPathToLeaf(des);
        double transferedAmount=0;
        while(transferedAmount<amount){
            /** Do transfer */
            double tAmount=_dataTransferOnce<amount-transferedAmount?
                _dataTransferOnce:amount-transferedAmount;
            for(int i=_maxLevel;i>0;i--){
                _switchList[path[i]]->reservePath(path[i-1],-1);
            }
            hold(tAmount/_bandwidth);
            for(int i=_maxLevel;i>0;i--){
                _switchList[path[i]]->releasePath(path[i-1],-1);
            }
            transferedAmount+=tAmount;
        }
        free(path);
        eve->set();
        return;
    }
    if(src<0||src>=_leafNum){
        fprintf(stderr,"Topology::dataTransfer(): out of index\n");
        eve->set();
        return;
    }
    if(amount==0){
        eve->set();
        return;
    }
    int* path=getPath(des,src);
    double transferedAmount=0;
    while(transferedAmount<amount){
        /** Do transfer */
        double tAmount=_dataTransferOnce<amount-transferedAmount?
            _dataTransferOnce:amount-transferedAmount;
        int switchNum=path[0]-2;
        int toTop=(switchNum-1)/2;
        int fromTop=(switchNum-1)/2;
        _switchList[path[2+toTop]]->reservePath(path[3+toTop],path[1+toTop]);
        for(int i=0;i<toTop;i++){
            _switchList[path[2+i]]->reservePath(-1,path[1+i]);
        }
        for(int i=0;i<fromTop;i++){
            _switchList[path[path[0]-1-i]]->reservePath(path[path[0]-i],-1);
        }
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
    free(path);
    eve->set();
    return;
}

int Topology::setHostRange(tNode* node){
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

/**
 * get the nearest node from list
 *
 * @param node int id of destination node
 * @param len int length of list
 * @param list int* list of source to choose
 */
int Topology::getNearest(int node,int len,int* list){
    tNode* t=_hostList[node];
    while(1){
        for(int i=0;i<len;i++){
            if(isInSwitch(list[i],t)==1){
                return list[i];
            }
        }
        if(t==_treeRoot){
            fprintf(stderr,"Topology::getNearest(): no nodes\n");
            return -1;
        }
        t=t->_parent;
    }
    /*
     * Dummy code, while not get executed
     */
    return -1;
}

int Topology::isInSwitch(int node,tNode* tn){
    return (node>=tn->_startHost)&&(node<=tn->_endHost)?1:0;
}





