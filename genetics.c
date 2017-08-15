#include "genetics.h"

/*
 * Free a genome, obvs.
 */
void gen_free(genome_t *ptr)
{
    if(ptr != NULL) {
        free(ptr->left);
        free(ptr->right);
        free(ptr);
    }
}

/*
 * roll your own RNG!
 * Some xshift function I got off wikipedia
 */
static
chromosome_t gen_xorshift()
{
    static chromosome_t state = 0;
    /* seed */
    if(state == 0) {
        int r;
        srand(time(NULL));
        r = rand();
        state = *((chromosome_t*) &r);
        r = rand();
        state ^= *((chromosome_t*) &r);
    }

    /* generate random number */
    state ^= state >> 12;
    state ^= state << 25;
    state ^= state >> 27;
    
    return state * 0x2545F4914F6CDD1D;
}
/*
 * Allocate memory for a genome of given size
 * Returns NULL if unsuccessful
 */
genome_t *gen_alloc(int size)
{
    genome_t *ptr = NULL;
    if(size > 0) {
        ptr = malloc(sizeof(genome_t));
    }
    if(ptr != NULL) {
        ptr->n = size;
        ptr->left = calloc(size, sizeof(chromosome_t));
        ptr->right = calloc(size, sizeof(chromosome_t));
        if(ptr->left == NULL || ptr->right == NULL) {
            free(ptr->left);
            free(ptr->right);
            free(ptr);
            ptr = NULL;
        }
    }
    return ptr;
}

/*
 * Randomize genome.
 * Returns 0 if error; else 1
 */
int gen_rand(genome_t *g)
{
    int i;

    if(g == NULL || g->left == NULL || g->right == NULL) {
        return 0;
    }

    for(i = 0; i < g->n; ++i) {
        g->left[i] = gen_xorshift();
        g->right[i] = gen_xorshift();
    }
    return 1;
}

/*
 * Print to stdout
 * Returns positive if successful; 0 or negative if failed
 */
int gen_print(genome_t *g)
{
    int ret = 0;
    if(g != NULL && g->left != NULL && g->right != NULL) {
        int i;
        int r;
        for(i = 0; i < g->n; ++i) {
            r = printf("%d: l = %lx  r = %lx\n", i, g->left[i], g->right[i]);
            if(r <= 0) return r;
            ret += r;
        }
    }
    return ret;
}

/*
 * Mutates a genome.
 * n determines intensity of mutation, and can be any positive integer:
 *    1 affects most of the genome
 *    3 affects several bits per chromosome
 *    5 affects ~1-2 bits per chromosome
 *    6 affects usually 0 bits
 */
static
void gen_mutate(genome_t *g, int n)
{
    int i, j;
    chromosome_t c[2];

    if(g == NULL || g->n == 0 || g->left == NULL || g->right == NULL || n <= 0)
        return;
    c[0] = gen_xorshift();
    c[1] = gen_xorshift();

    for(i = 0; i < g->n; ++i) {
        for(j = 1; j < n; ++j) {
            c[0] &= gen_xorshift();
            c[1] &= gen_xorshift();
        }
        g->left[i] ^= c[0];
        g->right[i] ^= c[1];
    }
}

/*
 * Generates a haploid genome from a parent
 * Each chromosome takes a random contiguous part from the homologous
 * left parent chromosome and the rest from the right parent, in order to
 * simulate chromosomal crossover.
 */
static
chromosome_t *gen_haploid_female(genome_t *g)
{
    int n;
    chromosome_t *ptr;
    if(g == NULL || g->n <= 0 || g->left == NULL || g->right == NULL)
        return NULL;
   
    ptr = calloc(g->n, sizeof(chromosome_t));
    if(ptr == NULL)
        return NULL;

    for(n = 0; n < g->n; ++n) {
        chromosome_t mask = -1; /* All ones */
        mask <<= gen_xorshift() % 64; /* Leftshift random number of bits */
        if(gen_xorshift() % 2 == 0)
            mask = ~mask;
        ptr[n] = (g->left[n] & mask) | (g->right[n] & (~mask));
    }

    return ptr;
}

