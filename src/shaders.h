
/* Gravinyon header, contaning declarations for shader compilation and shader
 * program ids.
 * Implementations are in "shaders.c". */

#ifndef GRAVINYON_HEADER_SHADERS_
#define GRAVINYON_HEADER_SHADERS_

#include <GL/glew.h>

/* These are OpenGL shader program ids that can be given to glUseProgram.
 * They are obtained by a call to shprog_build_all
 * and lost by a call to shprog_destroy_all. */
extern GLuint g_shprog_draw_ships;
extern GLuint g_shprog_draw_enemies;
extern GLuint g_shprog_draw_bullets;
extern GLuint g_shprog_draw_parts;
extern GLuint g_shprog_draw_bg;
extern GLuint g_shprog_draw_stars;

int shprog_build_all(void);
void shprog_destroy_all(void);

#endif /* GRAVINYON_HEADER_SHADERS_ */
