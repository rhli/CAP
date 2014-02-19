#include "trafficManager.hh"

trafficManager::trafficManager(config* c){
    _conf=c;
    _ecN=_conf->getEcN();
    _ecK=_conf->getEcK();
    _nodeTree=new NodeTree(c->getNodeNum(),c->getNodePerRack());
    _strM=new stripeManager(c);

    create("traM");
    printf("trafficManager::trafficManager()\n");
    test1();
    test2();
    test3();
    hold(10);
}

void trafficManager::test1(){
    create("test1");
    printf("trafficManager::test1() starts at %lf\n",simtime());
    hold(5);
    printf("trafficManager::test1() hold for 5s at %lf\n",simtime());
    return;
}

void trafficManager::test2(){
    create("test2");
    printf("trafficManager::test2() starts\n");
    hold(5);
    printf("trafficManager::test2() hold for 5s\n");
    return;
}

void trafficManager::test3(){
    create("test3");
    printf("trafficManager::test3() starts\n");
    hold(5);
    printf("trafficManager::test3() hold for 5s\n");
    return;
}


