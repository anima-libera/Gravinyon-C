
#ifndef GRAVINYON_HEADER_GAME_
#define GRAVINYON_HEADER_GAME_

#include "input.h"
#include "random.h"
#include "opengl.h"

#define GAME_VIEWPORT_LEFT 0
#define GAME_VIEWPORT_TOP 0
#define GAME_VIEWPORT_WIDTH 1600
#define GAME_VIEWPORT_HEIGHT 800
#define GAME_ASPECT_RATIO \
	((float)GAME_VIEWPORT_WIDTH / (float)GAME_VIEWPORT_HEIGHT)

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
	float r, g, b;
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
	rg_t rg;

	unsigned int ship_maximum_number;
	unsigned int ship_number;
	ship_t* ship_array;
	GLuint buf_ships_id;

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
ship_t* gs_alloc_ship(gs_t* gs);
enemy_t* gs_alloc_enemy(gs_t* gs);
bullet_t* gs_alloc_bullet(gs_t* gs);
part_t* gs_alloc_part(gs_t* gs);
void gs_spawn_ship(gs_t* gs);
void gs_spawn_enemies(gs_t* gs);
void gs_perform_iter(gs_t* gs, commands_t* commands);
void gs_perform_iter_stars(gs_t* gs);
void gs_render_stars(gs_t* gs);
void gs_render(gs_t* gs);

#endif /* GRAVINYON_HEADER_GAME_ */
