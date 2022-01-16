
#include "tests.h"
#include "game.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

void test_test(void)
{
	printf("test uwu\n");
}

void test_only_background(void)
{
	game_settings_t game_settings = {0};
	game_settings.enabled_background = true;
	game_loop(game_settings);
}

void test_no_background(void)
{
	game_settings_t game_settings = GAME_SETTINGS_ALL;
	game_settings.enabled_background = false;
	game_loop(game_settings);
}

void test_refactor(void)
{
	game_loop(GAME_SETTINGS_ALL);
}

const test_function_entry_t g_test_function_table[] = {
	{"test", test_test},
	{"only-background", test_only_background},
	{"no-background", test_no_background},
	{"refactor", test_refactor},
};
const unsigned int g_test_function_table_length =
	sizeof g_test_function_table / sizeof g_test_function_table[0];
