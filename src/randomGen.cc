#include "randomGen.hh"

randomGen::randomGen(int seed){
    _seed=seed;
    srand(_seed);
}

randomGen::randomGen(){
    _seed=12345;
    srand(_seed);
}

int randomGen::generateList(int tot,int req,int* buf){
    if(tot<req){
        return -1;
    }
    for(int i=0;i<req;i++){
        while(1){
            int generated=rand()%tot;
            /* check for duplication */
            int duplicated=0;
            for(int j=0;j<i;j++){
                if(generated==buf[j]){
                    duplicated=1;
                    break;
                }
            }
            if(duplicated==0){
                buf[i]=generated;
                break;
            }
        }
    }
    return 0;
}

int randomGen::generateInt(){
    return rand();
}

int randomGen::generateInt(int range){
    return rand()%range;
}


