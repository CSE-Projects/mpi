// https://cvw.cac.cornell.edu/parallel/block
// MPI_Send(), does blocking communication. In other words, the function call does not return control to your program until the data you are sending have been copied out of your buffer
// 
#include <stdio.h>
#include <mpi.h>

// variables
#define N 1000000
int input[N];

int main(int argc, char *argv[]) {
    
    // init
    for (int i = 1; i <= N; i++) {
        input[i] = i;
    }

    // MPI variables
    int rank, size;
    
    // Init MPI env
    MPI_Init(&argc, &argv);
    // get variables set for this env
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // status and request 
    MPI_Status *status;
    MPI_Request	send_request, recv_request;
    MPI_Request	send_request1, recv_request1;
    
    // calculate sum of elements asssigned(stride access) to each process
    long long partial_sum = 0;
    for (int i = rank; i < N; i += size) {
        partial_sum += input[i];
    }

    // indicates the total no of processes present for reduction
    int half = size;
    do {        
        // no of processes are odd then process will include the last processes's partial sum
        if (half % 2 != 0) {
            // last process will send its sum to process 0
            if (rank == half - 1) {
                MPI_Isend(&partial_sum, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD, &send_request);
            }
            // process 0 will receive and include the sum of the last process 
            if (rank  == 0) {
                long long recv_sum;
                MPI_Irecv(&recv_sum, 1, MPI_LONG_LONG, half - 1, 0, MPI_COMM_WORLD, &recv_request);

                MPI_Wait(&send_request, status);
                MPI_Wait(&recv_request, status);
                partial_sum += recv_sum;
            }
        }

        

        // reduce the processes
        half = half / 2;

        // Pi + n/2(half) send its partial sum to Pi for adding it up
        if (rank >= half) {
            MPI_Isend(&partial_sum, 1, MPI_LONG_LONG, rank - half, 0, MPI_COMM_WORLD, &send_request1);
        }
        else {
            long long recv_sum;
            MPI_Irecv(&recv_sum, 1, MPI_LONG_LONG, rank + half, 0, MPI_COMM_WORLD, &recv_request1);
            
            MPI_Wait(&send_request1, status);
            MPI_Wait(&recv_request1, status);
            partial_sum += recv_sum;
        }

    } while (half > 1);

    // finally the sum will be stored in the master processes's partial sum
    if (rank == 0) {
        printf("Sum: %lld", partial_sum);
    }

    // terminate MPI env
    MPI_Finalize();

    return 0;
}