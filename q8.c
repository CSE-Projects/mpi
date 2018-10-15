#include <stdio.h>
#include <mpi.h>

// variables
#define N 1000000
int input[N];

int main(int argc, char *argv[]) {
    
    // variables
    char c;
    int i[2];
    float f[4];
    int position = 0;
    char  buffer[100];

    // MPI variables
    int rank, size;
    
    // Init MPI env
    MPI_Init(&argc, &argv);
    // get variables set for this env
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (rank == 0) {
        // set variable values
        c = 'A';
        i[0] = 1; i[1] = 2;
        for (int i = 1; i <= 4; i++) {
            f[i] = (float)(i+22)/(float)9;
        }
        // pack variables into buffer
        MPI_Pack(&c, 1, MPI_CHAR, &buffer, 100, &position, MPI_COMM_WORLD);
        MPI_Pack(i, 2, MPI_INT,  &buffer, 100, &position, MPI_COMM_WORLD);
        MPI_Pack(f, 4, MPI_FLOAT, &buffer, 100, &position, MPI_COMM_WORLD);
        // send to all processes
        for (int i = 1; i < size; ++i) {
            MPI_Send(&buffer, 100, MPI_PACKED, i, 0, MPI_COMM_WORLD);
        }
    }
    else {
        // receive and unpack the variables
        MPI_Recv(&buffer, 100, MPI_PACKED, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Unpack(&buffer, 100, &position, &c, 1, MPI_CHAR, MPI_COMM_WORLD);
        MPI_Unpack(&buffer, 100, &position, i, 2, MPI_INT, MPI_COMM_WORLD);
        MPI_Unpack(&buffer, 100, &position, f, 4, MPI_FLOAT, MPI_COMM_WORLD);

        // let processes 1 print out the result
        if (rank == size - 1) {
            printf("P%d: \n", rank);
            printf("c: %c\n", c);
            printf("in: %d, %d\n", i[0], i[1]);
            printf("f: ");
            for (int i = 0; i < 4; i++) {
                printf("%f, ", f[i]);
            }
            printf("\n");
        }
    }
    

    // terminate MPI env
    MPI_Finalize();

    return 0;
}