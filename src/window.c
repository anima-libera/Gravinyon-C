
#include "utils.h"
#include "dbgmsg.h"
#include "opengl.h"
#include <SDL2/SDL.h>

SDL_Window* g_window = NULL;
SDL_GLContext g_opengl_context = NULL;

static GLuint s_vao_id;

#define WINDOW_NAME "Gravinyon"

int init_g_graphics(void)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0)
	{
		error_sdl2_fail("SDL_Init");
		return -1;
	}
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	g_window = SDL_CreateWindow(WINDOW_NAME,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 800,
		SDL_WINDOW_OPENGL);
	if (g_window == NULL)
	{
		error_sdl2_fail("SDL_CreateWindow");
		return -1;
	}
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4) != 0)
	{
		error_sdl2_fail(
			"SDL_GL_SetAttribute with SDL_GL_CONTEXT_MAJOR_VERSION");
		return -1;
	}
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3) != 0)
	{
		error_sdl2_fail(
			"SDL_GL_SetAttribute with SDL_GL_CONTEXT_MINOR_VERSION");
		return -1;
	}
	if (SDL_GL_SetAttribute(
		SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) != 0)
	{
		error_sdl2_fail(
			"SDL_GL_SetAttribute with SDL_GL_CONTEXT_PROFILE_MASK");
		return -1;
	}
	g_opengl_context = SDL_GL_CreateContext(g_window);
	if (g_opengl_context == NULL)
	{
		error_sdl2_fail("SDL_GL_CreateContext");
		return -1;
	}
	#ifdef USE_GLEW
		GLenum gnew_init_result = glewInit();
		if (gnew_init_result != GLEW_OK)
		{
			fprintf(stderr, "GLEW error: glewInit failed: \"%s\"\n",
				glewGetErrorString(gnew_init_result));
		}
	#endif
	enable_opengl_dbgmsg();
	glEnable(GL_MULTISAMPLE);
	if (SDL_GL_SetSwapInterval(-1) != 0)
	{
		SDL_GL_SetSwapInterval(1);
	}
	glGenVertexArrays(1, &s_vao_id);
	glBindVertexArray(s_vao_id);
	return 0;
}

void cleanup_g_graphics(void)
{
	glDeleteVertexArrays(1, &s_vao_id);
	SDL_GL_DeleteContext(g_opengl_context);
	g_opengl_context = NULL;
	SDL_DestroyWindow(g_window);
	g_window = NULL;
	SDL_Quit();
}
