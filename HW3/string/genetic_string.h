#ifndef HW3_GENETIC_H
#define HW3_GENETIC_H

#include <stdlib.h>

#define CHARSET "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz"
#define CHARSET_LEN strlen(CHARSET)

int rand_int(int min, int max);

typedef struct {
    char* value;
    char* goal;
} Organism;

Organism* spawnOrganism(char* value, char* goal);
int fitnessOrganism(const Organism* organism);
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
   char* goal;
   Organism** thisGeneration;
   Organism** nextGeneration;
} Population;

Population* spawnPopulation(int populationSize, char* goal, int generations, double mutateProbability);
void iteratePopulation(Population* population);
void printPopulation(Population* population);
void freePopulation(Population* population);

#endif
