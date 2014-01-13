#include <cstdio>
#include <cstdlib>
#include "enumerator.hh"
#include "graph.hh"
#include "config.hh"

int main(int argc,char** argv){
    config* conf=new config();
    graph* g=new graph(conf);
    g->addEdge(0,0,0);
    g->showAdjMat();
    return 0;
}

