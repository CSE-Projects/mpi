// http://www.math-cs.gordon.edu/courses/cps343/presentations/Intro_to_MPI.pdf
#include <stdio.h>
#include <mpi.h>

// variables
#define N 10000000
#define A 2
double X[N];
double Y[N];
double Z[N];

// set values for X and Y
void setXY () {
    for (long i = 0; i < N; i++) {
        X[i] = Y[i] = i;
    }
}

int main(int argc, char *argv[]) {
    
    MPI_Init(&argc, &argv);

    // serial
    double start_time, serial_time, parallel_time;
    // MPI variables
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // master process calculates serially to simulate uniprocessor
    if (rank == 0) {
        setXY();
        start_time = MPI_Wtime();
        for (long i = 0; i < N; i++) {
            Z[i] = A*X[i] + Y[i];
        }
        serial_time = MPI_Wtime() - start_time;
        
        start_time = MPI_Wtime();
    }

    // parallel calculation
    for (int i = rank; i < N; i += size) {
        Z[i] = A*X[i] + Y[i];
    }

    if (rank == 0) {
		parallel_time = MPI_Wtime() - start_time;
        printf("Serial Execution Time: %lf\n", serial_time);
        printf("Parallel Execution Time: %lf\n", parallel_time);
    }

    MPI_Finalize();

    return 0;
}