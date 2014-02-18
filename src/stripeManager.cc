#include "stripeManager.hh"

stripeManager::stripeManager(config* c){
    _conf=c;
    _stripeQueue=NULL;
    _stripeCount=0;
    _repPlaPolicy=_conf->getRepPlaPolicy();
    _nodeNum=_conf->getNodeNum();
    _rackNum=_conf->getRackNum();
    _repFac=_conf->getReplicaNum();
    _ecN=_conf->getEcN();
    _ecK=_conf->getEcK();

    _blockCountInNode=(int*)calloc(_nodeNum,sizeof(int));
    _blockCountInRack=(int*)calloc(_rackNum,sizeof(int));

    _lGen=new layoutGen(_conf);
    _lGen->setRandomGen(new randomGen(12345));
    _striping=new striping(_conf);
}

int stripeManager::pushStripe(stripe* str){
    stripeNode* tmp=(stripeNode*)calloc(1,sizeof(stripeNode));
    tmp->_str=str;
    tmp->_next=NULL;
    _sQueueTail=tmp;
    if(_stripeQueue==NULL){
        _stripeQueue=tmp;
    }else{
        _sQueueTail->_next=tmp;
    }
    /* TODO: add striping time */

    _stripeCount++;
    return 0;
}

stripe* stripeManager::popStripe(){
    stripe* retVal=_stripeQueue->_str;
    stripeNode* tmp=_stripeQueue;
    _stripeQueue=_stripeQueue->_next;
    free(tmp);
    _stripeCount--;
    return retVal;
}

int strOp(){
    while(1){
        //TODO: do striping ...
        ;
    }
    return 0;
}

stripe* stripeManager::write(){
    stripe* retVal=new stripe(_conf);
    //puts("write(): before set loc");
    if(_repPlaPolicy==0){
        retVal->setLoc(_lGen->randomPla());
    }else{
        retVal->setLoc(_lGen->SOP());
    }
    //puts("write(): after set loc");
    addRepStripe(retVal->getLoc());
    pushStripe(retVal);
    return retVal;
}

int stripeManager::addRepStripe(int* loc){
    for(int i=0;i<_repFac*_ecK;i++){
        _blockCountInNode[loc[i]]++;
    }
    return 0;
}

int stripeManager::showNodeCapacity(){
    printf("stripeManager::showNodeCapacity():\n");
    for(int i=0;i<_nodeNum;i++){
        printf("Node %5d:%5d\n",i,_blockCountInNode[i]);
    }
    return 0;
}

int stripeManager::showRackCapacity(){
    //TODO: implement this
    return 0;
}





