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

struct Stats {
    int node, images, time;
    
    Stats() {}
    Stats(int node, int images, int time) : node(node), images(images), time(time) {}
};

void sendStats(const Stats stats, int to) {
    MPI_Send(&stats, sizeof(Stats), MPI_BYTE, to, 2, MPI_COMM_WORLD);
}

Stats recvStats() {
    MPI_Status stat;
    Stats res;

    MPI_Recv(&res, sizeof(Stats), MPI_BYTE, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &stat);  
    return res;
}
