#ifndef _BLOCKINFO_HH_
#define _BLOCKINFO_HH_

#include "config.hh"

class blockInfo{
        unsigned int _fileInd;
        unsigned int _blockInd;
        unsigned int _stripeInd;
        int _ecN;
        int _ecK;
        int _repFac;
        config* _conf;
        int _isParity;
        int _isReplicated;
        /* the positions of copies */
        int* _repPos;
        /* the position after striping */
        int _ecPos;
    public:
        blockInfo(config*,int,int);
        int setList(int* list){_repPos=list;return 0;};
        int* getList(){return _repPos;};
        int toErasureCoded(int pos);
        int isReplicated(int val){_isReplicated=val;return 0;};
        int isParity(int val){_isParity=val;return 0;};
};

#endif

