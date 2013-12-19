#include "enumerator.hh"

/* Generate a first output of selected subset */
int enumerator::enumInit(int* selected){
    for(int i=0;i<_selNum;i++){
        selected[i]=i;
    }
    return 0;
}

/* Generate next selected subset */
int enumerator::enumNext(int* selected){
    int mark=-1;
    for(int i=_selNum-1;i>=0;i--){
        if(selected[i]==_totNum-_selNum+i){
            continue;
        }else{
            mark=i;
            break;
        }
    }
    if(mark==-1){
        /* Enumeration accomplished */
        return -1;
    }else{
        selected[mark]++;
        for(int i=mark+1;i<_selNum;i++){
            selected[i]=selected[i-1]+1;
        }
    }
    return 0;
}

