#ifndef HW3_GENETIC_H
#define HW3_GENETIC_H

#include <stdlib.h>

#define CHARSET "01"
#define CHARSET_LEN strlen(CHARSET)
#define BITSTRING_LEN 48

int rand_int(int min, int max);
double findDecimal(char* bitStringSlice);

typedef struct {
    char* value;
} Organism;

Organism* spawnOrganism(char* value);
double fitnessOrganism(const Organism* organism);
Organism** mateOrganisms(Organism* parent_1, Organism* parent_2);
void mutateOrganism(Organism* organism, double mutateProbability);
Organism* selectOrganism(Organism** organisms, size_t populationSize);
int compareOrganism(const void* organism_1, const void* organism_2);
void freeOrganism(Organism* organism);
void freeOrganisms(Organism** organisms, size_t populationSize);

typedef struct {
   size_t populationSize;
   double mutateProbability;
   int generations;
   int numOrganisms;
   Organism** thisGeneration;
   Organism** nextGeneration;
} Population;

Population* spawnPopulation(int populationSize, int generations, double mutateProbability);
void iteratePopulation(Population* population);
void printPopulation(Population* population);
void freePopulation(Population* population);

#endif
