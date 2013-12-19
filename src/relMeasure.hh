/*
 * Explore the reliability issue of traditional random placement of
 * HDFS while striping to erasure-coded data.
 */
#include <cstdio>
#include <cstdlib>
#include "matrix.hh"

class{
        int _nodePerRack;
        int _rackNum;
        int _nodeNum;
        int _ecN;
        int _ecK;
        matrix* nodePosMat;
        matrix* rackPosMat;
    public:
        relMeasure();
};


