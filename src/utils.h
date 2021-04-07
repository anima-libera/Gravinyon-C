
/* Gravinyon header, contaning declarations for various utility functions and
 * macros.
 * Implementations are in "utils.c". */

#ifndef GRAVINYON_HEADER_UTILS_
#define GRAVINYON_HEADER_UTILS_

/* The true fundamental circle constant.
 * See tauday.com/tau-manifesto for more. */
#define TAU 6.28318530717f

#include <stddef.h> /* size_t */

/* The common malloc wrapper that prints an error if the allocation fails. */
void* xmalloc(size_t size);

/* The common calloc wrapper that prints an error if the allocation fails. */
void* xcalloc(size_t number, size_t size);

/* The common realloc wrapper that prints an error and returns NULL if the
 * reallocation fails. */
void* xrealloc(void* ptr, size_t size);

/* Reads the file at the given path and returns its content in a buffer that
 * needs to be freed. */
char* read_file(const char* filepath);

/* Swaps the two given non-temporary values. */
#define SWAP(type_, x_, y_) \
	do{ \
		type_ tmp_ = x_; \
		x_ = y_; \
		y_ = tmp_; \
	}while(0)

/* Makes sure that inf_ <= sup_ by swapping the two non-temporary values if
 * needed. */
#define ORDER(type_, inf_, sup_) \
	do{ \
		if (inf_ > sup_) \
		{ \
			SWAP(type_, inf_, sup_); \
		} \
	}while(0)

#endif /* GRAVINYON_HEADER_UTILS_ */
