#include <cstdio>
#include <cstdlib>
#include "graph.hh"
#include "config.hh"
#include "randomGen.hh"
#include "layoutGen.hh"
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
    max_processes(10000000);
    max_events(10000000);
    trafficManager* traM=new trafficManager(conf);
    hold(conf->_runTime);
}

