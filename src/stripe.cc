#include "stripe.hh"

int stripe::getBlockPos(int blockID,int* pos){
    if(blockID>=_conf->getEcN()&&_isReplicated==0){
        fprintf(stderr,"stripe::getBlockPos(): id out of index\n");
        return -1;
    }
    if(blockID>=_conf->getEcK()&&_isReplicated==1){
        fprintf(stderr,"stripe::getBlockPos(): id out of index\n");
        return -1;
    }
    if(_isReplicated==1){
        memcpy((char*)pos,(char*)_locations+_repFac*blockID*sizeof(int),
                _repFac*sizeof(int));
    }else if(_isReplicated==0){
        memcpy((char*)pos,(char*)_locations+blockID*sizeof(int),sizeof(int));
    }
    return 0;
}



