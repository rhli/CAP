/*
 * This program explore the node level reliability after we
 * do striping without reallocating the data blocks.
 */

#include <cstdio>
#include <cstdlib>
#include "config.hh"

int main(int argc,char** argv){
    config* conf = new config();
    printf("erasure code: %d\n",conf->getEcK());
    return 0;
}
