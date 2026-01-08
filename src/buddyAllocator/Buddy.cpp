#include "../../include/Buddy.h"
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <cstring>
using namespace std;

Buddy::Buddy(int sz)
{
    size = sz;
    mxord = log2(sz);
    blocks = new Block *[mxord + 1];
    memset(blocks, 0, sizeof(Block *) * (mxord + 1));
    Block *init = (Block *)malloc(sz);
    init->ord = mxord;
    init->prev = init->next = nullptr;
    blocks[mxord] = init;
}

int Buddy::getOrd(int sz)
{
    int ord = 0;
    int cur = 1;
    while (cur < sz)
    {
        cur = cur << 1;
        ord++;
    }
    return ord;
}

void Buddy::add(Block *bl)
{
    int o = bl->ord;
    bl->prev = nullptr;
    bl->next = blocks[o];
    if (blocks[o])
    {
        blocks[o]->prev = bl;
    }
    blocks[o] = bl;
}

void Buddy::rem(Block *bl)
{
    int o = bl->ord;
    if (bl->prev)
    {
        bl->prev->next = bl->next;
    }
    else
    {
        blocks[o] = bl->next;
    }
    if (bl->next)
    {
        bl->next->prev = bl->prev;
    }
    bl->prev = bl->next = nullptr;
}

void Buddy::split(int ord)
{
    Block *bl = blocks[ord];
    rem(bl);
    int nord = ord - 1;
    int blsz = 1 << nord;
    Block *bud = (Block *)((char *)bl + blsz);
    bl->ord = nord;
    bud->ord = nord;
    add(bl);
    add(bud);
}

Buddy::Block *Buddy::getBud(Block *bl)
{
    int blsz = 1 << bl->ord;
    uintptr_t offset = (uintptr_t)bl;
    uintptr_t bud = offset ^ blsz;
    return (Block *)bud;
}

void Buddy::merge(Block *bl)
{
    while (bl->ord < mxord)
    {
        Block *bud = getBud(bl);
        if (bud->ord != bl->ord)
            break;
        rem(bud);
        if (bud < bl)
            bl = bud;
        bl->ord++;
    }
    add(bl);
}

Buddy::Block *Buddy::access(int sz)
{
    int ord = getOrd(sz);
    int i = ord;
    while (i <= mxord && blocks[i] == nullptr)
    {
        i++;
    }
    if (i > mxord)
    {
        cout << "Out of memory\n";
        return nullptr;
    }
    while (i > ord)
    {
        split(i);
        i--;
    }
    Block *res = blocks[ord];
    rem(res);
    return res;
}

void Buddy::free(void *bl)
{
    Block *blo = (Block *)bl;
    merge(blo);
}