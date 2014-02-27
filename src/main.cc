#include <cstdio>
#include <cstdlib>
#include "enumerator.hh"
#include "graph.hh"
#include "config.hh"
#include "randomGen.hh"
#include "layoutGen.hh"
#include "striping.hh"
#include "stripeManager.hh"
#include "trafficManager.hh"
#include "cpp.h"

extern "C" void sim(int argc,char** argv){
    create("sim");
    max_facilities(10000);
    max_processes(10000);
    config* conf=new config();
    trafficManager* traM=new trafficManager(conf);
    hold(11000);
    //stripeManager *strM=new stripeManager(conf);
    //for(int i=0;i<100;i++){
    //    strM->write();
    //}
    //strM->showNodeCapacity();
    //strM->strOp();
    //strM->showNodeCapacity();
    //if(argc!=2){
    //    printf("Usage: %s (seed)\n",argv[1]);
    //    exit(0);
    //}
    //layoutGen* layG=new layoutGen(100,conf);
    //randomGen* rg=new randomGen(atoi(argv[1]));
    //layG->setRandomGen(rg);
    ////int retVal=layG->examinePla(layG->randomPla());
    //int* pla=layG->SOP();
    //striping* strp=new striping(conf);
    //strp->setRandomGen(rg);
    //int* output=(int*)calloc(conf->getEcN(),sizeof(int));
    //strp->strOp(pla,output);
    //printf("%d\n",retVal);
    //graph* g=new graph(conf);
    //g->addEdge(0,0,0);
    //g->addEdge(1,3,1);
    //g->addEdge(2,9,2);
    //g->removeEdge(0,0,0);
    //g->showAdjMat();
    //printf("max flow: %d\n",g->maxFlow());
    //return retVal;
}

