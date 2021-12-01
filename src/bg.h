
#ifndef GRAVINYON_HEADER_BG_
#define GRAVINYON_HEADER_BG_

#include "random.h"
#include "opengl.h"

struct star_t
{
	float x, y;
	float r, g, b;
	float size;
	float speed;
};
typedef struct star_t star_t;

struct bg_t
{
	float time;
	rg_t rg;

	unsigned int star_maximum_number;
	unsigned int star_number;
	star_t* star_array;
	GLuint buf_stars_id;
};
typedef struct bg_t bg_t;

void bg_init(bg_t* bg);
void bg_cleanup(bg_t* bg);
void bg_render(bg_t* bg);
star_t* bg_alloc_star(bg_t* bg);
void bg_render(bg_t* bg);
void bg_perform_iter(bg_t* bg);

#endif /* GRAVINYON_HEADER_BG_ */
