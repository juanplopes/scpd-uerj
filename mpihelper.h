void sendArray(const uint64_t* V, int n, int to) {
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

void sendString(const char* V, int n, int to) {
    MPI_Send(&n, 1, MPI_INT, to, 1, MPI_COMM_WORLD);
    MPI_Send(V, n, MPI_CHAR, to, 1, MPI_COMM_WORLD);
}

int recvString(char* V, int from) {
    MPI_Status stat;
    int n; 
    MPI_Recv(&n, 1, MPI_INT, from, 1, MPI_COMM_WORLD, &stat);  
    MPI_Recv(V, n, MPI_CHAR, from, 1, MPI_COMM_WORLD, &stat);  
    return n;
}

void sendInt(int n, int to) {
    MPI_Send(&n, 1, MPI_INT, to, 1, MPI_COMM_WORLD);
}

int recvInt(int from) {
    MPI_Status stat;

    int n; 
    MPI_Recv(&n, 1, MPI_INT, from, 1, MPI_COMM_WORLD, &stat);  
    return n;
}
