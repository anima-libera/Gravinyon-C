
/* Gravinyon header, contaning declarations for shader compilation and shader
 * program ids.
 * Implementations are in "shaders.c". */

#ifndef GRAVINYON_HEADER_SHADERS_
#define GRAVINYON_HEADER_SHADERS_

#include <GL/glew.h>

extern GLuint g_shprog_draw_enemies;

void shprog_build_all(void);

#endif /* GRAVINYON_HEADER_SHADERS_ */
