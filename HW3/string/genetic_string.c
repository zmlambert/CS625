#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "genetic_string.h"

/*
random int within [min,max]
*/
int rand_int(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

/*
spawns organism. NULL value for randomly generated one
*/
Organism* spawnOrganism(char* value, char* goal)
{
    Organism* organism = malloc(sizeof(Organism));
    // if value is null create a random value string
    if(value == NULL) {
        organism->value = calloc(strlen(goal)+1, sizeof(char));
        for(size_t i = 0; i < strlen(goal); i++) {
            organism->value[i] = CHARSET[rand_int(0, CHARSET_LEN-1)];
        }
    } else {
        organism->value = calloc(strlen(value)+1, sizeof(char));
        strncpy(organism->value, value, strlen(value));
    }
    organism->goal = calloc(strlen(goal)+1, sizeof(char));
    strncpy(organism->goal, goal, strlen(goal));
    return organism;
}

/*
Determine fitness of string by number of correct characters
in correct positions
*/
int fitnessOrganism(const Organism* organism)
{
    if(organism->value == NULL) return 0;
    int fit = 0;
    size_t goalLen = strlen(organism->goal);
    for(size_t i = 0; i < goalLen; i++) {
        if(organism->goal[i] == organism->value[i]) fit++;
    }
    return fit;
}

/*
Mate and return two different organisms.
*/
Organism** mateOrganisms(Organism* parent_1, Organism* parent_2)
{
    if (parent_1->value == NULL || parent_2->value == NULL) return NULL;
    
    size_t length = strlen(parent_1->value);
    int crossOver = rand_int(0, length - 1);

    char* val_1 = calloc(length+1, sizeof(char));
    char* val_2 = calloc(length+1, sizeof(char));
    // val_1 for first organism
    strncpy(val_1, parent_1->value, crossOver);
    strncpy(val_1 + crossOver, parent_2->value, length - crossOver);
    // val_2 for second organism
    strncpy(val_2, parent_2->value, crossOver);
    strncpy(val_2 + crossOver, parent_1->value, length - crossOver);

    // return
    Organism** children = malloc(sizeof(Organism*) * 2);
    children[0] = spawnOrganism(val_1, parent_1->goal);
    children[1] = spawnOrganism(val_2, parent_1->goal);
    
    // free
    free(val_1);
    free(val_2);
    return children;
}

/*
Mutate random characters
*/
void mutateOrganism(Organism* organism, double mutateProbability)
{
    if (organism == NULL || organism->value == NULL) return;
    size_t length = strlen(organism->value);
    for  (size_t i = 0; i < length; i++) {
        double r = (double)rand_int(0, 100) / 100.0;
        if (r <= mutateProbability) organism->value[i] = CHARSET[rand_int(0, CHARSET_LEN-1)];
    }
}

/*
tournament selection
*/
Organism* selectOrganism(Organism** organisms, size_t populationSize)
{
    // number of elements selected to be part of the tournament
    int tournamentSize = 4;
    // best result
    Organism* bestOrganism = NULL;
    int bestFitness = -100000; // make sure it's lower than min fitness
    // select tournamentSize random organisms from population and choose highest fitness
    for (int i = 0; i < tournamentSize; i++) {
        int r = rand() % populationSize;
        Organism* candidate = organisms[r];
        // Evaluate fitness
        int candidateFitness = fitnessOrganism(candidate);
        // Track the best candidate in the tournament
        if (candidateFitness > bestFitness) {
            bestFitness = candidateFitness;
            bestOrganism = candidate;
        }
    }
    return bestOrganism;
}

/*
Comparison function for two organisms based on fitness. Made for qsort
*/
int compareOrganism(const void* a, const void* b)
{
    const Organism* const* organism_1 = (const Organism* const*)a;
    const Organism* const* organism_2 = (const Organism* const*)b;
    return fitnessOrganism(*organism_2) - fitnessOrganism(*organism_1);
}

/*
Create a population of organisms
*/
Population* spawnPopulation(int populationSize, char* goal, int generations, double mutateProbability)
{
    // create population and set values
    Population* population = malloc(sizeof(Population));
    population->populationSize = populationSize;
    population->mutateProbability = mutateProbability;
    population->generations = generations;
    population->goal = calloc(strlen(goal)+1, sizeof(char));
    strncpy(population->goal, goal, strlen(goal));
    // allocate thisGeneration based on populationSize
    population->thisGeneration = malloc(sizeof(Organism*) * populationSize);
    // spawn random value organisms
    for(int i = 0; i < populationSize; i++) {
        population->thisGeneration[i] = spawnOrganism(NULL, population->goal);
    }
    population->nextGeneration = NULL;
    return population;
}

/*
iterate through genetic algorithm
*/
void iteratePopulation(Population* population)
{
    // i think this helps with performance?
    int mutateProbability = population->mutateProbability;
    int populationSize = population->populationSize;
    int generations = population->generations;
    
    // store highest fitness in lowest gen for output
    int highestFitnessGeneration = generations+1; // make sure higher than generations. pretty easy
    int highestFitness = -100000; // make sure it's lower than min fitness
    // sort initial population
    qsort(population->thisGeneration, populationSize, sizeof(Organism*), compareOrganism);
    printPopulation(population);
    
    // for every generation
    for(int g = 0; g < generations; g++) {
        // allocate next generation
        population->nextGeneration = malloc(sizeof(Organism*) * populationSize);
        for(int i = 0; i < populationSize / 2; i++) {
            // for first two (highest fitness), copy them directly 
            if(i == 0) {
                population->nextGeneration[2*i] = spawnOrganism(population->thisGeneration[0]->value, population->goal);
                population->nextGeneration[2*i+1] = spawnOrganism(population->thisGeneration[1]->value, population->goal);
            }
            // tournament style selection for the rest
            else {
                // select two parents
                Organism* parent_1 = selectOrganism(population->thisGeneration, populationSize);
                Organism* parent_2 = selectOrganism(population->thisGeneration, populationSize);
                // mate
                Organism** mate_results = mateOrganisms(parent_1, parent_2);
                // mutate
                mutateOrganism(mate_results[0], mutateProbability);
                mutateOrganism(mate_results[1], mutateProbability);
                // assign to next generation
                population->nextGeneration[2*i] = mate_results[0];
                population->nextGeneration[2*i+1] = mate_results[1];
                // cleanup
                free(mate_results);
            }
        }
        // free current gen
        freeOrganisms(population->thisGeneration, populationSize);
        // replace current gen with next gen
        population->thisGeneration = population->nextGeneration;
        // remove the next gen, since it's now current gen
        population->nextGeneration = NULL;
        freeOrganisms(population->nextGeneration, populationSize);
        // sort current gen
        qsort(population->thisGeneration, populationSize, sizeof(Organism*), compareOrganism);
        
        // store highest fitness in lowest gen, for output
        int highestFitnessThisGen = fitnessOrganism(population->thisGeneration[0]);
        if(highestFitnessThisGen > highestFitness) {
            highestFitnessGeneration = g;
            highestFitness = highestFitnessThisGen;
        }
        if((g+1) % 100 == 0) printf("\r%d/%d", g+1, generations);
    }
    // all output below
    printf("\n");
    printPopulation(population);
    
    Organism* bestFitnessOrganism = population->thisGeneration[0];
    printf("\nMAX FITNESS %d FOUND IN GENERATION %d\n\t%s\n", fitnessOrganism(bestFitnessOrganism), highestFitnessGeneration, bestFitnessOrganism->value);
}

/*
Output information about the population
*/
void printPopulation(Population* population)
{
    // header line
    size_t lineLength = strlen(population->goal);
    size_t populationSize = population->populationSize;
    // bar of dashes
    char* dBar = calloc(lineLength + 1, sizeof(char));
    for(size_t i = 0; i < lineLength; i++) dBar[i] = '-';
    // bar of equals
    char* eBar = calloc(lineLength + 1, sizeof(char));
    for(size_t i = 0; i < lineLength; i++) eBar[i] = '=';

    // output details of population
    printf("Population: %lu\tGenerations: %d\tMutation Prob: %.02f\n", populationSize, population->generations, population->mutateProbability);
    printf("|=%s=|====|\n", eBar);
    printf("| %s | %.2lu |\n", population->goal, lineLength);
    printf("|=%s=|====|\n", eBar);
    for(size_t s = 0; s < 10; s++) {
        // if population is lower than 10 dont explode the program
        if(s >= populationSize) break;
        // organism string header
        printf("|-%s-|----|\n", dBar);
        printf("| %s | %.2d |\n", population->thisGeneration[s]->value, fitnessOrganism(population->thisGeneration[s]));
    }
    printf("|=%s=|====|\n", eBar);

    // cleanup
    free(dBar);
    free(eBar);
}

/*
Free memory of Organism*
*/
void freeOrganism(Organism* organism)
{
    if(organism == NULL) return;
    free(organism->goal);
    free(organism->value);
    free(organism);
}

/*
Free memory of Organism**
*/
void freeOrganisms(Organism** organisms, size_t size)
{
    if(organisms == NULL) return;
    for(size_t i = 0; i < size; i++) freeOrganism(organisms[i]);
    free(organisms);
}

/*
Free memory of Population*
*/
void freePopulation(Population* population)
{
    if(population == NULL) return;
    free(population->goal);
    if(population->nextGeneration != NULL) {
        freeOrganisms(population->nextGeneration, population->populationSize);
    }
    freeOrganisms(population->thisGeneration, population->populationSize);
    free(population);
}
