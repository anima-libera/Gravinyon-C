
#include "text.h"
#include "utils.h"

void font_init_alpha(font_t* font)
{
	unsigned int side = 256;
	font->texture_side = side;
	font->texture_data = xcalloc(side * side, 1);
	font->texture_data[2 + 16 * side] = 255;
	font->texture_data[1 + 15 * side] = 255;
	font->texture_data[3 + 15 * side] = 255;
	font->texture_data[0 + 14 * side] = 255;
	font->texture_data[4 + 14 * side] = 255;
	font->texture_data[0 + 13 * side] = 255;
	font->texture_data[4 + 13 * side] = 255;
	font->texture_data[0 + 12 * side] = 255;
	font->texture_data[1 + 12 * side] = 255;
	font->texture_data[2 + 12 * side] = 255;
	font->texture_data[3 + 12 * side] = 255;
	font->texture_data[4 + 12 * side] = 255;
	font->texture_data[0 + 11 * side] = 255;
	font->texture_data[4 + 11 * side] = 255;
	font->texture_data[0 + 10 * side] = 255;
	font->texture_data[4 + 10 * side] = 255;
	font->texture_data[0 +  9 * side] = 255;
	font->texture_data[4 +  9 * side] = 255;

	glGenTextures(1, &font->texture_id);
	glBindTexture(GL_TEXTURE_2D, font->texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0,
		GL_RED, side, side, 0,
		GL_RED, GL_UNSIGNED_BYTE, font->texture_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void text_layer_init(text_layer_t* tl)
{
	tl->gchar_maximum_number = 16;
	tl->gchar_number = 0;
	tl->gchar_array = xcalloc(tl->gchar_maximum_number, sizeof(gchar_t));
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
