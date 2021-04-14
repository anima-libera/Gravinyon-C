
#include "bg.h"
#include "utils.h"
#include <stdlib.h>
#include "random.h"
#include "shaders.h"

void bg_init(bg_t* bg)
{
	bg->time = 0.0f;

	/* Stars */
	bg->star_maximum_number = 256;
	bg->star_number = 0;
	bg->star_array = xcalloc(bg->star_maximum_number, sizeof(star_t));
	glGenBuffers(1, &bg->buf_stars_id);
	glBindBuffer(GL_ARRAY_BUFFER, bg->buf_stars_id);
	glBufferData(GL_ARRAY_BUFFER, bg->star_maximum_number * sizeof(star_t),
		bg->star_array, GL_DYNAMIC_DRAW);
}

void bg_cleanup(bg_t* bg)
{
	free(bg->star_array);
	glDeleteBuffers(1, &bg->buf_stars_id);
}

star_t* bg_alloc_star(bg_t* bg)
{
	DYNAMIC_VECTOR_RESIZE_IF_NEEDED(
		bg->star_number, bg->star_maximum_number, bg->star_array, star_t);
	star_t* new_star = &bg->star_array[bg->star_number++];
	return new_star;
}

void bg_render(bg_t* bg)
{
	/* Render the space background */
	glProgramUniform1f(g_shprog_draw_bg, 0, bg->time);
	glViewport(400, 0, 800, 800);
	glUseProgram(g_shprog_draw_bg);
	glDrawArrays(GL_POINTS, 0, 1);
	glUseProgram((GLuint)0);

	/* Render the stars */
	if (bg->star_number > 0)
	{
		#define ATTRIB_LOCATION_POS ((GLuint)0)
		#define ATTRIB_LOCATION_SIZE ((GLuint)1)
		#define ATTRIB_LOCATION_COLOR ((GLuint)2)

		glViewport(400, 0, 800, 800);
		glUseProgram(g_shprog_draw_stars);
		glEnableVertexAttribArray(ATTRIB_LOCATION_POS);
		glEnableVertexAttribArray(ATTRIB_LOCATION_SIZE);
		glEnableVertexAttribArray(ATTRIB_LOCATION_COLOR);
		
		glBindBuffer(GL_ARRAY_BUFFER, bg->buf_stars_id);
		glVertexAttribPointer(ATTRIB_LOCATION_POS, 2, GL_FLOAT,
			GL_FALSE, sizeof(star_t), (void*)offsetof(star_t, x));
		glVertexAttribPointer(ATTRIB_LOCATION_SIZE, 1, GL_FLOAT,
			GL_FALSE, sizeof(star_t), (void*)offsetof(star_t, size));
		glVertexAttribPointer(ATTRIB_LOCATION_COLOR, 3, GL_FLOAT,
			GL_FALSE, sizeof(star_t), (void*)offsetof(star_t, r));

		glDrawArrays(GL_POINTS, 0, bg->star_number);
		
		glDisableVertexAttribArray(ATTRIB_LOCATION_POS);
		glDisableVertexAttribArray(ATTRIB_LOCATION_SIZE);
		glDisableVertexAttribArray(ATTRIB_LOCATION_COLOR);
		glUseProgram((GLuint)0);

		#undef ATTRIB_LOCATION_POS
		#undef ATTRIB_LOCATION_SIZE
		#undef ATTRIB_LOCATION_COLOR
	}
}

void bg_perform_iter(bg_t* bg)
{
	bg->time += 1.0f;

	/* Spaw stars */
	while (rg_uint(g_rg, 0, 20) == 0)
	{
		star_t* star = bg_alloc_star(bg);
		star->x = 1.0f;
		star->y = rg_float(g_rg, -1.0f, 1.0f);
		star->r = 1.0f;
		star->g = 1.0f;
		star->b = 1.0f;
		star->size = //rg_uint(g_rg, 0, 4) == 0 ? 2.0f : 1.0f;
			rg_float(g_rg, 1.0f, 3.0f);
		star->speed = rg_float(g_rg, 0.001f, 0.008f);
	}

	/* Update the stars */
	for (unsigned int i = 0; i < bg->star_number; i++)
	{
		star_t* star = &bg->star_array[i];

		star->x -= star->speed;

		/* Out of the world */
		if (star->x < -1.0f - star->size)
		{
			/* The star dies */
			*star = bg->star_array[--bg->star_number];
			continue;
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, bg->buf_stars_id);
	glBufferData(GL_ARRAY_BUFFER, bg->star_maximum_number * sizeof(star_t),
		bg->star_array, GL_DYNAMIC_DRAW);
}
