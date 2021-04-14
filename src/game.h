
#ifndef GRAVINYON_HEADER_GAME_
#define GRAVINYON_HEADER_GAME_

#include "input.h"
#include <GL/glew.h>

struct ship_t
{
	float x, y;
	float draw_angle;
	float angle, speed;
	GLuint reload, reload_max;
};
typedef struct ship_t ship_t;

struct enemy_t
{
	float x, y;
	float r, g, b;
	float angle, speed;
};
typedef struct enemy_t enemy_t;

struct bullet_t
{
	float x, y;
	float tail_x, tail_y;
	float angle, speed;
	GLuint safe_time;
};
typedef struct bullet_t bullet_t;

struct part_t
{
	float x, y;
	float r, g, b;
	float radius, radius_max;
	float draw_angle, rotation;
	float angle, speed;
	GLuint life_time, life_time_max;
};
typedef struct part_t part_t;

/* Game state data */
struct gs_t
{
	ship_t ship;
	GLuint buf_ship_id;

	unsigned int enemy_maximum_number;
	unsigned int enemy_number;
	enemy_t* enemy_array;
	GLuint buf_enemies_id;

	unsigned int bullet_maximum_number;
	unsigned int bullet_number;
	bullet_t* bullet_array;
	GLuint buf_bullets_id;

	unsigned int part_maximum_number;
	unsigned int part_number;
	part_t* part_array;
	GLuint buf_parts_id;
};
typedef struct gs_t gs_t;

int init_g_all(void);
void cleanup_g_all(void);
void gs_init(gs_t* gs);
void gs_cleanup(gs_t* gs);
enemy_t* gs_alloc_enemy(gs_t* gs);
bullet_t* gs_alloc_bullet(gs_t* gs);
part_t* gs_alloc_part(gs_t* gs);
void gs_spawn_enemies(gs_t* gs);
void gs_perform_iter(gs_t* gs, commands_t* commands);
void gs_perform_iter_stars(gs_t* gs);
void gs_render_stars(gs_t* gs);
void gs_render(gs_t* gs);

#endif /* GRAVINYON_HEADER_GAME_ */
