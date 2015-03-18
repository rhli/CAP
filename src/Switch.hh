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
    public:
        Switch(int ,int );
        /** set the ind of children */
        int setStartEnd(int start,int end);
        /** use -1 to represent parents */
        int reservePath(int des,int src);
        int releasePath(int des,int src);
};

#endif



