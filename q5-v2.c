#include <stdio.h>
#include <mpi.h>

// variables
static long num_steps;
double pi;

int main(int argc, char *argv[]) {
    
    // MPI variables
    int rank, size;
    
    // Init MPI env
    MPI_Init(&argc, &argv);
    // get variables set for this env
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // master process sets the num_steps value
    if (rank == 0) {
        num_steps = 100000;
    }
    // master process broadcasts the value of num_steps tp all processes in MPI_COMM_WORLD communicator
    MPI_Bcast(&num_steps, 1, MPI_LONG, 0, MPI_COMM_WORLD);
    // there exists an implicit barrier after this so that all process get the broadcasted value

    // pi calculcation which each process calculates and stores in mypi
    double x, mypi, step, sum = 0.0;
    step = 1.0/(double)num_steps;
    for (int i = rank; i < num_steps; i += size) {
        x = (i+0.5)*step;
        sum = sum + 4.0/(1+x*x);
    }
    mypi = step * sum;

    // mypi value of each process combined and stored in pi of master process in MPI_COMM_WORLD communicator
    // implicit barrier before all values from processes are reduced 
    MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // master prints the pi value stored in it
    if (rank == 0) {
        printf("pi: %lf\n", pi);
    }

    // terminate MPI env
    MPI_Finalize();

    return 0;
}