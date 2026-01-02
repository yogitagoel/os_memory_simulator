#include "Cache.h"
#include <iostream>
#include <climits>

Cache::Cache(int size,int blockSize,int assoc){
    this->size=size;
    this->blockSize=blockSize;
    this->associativity=assoc;
    hits=0;
    misses=0;
    time=0;
    setnum=size/(blockSize*assoc);
    sets.resize(setnum,vector<CacheLine>(assoc));
}

bool Cache::access(int addr){
    time++;
    int blockAddr=addr/blockSize;
    int setIdx=blockAddr%setnum;
    int tag=blockAddr/setnum;
    for(auto &line:sets[setIdx]){
        if(line.valid && line.tag==tag){
            hits++;
            return true;
        }
    }
    misses++;
    for(auto &line:sets[setIdx]){
        if(!line.valid){
            line.valid=true;
            line.tag=tag;
            line.fifoTime=time;
            return false;
        }
    }
    int vic=0;
    long long oldest=LLONG_MAX;
    for(int i=0;i<associativity;i++){
        if(sets[setIdx][i].fifoTime<oldest){
            oldest=sets[setIdx][i].fifoTime;
            vic=i;
        }
    }
    sets[setIdx][vic].tag=tag;
    sets[setIdx][vic].fifoTime=time;
    sets[setIdx][vic].valid=true;

    return false;
}

void Cache::stats(const string& name) const{
    int total=hits+misses;
    double hitRatio=total?(double)hits/total:0.0;
    cout<<"\n"<<name<<"Cache Stats \n";
    cout<<"Hits: "<<hits<<"\n";
    cout<<"Misses: "<<misses<<"\n";
    cout<<"Hit Ratio: "<<hitRatio<<"\n";
}