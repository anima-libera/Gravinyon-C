
#include "bg.h"
#include "utils.h"
#include "random.h"
#include "shaders.h"
#include "game.h"
#include "opengl.h"
#include <stdlib.h>

static generic_bg_data_t* init_black(void)
{
	return NULL;
}

static void cleanup_black(generic_bg_data_t* bg_ptr)
{
	(void)bg_ptr;
}

static void iter_black(generic_bg_data_t* bg_ptr)
{
	(void)bg_ptr;
}

static void render_black(generic_bg_data_t* bg_ptr)
{
	(void)bg_ptr;

	glViewport(
		GAME_VIEWPORT_LEFT, GAME_VIEWPORT_TOP,
		GAME_VIEWPORT_WIDTH, GAME_VIEWPORT_HEIGHT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

const bg_description_t g_bg_description_black = {
	.init = init_black,
	.cleanup = cleanup_black,
	.iter = iter_black,
	.render = render_black,
};

struct star_t
{
	float x, y;
	float r, g, b;
	float size;
	float speed;
};
typedef struct star_t star_t;

struct data_blue_clouds_and_stars_t
{
	float time;
	rg_t rg;

	unsigned int star_maximum_number;
	unsigned int star_number;
	star_t* star_array;
	GLuint buf_stars_id;
};
typedef struct data_blue_clouds_and_stars_t data_blue_clouds_and_stars_t;

generic_bg_data_t* init_blue_clouds_and_stars(void)
{
	data_blue_clouds_and_stars_t* bg_data = malloc(sizeof (data_blue_clouds_and_stars_t));

	bg_data->time = 0.0f;
	rg_time_seed(&bg_data->rg);

	bg_data->star_maximum_number = 256;
	bg_data->star_number = 0;
	bg_data->star_array = xcalloc(bg_data->star_maximum_number, sizeof(star_t));
	glGenBuffers(1, &bg_data->buf_stars_id);
	glBindBuffer(GL_ARRAY_BUFFER, bg_data->buf_stars_id);
	glBufferData(GL_ARRAY_BUFFER, bg_data->star_maximum_number * sizeof(star_t),
		bg_data->star_array, GL_DYNAMIC_DRAW);

	return bg_data;
}

void cleanup_blue_clouds_and_stars(generic_bg_data_t* bg_ptr)
{
	data_blue_clouds_and_stars_t* bg_data = bg_ptr;

	free(bg_data->star_array);
	glDeleteBuffers(1, &bg_data->buf_stars_id);
}

star_t* blue_clouds_and_stars_alloc_star(data_blue_clouds_and_stars_t* bg_data)
{
	DYNAMIC_VECTOR_RESIZE_IF_NEEDED(
		bg_data->star_number, bg_data->star_maximum_number, bg_data->star_array, star_t);
	star_t* new_star = &bg_data->star_array[bg_data->star_number++];
	return new_star;
}

void render_blue_clouds_and_stars(generic_bg_data_t* bg_ptr)
{
	data_blue_clouds_and_stars_t* bg_data = bg_ptr;

	/* Render the space background */
	glProgramUniform1f(g_shprog_draw_bg, 0, bg_data->time);
	glViewport(
		GAME_VIEWPORT_LEFT, GAME_VIEWPORT_TOP,
		GAME_VIEWPORT_WIDTH, GAME_VIEWPORT_HEIGHT);
	glUseProgram(g_shprog_draw_bg);
	glDrawArrays(GL_POINTS, 0, 1);
	glUseProgram((GLuint)0);

	/* Render the stars */
	if (bg_data->star_number > 0)
	{
		#define ATTRIB_LOCATION_POS ((GLuint)0)
		#define ATTRIB_LOCATION_SIZE ((GLuint)1)
		#define ATTRIB_LOCATION_COLOR ((GLuint)2)

		glViewport(
			GAME_VIEWPORT_LEFT, GAME_VIEWPORT_TOP,
			GAME_VIEWPORT_WIDTH, GAME_VIEWPORT_HEIGHT);
		glUseProgram(g_shprog_draw_stars);
		glEnableVertexAttribArray(ATTRIB_LOCATION_POS);
		glEnableVertexAttribArray(ATTRIB_LOCATION_SIZE);
		glEnableVertexAttribArray(ATTRIB_LOCATION_COLOR);
		
		glBindBuffer(GL_ARRAY_BUFFER, bg_data->buf_stars_id);
		glVertexAttribPointer(ATTRIB_LOCATION_POS, 2, GL_FLOAT,
			GL_FALSE, sizeof(star_t), (void*)offsetof(star_t, x));
		glVertexAttribPointer(ATTRIB_LOCATION_SIZE, 1, GL_FLOAT,
			GL_FALSE, sizeof(star_t), (void*)offsetof(star_t, size));
		glVertexAttribPointer(ATTRIB_LOCATION_COLOR, 3, GL_FLOAT,
			GL_FALSE, sizeof(star_t), (void*)offsetof(star_t, r));

		glDrawArrays(GL_POINTS, 0, bg_data->star_number);
		
		glDisableVertexAttribArray(ATTRIB_LOCATION_POS);
		glDisableVertexAttribArray(ATTRIB_LOCATION_SIZE);
		glDisableVertexAttribArray(ATTRIB_LOCATION_COLOR);
		glUseProgram((GLuint)0);

		#undef ATTRIB_LOCATION_POS
		#undef ATTRIB_LOCATION_SIZE
		#undef ATTRIB_LOCATION_COLOR
	}
}

void iter_blue_clouds_and_stars(generic_bg_data_t* bg_ptr)
{
	data_blue_clouds_and_stars_t* bg_data = bg_ptr;
	bg_data->time += 1.0f;

	/* Spaw stars */
	while (rg_int(&bg_data->rg, 0, 20) == 0)
	{
		star_t* star = blue_clouds_and_stars_alloc_star(bg_data);
		star->x = 1.0f;
		star->y = rg_float(&bg_data->rg,
			-1.0f / GAME_ASPECT_RATIO, 1.0f / GAME_ASPECT_RATIO);
		star->r = 1.0f;
		star->g = 1.0f;
		star->b = 1.0f;
		star->size = //rg_int(&bg_data->rg, 0, 4) == 0 ? 2.0f : 1.0f;
			rg_float(&bg_data->rg, 1.0f, 3.0f);
		star->speed = rg_float(&bg_data->rg, 0.001f, 0.008f);
	}

	/* Update the stars */
	for (unsigned int i = 0; i < bg_data->star_number; i++)
	{
		star_t* star = &bg_data->star_array[i];

		star->x -= star->speed;

		/* Out of the world */
		if (star->x < -1.0f - star->size)
		{
			/* The star dies */
			*star = bg_data->star_array[--bg_data->star_number];
			continue;
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, bg_data->buf_stars_id);
	glBufferData(GL_ARRAY_BUFFER, bg_data->star_maximum_number * sizeof(star_t),
		bg_data->star_array, GL_DYNAMIC_DRAW);
}

const bg_description_t g_bg_description_blue_clouds_and_stars = {
	.init = init_blue_clouds_and_stars,
	.cleanup = cleanup_blue_clouds_and_stars,
	.iter = iter_blue_clouds_and_stars,
	.render = render_blue_clouds_and_stars,
};
