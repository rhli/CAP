#include "Switch.hh"

Switch::Switch(int portNum,int isRoot){
    _isRoot=isRoot;
    _childNum=portNum;
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
    if(_isRoot!=1){
        _toParent=new facility("swi");
        _fromParent=new facility("swi");
    }
}

int Switch::transferData(int des,int src,double amount){
    //printf("Switch::transferData(): des %d,src %d,amount %lf\n",des,src,amount);
    //printf("Switch::transferData(): startChild %d\n",_startChild);
    if(src==-1){
        _fromParent->reserve();
        /** TODO: add some randomness here */
        _toChildren[des-_startChild]->use(amount/_bandwidth);
        _fromParent->release();
    }else if(des==-1){
        _fromChildren[src-_startChild]->reserve();
        /** TODO: add some randomness here */
        _toParent->use(amount/_bandwidth);
        _fromChildren[src-_startChild]->release();
    }else{
        _fromChildren[src-_startChild]->reserve();
        /** TODO: add some randomness here */
        _toChildren[des-_startChild]->use(amount/_bandwidth);
        _fromChildren[src-_startChild]->release();
    }
    //printf(" Switch::transferData(): des %d,src %d,amount %lf\n",des,src,amount);
    return 0;
}

int Switch::reservePath(int des,int src){
    if(src==-1){
        _fromParent->reserve();
        _toChildren[des-_startChild]->reserve();
    }else if(des==-1){
        _fromChildren[src-_startChild]->reserve();
        _toParent->reserve();
    }else{
        _fromChildren[src-_startChild]->reserve();
        _toChildren[des-_startChild]->reserve();
    }
    return 0;
}

int Switch::releasePath(int des,int src){
    if(src==-1){
        _fromParent->release();
        _toChildren[des-_startChild]->release();
    }else if(des==-1){
        _fromChildren[src-_startChild]->release();
        _toParent->release();
    }else{
        _fromChildren[src-_startChild]->release();
        _toChildren[des-_startChild]->release();
    }
    return 0;
}
        
int Switch::setStartEnd(int start,int end){
    _startChild=start;
    _endChild=end;
    return 0;
}



