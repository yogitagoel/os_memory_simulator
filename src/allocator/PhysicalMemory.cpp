#include "PhysicalMemory.h"
#include <climits>
#include <iostream>
using namespace std;

PhysicalMemory::PhysicalMemory(int size){
    this->size=size;
    memory.resize(size);
    head=new Block{0,size,true,nullptr,nullptr};
    allocRequests=0;
    allocSuccess=0;
    allocFailure=0;
}

//Return the start of the allocated block
int PhysicalMemory::allocate(Block* cur,int reqSize){
    int st=cur->start;
    if(cur->size>reqSize){
        Block* newBlock=new Block{cur->start+reqSize,cur->size-reqSize,true,cur->next,cur};
        if(cur->next) cur->next->prev=newBlock;
    }
    cur->size=reqSize;
    cur->free=false;
    return st;
}
//First-fit memory allocation
int PhysicalMemory::allocateFirstFit(int reqSize){
    allocRequests++;
    Block* cur=head;
    while(cur){
        if(cur->free && cur->size>=reqSize){
            return allocate(cur,reqSize); 
        }
        cur=cur->next;
    }
    allocFailure++;
    return -1;
}
//Best-fit memory allocation
int PhysicalMemory::allocateBestFit(int reqSize){
    allocRequests++;
    Block* best=nullptr;
    int minSize=INT_MAX;
    Block* cur=head;
    while(cur){
        if(cur->free && cur->size>=reqSize){
            if(cur->size<minSize){
                minSize=cur->size;
                best=cur;
            }
        }
        cur=cur->next;
    }
    if(best){
        allocSuccess++;
        return allocate(best,reqSize);
    }else{
        allocFailure++;
        return -1;
    }
}
//Worst-fit memory allocation
int PhysicalMemory::allocateWorstFit(int reqSize){
    allocRequests++;
    Block* worst=nullptr;
    int maxSize=-1;
    Block* cur=head;
    while(cur){
        if(cur->free && cur->size>=reqSize){
            if(cur->size>maxSize){
                maxSize=cur->size;
                worst=cur;
            }
        }
        cur=cur->next;
    }
    if(worst){
        allocSuccess++;
        return allocate(worst,reqSize);
    }else{
        allocFailure++;
        return -1;
    }
}
void PhysicalMemory::freeMem(int st){
    Block* cur=head;
    while(cur){
        if(cur->start==st && !cur->free){
            cur->free=true;
        //Merge with next block
        if(cur->next && cur->next->free){
            Block* temp=cur->next;
            cur->size+=temp->size;
            cur->next=temp->next;
            if(temp->next){
                temp->next->prev=cur;
            }
            delete temp;
        }
        //Merge with previous block
        if(cur->prev && cur->prev->free){
            Block* temp=cur;
            cur->prev->size+=cur->size;
            cur->prev->next=cur->next;
            if(cur->next){
                cur->next->prev=cur->prev;
            }
            delete temp;
        }
        return;
        }
        cur=cur->next;
    }
}
void PhysicalMemory::dump(){
    cout<<"\n Physical Memory Layout: \n";
    Block* cur=head;
    while(cur){
        cout<<"["<<cur->start<<"-"<<cur->start+cur->size-1<<"]"<<(cur->free?"FREE":"USED")<<"("<<cur->size<<"bytes)\n";
        cur=cur->next;
    }
}
void PhysicalMemory::stats(){
    int totfree=0;
    int larfree=0;
    int allocated=0;
    Block* cur=head;
    double success=(allocRequests>0)?((double)allocSuccess/allocRequests):0;
    double fail=1-success;
    while(cur){
        if(cur->free){
            totfree+=cur->size;
            larfree=max(larfree,cur->size);
        }else{
            allocated+=cur->size;
        }
        cur=cur->next;
    }
    cout<<"\nStats:\n";
    cout<<"Free Memory: "<<totfree<<"bytes\n";
    cout<<"Allocated memory: "<<allocated<<"bytes\n";
    cout<<"Largest Free Block: "<<larfree<<"bytes\n";
    cout<<"Internal Fragmentation:"<<0<<'\n';

    if(totfree>0){
        double extfrag=1.0-(double)larfree/totfree;
        cout<<"External Fragmentation: "<<extfrag<<"\n";
    }
    double util=(double)allocated/size;
    cout<<"Memory Utilisation"<<util<<"\n";
    cout<<"Allocation Success Rate: "<<success<<"\n";
    cout<<"Allocation Failure Rate: "<<fail<<"\n";
}


