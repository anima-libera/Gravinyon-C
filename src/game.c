
#include "game.h"
#include "utils.h"
#include "random.h"
#include "audio.h"
#include "shaders.h"
#include "window.h"
#include "bg.h"
#include "opengl.h"

inline static float square_length(float x, float y)
{
	return (x * x) + (y * y);
}

inline static float length(float x, float y)
{
	return sqrtf(square_length(x, y));
}

#define INGAME_LEFT (-1.0f)
#define INGAME_RIGHT (1.0f)
#define INGAME_TOP (1.0f / GAME_ASPECT_RATIO)
#define INGAME_BOTTOM (-1.0f / GAME_ASPECT_RATIO)

void gs_init(gs_t* gs)
{
	rg_time_seed(&gs->rg);

	/* Ship */
	gs->ship_maximum_number = 0;
	gs->ship_number = 0;
	gs->ship_array = xcalloc(gs->ship_maximum_number, sizeof(ship_t));
	glGenBuffers(1, &gs->buf_ships_id);
	glBindBuffer(GL_ARRAY_BUFFER, gs->buf_ships_id);
	glBufferData(GL_ARRAY_BUFFER, gs->ship_maximum_number * sizeof(ship_t),
		&gs->ship_array, GL_DYNAMIC_DRAW);

	/* Enemies */
	gs->enemy_maximum_number = 0;
	gs->enemy_number = 0;
	gs->enemy_array = xcalloc(gs->enemy_maximum_number, sizeof(enemy_t));
	glGenBuffers(1, &gs->buf_enemies_id);
	glBindBuffer(GL_ARRAY_BUFFER, gs->buf_enemies_id);
	glBufferData(GL_ARRAY_BUFFER, gs->enemy_maximum_number * sizeof(enemy_t),
		gs->enemy_array, GL_DYNAMIC_DRAW);

	/* Bullets */
	gs->bullet_maximum_number = 0;
	gs->bullet_number = 0;
	gs->bullet_array = xcalloc(gs->bullet_maximum_number, sizeof(bullet_t));
	glGenBuffers(1, &gs->buf_bullets_id);
	glBindBuffer(GL_ARRAY_BUFFER, gs->buf_bullets_id);
	glBufferData(GL_ARRAY_BUFFER, gs->bullet_maximum_number * sizeof(bullet_t),
		gs->bullet_array, GL_DYNAMIC_DRAW);

	/* Particles */
	gs->part_maximum_number = 0;
	gs->part_number = 0;
	gs->part_array = xcalloc(gs->part_maximum_number, sizeof(part_t));
	glGenBuffers(1, &gs->buf_parts_id);
	glBindBuffer(GL_ARRAY_BUFFER, gs->buf_parts_id);
	glBufferData(GL_ARRAY_BUFFER, gs->part_maximum_number * sizeof(part_t),
		gs->part_array, GL_DYNAMIC_DRAW);
}

void gs_cleanup(gs_t* gs)
{
	free(gs->ship_array);
	free(gs->enemy_array);
	free(gs->bullet_array);
	free(gs->part_array);
	glDeleteBuffers(1, &gs->buf_ships_id);
	glDeleteBuffers(1, &gs->buf_enemies_id);
	glDeleteBuffers(1, &gs->buf_bullets_id);
	glDeleteBuffers(1, &gs->buf_parts_id);
}

