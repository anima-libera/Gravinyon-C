
#include "window.h"
#include "dbgmsg.h"
#include "shaders.h"
#include "embedded.h"
#include "utils.h"
#include "random.h"
#include "audio.h"
#include "bg.h"
#include "game.h"
#include "input.h"
#include "text.h"
#include "opengl.h"
#include "tests.h"
#include <string.h>

int init_g_all(void)
{
	if (init_g_graphics() != 0)
	{
		return -1;
	}
	if (shprog_build_all() != 0)
	{
		return -1;
	}
	glProgramUniform1f(g_shprog_draw_ships, 0, GAME_ASPECT_RATIO);
	glProgramUniform1f(g_shprog_draw_bullets, 0, GAME_ASPECT_RATIO);
	glProgramUniform1f(g_shprog_draw_enemies, 0, GAME_ASPECT_RATIO);
	glProgramUniform1f(g_shprog_draw_parts, 0, GAME_ASPECT_RATIO);
	glProgramUniform1f(g_shprog_draw_stars, 0, GAME_ASPECT_RATIO);
	glProgramUniform1f(g_shprog_draw_gchars, 0, GAME_ASPECT_RATIO);
	if (init_g_audio() != 0)
	{
		return -1;
	}
	return 0;
}

void cleanup_g_all(void)
{
	cleanup_g_audio();
	shprog_destroy_all();
	cleanup_g_graphics();
}

int main(int argc, const char** argv)
{
	if (init_g_all() != 0)
	{
		return EXIT_FAILURE;
	}

	const char* test_name = NULL;
	for (unsigned int i = 1; i < (unsigned int)argc; i++)
	{
		if (strcmp(argv[i], "--test") == 0)
		{
			test_name = argv[++i];
		}
	}

	const test_function_entry_t* test_function_entry = NULL;
	if (test_name != NULL)
	{
		for (unsigned int i = 0; i < g_test_function_table_length; i++)
		{
			if (strcmp(g_test_function_table[i].name, test_name) == 0)
			{
				test_function_entry = &g_test_function_table[i];
				break;
			}
		}
		if (test_function_entry == NULL)
		{
			fprintf(stderr, "Error: Unknown test name \"%s\"\n", test_name);
			return EXIT_FAILURE;
		}
	}
	if (test_function_entry != NULL)
	{
		test_function_entry->function();
	}
	else
	{
		game_loop(GAME_SETTINGS_ALL);
	}

	cleanup_g_all();
	return 0;
}
