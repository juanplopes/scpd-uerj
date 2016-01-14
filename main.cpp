#include "mpi.h"
#include "imghash.h"
#include <bitset>
#include <vector>
#include <map>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#define B 4
#define R 64/B
#define RMASK (1<<R)-1

using namespace std;

void sendArray(uint64_t* V, int n, int to) {
    MPI_Send(&n, 1, MPI_INT, to, 1, MPI_COMM_WORLD);
    MPI_Send(V, n, MPI_UNSIGNED_LONG_LONG, to, 1, MPI_COMM_WORLD);
}

int recvArray(uint64_t* V, int from) {
    MPI_Status stat;
    int n; 
    MPI_Recv(&n, 1, MPI_INT, from, 1, MPI_COMM_WORLD, &stat);  
    MPI_Recv(V, n, MPI_UNSIGNED_LONG_LONG, from, 1, MPI_COMM_WORLD, &stat);  
    return n;
}

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
}

void coordinator(int workers) {
    map<int, set<uint64_t> > S[B];

    uint64_t A[1000000];
    
    for(int i=1; i<workers; i++) {
        int n = recvArray(A, i);
        
        for(int j=0; j<n; j++) {
            uint64_t id = (uint64_t)i << 32 | j;
        
            for(int k=0; k<B; k++) {
                int sub = A[j] & RMASK;
                S[k][sub].insert(id);
            }
        }
    }
    
    set<set<uint64_t> > FS;
    
    for(int i=0; i<B; i++) {
        for(map<int, set<uint64_t> >::iterator it = S[i].begin(); it!=S[i].end(); it++) {
            if (it->second.size() == 1) continue;
            FS.insert(it->second);
        }
    }

    printf("%lu\n", FS.size());
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
