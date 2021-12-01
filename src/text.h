
#ifndef GRAVINYON_HEADER_TEXT_
#define GRAVINYON_HEADER_TEXT_

#include "opengl.h"

struct font_t
{
	unsigned int texture_side;
	unsigned char* texture_data;
	GLuint texture_id;
};
typedef struct font_t font_t;

/* Graphical character, one that is displayed on screen. */
struct gchar_t
{
	float x, y, w, h;
	float font_x, font_y, font_w, font_h;
};
typedef struct gchar_t gchar_t;

/* An OpenGL buffer of graphical characters. */
struct text_layer_t
{
	unsigned int gchar_maximum_number;
	unsigned int gchar_number;
	gchar_t* gchar_array;
	GLuint buf_gchars_id;
};
typedef struct text_layer_t text_layer_t;

#endif /* GRAVINYON_HEADER_TEXT_ */