#define DEFINE_GS_ALLOC_FUNCTION(name_) \
	name_##_t* gs_alloc_##name_(gs_t* gs) \
	{ \
		DYNAMIC_VECTOR_RESIZE_IF_NEEDED( \
			gs->name_##_number, gs->name_##_maximum_number, \
			gs->name_##_array, name_##_t); \
		name_##_t* new_object = &gs->name_##_array[gs->name_##_number++]; \
		*new_object = (name_##_t){0}; \
		return new_object; \
	} \
	name_##_t* gs_alloc_##name_(gs_t* gs)

DEFINE_GS_ALLOC_FUNCTION(ship);
DEFINE_GS_ALLOC_FUNCTION(enemy);
DEFINE_GS_ALLOC_FUNCTION(bullet);
DEFINE_GS_ALLOC_FUNCTION(part);

void gs_spawn_ship(gs_t* gs)
{
	ship_t* new_ship = gs_alloc_ship(gs);
	new_ship->x = 0.0f;
	new_ship->y = 0.0f;
	new_ship->reload_max = 5;
}

void enemy_behavior_basic(gs_t* gs, struct enemy_t* enemy)
{
	(void)gs;

	float vx = cosf(enemy->angle) * enemy->speed;
	float vy = sinf(enemy->angle) * enemy->speed;

	enemy->x += vx;
	enemy->y += vy;

	if (enemy->x < INGAME_LEFT)
	{
		enemy->x = INGAME_RIGHT;
	}
	else if (enemy->x > INGAME_RIGHT)
	{
		enemy->x = INGAME_LEFT;
	}
	if (enemy->y < INGAME_BOTTOM)
	{
		enemy->y = INGAME_BOTTOM;
		vy *= -1.0f;
	}
	else if (enemy->y > INGAME_TOP)
	{
		enemy->y = INGAME_TOP;
		vy *= -1.0f;
	}

	enemy->angle = atan2f(vy, vx);
	enemy->speed = length(vx, vy);
}

void enemy_behavior_aimer(gs_t* gs, struct enemy_t* enemy)
{
	float vx = cosf(enemy->angle) * enemy->speed;
	float vy = sinf(enemy->angle) * enemy->speed;

	enemy->x += vx;
	enemy->y += vy;

	if (enemy->x < INGAME_LEFT)
	{
		enemy->x = INGAME_RIGHT;
	}
	else if (enemy->x > INGAME_RIGHT)
	{
		enemy->x = INGAME_LEFT;
	}
	if (enemy->y < INGAME_BOTTOM)
	{
		enemy->y = INGAME_BOTTOM;
		vy *= -1.0f;
	}
	else if (enemy->y > INGAME_TOP)
	{
		enemy->y = INGAME_TOP;
		vy *= -1.0f;
	}

	if (gs->ship_number >= 1 && rg_int(&gs->rg, 0, 120) == 0)
	{
		unsigned int ship_index = rg_int(&gs->rg, 0, gs->ship_number-1);
		float ship_angle = atan2f(gs->ship_array[ship_index].y - enemy->y,
			gs->ship_array[ship_index].x - enemy->x);

		vx = cosf(ship_angle) * enemy->speed;
		vy = sinf(ship_angle) * enemy->speed;
	}

	enemy->angle = atan2f(vy, vx);
	enemy->speed = length(vx, vy);
}

void enemy_behavior_random(gs_t* gs, struct enemy_t* enemy)
{
	float vx = cosf(enemy->angle) * enemy->speed;
	float vy = sinf(enemy->angle) * enemy->speed;

	enemy->x += vx;
	enemy->y += vy;

	if (enemy->x < INGAME_LEFT)
	{
		enemy->x = INGAME_RIGHT;
	}
	else if (enemy->x > INGAME_RIGHT)
	{
		enemy->x = INGAME_LEFT;
	}
	if (enemy->y < INGAME_BOTTOM)
	{
		enemy->y = INGAME_BOTTOM;
		vy *= -1.0f;
	}
	else if (enemy->y > INGAME_TOP)
	{
		enemy->y = INGAME_TOP;
		vy *= -1.0f;
	}

	if (rg_int(&gs->rg, 0, 120) == 0)
	{
		float random_angle = rg_float(&gs->rg, TAU * 1.0f/4.0f, TAU * 3.0f/4.0f);

		vx = cosf(random_angle) * enemy->speed;
		vy = sinf(random_angle) * enemy->speed;
	}
	if (rg_int(&gs->rg, 0, 120) == 0)
	{
		float random_speed = rg_float(&gs->rg, 0.001f, 0.009f);

		vx = cosf(enemy->angle) * random_speed;
		vy = sinf(enemy->angle) * random_speed;
	}

	enemy->angle = atan2f(vy, vx);
	enemy->speed = length(vx, vy);
}

struct enemy_type_t
{
	float r, g, b;
	void (*behavior)(gs_t* gs, struct enemy_t* enemy);
};
typedef struct enemy_type_t enemy_type_t;

static const enemy_type_t s_enemy_type_table[] = {
	{.r = 0.0f, .g = 1.0f, .b = 1.0f, .behavior = enemy_behavior_basic},
	{.r = 0.3f, .g = 1.0f, .b = 0.0f, .behavior = enemy_behavior_aimer},
	{.r = 0.9f, .g = 0.4f, .b = 0.1f, .behavior = enemy_behavior_random},
};
static const unsigned int s_enemy_type_number = sizeof s_enemy_type_table / sizeof (enemy_type_t);

void gs_spawn_enemies(gs_t* gs)
{
	unsigned int spawn_number = rg_int(&gs->rg, 2, 8);
	for (unsigned int i = 0; i < spawn_number; i++)
	{
		const enemy_type_t* type = &s_enemy_type_table[rg_int(&gs->rg, 0, s_enemy_type_number-1)];
		
		enemy_t* new_enemy = gs_alloc_enemy(gs);
		new_enemy->r = type->r;
		new_enemy->g = type->g;
		new_enemy->b = type->b;
		new_enemy->behavior = type->behavior;

		new_enemy->x = 1.0f;

		if (rg_int(&gs->rg, 0, 3) == 0)
		{
			new_enemy->y = rg_float(&gs->rg, 
				-1.0f/GAME_ASPECT_RATIO, 1.0f/GAME_ASPECT_RATIO);
			new_enemy->angle = -TAU/2.0f + rg_float(&gs->rg, -0.06f, 0.06f);
			new_enemy->speed = rg_float(&gs->rg, 0.004f, 0.009f);
		}
		else
		{
			new_enemy->y = rg_float(&gs->rg, 
				-1.0f/GAME_ASPECT_RATIO, 1.0f/GAME_ASPECT_RATIO);
			new_enemy->angle = rg_float(&gs->rg, TAU*5.0f/16.0f, TAU*11.0f/16.0f);
			new_enemy->speed = rg_float(&gs->rg, 0.001f, 0.006f);
		}
	}
}

void gs_particles_boom(gs_t* gs,
	float x, float y, float angle, float speed, unsigned int number)
{
	for (unsigned int k = 0; k < number; k++)
	{
		part_t* new_part = gs_alloc_part(gs);
		new_part->x = x;
		new_part->y = y;
		new_part->r = 1.0f;
		new_part->g = 1.0f;
		new_part->b = 1.0f;
		new_part->radius_max = rg_float(&gs->rg, 0.005f, 0.018f);
		new_part->radius = new_part->radius_max;
		new_part->draw_angle = rg_float(&gs->rg, 0.0f, TAU);
		new_part->angle = angle + rg_float(&gs->rg, -0.4f, 0.4f);
		new_part->speed = speed * rg_float(&gs->rg, 0.1f, 1.3f);
		new_part->rotation = rg_float(&gs->rg, -0.03f, 0.03f);
		new_part->life_time_max = rg_int(&gs->rg, 20, 70);
		new_part->life_time = new_part->life_time_max;
	}
}

void gs_render_ships(gs_t* gs)
{
	if (gs->ship_number > 0)
	{
		#define ATTRIB_LOCATION_POS ((GLuint)0)
		#define ATTRIB_LOCATION_DRAW_ANGLE ((GLuint)1)

		glViewport(
			GAME_VIEWPORT_LEFT, GAME_VIEWPORT_TOP,
			GAME_VIEWPORT_WIDTH, GAME_VIEWPORT_HEIGHT);
		glUseProgram(g_shprog_draw_ships);
		glEnableVertexAttribArray(ATTRIB_LOCATION_POS);
		glEnableVertexAttribArray(ATTRIB_LOCATION_DRAW_ANGLE);
		
		glBindBuffer(GL_ARRAY_BUFFER, gs->buf_ships_id);
		glVertexAttribPointer(ATTRIB_LOCATION_POS, 2, GL_FLOAT,
			GL_FALSE, sizeof(ship_t), (void*)offsetof(ship_t, x));
		glVertexAttribPointer(ATTRIB_LOCATION_DRAW_ANGLE, 1, GL_FLOAT,
			GL_FALSE, sizeof(ship_t), (void*)offsetof(ship_t, draw_angle));

		glDrawArrays(GL_POINTS, 0, gs->ship_number);
		
		glDisableVertexAttribArray(ATTRIB_LOCATION_POS);
		glDisableVertexAttribArray(ATTRIB_LOCATION_DRAW_ANGLE);
		glUseProgram((GLuint)0);

		#undef ATTRIB_LOCATION_POS
		#undef ATTRIB_LOCATION_DRAW_ANGLE
	}
}

void gs_render_enemies(gs_t* gs)
{
	if (gs->enemy_number > 0)
	{
		#define ATTRIB_LOCATION_POS ((GLuint)0)
		#define ATTRIB_LOCATION_COLOR ((GLuint)1)
		#define ATTRIB_LOCATION_ANGLE ((GLuint)2)

		glViewport(
			GAME_VIEWPORT_LEFT, GAME_VIEWPORT_TOP,
			GAME_VIEWPORT_WIDTH, GAME_VIEWPORT_HEIGHT);
		glUseProgram(g_shprog_draw_enemies);
		glEnableVertexAttribArray(ATTRIB_LOCATION_POS);
		glEnableVertexAttribArray(ATTRIB_LOCATION_COLOR);
		glEnableVertexAttribArray(ATTRIB_LOCATION_ANGLE);
		
		glBindBuffer(GL_ARRAY_BUFFER, gs->buf_enemies_id);
		glVertexAttribPointer(ATTRIB_LOCATION_POS, 2, GL_FLOAT,
			GL_FALSE, sizeof(enemy_t), (void*)offsetof(enemy_t, x));
		glVertexAttribPointer(ATTRIB_LOCATION_COLOR, 3, GL_FLOAT,
			GL_FALSE, sizeof(enemy_t), (void*)offsetof(enemy_t, r));
		glVertexAttribPointer(ATTRIB_LOCATION_ANGLE, 1, GL_FLOAT,
			GL_FALSE, sizeof(enemy_t), (void*)offsetof(enemy_t, angle));

		glDrawArrays(GL_POINTS, 0, gs->enemy_number);
		
		glDisableVertexAttribArray(ATTRIB_LOCATION_POS);
		glDisableVertexAttribArray(ATTRIB_LOCATION_COLOR);
		glDisableVertexAttribArray(ATTRIB_LOCATION_ANGLE);
		glUseProgram((GLuint)0);

		#undef ATTRIB_LOCATION_POS
		#undef ATTRIB_LOCATION_COLOR
		#undef ATTRIB_LOCATION_ANGLE
	}
}

void gs_render_bullets(gs_t* gs)
{
	if (gs->bullet_number > 0)
	{
		#define ATTRIB_LOCATION_POS ((GLuint)0)
		#define ATTRIB_LOCATION_SAFE_TIME ((GLuint)1)
		#define ATTRIB_LOCATION_ANGLE ((GLuint)2)
		#define ATTRIB_LOCATION_TAIL_POS ((GLuint)3)

		glViewport(
			GAME_VIEWPORT_LEFT, GAME_VIEWPORT_TOP,
			GAME_VIEWPORT_WIDTH, GAME_VIEWPORT_HEIGHT);
		glUseProgram(g_shprog_draw_bullets);
		glEnableVertexAttribArray(ATTRIB_LOCATION_POS);
		glEnableVertexAttribArray(ATTRIB_LOCATION_SAFE_TIME);
		glEnableVertexAttribArray(ATTRIB_LOCATION_ANGLE);
		glEnableVertexAttribArray(ATTRIB_LOCATION_TAIL_POS);
		
		glBindBuffer(GL_ARRAY_BUFFER, gs->buf_bullets_id);
		glVertexAttribPointer(ATTRIB_LOCATION_POS, 2, GL_FLOAT,
			GL_FALSE, sizeof(bullet_t), (void*)offsetof(bullet_t, x));
		glVertexAttribPointer(ATTRIB_LOCATION_SAFE_TIME, 3, GL_FLOAT,
			GL_FALSE, sizeof(bullet_t), (void*)offsetof(bullet_t, r));
		glVertexAttribPointer(ATTRIB_LOCATION_ANGLE, 1, GL_FLOAT,
			GL_FALSE, sizeof(bullet_t), (void*)offsetof(bullet_t, angle));
		glVertexAttribPointer(ATTRIB_LOCATION_TAIL_POS, 2, GL_FLOAT,
			GL_FALSE, sizeof(bullet_t), (void*)offsetof(bullet_t, tail_x));

		glDrawArrays(GL_POINTS, 0, gs->bullet_number);
		
		glDisableVertexAttribArray(ATTRIB_LOCATION_POS);
		glDisableVertexAttribArray(ATTRIB_LOCATION_SAFE_TIME);
		glDisableVertexAttribArray(ATTRIB_LOCATION_ANGLE);
		glDisableVertexAttribArray(ATTRIB_LOCATION_TAIL_POS);
		glUseProgram((GLuint)0);

		#undef ATTRIB_LOCATION_POS
		#undef ATTRIB_LOCATION_SAFE_TIME
		#undef ATTRIB_LOCATION_ANGLE
		#undef ATTRIB_LOCATION_TAIL_POS
	}
}

void gs_render_particles(gs_t* gs)
{
	if (gs->part_number > 0)
	{
		#define ATTRIB_LOCATION_POS ((GLuint)0)
		#define ATTRIB_LOCATION_COLOR ((GLuint)1)
		#define ATTRIB_LOCATION_ANGLE ((GLuint)2)
		#define ATTRIB_LOCATION_RADIUS ((GLuint)3)

		glViewport(
			GAME_VIEWPORT_LEFT, GAME_VIEWPORT_TOP,
			GAME_VIEWPORT_WIDTH, GAME_VIEWPORT_HEIGHT);
		glUseProgram(g_shprog_draw_parts);
		glEnableVertexAttribArray(ATTRIB_LOCATION_POS);
		glEnableVertexAttribArray(ATTRIB_LOCATION_COLOR);
		glEnableVertexAttribArray(ATTRIB_LOCATION_ANGLE);
		glEnableVertexAttribArray(ATTRIB_LOCATION_RADIUS);
		
		glBindBuffer(GL_ARRAY_BUFFER, gs->buf_parts_id);
		glVertexAttribPointer(ATTRIB_LOCATION_POS, 2, GL_FLOAT,
			GL_FALSE, sizeof(part_t), (void*)offsetof(part_t, x));
		glVertexAttribPointer(ATTRIB_LOCATION_COLOR, 3, GL_FLOAT,
			GL_FALSE, sizeof(part_t), (void*)offsetof(part_t, r));
		glVertexAttribPointer(ATTRIB_LOCATION_ANGLE, 1, GL_FLOAT,
			GL_FALSE, sizeof(part_t), (void*)offsetof(part_t, draw_angle));
		glVertexAttribPointer(ATTRIB_LOCATION_RADIUS, 1, GL_FLOAT,
			GL_FALSE, sizeof(part_t), (void*)offsetof(part_t, radius));

		glDrawArrays(GL_POINTS, 0, gs->part_number);
		
		glDisableVertexAttribArray(ATTRIB_LOCATION_POS);
		glDisableVertexAttribArray(ATTRIB_LOCATION_COLOR);
		glDisableVertexAttribArray(ATTRIB_LOCATION_ANGLE);
		glDisableVertexAttribArray(ATTRIB_LOCATION_RADIUS);
		glUseProgram((GLuint)0);

		#undef ATTRIB_LOCATION_POS
		#undef ATTRIB_LOCATION_COLOR
		#undef ATTRIB_LOCATION_ANGLE
		#undef ATTRIB_LOCATION_RADIUS
	}
}

void gs_render(gs_t* gs)
{
	gs_render_ships(gs);
	gs_render_enemies(gs);
	gs_render_bullets(gs);
	gs_render_particles(gs);
}

void gs_spawn_enemies_maybe(gs_t* gs)
{
	if (gs->enemy_number <= 3 || rg_int(&gs->rg, 0, 999) == 0)
	{
		gs_spawn_enemies(gs);
	}
}

void gs_update_cursor(gs_t* gs, commands_t* commands)
{
	gs->cursor_x =
		2.0f * (
			(float)commands->inwindow_cursor_x - (float)GAME_VIEWPORT_LEFT
		) / (float)GAME_VIEWPORT_WIDTH - 1.0f;
	gs->cursor_y =
		(
			2.0f * (
				(float)GAME_VIEWPORT_TOP - (float)commands->inwindow_cursor_y
			) + GAME_VIEWPORT_HEIGHT
		) / (float)GAME_VIEWPORT_WIDTH;
}

void gs_perform_iter(gs_t* gs, commands_t* commands)
{
	if (gs->settings.enabled_spawning_enemies)
	{
		gs_spawn_enemies_maybe(gs);
	}

	gs_update_cursor(gs, commands);

	#define SHIP_COLLIDE_RADIUS 0.013
	#define ENEMY_RADIUS 0.012
	
	/* Update the ships and handle shooting. */
	for (unsigned int i = 0; i < gs->ship_number; i++)
	{
		ship_t* ship = &gs->ship_array[i];

		float vx = cosf(ship->angle) * ship->speed;
		float vy = sinf(ship->angle) * ship->speed;

		float cursor_dist = length(ship->x - gs->cursor_x, ship->y - gs->cursor_y);

		/* Handle collision with the cursor. */
		if (cursor_dist < SHIP_COLLIDE_RADIUS)
		{
			play_sound(g_sound_die);
			gs_particles_boom(gs,
				ship->x, ship->y, ship->angle, ship->speed,
				rg_int(&gs->rg, 50, 70));

			/* Ship death. */
			*ship = gs->ship_array[--gs->ship_number];
			i--;
			continue;
		}
		
		/* Handle collision with enemies. */
		for (unsigned int j = 0; j < gs->enemy_number; j++)
		{
			float dist = length(
				ship->x - gs->enemy_array[j].x, ship->y - gs->enemy_array[j].y);
			if (dist < SHIP_COLLIDE_RADIUS + ENEMY_RADIUS)
			{
				play_sound(g_sound_die);
				gs_particles_boom(gs,
					ship->x, ship->y, ship->angle, ship->speed,
					rg_int(&gs->rg, 50, 70));

				/* Ship death. */
				*ship = gs->ship_array[--gs->ship_number];
				i--;
				goto continue_ships;
			}
		}

		float cursor_angle = atan2f(gs->cursor_y - ship->y, gs->cursor_x - ship->x);

		/* Handle shooting. */
		if (gs->settings.enabled_shooting)
		{
			if (ship->reload > 0)
			{
				ship->reload--;
			}
			else if (commands->is_firing)
			{
				ship->reload = ship->reload_max;

				play_sound(g_sound_pew);

				#if 0
				#define RECOIL_FACTOR 0.003f
				vx -= RECOIL_FACTOR * cosf(cursor_angle);
				vy -= RECOIL_FACTOR * sinf(cursor_angle);
				#undef RECOIL_FACTOR
				#endif

				bullet_t* new_bullet = gs_alloc_bullet(gs);
				new_bullet->x =
					ship->x + cosf(cursor_angle) * SHIP_COLLIDE_RADIUS;
				new_bullet->y =
					ship->y + sinf(cursor_angle) * SHIP_COLLIDE_RADIUS;
				new_bullet->r = 1.0f;
				new_bullet->g = 1.0f;
				new_bullet->b = 1.0f;
				new_bullet->tail_x = ship->x;
				new_bullet->tail_y = ship->y;
				new_bullet->angle = cursor_angle;
				new_bullet->speed = 0.03f / cursor_dist;
				new_bullet->safe_time = 20;
			}
		}
		
		#define GRAVITY_FACTOR 0.0008f
		vx += GRAVITY_FACTOR * (gs->cursor_x - ship->x) / cursor_dist;
		vy += GRAVITY_FACTOR * (gs->cursor_y - ship->y) / cursor_dist;
		#undef GRAVITY_FACTOR

		#define SHIP_SPEED_FACTOR 0.65f
		ship->x += vx * SHIP_SPEED_FACTOR;
		ship->y += vy * SHIP_SPEED_FACTOR;
		#undef SHIP_SPEED_FACTOR

		/* The ship bounces or dies on the edges of the area. */
		#define BOUNCE_FACTOR 0.8f
		if (ship->x < INGAME_LEFT || ship->x > INGAME_RIGHT)
		{
			play_sound(g_sound_die);
			gs_particles_boom(gs,
				ship->x, ship->y, ship->angle, -ship->speed,
				rg_int(&gs->rg, 50, 70));

			/* Ship death. */
			*ship = gs->ship_array[--gs->ship_number];
			i--;
			continue;
		}
		if (ship->y < INGAME_BOTTOM)
		{
			ship->y = INGAME_BOTTOM;
			vy *= -BOUNCE_FACTOR;
		}
		else if (ship->y > INGAME_TOP)
		{
			ship->y = INGAME_TOP;
			vy *= -BOUNCE_FACTOR;
		}
		#undef BOUNCE_FACTOR

		ship->angle = atan2f(vy, vx);
		ship->speed = length(vx, vy);
		ship->draw_angle = cursor_angle;

		if (0)
		{
			/* Warning: Super high quality code, do not stare for too
			* long without eye protection. */
			continue_ships: continue;
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, gs->buf_ships_id);
	glBufferData(GL_ARRAY_BUFFER, gs->ship_number * sizeof(ship_t),
		gs->ship_array, GL_DYNAMIC_DRAW);

	/* Update the bullets. */
	for (unsigned int i = 0; i < gs->bullet_number; i++)
	{
		bullet_t* bullet = &gs->bullet_array[i];

		bullet->tail_x = (bullet->x + 4.0f * bullet->tail_x) / 5.0f;
		bullet->tail_y = (bullet->y + 4.0f * bullet->tail_y) / 5.0f;

		float vx = cosf(bullet->angle) * bullet->speed;
		float vy = sinf(bullet->angle) * bullet->speed;

		bullet->x += vx;
		bullet->y += vy;

		/* Bullets dispawn when exiting the area. */
		if (bullet->tail_x < -1.0f || bullet->tail_x > 1.0f ||
			bullet->tail_y < -1.0f || bullet->tail_y > 1.0f)
		{
			/* Dullet dispawning. */
			*bullet = gs->bullet_array[--gs->bullet_number];
			i--;
			continue;
		}

		/* Collision with enemies. */
		for (unsigned int j = 0; j < gs->enemy_number; j++)
		{
			enemy_t* enemy = &gs->enemy_array[j];

			float bullet_length = length(
				bullet->x - bullet->tail_x, bullet->y - bullet->tail_y);
			if (bullet_length == 0.0f)
			{
				bullet_length = 0.0001f;
			}

			#define STEP 0.010
			for (float s = 0.0f; s <= bullet_length; s += STEP)
			{
				float s_unit = s / bullet_length;

				float s_x =
					bullet->tail_x * s_unit + bullet->x * (1.0f - s_unit);
				float s_y =
					bullet->tail_y * s_unit + bullet->y * (1.0f - s_unit);

				float dist = length(enemy->x - s_x, enemy->y - s_y);
				if (dist < ENEMY_RADIUS)
				{
					play_sound(g_sound_boom);

					gs_particles_boom(gs,
						enemy->x, enemy->y, bullet->angle, bullet->speed,
						rg_int(&gs->rg, 30, 50));

					/* Enemy death. */
					*enemy = gs->enemy_array[--gs->enemy_number];

					/* Bullet dispawning. */
					*bullet = gs->bullet_array[--gs->bullet_number];
					i--;
					goto continue_bullets;
				}
			}
			#undef STEP
		}

		/* Collision with ships. */
		if (bullet->safe_time > 0)
		{
			bullet->safe_time--;
		}
		else
		{
			for (unsigned int j = 0; j < gs->ship_number; j++)
			{
				ship_t* ship = &gs->ship_array[j];

				float ship_dist =
					length(bullet->x - ship->x, bullet->y - ship->y);
				if (ship_dist < SHIP_COLLIDE_RADIUS)
				{
					play_sound(g_sound_die);
					gs_particles_boom(gs,
						ship->x, ship->y, ship->angle, ship->speed,
						rg_int(&gs->rg, 50, 70));

					/* Ship death. */
					*ship = gs->ship_array[--gs->ship_number];

					/* Bullet dispawning. */
					*bullet = gs->bullet_array[--gs->bullet_number];
					i--;
					goto continue_bullets;
				}
			}
		}

		if (0)
		{
			/* Warning: Super high quality code, do not stare for too
			* long without eye protection. */
			continue_bullets: continue;
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, gs->buf_bullets_id);
	glBufferData(GL_ARRAY_BUFFER, gs->bullet_maximum_number * sizeof(bullet_t),
		gs->bullet_array, GL_DYNAMIC_DRAW);

	/* Update the enemies. */
	for (unsigned int i = 0; i < gs->enemy_number; i++)
	{
		enemy_t* enemy = &gs->enemy_array[i];

		enemy->behavior(gs, enemy);
	}
	glBindBuffer(GL_ARRAY_BUFFER, gs->buf_enemies_id);
	glBufferData(GL_ARRAY_BUFFER, gs->enemy_maximum_number * sizeof(enemy_t),
		gs->enemy_array, GL_DYNAMIC_DRAW);

	#undef SHIP_COLLIDE_RADIUS
	#undef ENEMY_RADIUS

	/* Update the particles. */
	for (unsigned int i = 0; i < gs->part_number; i++)
	{
		part_t* part = &gs->part_array[i];

		if (part->life_time == 0)
		{
			/* Particle dispawning. */
			*part = gs->part_array[--gs->part_number];
			continue;
		}
		part->life_time--;
		part->radius = part->radius_max *
			((float)part->life_time / (float)part->life_time_max);

		float vx = cosf(part->angle) * part->speed;
		float vy = sinf(part->angle) * part->speed;

		part->x += vx;
		part->y += vy;

		/* Bullets dispawn when exiting the area. */
		if (part->x < -1.0f - part->radius ||
			part->x > 1.0f + part->radius ||
			part->y < -1.0f - part->radius ||
			part->y > 1.0f + part->radius)
		{
			/* Particle dispawning. */
			*part = gs->part_array[--gs->part_number];
			continue;
		}

		part->draw_angle += part->rotation;
	}
	glBindBuffer(GL_ARRAY_BUFFER, gs->buf_parts_id);
	glBufferData(GL_ARRAY_BUFFER, gs->part_maximum_number * sizeof(part_t),
		gs->part_array, GL_DYNAMIC_DRAW);
}

void game_loop(game_settings_t game_settings)
{
	gs_t gs = {0};
	gs.settings = game_settings;

	rg_time_seed(&gs.rg);

	gs.ship_maximum_number = 0;
	gs.ship_number = 0;
	gs.ship_array = xcalloc(gs.ship_maximum_number, sizeof(ship_t));
	glGenBuffers(1, &gs.buf_ships_id);
	glBindBuffer(GL_ARRAY_BUFFER, gs.buf_ships_id);
	glBufferData(GL_ARRAY_BUFFER, gs.ship_maximum_number * sizeof(ship_t),
		&gs.ship_array, GL_DYNAMIC_DRAW);

	gs.enemy_maximum_number = 0;
	gs.enemy_number = 0;
	gs.enemy_array = xcalloc(gs.enemy_maximum_number, sizeof(enemy_t));
	glGenBuffers(1, &gs.buf_enemies_id);
	glBindBuffer(GL_ARRAY_BUFFER, gs.buf_enemies_id);
	glBufferData(GL_ARRAY_BUFFER, gs.enemy_maximum_number * sizeof(enemy_t),
		gs.enemy_array, GL_DYNAMIC_DRAW);

	gs.bullet_maximum_number = 0;
	gs.bullet_number = 0;
	gs.bullet_array = xcalloc(gs.bullet_maximum_number, sizeof(bullet_t));
	glGenBuffers(1, &gs.buf_bullets_id);
	glBindBuffer(GL_ARRAY_BUFFER, gs.buf_bullets_id);
	glBufferData(GL_ARRAY_BUFFER, gs.bullet_maximum_number * sizeof(bullet_t),
		gs.bullet_array, GL_DYNAMIC_DRAW);

	gs.part_maximum_number = 0;
	gs.part_number = 0;
	gs.part_array = xcalloc(gs.part_maximum_number, sizeof(part_t));
	glGenBuffers(1, &gs.buf_parts_id);
	glBindBuffer(GL_ARRAY_BUFFER, gs.buf_parts_id);
	glBufferData(GL_ARRAY_BUFFER, gs.part_maximum_number * sizeof(part_t),
		gs.part_array, GL_DYNAMIC_DRAW);

	if (gs.settings.enabled_spawning_ships)
	{
		gs_spawn_ship(&gs);
	}

	generic_bg_data_t* bg_data = gs.settings.background->init();

	commands_t commands = {0};
	
	while (1)
	{
		apply_input_commands(&commands);

		if (commands.is_escaping || commands.is_quitting)
		{
			break;
		}

		if (commands.debug_spawn_ships && gs.ship_number == 0)
		{
			gs_spawn_ship(&gs);
		}
		if (commands.debug_spawn_enemies)
		{
			gs_spawn_enemies(&gs);
		}

		gs_perform_iter(&gs, &commands);

		gs.settings.background->iter(bg_data);

		gs.settings.background->render(bg_data);

		gs_render_ships(&gs);
		gs_render_enemies(&gs);
		gs_render_bullets(&gs);
		gs_render_particles(&gs);

		SDL_GL_SwapWindow(g_window);
	}

	gs.settings.background->cleanup(bg_data);

	free(gs.ship_array);
	glDeleteBuffers(1, &gs.buf_ships_id);
	free(gs.enemy_array);
	glDeleteBuffers(1, &gs.buf_enemies_id);
	free(gs.bullet_array);
	glDeleteBuffers(1, &gs.buf_bullets_id);
	free(gs.part_array);
	glDeleteBuffers(1, &gs.buf_parts_id);
}
