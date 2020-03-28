#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of all processes and rank of current prcess
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    printf("I'm process %d of %d and I'll integrate soon\n", world_rank, world_size);

    MPI_Finalize();
    return EXIT_SUCCESS;
}