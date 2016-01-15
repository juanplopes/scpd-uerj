#include "mpi.h"
#include "imghash.h"
#include "mpihelper.h"
#include <iostream>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#define MAX 10000

using namespace std;
using namespace std::chrono;


void worker(int rank) {
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
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

    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>( t2 - t1 ).count();

    sendStats(Stats(rank, files.size(), duration), 0);
    sendArray(&hashes[0], hashes.size(), 0);
    
    for(int i; (i = recvInt(0)) >= 0; ) {
        sendString(files[i].c_str(), files[i].size() + 1, 0);
    }
}

void coordinator(int workers) {
    ImageSet images;
    uint64_t A[1000000];

    for(int i=1; i<workers; i++) {
        Stats stats = recvStats();
        cerr << "Node #" << stats.node << " of " << (workers-1) << ": " << stats.images << " images in " << stats.time << "ms" << endl;
    }
    
    for(int i=1; i<workers; i++) {
        int n = recvArray(A, i);
        
        for(int j=0; j<n; j++) {
            uint64_t id = (uint64_t)i << 32 | j;
            images.add(id, A[j]);
        }
    }
    
    set<uint64_t> allIds;
    for(auto it = images.S.begin(); it!=images.S.end(); it++) {
        if (it->second.size() == 1) continue;
        allIds.insert(it->second.begin(), it->second.end());
    }
    
    char file[256];
    map<uint64_t, string> names;
    for(auto it = allIds.begin(); it!=allIds.end(); it++) {
        int node = *it >> 32;
        int n = *it & 0xFFFFFFFF;
        sendInt(n, node);
        recvString(file, node);
        names[*it] = string(file);
    }
    
    for(int i=1; i<workers; i++) {
        sendInt(-1, i);
    }

    for(auto it = images.S.begin(); it!=images.S.end(); it++) {
        if (it->second.size() == 1) continue;

        cout << it->second.size() << endl;

        for(vector<uint64_t>::iterator jt = it->second.begin(); jt != it->second.end(); jt++) {
            cout << images.SC[*jt] << " " << names[*jt] <<endl;
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
