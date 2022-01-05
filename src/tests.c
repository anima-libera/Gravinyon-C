
#include "tests.h"
#include <stdlib.h>
#include <stdio.h>

void test_test(void)
{
	printf("test uwu\n");
	exit(0);
}

const test_function_entry_t g_test_function_table[] = {
	{"test", test_test},
};
const unsigned int g_test_function_table_length =
	sizeof g_test_function_table / sizeof g_test_function_table[0];
