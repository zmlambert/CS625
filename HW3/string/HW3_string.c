#include <time.h>
#include <stdio.h>
#include "genetic_string.h"

#define POPULATION_SIZE 100
#define GOAL            "Three Faulty cats climb the forbidden tree"
#define GENERATIONS     100000
#define MUTATEPROB      0.15

int main()
{
    srand(time(NULL)); // seed random otherwise i could cheat lol
    setvbuf(stdout, NULL, _IONBF, 0); // disable output buffering
    // create population, iterate, free, return.
    Population* population = spawnPopulation(POPULATION_SIZE, GOAL, GENERATIONS ,MUTATEPROB);
    iteratePopulation(population);
    freePopulation(population);
    return 0;
}
