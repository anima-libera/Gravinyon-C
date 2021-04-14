
/* Gravinyon header, contaning declarations for various utility functions and
 * macros.
 * Implementations are in "utils.c". */

#ifndef GRAVINYON_HEADER_UTILS_
#define GRAVINYON_HEADER_UTILS_

/* The true fundamental circle constant.
 * See tauday.com/tau-manifesto for more. */
#define TAU 6.28318530717f

#include <stddef.h> /* size_t */
#include <stdio.h>

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

#ifdef DEBUG
	#define DEBUG_PRINTF_1(format_, arg1_) \
		fprintf(stderr, format_, arg1_)
#else
	#define DEBUG_PRINTF_1(format_, arg1_) \
		do { } while (0)
#endif

#define DYNAMIC_VECTOR_RESIZE_IF_NEEDED(elem_count_, elem_count_max_, ptr_, elem_type_) \
	do { \
		if (elem_count_ == elem_count_max_) \
		{ \
			unsigned int new_maximum_wanted = \
				((float)elem_count_max_ + 2.3f) * 1.3f; \
			elem_type_* new_array = xrealloc(ptr_, \
				new_maximum_wanted * sizeof(elem_type_)); \
			if (new_array == NULL) \
			{ \
				fprintf(stderr, "TODO: handle error\n"); \
			} \
			else \
			{ \
				DEBUG_PRINTF_1("Resize " #ptr_ " to %d\n", new_maximum_wanted); \
			} \
			ptr_ = new_array; \
			elem_count_max_ = new_maximum_wanted; \
		} \
	} while (0)

#endif /* GRAVINYON_HEADER_UTILS_ */
