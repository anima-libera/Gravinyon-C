
#ifndef GRAVINYON_HEADER_RANDOM_
#define GRAVINYON_HEADER_RANDOM_

#include <stdint.h> /* uint32_t */

/* Type of the seed object used to initialise a rg_t object.
 * Note that the size field of a rg_t object can be considered as part of the
 * full seed since it also influences the rg's output. */
typedef uint32_t rg_seed_t;

/* Random generator state object type. */
struct rg_t
{
	int version; /* Ensures backward compatibility, maybe. */
	rg_seed_t seed;
	int size;
	uint32_t* lcg_array;
	int cur;
};
typedef struct rg_t rg_t;

#define RG_DEFAULT_VERSION 1
#define RG_DEFAULT_SIZE 41

/* The global random generator.
 * TODO: Remove it ? */
extern rg_t* g_rg;

/* Creates a rg with the given size and initialises it with the given seed.
 * If the size parameter is 0, a default size will be used instead. */
rg_t* rg_create_seeded(int size, rg_seed_t seed);

/* Same as the rg_create_seeded function, but uses the system's timestamp to
 * seed the created rg. */
rg_t* rg_create_timeseeded(int size);

/* Frees the given rg. */
void rg_destroy(rg_t* rg);

/* Performs a rg step and returns a random unsigned int. */
unsigned int rg_uint_full(rg_t* rg);

/* Performs a rg step and returns a random unsigned int greater than or equal
 * to inf and less than or equal to sup. */
unsigned int rg_uint(rg_t* rg, unsigned int inf, unsigned int sup);

/* Performs a rg step and returns a random int. */
int rg_int_full(rg_t* rg);

/* Performs a rg step and returns a random int greater than or equeal to inf
 * and less than or equal to sup. */
int rg_int(rg_t* rg, int inf, int sup);

/* Performs at least one rg step and returns a random float (non-infinite and
 * non-NaN). */
float rg_float_full(rg_t* rg);

/* Performs a rg step and returns a random float between 0.0f and 1.0f. */
float rg_float_unit(rg_t* rg);

/* Performs a rg step and returns a random float between inf and sup. */
float rg_float(rg_t* rg, float inf, float sup);

/* Performs a rg step and returns a random seed object. */
rg_seed_t rg_rg_seed(rg_t* rg);

#endif /* GRAVINYON_HEADER_RANDOM_ */
