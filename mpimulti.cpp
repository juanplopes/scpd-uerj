#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MAXN 16

void send(double* V, int n, int to) {
    MPI_Send(V, n, MPI_DOUBLE, to, 1, MPI_COMM_WORLD);
}

void recv(double* V, int n, int from) {
    MPI_Status stat;
    MPI_Recv(V, n, MPI_DOUBLE, from, 1, MPI_COMM_WORLD, &stat);  
}

void sendBoundaries(double V[][MAXN], int h, int rank, int numtasks) {
    if (rank-1>=0)
        send(V[1], MAXN, rank-1);
    if (rank+1<numtasks)
        send(V[h], MAXN, rank+1);
}

void recvBoundaries(double V[][MAXN], int h, int rank, int numtasks) {
    if (rank-1>=0)
        recv(V[0], MAXN, rank-1);
    if (rank+1<numtasks)
        recv(V[h+1], MAXN, rank+1);
}

void init(double V[][MAXN], int localn, int rank, int numtasks) {
    for(int i=1; i<=localn; i++) {
        for(int j=0; j<MAXN; j++) {
            if (i==1 and rank==0 or i==localn and rank==numtasks-1)
                V[i][j] = -1;
            else
                V[i][j] = rank;
        }
    }
}

void sendAndReceive(double V[][MAXN], int localn, int rank, int numtasks) {
    if (rank%2==0) {
        sendBoundaries(V, localn, rank, numtasks);
        recvBoundaries(V, localn, rank, numtasks);
    } else {
        recvBoundaries(V, localn, rank, numtasks);
        sendBoundaries(V, localn, rank, numtasks);
    }
}

double computeNext(double V[][MAXN], double T[][MAXN], int localn, int rank, int numtasks) {
    double diffnorm = 0;
    for(int i=1; i<=localn; i++) {
        for(int j=0; j<MAXN; j++) {
            if (i==1 and rank==0 or i==localn and rank==numtasks-1 or j==0 or j==MAXN-1)
                T[i][j] = V[i][j];
            else
                T[i][j] = (V[i-1][j]+V[i+1][j]+V[i][j-1]+V[i][j+1])/4;
            diffnorm += (T[i][j]-V[i][j])*(T[i][j]-V[i][j]);

        }
    }

    for(int i=1; i<=localn; i++)
        memcpy(V[i], T[i], sizeof(T[i]));
    return diffnorm;
}


int main(int argc, char **argv) {
    int numtasks, rank, count, msg;
    MPI_Status stat;  

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int localn = MAXN/numtasks;
    double xLocal[localn+2][MAXN], xNew[localn+2][MAXN];
    
    init(xLocal, localn, rank, numtasks);
    FILE *fp;
    if (rank==0)
        fp = fopen(argv[1], "w");


    double totalnorm = 0;
    for(int i=0; i<100; i++) {
        sendAndReceive(xLocal, localn, rank, numtasks);
        double diffnorm = computeNext(xLocal, xNew, localn, rank, numtasks);
        
        MPI_Allreduce(&diffnorm, &totalnorm, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

        totalnorm = sqrt(totalnorm);        
        if (rank == 0)
            fprintf(fp, "Iteration: %d. Diffnorm: %.5f\n", i+1, totalnorm);
        if (totalnorm < 1e-2) break;
    }
    if (rank == 0)
        fclose(fp);
    
    MPI_Finalize();
    return 0;
}
