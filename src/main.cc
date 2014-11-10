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

#define FAIL_IND 2

int compare(const void* a,const void* b){
    return (*(int*)b - *(int*)a);
}

extern "C" void sim(int argc,char** argv){
    create("sim");
    config* conf=new config();
    max_facilities(10000);
    max_processes(10000);
    trafficManager* traM=new trafficManager(conf);
    hold(3601);

    //workloadGen* wGen=new workloadGen(conf);
    //hold(11000);

    //stripeManager *strM=new stripeManager(conf);
    //for(int i=0;i<100;i++){
    //    strM->write();
    //}
    //strM->showNodeCapacity();
    //strM->strOp();
    //strM->showNodeCapacity();
    

    //config* conf=new config();
    //layoutGen* layG=new layoutGen(100,conf);
    //randomGen* rg=new randomGen();
    //layG->setRandomGen(rg);
    //int* output=(int*)calloc(conf->getEcK()*conf->getReplicaNum(),sizeof(int));
    //int* rackCountSop=(int*)calloc(conf->getRackNum(),sizeof(int));
    //int* rackCountConv=(int*)calloc(conf->getRackNum(),sizeof(int));

    //int* rackAccessLoadSop=(int*)calloc(conf->getRackNum(),sizeof(int));
    //int* rackAccessLoadConv=(int*)calloc(conf->getRackNum(),sizeof(int));
    //int* nodeCountSop=(int*)calloc(conf->getNodeNum(),sizeof(int));
    //int* nodeCountConv=(int*)calloc(conf->getNodeNum(),sizeof(int));
    //int retVal=0;
    //for(int i=0;i<10000;i++){
    //    rg->resetSeed(i);
    //    //layG->setRandomGen(rg);
    //    //layG->randomPla(output);
    //    layG->SOP(output);
    //    //layG->coreRackOnly(output);
    //    //retVal+=layG->examinePla(output);
    //    for(int i=0;i<conf->getEcK();i++){
    //        for(int j=0;j<conf->getReplicaNum();j++){
    //            rackCountSop[(output[i*conf->getReplicaNum()+j])/conf->getNodePerRack()]++;
    //            nodeCountSop[output[i*conf->getReplicaNum()+j]]++;
    //        }
    //        // HARD CODE HERE!!!
    //        rackAccessLoadSop[output[i*conf->getReplicaNum()]/conf->getNodePerRack()]++;
    //        rackAccessLoadSop[output[i*conf->getReplicaNum()+1]/conf->getNodePerRack()]++;
    //        //for(int j=0;j<conf->getReplicaNum();j++){
    //        //    if(output[i*conf->getReplicaNum()+j]==FAIL_IND){
    //        //        if(j==0){
    //        //            rackAccessLoadSop[output[i*conf->getReplicaNum()+1]/conf->getNodePerRack()]++;
    //        //        }else{
    //        //            //rackAccessLoadSop[output[i*conf->getReplicaNum()+1]/conf->getNodePerRack()]++;
    //        //        }
    //        //    }
    //        //}
    //    }
    //    layG->randomPla(output);
    //    for(int i=0;i<conf->getEcK();i++){
    //        for(int j=0;j<conf->getReplicaNum();j++){
    //            rackCountConv[(output[i*conf->getReplicaNum()+j])/conf->getNodePerRack()]++;
    //            nodeCountConv[output[i*conf->getReplicaNum()+j]]++;
    //        }
    //        rackAccessLoadConv[output[i*conf->getReplicaNum()]/conf->getNodePerRack()]++;
    //        rackAccessLoadConv[output[i*conf->getReplicaNum()+1]/conf->getNodePerRack()]++;
    //        //for(int j=0;j<conf->getReplicaNum();j++){
    //        //    if(output[i*conf->getReplicaNum()+j]==FAIL_IND){
    //        //        if(j==0){
    //        //            rackAccessLoadConv[output[i*conf->getReplicaNum()+1]/conf->getNodePerRack()]++;
    //        //        }else{
    //        //            //rackAccessLoadConv[output[i*conf->getReplicaNum()+1]/conf->getNodePerRack()]++;
    //        //        }
    //        //    }
    //        //}
    //    }
    //}
    //qsort(rackCountSop,conf->getRackNum(),sizeof(int),compare);
    //qsort(rackCountConv,conf->getRackNum(),sizeof(int),compare);
    //qsort(nodeCountSop,conf->getNodeNum(),sizeof(int),compare);
    //qsort(nodeCountConv,conf->getNodeNum(),sizeof(int),compare);
    //qsort(rackAccessLoadSop,conf->getRackNum(),sizeof(int),compare);
    //qsort(rackAccessLoadConv,conf->getRackNum(),sizeof(int),compare);
    //int rackALCountSop=0;
    //int rackALCountConv=0;
    //for(int i=0;i<conf->getRackNum();i++){
    //    rackALCountSop+=rackAccessLoadSop[i];
    //    rackALCountConv+=rackAccessLoadConv[i];
    //}
    ////qsort(nodeCount,conf->getNodeNum(),sizeof(int),compare);
    //for(int i=0;i<conf->getRackNum();i++){
    //    printf("%d %lf %lf %d %d",i,(double)rackCountConv[i]/360000,(double)rackCountSop[i]/360000,
    //            rackCountConv[i],rackCountSop[i]);
    //    printf("\n");
    //}
    //for(int i=0;i<conf->getNodeNum();i++){
    //    printf("%d %lf %lf",i,(double)nodeCountConv[i]/360000,(double)nodeCountSop[i]/360000);
    //    printf("\n");
    //}
    //for(int i=0;i<conf->getRackNum();i++){
    //    printf("%d %lf %lf",i,(double)rackAccessLoadConv[i]/rackALCountConv,
    //            (double)rackAccessLoadSop[i]/rackALCountSop);
    //    printf("\n");
    //}
    //for(int i=0;i<conf->getNodeNum();i++){
    //    printf("%d ",nodeCount[i]);
    //}
    //printf("\n");
    //printf("%d\n",retVal);

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

