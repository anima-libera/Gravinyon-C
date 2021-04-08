
#include "random.h"
#include "utils.h"
#include <stdlib.h>
#include <stdint.h> /* uint32_t */
#include <time.h> /* time */
#include <math.h> /* isnan, isinf */
#include <limits.h> /* UINT_MAX */

/* Allocates a rg_t object and allocates its lcg_array field according to the
 * given size. The actual size of the lcg_array is written to the size field.
 * Nothing else is initialized. */
static rg_t* rg_alloc(int size);

/* Initialises the given rg with the given seed. */
static void rg_seed(rg_t* rg, rg_seed_t seed);

/* Returns the next raw number of the given lcg. The lcg parameters used here
 * are the ones used by the GNU libc implementation (even though rand may be
 * more complicated than just using a lcg). */
static uint32_t lcg_next(uint32_t lcg);

/* Performs a half step on the rg and returns a raw uint32_t.
 * Note that the lower bits are not very random, thus a complete step uses high
 * bits (bit 1 to bit 17 (bit 0 is always 0)). */
static uint32_t rg_raw_uint32(rg_t* rg);

/* Performs a step on the rg and returns a well randomized uint32_t. */
static uint32_t rg_clean_step(rg_t* rg);

static rg_t* rg_alloc(int size)
{
	if (size == 0) size = RG_DEFAULT_SIZE;
	rg_t* rg = xmalloc(sizeof(rg_t));
	rg->lcg_array = xmalloc(size * sizeof(uint32_t));
	rg->size = size;
	return rg;
}

static void rg_seed(rg_t* rg, rg_seed_t seed)
{
	rg->seed = seed;
	rg->cur = 0;
	uint32_t lcg_raw = seed;
	for (int i = 0; i < rg->size; ++i)
	{
		lcg_raw = lcg_next(lcg_raw);
		rg->lcg_array[i] = ~lcg_raw;
	}
}

static uint32_t lcg_next(uint32_t lcg)
{
	return (lcg * 1103515245 + 12345) % 2147483648;
}

static uint32_t rg_raw_uint32(rg_t* rg)
{
	uint32_t raw = lcg_next(rg->lcg_array[rg->cur]);
	rg->lcg_array[rg->cur] = raw;
	rg->cur = (rg->cur + (raw >> 16)) % rg->size;
	return raw;
}

static uint32_t rg_clean_step(rg_t* rg)
{
	uint32_t raw_a = rg_raw_uint32(rg);
	uint32_t raw_b = rg_raw_uint32(rg);
	return (raw_a >> 15) + ((raw_b >> 15) << 16);
}

rg_t* g_rg = NULL;

rg_t* rg_create_seeded(int size, rg_seed_t seed)
{
	rg_t* rg = rg_alloc(size);
	rg->version = RG_DEFAULT_VERSION;
	rg_seed(rg, seed);
	return rg;
}

rg_t* rg_create_timeseeded(int size)
{
	return rg_create_seeded(size, time(NULL));
}

void rg_destroy(rg_t* rg)
{
	free(rg->lcg_array);
	free(rg);
}

unsigned int rg_uint_full(rg_t* rg)
{
	return rg_clean_step(rg);
}

unsigned int rg_uint(rg_t* rg, unsigned int inf, unsigned int sup)
{
	return inf + rg_uint_full(rg) % (sup - inf + 1);
}

int rg_int_full(rg_t* rg)
{
	return ((union {uint32_t u; int i;}){.u = rg_clean_step(rg)}).i;
}

int rg_int(rg_t* rg, int inf, int sup)
{
	return inf + rg_int_full(rg) % (sup - inf + 1);
}

float rg_float_full(rg_t* rg)
{
	float f;
	do {
		f = ((union {uint32_t u; float f;}){.u = rg_clean_step(rg)}).f;
	} while (isnan(f) || isinf(f));
	return f;
}

float rg_float_unit(rg_t* rg)
{
	return ((float)rg_uint_full(rg)) / ((float)(UINT_MAX-1));
}

float rg_float(rg_t* rg, float inf, float sup)
{
	return inf + rg_float_unit(rg) * (sup - inf);
}

rg_seed_t rg_rg_seed(rg_t* rg)
{
	return rg_uint_full(rg);
}
