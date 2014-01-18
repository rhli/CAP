#include <cstdio>
#include <cstdlib>
#include "enumerator.hh"
#include "graph.hh"
#include "config.hh"
#include "randomGen.hh"

int main(int argc,char** argv){
    config* conf=new config();
    //graph* g=new graph(conf);
    //g->addEdge(0,0,0);
    //g->addEdge(1,3,1);
    //g->addEdge(2,9,2);
    //g->removeEdge(0,0,0);
    //g->showAdjMat();
    //printf("max flow: %d\n",g->maxFlow());
    randomGen* rg=new randomGen(12345);
    int tot=10;
    int req=5;
    int* temp=(int*)calloc(req,sizeof(int));
    rg->generateList(tot,req,temp);
    for(int i=0;i<req;i++){
        printf("%d ",temp[i]);
    }
    printf("\n");
    return 0;
}

