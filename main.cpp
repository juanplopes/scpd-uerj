#include "mpi.h"
#include "imghash.h"
#include "mpihelper.h"
#include <bitset>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#define B 4
#define R 64/B
#define RMASK (1ull<<R)-1

using namespace std;


void worker(int rank) {
    struct dirent *entry;

    char dir[256];
    char file[256];
    sprintf(dir, "node%d", rank);
    DIR *dp;

    dp = opendir(dir);
    
    vector<uint64_t> hashes;
    vector<string> files;
    
    if (dp != NULL) {
        while((entry = readdir(dp))) {
            sprintf(file, "node%d/%s", rank, entry->d_name);

            if (entry->d_type == DT_REG) {
                uint64_t hash = imghash(file);
                hashes.push_back(hash);
                files.push_back(string(file));
            }
        }
        closedir(dp);
    }

    sendArray(&hashes[0], hashes.size(), 0);
    
    for(int i; (i = recvInt(0)) >= 0; ) {
        sendString(files[i].c_str(), files[i].size() + 1, 0);
    }
}

void coordinator(int workers) {
    map<uint64_t, set<uint64_t> > S[B];

    uint64_t A[1000000];
    
    for(int i=1; i<workers; i++) {
        int n = recvArray(A, i);
        
        for(int j=0; j<n; j++) {
            uint64_t id = (uint64_t)i << 32 | j;
        
            for(int k=0; k<B; k++) {
                uint64_t sub = A[j] & RMASK;
                S[k][sub].insert(id);
            }
        }
    }
    
    set<set<uint64_t> > groups;
    set<uint64_t> allIds;
    
    for(int i=0; i<B; i++) {
        for(map<uint64_t, set<uint64_t> >::iterator it = S[i].begin(); it!=S[i].end(); it++) {
            if (it->second.size() == 1) continue;
            groups.insert(it->second);
            allIds.insert(it->second.begin(), it->second.end());
        }
    }
    
    char file[256];
    map<uint64_t, string> names;
    for(set<uint64_t>::iterator it = allIds.begin(); it!=allIds.end(); it++) {
        int node = *it >> 32;
        int n = *it & 0xFFFFFFFF;
        sendInt(n, node);
        recvString(file, node);
        names[*it] = string(file);
    }
    
    for(int i=1; i<workers; i++) {
        sendInt(-1, i);
    }

    for(set<set<uint64_t> >::iterator it = groups.begin(); it!=groups.end(); it++) {
        cout << it->size() << endl;
        for(set<uint64_t>::iterator jt = it->begin(); jt != it->end(); jt++) {
            cout << names[*jt] << endl;
        }
    }

}


int main(int argc, char **argv) {
    int mpiTasks, mpiRank;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &mpiTasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
    
    if (mpiRank != 0)
        worker(mpiRank);
    else
        coordinator(mpiTasks);
    
    MPI_Finalize();
    return 0;
}
