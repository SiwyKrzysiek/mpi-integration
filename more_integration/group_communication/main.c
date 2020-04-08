// Realizacja programu całkującego z wykorzystaniem kolektywnej (MPI_Scatter, MPI_Gather/MPI_Reduce)

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

// y = 1
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

    if (argc != 4)
    {
        if (world_rank == 0)
            puts("Invalid arguments.\nProgram interface: ./integrate begin end num_points");
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }

    int begin = atoi(argv[1]);
    int end = atoi(argv[2]);
    int nodesNumber = atoi(argv[3]);

    if (nodesNumber % world_size != 0)
    {
        if (world_rank == 0)
            puts("Points can not be evenly distributed between processes, wich is required for the use of MPI_Scatter()\n"
                 "Adjust the number of nodes or processes");
        MPI_Finalize();
        return(EXIT_FAILURE);
    }

    if (world_rank == 0)
        puts("Program will integrate function y = x^2");
    double result = integrate(x2, begin, end, nodesNumber);

    if (world_rank == 0)
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
#ifdef DEBUG
    printf("Nodes number = %d\n", nodesNumber);
#endif
    if (nodesNumber <= 1)
        return 0.;

    double sum = 0.;
    for (int i = 1; i < nodesNumber; i++)
    {
        double dx = nodes[i] - nodes[i - 1];
        double a = func(nodes[i - 1]);
        double b = func(nodes[i]);

        double rectField = (a + b) * dx / 2.;
#ifdef DEBUG
        fprintf(stderr, "Partial sum x0=%lf, x1=%lf, a=%lf, b=%lf, dx=%lf, sum=%lf\n",
                nodes[i - 1], nodes[i], a, b, dx, rectField);
#endif

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

    double *nodes = NULL;
    if (world_rank == 0) // Main process
    {
        nodes = generatePoints(begin, end, num_points);
    }

    const int partSize = num_points / world_size;
    double nodesPart[partSize];
    // Spread points
    MPI_Scatter(nodes, partSize, MPI_DOUBLE, nodesPart, partSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Each process calculates partial result
    double partialResult = integrateRange(func, nodesPart, partSize);

    double finalResult;
    MPI_Reduce(&partialResult, &finalResult, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    free(nodes);
    return finalResult;
}