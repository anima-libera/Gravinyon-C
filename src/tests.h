
#ifndef GRAVINYON_HEADER_TESTS_
#define GRAVINYON_HEADER_TESTS_

void test_test(void);

typedef void (*test_function_t)(void);

struct test_function_entry_t
{
	const char* name;
	test_function_t function;
};
typedef struct test_function_entry_t test_function_entry_t;

extern const test_function_entry_t g_test_function_table[];
extern const unsigned int g_test_function_table_length;

#endif /* GRAVINYON_HEADER_TESTS_ */
