
#include "random.h"
#include <stdint.h> /* uint32_t */
#include <time.h> /* time */
#include <limits.h> /* UINT_MAX */
#include <math.h> /* isfinite */
#include <stdio.h>

/* An LCG (Linear Congruencial Generator) is a random number generator with a
 * state containing only one 32-bits unsigned integer (at least here) and
 * that gets to the next state by ((state*A+C)%M)ing itself.
 * However it is said that such a random number generator is kind of bad, and
 * it is observed that the low bits of the states in the state sequence are
 * not very random.
 * Thus, here, the random number generator (rg_t type) uses an array of LCGs,
 * and uses only the hight bits of every LCG, for better random numbers.
 * In details, with the A, C and M static parameters used for the LCGs, the
 * high bit of a state is always 0. The next 16-bits are extracted by
 * (state>>15)ing a state, and two numbers of 16-bits make a number of 32-bits.
 * Two LCG iterations are performed to produce one 32-bits random number,
 * such an iteration is refered to as a half-step and produces 16 random bits.
 * Everything else is pretty interface. */

/* Returns the next state of the given LCG. */
static uint32_t lcg_iter(uint32_t lcg);

/* Performs a half-step on the rg and returns 16 random bits. */
static uint32_t rg_half_step(rg_t* rg);

/* Performs a step on the rg and returns 32 random bits. */
static uint32_t rg_step(rg_t* rg);

static uint32_t lcg_iter(uint32_t lcg)
{
	/* The LCG parameters used here are the ones used by the
	 * GNU libc implementation (even though rand may be more complicated
	 * than just using a LCG). */
	return (lcg * 1103515245 + 12345) % 2147483648;
}

static uint32_t rg_half_step(rg_t* rg)
{
	uint32_t raw = lcg_iter(rg->lcg_array[rg->cur]);
	rg->lcg_array[rg->cur] = raw;
	rg->cur = (rg->cur + (raw >> 16) + 3) % RG_SIZE;
	return raw >> 15;
}

static uint32_t rg_step(rg_t* rg)
{
	uint32_t half_a = rg_half_step(rg);
	uint32_t half_b = rg_half_step(rg);
	return half_a + (half_b << 16);
}

void rg_seed(rg_t* rg, uint32_t seed)
{
	rg->seed = seed;
	rg->cur = !!(seed & (1 << 31));
	uint32_t local_lcg = seed & ~(1 << 31);
	for (int i = 0; i < RG_SIZE; ++i)
	{
		local_lcg = lcg_iter(local_lcg);
		rg->lcg_array[i] = ~local_lcg;
	}
}

void rg_time_seed(rg_t* rg)
{
	rg_seed(rg, time(NULL));
}

int rg_int(rg_t* rg, int inf, int sup)
{
	return inf + (int)(rg_step(rg) % (sup - inf + 1));
}

float rg_float(rg_t* rg, float inf, float sup)
{
	float random_unit;
	while (
		random_unit = ((float)rg_step(rg)) / ((float)(UINT_MAX-1)),
		!isfinite(random_unit)
	);
	return inf + random_unit * (sup - inf);
}

uint32_t rg_uint32_full(rg_t* rg)
{
	return rg_step(rg);
}
