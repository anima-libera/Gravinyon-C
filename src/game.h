
#ifndef GRAVINYON_HEADER_GAME_
#define GRAVINYON_HEADER_GAME_

#include "input.h"
#include "random.h"
#include "opengl.h"
#include <stdbool.h>

#define GAME_VIEWPORT_LEFT 0
#define GAME_VIEWPORT_TOP 0
#define GAME_VIEWPORT_WIDTH 1600
#define GAME_VIEWPORT_HEIGHT 800
#define GAME_ASPECT_RATIO \
	((float)GAME_VIEWPORT_WIDTH / (float)GAME_VIEWPORT_HEIGHT)

typedef struct gs_t gs_t;

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
	void (*behavior)(gs_t* gs, struct enemy_t* enemy);
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

struct game_settings_t
{
	bool enabled_spawning_ships;
	bool enabled_spawning_enemies;
	bool enabled_shooting;
	bool enabled_spawning_particles;
	bool enabled_background;
};
typedef struct game_settings_t game_settings_t;

#define GAME_SETTINGS_ALL \
	(game_settings_t) \
	{ \
		.enabled_spawning_ships = true, \
		.enabled_spawning_enemies = true, \
		.enabled_shooting = true, \
		.enabled_spawning_particles = true, \
		.enabled_background = true, \
	}


/* Game state data */
struct gs_t
{
	game_settings_t settings;
	
	rg_t rg;

	float cursor_x, cursor_y;

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

int init_g_all(void);
void cleanup_g_all(void);

void game_loop(game_settings_t game_settings);

#endif /* GRAVINYON_HEADER_GAME_ */