/*
 * This function works like gen_haploid_female(), however it disallows
 * chromosomal crossover in chromosome 0, which is assumed to be the allosome.
 * Instead, chromosome 0 is the full chromosome from either left or right.
 */
static
chromosome_t *gen_haploid_male(genome_t *g)
{
    int n;
    chromosome_t *ptr;
    if(g == NULL || g->n <= 0 || g->left == NULL || g->right == NULL)
        return NULL;
   
    ptr = calloc(g->n, sizeof(chromosome_t));
    if(ptr == NULL)
        return NULL;

    if(gen_xorshift() % 2 == 0) {
        ptr[0] = g->left[0];
    } else {
        ptr[0] = g->right[0];
    }

    for(n = 1; n < g->n; ++n) {
        chromosome_t mask = -1; 
        mask <<= gen_xorshift() % 64;
        if(gen_xorshift() % 2 == 0)
            mask = ~mask;
        ptr[n] = (g->left[n] & mask) | (g->right[n] & (~mask));
    }

    return ptr;
}


/*
 * Simulates sexual reproduction and generates a child, which is returned.
 * The left chromosomes are from parent x; the right are from parent y.
 * x is assumed to be female; y is male.
 */
genome_t *gen_sex(genome_t *x, genome_t *y)
{
    genome_t *ptr = NULL;
    if(x == NULL || y == NULL || x->n != y->n  ||
            x->left == NULL || y->left == NULL ||
            x->right == NULL || y->right == NULL)
        return NULL;

    ptr = gen_alloc(x->n);
    if(ptr == NULL)
        return NULL;

    ptr->left = gen_haploid_female(x);
    ptr->right = gen_haploid_male(y);

    if(ptr->left == NULL || ptr->right == NULL)
        gen_free(ptr);

    gen_mutate(ptr, 6);

    return ptr;
}

/*
 * Generates a genome based upon a given recipe.
 * For each bit that is 0 in mask, genome will be identical to recipe.
 *  "   "    "   "   "  1 "   "    genome will be random.
 * Return NULL if alloc failed.
 */
genome_t *gen_make_from_recipe(recipe_t *r)
{
    int i;
    genome_t *ptr;
   
    if(r == NULL || r->n <= 0 || r->code == NULL || r->mask == NULL)
        return NULL;
    ptr = gen_alloc(r->n);
    if(ptr == NULL)
        return NULL;

    for(i = 0; i < r->n; ++i) {
        ptr->left[i] = r->code[i];
        ptr->right[i] = r->code[i];

        ptr->left[i] ^= (gen_xorshift() & r->mask[i]);
        ptr->right[i] ^= (gen_xorshift() & r->mask[i]);
    }
    return ptr;
}
 
/*
 * Main function for testing.
 */

int main(int argc, char *argv[])
{
    char *arg;
    int s;
    genome_t *x, *y, *z;
    recipe_t r;

    r.n = 1;
    r.code = malloc(sizeof(chromosome_t));
    r.mask = malloc(sizeof(chromosome_t));
    r.code[0] = 0xAAAAAAAAAAAAAAAA;
    r.mask[0] = 0xFFFFFFFF00000000;

    arg = NULL;
    /* determine n */
    if(argc > 1) {
        arg = argv[1];
    }
    
    s = (arg == NULL) ? 8 : atoi(arg);
    if(s == 0) s = 4;

    x = gen_alloc(s);
    gen_rand(x);
    y = gen_make_from_recipe(&r);
    z = gen_sex(x,y);

    gen_print(x);
    gen_print(y);
    gen_print(z);

    gen_free(x);
    gen_free(y);
    gen_free(z);

    return 0;
}

