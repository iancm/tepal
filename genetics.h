/*
 * genetics.h
 *
 * This file manages the genome struct, which stores genetic information for
 * individual organisms. It can simulate sexual reproduction.
 *
 */

#ifndef GENETICS_H
#define GENETICS_H

#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MUTATE_TIMES 7

typedef uint64_t chromosome_t;

typedef struct genome
{
    int n;  /* number of chromosomes per pointer;
             * sizeof(genome_t) ~ 2n * sizeof(chromosome_t) + sizeof(int) */
    chromosome_t *left;
    chromosome_t *right;
} genome_t;

typedef struct recipe
{
    int n;
    chromosome_t *code; /* Genetic code of recipe */
    chromosome_t *mask; /* For each bit:
                         * 0: This bit must be same as code
                         * 1: This bit should be random */
} recipe_t;

void gen_free(genome_t *g);     /* Frees a genome             */
genome_t *gen_alloc(int size);  /* Allocate memory for genome */
int gen_rand(genome_t *g);      /* Randomize chromosomes      */
int gen_print(genome_t *g);     /* Print chromosomes to s;
                                 * if s==NULL print to stdout */

/* Produces offspring via simulated sexual reproduction
 * with two parent genomes x and y. Requires that n
 * is identical for both parents. Returns NULL if failure.
 */
genome_t *gen_sex(genome_t *female, genome_t *male);

/* Generates a genome based on a recipe */
genome_t *gen_make_from_recipe(recipe_t *r);

#endif
