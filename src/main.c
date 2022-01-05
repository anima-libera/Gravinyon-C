
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

	#if 0
	#define FONT_TEXTURE_SIDE 256
	unsigned char* font_data = xcalloc(FONT_TEXTURE_SIDE * FONT_TEXTURE_SIDE, 1);
	font_data[2 + 16 * FONT_TEXTURE_SIDE] = 255;
	font_data[1 + 15 * FONT_TEXTURE_SIDE] = 255;
	font_data[3 + 15 * FONT_TEXTURE_SIDE] = 255;
	font_data[0 + 14 * FONT_TEXTURE_SIDE] = 255;
	font_data[4 + 14 * FONT_TEXTURE_SIDE] = 255;
	font_data[0 + 13 * FONT_TEXTURE_SIDE] = 255;
	font_data[4 + 13 * FONT_TEXTURE_SIDE] = 255;
	font_data[0 + 12 * FONT_TEXTURE_SIDE] = 255;
	font_data[1 + 12 * FONT_TEXTURE_SIDE] = 255;
	font_data[2 + 12 * FONT_TEXTURE_SIDE] = 255;
	font_data[3 + 12 * FONT_TEXTURE_SIDE] = 255;
	font_data[4 + 12 * FONT_TEXTURE_SIDE] = 255;
	font_data[0 + 11 * FONT_TEXTURE_SIDE] = 255;
	font_data[4 + 11 * FONT_TEXTURE_SIDE] = 255;
	font_data[0 + 10 * FONT_TEXTURE_SIDE] = 255;
	font_data[4 + 10 * FONT_TEXTURE_SIDE] = 255;
	font_data[0 +  9 * FONT_TEXTURE_SIDE] = 255;
	font_data[4 +  9 * FONT_TEXTURE_SIDE] = 255;

	GLuint font_texture_id;
	glGenTextures(1, &font_texture_id);
	glBindTexture(GL_TEXTURE_2D, font_texture_id);
	glTexImage2D(GL_TEXTURE_2D,
		0, GL_RED, FONT_TEXTURE_SIDE, FONT_TEXTURE_SIDE, 0, GL_RED, GL_UNSIGNED_BYTE, font_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	#endif
	
	#if 0
	unsigned int gchar_maximum_number;
	unsigned int gchar_number;
	gchar_t* gchar_array;
	GLuint buf_gchars_id;

	gchar_maximum_number = 16;
	gchar_number = 0;
	gchar_array = xcalloc(gchar_maximum_number, sizeof(gchar_t));
	gchar_t* new_gchar = &gchar_array[gchar_number++];
	new_gchar->x = -0.1f;
	new_gchar->y = -0.1f;
	new_gchar->w = 0.2f;
	new_gchar->h = 0.2f;
	new_gchar->font_x = 0.0f;
	new_gchar->font_y = 0.0f;
	new_gchar->font_w = 0.1f;
	new_gchar->font_h = 0.1f;
	glGenBuffers(1, &buf_gchars_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_gchars_id);
	glBufferData(GL_ARRAY_BUFFER, gchar_maximum_number * sizeof(gchar_t),
		gchar_array, GL_DYNAMIC_DRAW);
	#endif

	gs_t gs = {0};
	gs_init(&gs);

	gs_spawn_ship(&gs);

	bg_t bg = {0};
	bg_init(&bg);

	commands_t commands = {0};
	
	while (1)
	{
		apply_input_commands(&commands);

		if (commands.is_escaping || commands.is_quitting)
		{
			break;
		}

		if (commands.debug_spawn_ships && gs.ship_number == 0)
		{
			gs_spawn_ship(&gs);
		}

		if (commands.debug_spawn_enemies)
		{
			gs_spawn_enemies(&gs);
		}

		gs_perform_iter(&gs, &commands);
		bg_perform_iter(&bg);

		bg_render(&bg);
		gs_render(&gs);

		#if 0
		if (gchar_number > 0)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, font_texture_id);
			glProgramUniform1i(g_shprog_draw_gchars, 1, 0);
	
			#define ATTRIB_LOCATION_POS_XYWH ((GLuint)0)
			#define ATTRIB_LOCATION_FONT_XYWH ((GLuint)1)

			glViewport(
				GAME_VIEWPORT_LEFT, GAME_VIEWPORT_TOP,
				GAME_VIEWPORT_WIDTH, GAME_VIEWPORT_HEIGHT);
			glUseProgram(g_shprog_draw_gchars);
			glEnableVertexAttribArray(ATTRIB_LOCATION_POS_XYWH);
			glEnableVertexAttribArray(ATTRIB_LOCATION_FONT_XYWH);
			
			glBindBuffer(GL_ARRAY_BUFFER, buf_gchars_id);
			glVertexAttribPointer(ATTRIB_LOCATION_POS_XYWH, 4, GL_FLOAT,
				GL_FALSE, sizeof(gchar_t), (void*)offsetof(gchar_t, x));
			glVertexAttribPointer(ATTRIB_LOCATION_FONT_XYWH, 4, GL_FLOAT,
				GL_FALSE, sizeof(gchar_t), (void*)offsetof(gchar_t, font_x));

			glDrawArrays(GL_POINTS, 0, gchar_number);
			
			glDisableVertexAttribArray(ATTRIB_LOCATION_POS_XYWH);
			glDisableVertexAttribArray(ATTRIB_LOCATION_FONT_XYWH);
			glUseProgram((GLuint)0);

			#undef ATTRIB_LOCATION_POS_XYWH
			#undef ATTRIB_LOCATION_FONT_XYWH
		}
		#endif

		SDL_GL_SwapWindow(g_window);
	}

	bg_cleanup(&bg);
	gs_cleanup(&gs);

	cleanup_g_all();
	return 0;
}
