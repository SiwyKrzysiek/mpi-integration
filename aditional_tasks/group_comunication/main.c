// Wykorzystam funkcje MPI_Scatter oraz MPI_Reduce do policzenia średniej wartości liczb z tablicy
// Zbadam średnią wartość generatora liczb pseudolosowych

#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

// How much data will each process receive
#define PORTION_SIZE 10

int main(int argc, char const *argv[])
{
    MPI_Init(NULL, NULL);

    // printf("%lf\n", (double) RAND_MAX / (double) 2);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int *wholeTable;
    const int wholeTableSize = world_size * PORTION_SIZE;
    if (world_rank == 0) // Master thread generates random numbers
    {
        srand(time(NULL));

        wholeTable = alloca(wholeTableSize * sizeof(*wholeTable));
        for (int i = 0; i < wholeTableSize; i++)
        {
            wholeTable[i] = rand();
        }
    }

    // Send parts of the table to each process
    int tablePart[PORTION_SIZE];
    MPI_Scatter(wholeTable, PORTION_SIZE, MPI_INT, tablePart, PORTION_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

    // Calculate partial average
    long long partialSum = 0.;
    for (int i = 0; i < PORTION_SIZE; i++)
    {
        partialSum += tablePart[i];
    }
    double partialAverage = (double) partialSum / (double) PORTION_SIZE;

    // Reduce partial values to final average
    double finalSum;
    MPI_Reduce(&partialAverage, &finalSum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);


    if (world_rank == 0)
    {
        double finalAverage = finalSum / (double) world_size;
        printf("Final average: %lf\n", finalAverage);
    }


    MPI_Finalize();
    return 0;
}
