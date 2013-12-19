#include <cstdio>
#include <cstdlib>
#include "enumerator.hh"

int printArray(int* arr, int len){
    for(int i=0;i<len;i++){
        printf("%3d",arr[i]);
    }
    printf("\n");
    return 0;
}

int main(int argc,char** argv){
    int tot=6;
    int sel=3;
    enumerator* eNum=new enumerator(tot,sel);
    int* selected=(int*)calloc(sel,sizeof(int));
    eNum->enumInit(selected);
    printArray(selected,sel);
    while(eNum->enumNext(selected)==0){
        printArray(selected,sel);
    }
    return 0;
}
