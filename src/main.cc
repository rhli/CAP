#include <cstdio>
#include <cstdlib>
#include "config.hh"
#include "TrafficManager.hh"
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
    TrafficManager* traM=new TrafficManager(conf);
    hold(conf->_runTime);
}

