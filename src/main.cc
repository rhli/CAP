#include <cstdio>
#include <cstdlib>
#include "enumerator.hh"
#include "graph.hh"
#include "config.hh"

int main(int argc,char** argv){
    config* conf=new config();
    graph* g=new graph(conf);
    g->addEdge(0,0,0);
    g->addEdge(1,3,1);
    g->addEdge(2,9,2);
    g->removeEdge(0,0,0);
    g->showAdjMat();
    printf("max flow: %d\n",g->maxFlow());
    return 0;
}

