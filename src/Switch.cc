#include "Switch.hh"

/**
 * Switch simulates the switch in real world
 * @param childNum int number of lower level devices/hosts connected by this switch
 * @param isRoot int flag showing whether this switch is root switch or not
 */
Switch::Switch(int childNum,int isRoot){
    _isRoot=isRoot;
    _childNum=childNum;
    _bandwidth=1000000000/1024/1024/8;
    /** if this is the root, create facility for children ports
     * otherwise, also create facilities connecting parent
     */
    _toChildren=(facility**)calloc(_childNum,sizeof(facility*));
    _fromChildren=(facility**)calloc(_childNum,sizeof(facility*));
    for(int i=0;i<_childNum;i++){
        _toChildren[i]=new facility("swi");
        _fromChildren[i]=new facility("swi");
    }
}

/**
 * reserve the path from des to src
 *
 * @param des int destination node id
 * @param src int source node id
 */
int Switch::reservePath(int des,int src){
    if(src==-1){
        _toChildren[des-_startChild]->reserve();
    }else if(des==-1){
        _fromChildren[src-_startChild]->reserve();
    }else{
        _fromChildren[src-_startChild]->reserve();
        _toChildren[des-_startChild]->reserve();
    }
    return 0;
}

/**
 * release the path from des to src
 *
 * @param des int destination node id
 * @param src int source node id
 */
int Switch::releasePath(int des,int src){
    if(src==-1){
        _toChildren[des-_startChild]->release();
    }else if(des==-1){
        _fromChildren[src-_startChild]->release();
    }else{
        _fromChildren[src-_startChild]->release();
        _toChildren[des-_startChild]->release();
    }
    return 0;
}
        
/**
 * set the start and end id of children
 *
 * @param start int node id of first child
 * @param end int node id of last child
 */
int Switch::setStartEnd(int start,int end){
    _startChild=start;
    _endChild=end;
    return 0;
}



