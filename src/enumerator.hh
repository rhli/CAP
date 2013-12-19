/*
 * This class enumerates all possibilities of choosing
 * A elements out of a B-element set
 */
#include <cstdio>
#include <cstdlib>
#include <cstring>

class enumerator{
        int _totNum;
        int _selNum;
    public:
        enumerator(int total,int selected){_totNum=total;_selNum=selected;};
        int enumInit(int* selected);
        int enumNext(int* selected);
};
