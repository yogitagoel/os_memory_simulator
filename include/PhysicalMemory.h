#ifndef PHYSICAL_MEMORY
#define PHYSICAL_MEMORY

#include <iostream>
#include <vector>
using namespace std;

struct Block{
    int start;
    int size;
    bool free;
    Block* next;
    Block* prev;
};

class PhysicalMemory{
    private:
    int size;
    int allocRequests;
    int allocSuccess;
    int allocFailure;
    Block* head;
    vector<char> memory;
    vector<Block> blocks;
    
    int allocate(Block* cur,int reqSize);

    public:

    PhysicalMemory(int size);

    int allocateFirstFit(int reqSize);
    int allocateBestFit(int reqSize);
    int allocateWorstFit(int reqSize);

    void freeMem(int st);

    void dump();
    void stats();
};

#endif