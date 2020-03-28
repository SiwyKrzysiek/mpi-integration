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

    double result = integrate(x2, 0, 2, 10);
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
    printf("Nodes number = %d\n", nodesNumber);
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
        // printf("My range: [%d, %d)\n", myRange.a, myRange.b);

        // Send data to other processes
        for (int i = 1; i < world_size; i++)
        {
            // printf("Range send to %d: [%d, %d)\n", i, ranges[i].a, ranges[i].b);

            int rangeLength = ranges[i].b - ranges[i].a;
            double *firstValue = nodes + ranges[i].a;

            MPI_Send(&rangeLength, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(firstValue, rangeLength, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        }

        // double chetedResult = integrateRange(func, nodes, num_points);
        double chetedResult = -1;

        free(ranges);
        free(nodes);
        return chetedResult;
    }
    else
    {
        int nodesNumber;
        MPI_Recv(&nodesNumber, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d -> nodesNumber = %d\n", world_rank, nodesNumber);

        double nodes[nodesNumber];
        MPI_Recv(nodes, nodesNumber, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int i = 0; i < nodesNumber; i++)
        {
            printf("Process %d -> node[%d] = %lf\n", world_rank, i, nodes[i]);
        }

        double partialResult = integrateRange(func, nodes, nodesNumber);
        // printf("Process %d -> partial sum = %lf\n", world_rank, partialResult);
        // printf("Process %d will work on range [%d, %d)\n", world_rank, rangeStart, rangeEnd);
    }

    return -1;
}