
/* Gravinyon header, contaning declarations for window-related initalization
 * and cleaning up, via SDL2 and OpenGL.
 * Implementations are in "window.c". */

#ifndef GRAVINYON_HEADER_WINDOW_
#define GRAVINYON_HEADER_WINDOW_

#include "opengl.h"
#include <SDL2/SDL.h>

/* The unique global Gravinyon window.
 * It is created by init_g_graphics and destroyed by cleanup_g_graphics. */
extern SDL_Window* g_window;

/* The unique global SDL2-window-compatible OpenGL context used by Gravinyon.
 * It is created by init_g_graphics and destroyed by cleanup_g_graphics. */
extern SDL_GLContext g_opengl_context;

/* Initializes the SDL2 library and the GLEW OpenGL extension loader.
 * Creates the unique global window g_window, the unique global OpenGL
 * context g_opengl_context, and an OpenGL VAO.
 * Returns -1 if anything goes wrong, returns 0 if everything goes well.
 * This function should be called once at the beginning of execution, and
 * a unique matching call to cleanup_g_graphics should be done at the end of
 * execution. */
int init_g_graphics(void);

/* Cleans up and destroys what is initialized and created by
 * init_g_graphics. */
void cleanup_g_graphics(void);

#endif /* GRAVINYON_HEADER_WINDOW_ */
