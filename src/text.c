
#include "text.h"

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
