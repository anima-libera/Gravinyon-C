
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
 * compatible with the generated literal. The escape_mode_ parameter can be
 * TEXT, BINARY or SIZE and determines the way the content to embed is
 * escaped into a literal. */
#define EMBEDDED(filename_, escape_mode_) extern
#define TEXT /* Escapes the file content as a string literal. */
#define BINARY /* Escapes the file content as an array of bytes. */
#define SIZE /* Just produces an integer literal of the size in bytes. */

EMBEDDED("shaders/ship.vert", TEXT) const char g_shader_ship_vert[];
EMBEDDED("shaders/ship.geom", TEXT) const char g_shader_ship_geom[];
EMBEDDED("shaders/ship.frag", TEXT) const char g_shader_ship_frag[];
EMBEDDED("shaders/enemies.vert", TEXT) const char g_shader_enemies_vert[];
EMBEDDED("shaders/enemies.geom", TEXT) const char g_shader_enemies_geom[];
EMBEDDED("shaders/enemies.frag", TEXT) const char g_shader_enemies_frag[];
EMBEDDED("shaders/bullets.vert", TEXT) const char g_shader_bullets_vert[];
EMBEDDED("shaders/bullets.geom", TEXT) const char g_shader_bullets_geom[];
EMBEDDED("shaders/bullets.frag", TEXT) const char g_shader_bullets_frag[];
EMBEDDED("shaders/parts.vert", TEXT) const char g_shader_parts_vert[];
EMBEDDED("shaders/parts.geom", TEXT) const char g_shader_parts_geom[];
EMBEDDED("shaders/parts.frag", TEXT) const char g_shader_parts_frag[];

EMBEDDED("assets/pew.wav", BINARY) const unsigned char g_sound_pew[];
EMBEDDED("assets/pew.wav", SIZE) const unsigned int g_sound_pew_size;

#endif /* GRAVINYON_HEADER_EMBEDDED_ */
