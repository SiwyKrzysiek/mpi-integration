#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pair.h"
#include "utils.h"

// y = x^2
double x2(double x)
{
    return x * x;
}

double const1(double x)
{
    return 1;
}

// Calculate integral with usage of other MPI processes
double integrate(double (*func)(double), double begin, double end, int num_points);

// Single process function to integrate over selected part of nodes
double integrateRange(double (*func)(double), const double *nodes, const int nodesNumber);

// Create evenly spaced points in given range
// First and last point will be at given coordinates
// Returned array must be freed manually
double *generatePoints(const double start, const double end, const int pointsNumber);

int main(int argc, char **argv)
{
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of all processes and rank of current process
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // printf("I'm process %d of %d and I'll integrate soon\n", world_rank, world_size);

    double result = integrate(x2, 0, 2, 100);
    printf("Result: %lf\n", result);

    MPI_Finalize();
    return EXIT_SUCCESS;
}

double *generatePoints(const double start, const double end, const int pointsNumber)
{
    double *results = malloc(pointsNumber * sizeof(double));
    const double step = (end - start) / (pointsNumber - 1);

    double x = start;
    for (int i = 0; i < pointsNumber; i++)
    {
        results[i] = x;
        x += step;
    }

    return results;
}

double integrateRange(double (*func)(double), const double *nodes, const int nodesNumber)
{
    if (nodesNumber <= 1)
        return 0.;

    double sum = 0.;
    for (int i = 1; i < nodesNumber; i++)
    {
        double dx = nodes[i] - nodes[i - 1];
        double a = func(nodes[i - 1]);
        double b = func(nodes[i]);

        double rectField = (a + b) * dx / 2.;
        // fprintf(stderr, "Partial sum x0=%lf, x1=%lf, a=%lf, b=%lf, dx=%lf, sum=%lf\n",
        //         nodes[i - 1], nodes[i], a, b, dx, rectField);

        sum += rectField;
    }

    return sum;
}

double integrate(double (*func)(double), double begin, double end, int num_points)
{
    // Get the number of all processes and rank of current process
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (world_rank == 0) // Main process
    {
        double partialSums[world_size];
        double *nodes = generatePoints(begin, end, num_points);

        fputs("Main process generated nodes\n", stderr);
        // for (int i = 0; i < num_points; i++)
        // {
        //     printf("%lf\n", nodes[i]);
        // }

        Pair *ranges = splitIntoRanges(num_points, world_size);
        Pair myRange = ranges[0];

        // Send ranges to other processes
        for (int i = 1; i < world_size; i++)
        {
            MPI_Send(&ranges[i].a, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&ranges[i].b, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        double chetedResult = integrateRange(func, nodes, num_points);

        free(ranges);
        free(nodes);
        return chetedResult;
    }
    else
    {
        int rangeStart, rangeEnd;
        MPI_Recv(&rangeStart, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&rangeEnd, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // printf("Process %d will work on range [%d, %d)\n", world_rank, rangeStart, rangeEnd);
    }

    return -1;
}