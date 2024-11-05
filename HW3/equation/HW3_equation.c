#include <time.h>
#include <stdio.h>
#include "genetic_equation.h"

#define POPULATION_SIZE 300
#define GENERATIONS     15000
#define MUTATEPROB      0.05

int main()
{
    srand(time(NULL)); // seed random otherwise i could cheat lol
    setvbuf(stdout, NULL, _IONBF, 0); // disable output buffering
    // create population, iterate, free, return.
    Population* population = spawnPopulation(POPULATION_SIZE, GENERATIONS, MUTATEPROB);
    iteratePopulation(population);
    freePopulation(population);
    return 0;
}
