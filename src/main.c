
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
#include <GL/glew.h>

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
	glProgramUniform1f(g_shprog_draw_ship, 0, GAME_ASPECT_RATIO);
	glProgramUniform1f(g_shprog_draw_bullets, 0, GAME_ASPECT_RATIO);
	glProgramUniform1f(g_shprog_draw_enemies, 0, GAME_ASPECT_RATIO);
	glProgramUniform1f(g_shprog_draw_parts, 0, GAME_ASPECT_RATIO);
	glProgramUniform1f(g_shprog_draw_stars, 0, GAME_ASPECT_RATIO);
	if (init_g_audio() != 0)
	{
		return -1;
	}
	g_rg = rg_create_timeseeded(0);
	return 0;
}

void cleanup_g_all(void)
{
	rg_destroy(g_rg);
	cleanup_g_audio();
	shprog_destroy_all();
	cleanup_g_graphics();
}

int main(void)
{
	if (init_g_all() != 0)
	{
		return -1;
	}

	gs_t gs = {0};
	gs_init(&gs);

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

		if (commands.debug_spawn_enemies)
		{
			gs_spawn_enemies(&gs);
		}

		gs_perform_iter(&gs, &commands);
		bg_perform_iter(&bg);

		bg_render(&bg);
		gs_render(&gs);

		SDL_GL_SwapWindow(g_window);
	}

	bg_cleanup(&bg);
	gs_cleanup(&gs);

	cleanup_g_all();
	return 0;
}
