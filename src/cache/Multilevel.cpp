#include "../../include/MultilevelCache.h"
#include <iostream>
#include <climits>

MultilevelCache::MultilevelCache(Cache& l1,Cache& l2):L1(l1),L2(l2){}

void MultilevelCache::access(int addr){
    if(L1.access(addr)){
        l1_hit++;
        std::cout<<addr<<" Found in L1 cache\n";
        return;        
    }
    l1_miss++;
    if(L2.access(addr)){
        l2_hit++;
        std::cout<<addr<<" Found in L2 cache\n";
    }else{
        l2_miss++;
        std::cout<<addr<<" Not found in L1 and L2 cache\n";
    }
}

void MultilevelCache::cacheStats(){
    std::cout<<"L1 hits: "<<l1_hit<<"\n";
    std::cout<<"L1 misses: "<<l1_miss<<"\n";
    std::cout<<"L2 hits: "<<l2_hit<<"\n";
    std::cout<<"L2 misses: "<<l2_miss<<"\n";
}