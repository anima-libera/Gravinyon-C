
#include "game.h"
#include "utils.h"
#include "random.h"
#include "audio.h"
#include "shaders.h"

inline static float square_length(float x, float y)
{
	return (x * x) + (y * y);
}

inline static float length(float x, float y)
{
	return sqrtf(square_length(x, y));
}

void gs_init(gs_t* gs)
{
	/* Ship */
	gs->ship.x = 0.5f;
	gs->ship.y = 0.5f;
	gs->ship.reload_max = 5;
	glGenBuffers(1, &gs->buf_ship_id);
	glBindBuffer(GL_ARRAY_BUFFER, gs->buf_ship_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ship_t), &gs->ship, GL_DYNAMIC_DRAW);

	/* Enemies */
	gs->enemy_maximum_number = 256;
	gs->enemy_number = 0;
	gs->enemy_array = xcalloc(gs->enemy_maximum_number, sizeof(enemy_t));
	gs_spawn_enemies(gs);
	glGenBuffers(1, &gs->buf_enemies_id);
	glBindBuffer(GL_ARRAY_BUFFER, gs->buf_enemies_id);
	glBufferData(GL_ARRAY_BUFFER, gs->enemy_maximum_number * sizeof(enemy_t),
		gs->enemy_array, GL_DYNAMIC_DRAW);

	/* Bullets */
	gs->bullet_maximum_number = 256;
	gs->bullet_number = 0;
	gs->bullet_array = xcalloc(gs->bullet_maximum_number, sizeof(bullet_t));
	glGenBuffers(1, &gs->buf_bullets_id);
	glBindBuffer(GL_ARRAY_BUFFER, gs->buf_bullets_id);
	glBufferData(GL_ARRAY_BUFFER, gs->bullet_maximum_number * sizeof(bullet_t),
		gs->bullet_array, GL_DYNAMIC_DRAW);

	/* Particles */
	gs->part_maximum_number = 256;
	gs->part_number = 0;
	gs->part_array = xcalloc(gs->part_maximum_number, sizeof(part_t));
	glGenBuffers(1, &gs->buf_parts_id);
	glBindBuffer(GL_ARRAY_BUFFER, gs->buf_parts_id);
	glBufferData(GL_ARRAY_BUFFER, gs->part_maximum_number * sizeof(part_t),
		gs->part_array, GL_DYNAMIC_DRAW);
}

void gs_cleanup(gs_t* gs)
{
	free(gs->enemy_array);
	free(gs->bullet_array);
	free(gs->part_array);
	glDeleteBuffers(1, &gs->buf_ship_id);
	glDeleteBuffers(1, &gs->buf_enemies_id);
	glDeleteBuffers(1, &gs->buf_bullets_id);
	glDeleteBuffers(1, &gs->buf_parts_id);
}

enemy_t* gs_alloc_enemy(gs_t* gs)
{
	DYNAMIC_VECTOR_RESIZE_IF_NEEDED(
		gs->enemy_number, gs->enemy_maximum_number, gs->enemy_array, enemy_t);
	enemy_t* new_enemy = &gs->enemy_array[gs->enemy_number++];
	return new_enemy;
}

bullet_t* gs_alloc_bullet(gs_t* gs)
{
	DYNAMIC_VECTOR_RESIZE_IF_NEEDED(
		gs->bullet_number, gs->bullet_maximum_number, gs->bullet_array, bullet_t);
	bullet_t* new_bullet = &gs->bullet_array[gs->bullet_number++];
	return new_bullet;
}

part_t* gs_alloc_part(gs_t* gs)
{
	DYNAMIC_VECTOR_RESIZE_IF_NEEDED(
		gs->part_number, gs->part_maximum_number, gs->part_array, part_t);
	part_t* new_part = &gs->part_array[gs->part_number++];
	return new_part;
}

