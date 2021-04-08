
/* Gravinyon header, contaning declarations for global variables containing
 * embedded content, such as shader source code.
 * See the compilation script "_comp.py" or the EMBEDDED macro for more. */

#ifndef GRAVINYON_HEADER_EMBEDDED_
#define GRAVINYON_HEADER_EMBEDDED_

/* If this macro is used as would be the extern keyword in a variable
 * declaration, and if filename_ is a string literal, then the compilation
 * script "_comp.py" will parse the variable declaration as a special
 * declaration that requiers the variable to be set to the content of the file
 * found at filename_ in the generated source file "embedded.c". The variable
 * declaration is expected to be in the global scope and to have a type
 * compatible with const char*. */
#define EMBEDDED(filename_) extern

EMBEDDED("shaders/ship.vert") const char* g_ship_vert;
EMBEDDED("shaders/ship.geom") const char* g_ship_geom;
EMBEDDED("shaders/ship.frag") const char* g_ship_frag;
EMBEDDED("shaders/enemies.vert") const char* g_enemies_vert;
EMBEDDED("shaders/enemies.geom") const char* g_enemies_geom;
EMBEDDED("shaders/enemies.frag") const char* g_enemies_frag;
EMBEDDED("shaders/bullets.vert") const char* g_bullets_vert;
EMBEDDED("shaders/bullets.geom") const char* g_bullets_geom;
EMBEDDED("shaders/bullets.frag") const char* g_bullets_frag;
EMBEDDED("shaders/parts.vert") const char* g_parts_vert;
EMBEDDED("shaders/parts.geom") const char* g_parts_geom;
EMBEDDED("shaders/parts.frag") const char* g_parts_frag;

#endif /* GRAVINYON_HEADER_EMBEDDED_ */
