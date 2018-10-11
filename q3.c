// The group of MPI_COMM_WORLD is the set of all MPI processes.
// Using MPI_COMM_WORLD, every process can communicate with every other
#include <stdio.h>
#include <string.h>
#include <mpi.h>


int main(int argc, char * argv[]) {
    // variables
    int rank, size;
    
    // initialize mpi parallel environment
    MPI_Init(&argc, &argv);

    // get rank and size
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // master process
    if (rank == 0) {
        char buffer[256];
        for (int i = 1; i < size; ++i) {
            // receive message MPI_COMM_WORLD communicator with any tag and any source
            // also ignore the status message
            MPI_Recv(&buffer, 256, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Message received: %s\n", buffer);
        }
    }
    // others
    else {
        // message to be send
        char msg[256];
        sprintf(msg, "Hello World! %d", rank);
        // send message to process 0 in the MPI_COMM_WORLD communicator
        MPI_Send(msg, strlen(msg), MPI_CHAR, 0, rank, MPI_COMM_WORLD);
    }

    // terminate mpi env
    MPI_Finalize();
}