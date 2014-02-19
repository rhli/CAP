#ifndef _SWITCH_HH_
#define _SWITCH_HH_

#include <cstdio>
#include <cstdlib>
#include "cpp.h"

class Switch{
        int _isRoot;
        int _childNum;
        int _startChild;
        int _endChild;

        double _bandwidth;
        facility** _toChildren;
        facility** _fromChildren;
        //facility* _toParent;
        //facility* _fromParent;
    public:
        Switch(int portNum,int isRoot);
        /** set the ind of children */
        int setStartEnd(int start,int end);
        /** use -1 to represent parents */
        /* Currently not in use */
        int transferData(int des,int src,double amount);
        int reservePath(int des,int src);
        int releasePath(int des,int src);
};

#endif



