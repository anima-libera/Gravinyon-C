
#include "tests.h"
#include "game.h"
#include "bg.h"
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
	game_settings.background = &g_bg_description_blue_clouds_and_stars;
	game_loop(game_settings);
}

void test_no_background(void)
{
	game_settings_t game_settings = GAME_SETTINGS_ALL;
	game_settings.background = &g_bg_description_black;
	game_loop(game_settings);
}

void test_hard(void)
{
	game_settings_t game_settings = GAME_SETTINGS_ALL;
	game_settings.spawning_enemies_intensity = 20;
	game_loop(game_settings);
}

const test_function_entry_t g_test_function_table[] = {
	{"test", test_test},
	{"only-background", test_only_background},
	{"no-background", test_no_background},
	{"hard", test_hard},
};
const unsigned int g_test_function_table_length =
	sizeof g_test_function_table / sizeof g_test_function_table[0];
