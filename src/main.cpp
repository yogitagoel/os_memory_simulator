#include <iostream>
#include "../include/PhysicalMemory.h"
#include <sstream>
#include "../include/Cache.h"
#include "../include/MultilevelCache.h"
using namespace std;

void printHelp(){
    cout<<"\nAvailable commands:\n";
    cout<<" malloc <size> <first|best|worst>\n";
    cout<<" free <address>\n";
    cout<<" dump\n";
    cout<<" stats\n";
    cout<<" help\n";
    cout<<" CacheAccess <address>\n";
    cout<<" CacheStats\n";
    cout<<" exit \n";
}

int main(){
    PhysicalMemory pm(256);
    Cache l1(4,4,1);
    Cache l2(16,4,1);
    MultilevelCache Mc(l1,l2);
    cout<<"Memory management Simulator\n";
    string line;
    while(true){
        cout<<"mem> ";
        getline(cin,line);
        if(line.empty()) continue;
        stringstream ss(line);
        string cmd;
        ss>>cmd;
        if(cmd=="malloc"){
            int size;
            string strat;
            ss>>size>>strat;
            if(ss.fail()){
                cout<<"Usage: malloc <size> <first|best|worst>\n";
                continue;
            }
            int add=-1;
            if(strat=="first") add=pm.allocateFirstFit(size);
            else if(strat=="best") add=pm.allocateBestFit(size);
            else if(strat=="worst") add=pm.allocateWorstFit(size);
            else{
                cout<<"Unknown strategy\n";
                continue;
            }
            if(add==-1) cout<<"Allocation failed\n";
            else
                cout<<"Allocated "<< size<<" bytes at address "<<add<<"\n";
        }else if(cmd=="free"){
            int add;
            ss>>add;
            if(ss.fail()){
                cout<<"Usage: free <address>\n";
                continue;
            }
            pm.freeMem(add);
            cout<<"Freed Memory at address "<<add<<'\n';
        }else if(cmd=="dump"){
            pm.dump();
        }else if(cmd=="stats"){
            pm.stats();
        }else if(cmd=="CacheAccess"){
            int addr;
            ss>>addr;
            if(ss.fail()){
                cout<<"Usage: CacheAccess <address>\n";
                continue;
            }
            Mc.access(addr);
        }else if(cmd=="Cachestats"){
            Mc.cacheStats();
        }else if(cmd=="help"){
            printHelp();
        }else if(cmd=="exit"){
            cout<<"Exiting simulator.\n";
            break;
        }else{
            cout<<"Unknown command.Type 'help for commands.\n";
        }
    }
    return 0;
}
