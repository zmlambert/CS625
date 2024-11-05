#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "genetic_equation.h"

/*
random int within [min,max]
*/
int rand_int(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

/*
convert bitString slice into decimal
[0-9]     -> whole number
[10-14]   -> decimal
[15]      -> sign
*/
double findDecimal(char* bitStringSlice)
{
    double decimal = 0.0;
    // bits 0 - 9 are whole number, so 9-i will be raised to powers in range [9, 0]
    // bits 10 - 14 are decimal, so 9-i will be raised to powers in range [-1, -5]
    for(int i = 0; i < 15; i++) {
        if(bitStringSlice[i] == '1') decimal += pow(2, 9-i);
    }
    // bit 16 is sign bit x
    if(bitStringSlice[15] == '1') decimal *= -1.0;
    return decimal;
}

/*
spawns organism. NULL value for randomly generated one
*/
Organism* spawnOrganism(char* value)
{
    Organism* organism = malloc(sizeof(Organism));
    organism->value = calloc(BITSTRING_LEN + 1, sizeof(char));
    // if value is null create a random value string
    if(value == NULL) {
        for(size_t i = 0; i < BITSTRING_LEN; i++) {
            organism->value[i] = CHARSET[rand_int(0, CHARSET_LEN-1)];
        }
    }
    // otherwise just copy it straight in
    else {
        memcpy(organism->value, value, BITSTRING_LEN);
    }
    return organism;
}

/*
Determine fitness of string by number of correct characters
in correct positions
*/
double fitnessOrganism(const Organism* organism)
{
    // char* to hold a sectioned bit string
    char* bitString = calloc(16+1, sizeof(char));
    // x
    memcpy(bitString, organism->value, 16);
    double x = findDecimal(bitString);
    // y
    memcpy(bitString, organism->value+16, 16);
    double y = findDecimal(bitString);
    // z
    memcpy(bitString, organism->value+32, 16);
    double z = findDecimal(bitString);

    free(bitString);

    // initial fitness calculated using formula to maximize
    double fit = x + 3*y + z;
    // x + 4y <= 12
    if(x + 4*y > 12) fit -= 10000;
    // 3x + 6y + 4z <= 48
    if(3*x + 6*y + 4*z > 48) fit -= 10000;
    // y + z <= 8
    if(y + z > 8) fit -= 10000;
    // if any is less than zero
    if(x < 0) fit -= 10000;
    if(y < 0) fit -= 10000;
    if(z < 0) fit -= 10000;
    return fit;
}

/*
Mate and return two different organisms.
*/
Organism** mateOrganisms(Organism* parent_1, Organism* parent_2)
{
    if (parent_1->value == NULL || parent_2->value == NULL) return NULL;

    int crossOver = rand_int(0, BITSTRING_LEN-1);
    Organism** children = malloc(sizeof(Organism*) * 2);
    char* value = calloc(BITSTRING_LEN+1, sizeof(char));

    // value for first organism
    memcpy(value, parent_1->value, crossOver);
    memcpy(value + crossOver, parent_2->value, BITSTRING_LEN - crossOver);
    children[0] = spawnOrganism(value);

    // value for second organism
    memcpy(value, parent_2->value, crossOver);
    memcpy(value + crossOver, parent_1->value, BITSTRING_LEN - crossOver);
    children[1] = spawnOrganism(value);

    // free
    free(value);
    return children;
}

/*
Mutate random characters
*/
void mutateOrganism(Organism* organism, double mutateProbability)
{
    if (organism == NULL || organism->value == NULL) return;
    for  (size_t i = 0; i < BITSTRING_LEN; i++) {
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
    int tournamentSize = 5;
    // best result
    Organism* bestOrganism = NULL;
    int bestFitness = -100000; // make sure it's lower than min fitness
    // select tournamentSize random organisms from population and choose highest fitness
    for (int i = 0; i < tournamentSize; i++) {
        int r = rand() % populationSize;
        Organism* candidate = organisms[r];
        // fitness
        double candidateFitness = fitnessOrganism(candidate);
        // track the best candidate in the tournament
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
    const Organism* organism_1 = *(const Organism**)a;
    const Organism* organism_2 = *(const Organism**)b;

    double fitness1 = fitnessOrganism(organism_1);
    double fitness2 = fitnessOrganism(organism_2);

    if (fabs(fitness1 - fitness2) < 0.0001) return 0;
    if (fitness1 < fitness2) return 1;
    if (fitness1 > fitness2) return -1;
    return 0;
}

/*
Create a population of organisms
*/
Population* spawnPopulation(int populationSize, int generations, double mutateProbability)
{
    // create population and set values
    Population* population = malloc(sizeof(Population));
    population->populationSize = populationSize;
    population->mutateProbability = mutateProbability;
    population->generations = generations;
    // allocate thisGeneration based on populationSize
    population->thisGeneration = malloc(sizeof(Organism*) * populationSize);
    // spawn random value organisms
    for(int i = 0; i < populationSize; i++) {
        population->thisGeneration[i] = spawnOrganism(NULL);
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
    double highestFitness = -100000.0; // make sure it's lower than min fitness
    // sort initial population
    qsort(population->thisGeneration, populationSize, sizeof(Organism*), compareOrganism);
    printPopulation(population);
    // for every generation
    for(int g = 0; g < generations; g++) {
        // allocate next generation
        population->nextGeneration = malloc(sizeof(Organism*) * populationSize);
        // elitism: for first two (highest fitness), copy them directly
        population->nextGeneration[0] = spawnOrganism(population->thisGeneration[0]->value);
        population->nextGeneration[1] = spawnOrganism(population->thisGeneration[1]->value);
        // for the rest, breed with tournament style
        for(int i = 1; i < populationSize / 2; i++) {
            // select two parents using tournament selection
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
        // free current gen
        freeOrganisms(population->thisGeneration, populationSize);
        // replace current gen with next gen
        population->thisGeneration = population->nextGeneration;
        // remove the next gen, since it's now current gen
        population->nextGeneration = NULL;
        freeOrganisms(population->nextGeneration, populationSize);
        // sort current gen
        qsort(population->thisGeneration, populationSize, sizeof(Organism*), compareOrganism);

        // store highest fitness in lowest gen for output
        double highestFitnessThisGen = fitnessOrganism(population->thisGeneration[0]);
        if((g+1) % 100 == 0)
        {
            printf("\r%d/%d", g+1, generations);
            printf("\tNew best fitness %9.02f at generation %d", highestFitness, highestFitnessGeneration);
        }
        if(highestFitnessThisGen > highestFitness) {
            highestFitnessGeneration = g;
            highestFitness = highestFitnessThisGen;
        }
    }
    // clear the live update line
    printf("\n");
    printPopulation(population);

    Organism* bestFitnessOrganism = population->thisGeneration[0];
    // char* to hold a sectioned bit string
    char* bitString = calloc(16+1, sizeof(char));
    // x
    memcpy(bitString, bestFitnessOrganism->value, 16);
    double x = findDecimal(bitString);
    // y
    memcpy(bitString, bestFitnessOrganism->value+16, 16);
    double y = findDecimal(bitString);
    // z
    memcpy(bitString, bestFitnessOrganism->value+32, 16);
    double z = findDecimal(bitString);
    free(bitString);

    printf("\nMAX VALUE %.02f FOUND IN GENERATION %d\n", fitnessOrganism(bestFitnessOrganism), highestFitnessGeneration);
    printf("\tx=%.02f\n", x);
    printf("\ty=%.02f\n", y);
    printf("\tz=%.02f\n", z);
}

/*
Output information about the population
*/
void printPopulation(Population* population)
{
    // header line
    size_t populationSize = population->populationSize;
    // bar of dashes
    char* dBar = calloc(BITSTRING_LEN + 1, sizeof(char));
    for(size_t i = 0; i < BITSTRING_LEN; i++) dBar[i] = '-';
    // bar of equals
    char* eBar = calloc(BITSTRING_LEN + 1, sizeof(char));
    for(size_t i = 0; i < BITSTRING_LEN; i++) eBar[i] = '=';
    // output details of population
    printf("Population: %lu\tGenerations: %d\tMutation Prob: %.02f\n", populationSize, population->generations, population->mutateProbability);
    printf("|=%s=|===========|\n", eBar);
    for(size_t s = 0; s < 10; s++) {
        // if population is lower than 10 dont explode the program
        if(s >= populationSize) break;
        // organism string header
        if(s != 0) printf("|-%s-|-----------|\n", dBar);
        printf("| %s | %9.02f |\n", population->thisGeneration[s]->value, fitnessOrganism(population->thisGeneration[s]));
    }
    printf("|=%s=|===========|\n", eBar);

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
    if(population->nextGeneration != NULL) {
        freeOrganisms(population->nextGeneration, population->populationSize);
    }
    freeOrganisms(population->thisGeneration, population->populationSize);
    free(population);
}
