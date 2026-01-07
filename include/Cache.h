#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <string>
using namespace std;

struct CacheLine{
    bool valid;
    int tag;
    long long fifoTime;

    CacheLine():valid(false),tag(0),fifoTime(0){}
};

class Cache{
   private:
   int size;
   int blockSize;
   int associativity;
   int setnum;
   long long time;
   int hits;
   int misses;

   vector<vector<CacheLine>> sets;

   public:
   Cache(int size,int blockSize,int associativity);
   bool access(int addr);
   void stats(const string& name) const;
};

#endif