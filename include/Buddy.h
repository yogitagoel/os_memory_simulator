#ifndef BUDDY_H
#define BUDDY_H
#include <vector>
#include <iostream>
using namespace std;
class Buddy
{
    struct Block
    {
        int ord;
        Block *prev;
        Block *next;
    };
    int mxord;
    int size;
    Block **blocks;

    int getOrd(int sz);
    void split(int ord);
    void merge(Block *bl);

    Block *getBud(Block *bl);
    void rem(Block *bl);
    void add(Block *bl);

public:
    Buddy(int sz);
    Block *access(int sz);
    void free(void *bl);
};

#endif