#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

// variables
#define N 1000000
int data[N];

int main(int argc, char *argv[]) {
    
    // init
    float * data;
    float * recv = (float *)malloc(sizeof(float)*100);
    // variables for gather and scater
    int *displs;
    int *sendcounts;
    // MPI variables
    int rank, size;
    
    // Init MPI env
    MPI_Init(&argc, &argv);
    // get variables set for this env
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // master process gets the data array 
    // also decides the displacements and sencounts arrays
    if (rank == 0) {
		data = (float *)malloc(sizeof(float) * (size * (size + 1) / 2 + 1));
		printf("Input array: \n");
		for (int i = 0; i < size * (size + 1) / 2; i++) {
			data[i] = (i + 1)*(i + 1);
			printf("%f ", data[i]);
		}

        
	}

    // 0 gets 1, 1 get 2, 3 gets 3 ...
    displs = (int *)malloc(sizeof(int) * size);
    sendcounts = (int *)malloc(sizeof(int) * size);
    for (int i = 0; i < size; i++) {
        sendcounts[i] = i+1;
        displs[i] = i > 0  ? displs[i-1] + sendcounts[i-1] : 0;
    }

	// process 0 does the non-uniform scatter to other processes
	MPI_Scatterv(data, sendcounts, displs, MPI_FLOAT, recv, 100, MPI_FLOAT, 0, MPI_COMM_WORLD);
    // implicit barrier here

    // calculate sqrt of received data
    for (int i = 0; i < sendcounts[rank]; i++) {
        recv[i] = sqrt(recv[i]);
    }

	// gather all results from recv to accumulate in data array of process 0 
	// implicit barrier here
    MPI_Gatherv(recv, sendcounts[rank], MPI_FLOAT, data, sendcounts, displs, MPI_FLOAT, 0, MPI_COMM_WORLD);

	// Printing the data of process 0
	if (rank == 0)
	{
		printf("\nOutput array:\n");
		for(int i = 0; i < size * (size + 1) / 2; i++)
			printf("%f ", data[i]);
		printf("\n");
	}

    // terminate MPI env
    MPI_Finalize();

    return 0;
}