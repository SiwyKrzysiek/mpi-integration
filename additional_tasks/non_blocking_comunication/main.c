// Wykorzystam komunikację nie blokującą do zrobienia prostej symulacji
// klienta zamawiającego co jakiś czas pizzę z restauracji

#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

typedef enum PizzaType
{
    Margarita,
    Neapolitan,
    Sicilian,
    Greek
} PizzaType;

const char* pizzaTypeToString(PizzaType pizza)
{
    switch (pizza)
    {
    case Margarita:
        return "Margarita";
    case Neapolitan:
        return "Neapolitan";
    case Sicilian:
        return "Sicilian";
    case Greek:
        return "Greek";
    default:
        return "Empty plate";
    }
}

int main(int argc, char const *argv[])
{
    MPI_Init(NULL, NULL);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (world_size < 2)
    {
        fputs("Program requires at lest 2 processes to run\n", stderr);
        exit(EXIT_FAILURE);
    }

    if (world_rank == 0) // Pizza store
    {

    }
    if (world_rank == 1) // Customer
    {
        while (true)
        {
            // MPI_Isend()
        }

    }


    MPI_Finalize();
    return 0;
}
