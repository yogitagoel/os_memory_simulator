#ifndef MULTILEVEL_CACHE_H
#define MULTILEVEL_CACHE_H

#include "Cache.h"

class MultilevelCache{
    Cache L1;
    Cache L2;

    int l1_hit=0;
    int l1_miss=0;
    int l2_hit=0;
    int l2_miss=0;

    public:
    MultilevelCache(Cache& l1,Cache& l2);

    void access(int addr);

    void cacheStats();
};

#endif