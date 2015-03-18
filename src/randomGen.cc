#include "randomGen.hh"

/**
 * Constructor
 * randomGen generate random integers and non-overlapping lists.
 *
 * @param seed int random seed of the generator
 */
randomGen::randomGen(int seed){
    _seed=seed;
    srand(_seed);
}

randomGen::randomGen(){
    _seed=12345;
    srand(_seed);
}

/**
 * Generate a random list
 *
 * @param tot int total number of elements to choose from.
 * @param req int requested number of elements
 * @param buf int* list for storing data
 *
 * @return -1 for failure 0 for success
 */
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

/** 
 * Generate a random integer without limit
 *
 * @return generated integer
 */
int randomGen::generateInt(){
    return rand();
}

/** 
 * Generate a random integer without limit
 *
 * @param range int range to choose from.
 * @return generated random integer
 */
int randomGen::generateInt(int range){
    return rand()%range;
}


