/*
 * Used for generate random list
 */
#ifndef _RANDOMGEN_HH_
#define _RANDOMGEN_HH_

#include <cstdio>
#include <cstdlib>

class randomGen{
        int _seed;
    public:
        randomGen(int seed);
        int resetSeed(int seed){_seed=seed;return 0;};
        int generateList(int total,int req,int* buf);
};

#endif

