
#ifndef GRAVINYON_HEADER_RANDOM_
#define GRAVINYON_HEADER_RANDOM_

#include <stdint.h> /* uint32_t */

/* Random generator state. */
struct rg_t
{
	uint32_t seed; /* Will not be changed by using the generator. */
	#define RG_SIZE 7 /* Arbitrary choice with no thoughts behind it. */
	uint32_t lcg_array[RG_SIZE];
	unsigned int cur; /* Index of the next LCG to be used. */
};
typedef struct rg_t rg_t;

/* Initialises the given rg with the given seed. */
void rg_seed(rg_t* rg, uint32_t seed);

/* Initialises the given rg with the timestamp as the seed. */
void rg_time_seed(rg_t* rg);

/* Performs a rg step and returns a random int greater than or equeal to inf
 * and less than or equal to sup. */
int rg_int(rg_t* rg, int inf, int sup);

/* Performs a rg step and returns a random float between inf and sup. */
float rg_float(rg_t* rg, float inf, float sup);

/* Performs a rg step and returns 32 random bits. */
uint32_t rg_uint32_full(rg_t* rg);

#endif /* GRAVINYON_HEADER_RANDOM_ */
