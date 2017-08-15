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
    size_t n;  /* number of chromosomes per pointer;
             * sizeof(genome_st) ~ 2n * sizeof(chromosome_t) + sizeof(int) */
    chromosome_t *left;
    chromosome_t *right;
} genome_st;

typedef struct recipe
{
    size_t n;
    chromosome_t *code; /* Genetic code of recipe */
    chromosome_t *mask; /* For each bit:
                         * 0: This bit must be same as code
                         * 1: This bit should be random */
} recipe_st;

void gen_free(genome_st *g);     /* Frees a genome             */
genome_st *gen_alloc(size_t size);  /* Allocate memory for genome */
int gen_rand(genome_st *g);      /* Randomize chromosomes      */
int gen_print(genome_st *g);     /* Print chromosomes to s;
                                 * if s==NULL print to stdout */

/* Produces offspring via simulated sexual reproduction
 * with two parent genomes x and y. Requires that n
 * is identical for both parents. Returns NULL if failure.
 */
genome_st *gen_sex(genome_st *female, genome_st *male);

/* Generates a genome based on a recipe */
genome_st *gen_make_from_recipe(recipe_st *r);

#endif