void gs_spawn_enemies(gs_t* gs)
{
	for (unsigned int i = 0; i < 4; i++)
	{
		enemy_t* new_enemy = gs_alloc_enemy(gs);
		new_enemy->x = -0.5f;
		new_enemy->y = -0.5f;
		new_enemy->r = 0.0f;
		new_enemy->g = 1.0f;
		new_enemy->b = 1.0f;
		new_enemy->angle = rg_float(g_rg, 0.0f, TAU);
		new_enemy->speed = rg_float(g_rg, 0.001f, 0.015f);
	}
}

void gs_perform_iter(gs_t* gs, commands_t* commands)
{
	float ingame_cursor_x =
		2.0f * (
			(float)commands->inwindow_cursor_x - (float)GAME_VIEWPORT_LEFT
		) / (float)GAME_VIEWPORT_WIDTH - 1.0f;
	float ingame_cursor_y =
		(
			2.0f * (
				(float)GAME_VIEWPORT_TOP - (float)commands->inwindow_cursor_y
			) + GAME_VIEWPORT_HEIGHT
		) / (float)GAME_VIEWPORT_WIDTH;

	#define INGAME_LEFT (-1.0f)
	#define INGAME_RIGHT (1.0f)
	#define INGAME_TOP (1.0f / GAME_ASPECT_RATIO)
	#define INGAME_BOTTOM (-1.0f / GAME_ASPECT_RATIO)

	#define SHIP_COLLIDE_RADIUS 0.013
	#define ENEMY_RADIUS 0.012
	
	/* Update the ship and firing */
	{
		float vx = cosf(gs->ship.angle) * gs->ship.speed;
		float vy = sinf(gs->ship.angle) * gs->ship.speed;

		float cursor_squaredist =
			square_length(gs->ship.x - ingame_cursor_x, gs->ship.y - ingame_cursor_y);
		float cursor_dist = sqrtf(cursor_squaredist);

		/* Collision with the cursor */
		if (cursor_dist < SHIP_COLLIDE_RADIUS)
		{
			printf("TODO: die\n");
			play_sound(g_sound_die);
			unsigned int count = rg_uint(g_rg, 50, 70);
			for (unsigned int k = 0; k < count; k++)
			{
				part_t* new_part = gs_alloc_part(gs);
				new_part->x = gs->ship.x;
				new_part->y = gs->ship.y;
				new_part->r = 1.0f;
				new_part->g = 1.0f;
				new_part->b = 1.0f;
				new_part->radius_max =
					rg_float(g_rg, 0.005f, 0.018f);
				new_part->radius = new_part->radius_max;
				new_part->draw_angle =
					rg_float(g_rg, 0.0f, TAU);
				new_part->angle =
					gs->ship.angle + rg_float(g_rg, -0.4f, 0.4f);
				new_part->speed =
					gs->ship.speed * rg_float(g_rg, 0.2f, 1.3f);
				new_part->rotation =
					rg_float(g_rg, -0.03f, 0.03f);
				new_part->life_time_max =
					rg_uint(g_rg, 20, 70);
				new_part->life_time = new_part->life_time_max;
			}
			gs->ship.x = 0.5f;
			gs->ship.y = 0.5f;
			gs->ship.speed = 0.0f;
			goto ship_update_end;
		}
		
		/* Collision with enemies */
		for (unsigned int i = 0; i < gs->enemy_number; i++)
		{
			float dist = length(
				gs->ship.x - gs->enemy_array[i].x, gs->ship.y - gs->enemy_array[i].y);
			if (dist < SHIP_COLLIDE_RADIUS + ENEMY_RADIUS)
			{
				printf("TODO: die\n");
				play_sound(g_sound_die);
				unsigned int count = rg_uint(g_rg, 50, 70);
				for (unsigned int k = 0; k < count; k++)
				{
					part_t* new_part = gs_alloc_part(gs);
					new_part->x = gs->ship.x;
					new_part->y = gs->ship.y;
					new_part->r = 1.0f;
					new_part->g = 1.0f;
					new_part->b = 1.0f;
					new_part->radius_max =
						rg_float(g_rg, 0.005f, 0.018f);
					new_part->radius = new_part->radius_max;
					new_part->draw_angle =
						rg_float(g_rg, 0.0f, TAU);
					new_part->angle =
						gs->ship.angle + rg_float(g_rg, -0.4f, 0.4f);
					new_part->speed =
						gs->ship.speed * rg_float(g_rg, 0.2f, 1.3f);
					new_part->rotation =
						rg_float(g_rg, -0.03f, 0.03f);
					new_part->life_time_max =
						rg_uint(g_rg, 20, 70);
					new_part->life_time = new_part->life_time_max;
				}
				gs->ship.x = 0.5f;
				gs->ship.y = 0.5f;
				gs->ship.speed = 0.0f;
				goto ship_update_end;
			}
		}

		float cursor_angle = atan2f(ingame_cursor_y - gs->ship.y, ingame_cursor_x - gs->ship.x);

		/* Firing mechanics */
		if (gs->ship.reload > 0)
		{
			gs->ship.reload--;
		}
		else if (commands->is_firing)
		{
			gs->ship.reload = gs->ship.reload_max;

			play_sound(g_sound_pew);

			#if 0
			#define RECOIL_FACTOR 0.003f
			vx -= RECOIL_FACTOR * cosf(cursor_angle);
			vy -= RECOIL_FACTOR * sinf(cursor_angle);
			#undef RECOIL_FACTOR
			#endif

			bullet_t* new_bullet = gs_alloc_bullet(gs);
			new_bullet->x =
				gs->ship.x + cosf(cursor_angle) * SHIP_COLLIDE_RADIUS;
			new_bullet->y =
				gs->ship.y + sinf(cursor_angle) * SHIP_COLLIDE_RADIUS;
			new_bullet->tail_x = gs->ship.x;
			new_bullet->tail_y = gs->ship.y;
			new_bullet->angle = cursor_angle;
			new_bullet->speed = 0.009f / cursor_dist;
			new_bullet->safe_time = 20;
			if (gs->bullet_number > gs->bullet_maximum_number)
			{
				fprintf(stderr, "TODO: expand the bullet buffer\n");
			}
		}
		
		#define GRAVITY_FACTOR 0.0008f
		vx += GRAVITY_FACTOR * (ingame_cursor_x - gs->ship.x) / cursor_dist;
		vy += GRAVITY_FACTOR * (ingame_cursor_y - gs->ship.y) / cursor_dist;
		#undef GRAVITY_FACTOR

		gs->ship.x += vx;
		gs->ship.y += vy;

		/* Bounce on the edges of the world */
		#define BOUNCE_FACTOR 0.8f
		if (gs->ship.x < INGAME_LEFT)
		{
			gs->ship.x = INGAME_LEFT;
			vx *= -BOUNCE_FACTOR;
		}
		else if (gs->ship.x > INGAME_RIGHT)
		{
			gs->ship.x = INGAME_RIGHT;
			vx *= -BOUNCE_FACTOR;
		}
		if (gs->ship.y < INGAME_BOTTOM)
		{
			gs->ship.y = INGAME_BOTTOM;
			vy *= -BOUNCE_FACTOR;
		}
		else if (gs->ship.y > INGAME_TOP)
		{
			gs->ship.y = INGAME_TOP;
			vy *= -BOUNCE_FACTOR;
		}
		#undef BOUNCE_FACTOR

		gs->ship.angle = atan2f(vy, vx);
		gs->ship.speed = length(vx, vy);
		gs->ship.draw_angle = cursor_angle;

		ship_update_end:;
	}
	glBindBuffer(GL_ARRAY_BUFFER, gs->buf_ship_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ship_t), &gs->ship, GL_DYNAMIC_DRAW);

	/* Update the bullets */
	for (unsigned int i = 0; i < gs->bullet_number; i++)
	{
		bullet_t* bullet = &gs->bullet_array[i];

		bullet->tail_x = (bullet->x + 4.0f * bullet->tail_x) / 5.0f;
		bullet->tail_y = (bullet->y + 4.0f * bullet->tail_y) / 5.0f;

		float vx = cosf(bullet->angle) * bullet->speed;
		float vy = sinf(bullet->angle) * bullet->speed;

		bullet->x += vx;
		bullet->y += vy;

		/* Out of the world */
		if (bullet->tail_x < -1.0f || bullet->tail_x > 1.0f ||
			bullet->tail_y < -1.0f || bullet->tail_y > 1.0f)
		{
			/* The bullet dies */
			*bullet = gs->bullet_array[--gs->bullet_number];
			continue;
		}

		/* Collision with enemies */
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

					/* Spaw particles */
					unsigned int count = rg_uint(g_rg, 10, 30);
					for (unsigned int k = 0; k < count; k++)
					{
						part_t* new_part = gs_alloc_part(gs);
						new_part->x = enemy->x;
						new_part->y = enemy->y;
						new_part->r = 1.0f;
						new_part->g = 1.0f;
						new_part->b = 1.0f;
						new_part->radius_max =
							rg_float(g_rg, 0.005f, 0.018f);
						new_part->radius = new_part->radius_max;
						new_part->draw_angle =
							rg_float(g_rg, 0.0f, TAU);
						new_part->angle =
							bullet->angle + rg_float(g_rg, -0.4f, 0.4f);
						new_part->speed =
							bullet->speed * rg_float(g_rg, 0.2f, 1.3f);
						new_part->rotation =
							rg_float(g_rg, -0.03f, 0.03f);
						new_part->life_time_max =
							rg_uint(g_rg, 20, 70);
						new_part->life_time = new_part->life_time_max;
					}

					/* The enemy dies */
					*enemy = gs->enemy_array[--gs->enemy_number];

					/* The bullet dies */
					*bullet = gs->bullet_array[--gs->bullet_number];
					goto continue_bullets;
				}
			}
			#undef STEP
		}
		if (0)
		{
			/* Warning: Super high quality code, do not stare for too
			 * long without eye protection. */
			continue_bullets: continue;
		}

		/* Collision with the ship */
		if (bullet->safe_time > 0)
		{
			bullet->safe_time--;
		}
		else
		{
			float ship_dist =
				length(bullet->x - gs->ship.x, bullet->y - gs->ship.y);
			if (ship_dist < SHIP_COLLIDE_RADIUS)
			{
				printf("TODO: die\n");
				play_sound(g_sound_die);
				unsigned int count = rg_uint(g_rg, 50, 70);
				for (unsigned int k = 0; k < count; k++)
				{
					part_t* new_part = gs_alloc_part(gs);
					new_part->x = gs->ship.x;
					new_part->y = gs->ship.y;
					new_part->r = 1.0f;
					new_part->g = 1.0f;
					new_part->b = 1.0f;
					new_part->radius_max =
						rg_float(g_rg, 0.005f, 0.018f);
					new_part->radius = new_part->radius_max;
					new_part->draw_angle =
						rg_float(g_rg, 0.0f, TAU);
					new_part->angle =
						gs->ship.angle + rg_float(g_rg, -0.4f, 0.4f);
					new_part->speed =
						gs->ship.speed * rg_float(g_rg, 0.2f, 1.3f);
					new_part->rotation =
						rg_float(g_rg, -0.03f, 0.03f);
					new_part->life_time_max =
						rg_uint(g_rg, 20, 70);
					new_part->life_time = new_part->life_time_max;
				}
				gs->ship.x = 0.5f;
				gs->ship.y = 0.5f;
				gs->ship.speed = 0.0f;

				/* The bullet dies */
				*bullet = gs->bullet_array[--gs->bullet_number];
				continue;
			}
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, gs->buf_bullets_id);
	glBufferData(GL_ARRAY_BUFFER, gs->bullet_maximum_number * sizeof(bullet_t),
		gs->bullet_array, GL_DYNAMIC_DRAW);

	/* Update the enemies */
	for (unsigned int i = 0; i < gs->enemy_number; i++)
	{
		enemy_t* enemy = &gs->enemy_array[i];

		float vx = cosf(enemy->angle) * enemy->speed;
		float vy = sinf(enemy->angle) * enemy->speed;

		enemy->x += vx;
		enemy->y += vy;

		if (enemy->x < INGAME_LEFT)
		{
			enemy->x = INGAME_LEFT;
			vx *= -1.0f;
		}
		else if (enemy->x > INGAME_RIGHT)
		{
			enemy->x = INGAME_RIGHT;
			vx *= -1.0f;
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
	glBindBuffer(GL_ARRAY_BUFFER, gs->buf_enemies_id);
	glBufferData(GL_ARRAY_BUFFER, gs->enemy_maximum_number * sizeof(enemy_t),
		gs->enemy_array, GL_DYNAMIC_DRAW);

	#undef SHIP_COLLIDE_RADIUS
	#undef ENEMY_RADIUS

	/* Update the parts */
	for (unsigned int i = 0; i < gs->part_number; i++)
	{
		part_t* part = &gs->part_array[i];

		if (part->life_time == 0)
		{
			/* The particle dies */
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

		/* Out of the world */
		if (part->x < -1.0f - part->radius ||
			part->x > 1.0f + part->radius ||
			part->y < -1.0f - part->radius ||
			part->y > 1.0f + part->radius)
		{
			/* The particle dies */
			*part = gs->part_array[--gs->part_number];
			continue;
		}

		part->draw_angle += part->rotation;
	}
	glBindBuffer(GL_ARRAY_BUFFER, gs->buf_parts_id);
	glBufferData(GL_ARRAY_BUFFER, gs->part_maximum_number * sizeof(part_t),
		gs->part_array, GL_DYNAMIC_DRAW);
}

void gs_render(gs_t* gs)
{
	#if 0
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	#endif

	/* Render the ship */
	{
		#define ATTRIB_LOCATION_POS ((GLuint)0)
		#define ATTRIB_LOCATION_DRAW_ANGLE ((GLuint)1)

		glViewport(
			GAME_VIEWPORT_LEFT, GAME_VIEWPORT_TOP,
			GAME_VIEWPORT_WIDTH, GAME_VIEWPORT_HEIGHT);
		glUseProgram(g_shprog_draw_ship);
		glEnableVertexAttribArray(ATTRIB_LOCATION_POS);
		glEnableVertexAttribArray(ATTRIB_LOCATION_DRAW_ANGLE);
		
		glBindBuffer(GL_ARRAY_BUFFER, gs->buf_ship_id);
		glVertexAttribPointer(ATTRIB_LOCATION_POS, 2, GL_FLOAT,
			GL_FALSE, sizeof(ship_t), (void*)offsetof(ship_t, x));
		glVertexAttribPointer(ATTRIB_LOCATION_DRAW_ANGLE, 1, GL_FLOAT,
			GL_FALSE, sizeof(ship_t), (void*)offsetof(ship_t, draw_angle));

		glDrawArrays(GL_POINTS, 0, 1);
		
		glDisableVertexAttribArray(ATTRIB_LOCATION_POS);
		glDisableVertexAttribArray(ATTRIB_LOCATION_DRAW_ANGLE);
		glUseProgram((GLuint)0);

		#undef ATTRIB_LOCATION_POS
		#undef ATTRIB_LOCATION_DRAW_ANGLE
	}

	/* Render the enemies */
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

	/* Render the bullets */
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
		glVertexAttribPointer(ATTRIB_LOCATION_SAFE_TIME, 1, GL_UNSIGNED_INT,
			GL_FALSE, sizeof(bullet_t), (void*)offsetof(bullet_t, safe_time));
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

	/* Render the particles */
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
