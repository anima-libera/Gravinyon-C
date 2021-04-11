
#include "window.h"
#include "dbgmsg.h"
#include "shaders.h"
#include "embedded.h"
#include "utils.h"
#include "random.h"
#include "audio.h"

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

inline static float square_length(float x, float y)
{
	return (x * x) + (y * y);
}
inline static float length(float x, float y)
{
	return sqrtf(square_length(x, y));
}

int main(void)
{
	if (init_g_graphics() != 0)
	{
		return -1;
	}
	enable_opengl_dbgmsg();

	GLuint vao_id;
	glGenVertexArrays(1, &vao_id);
	glBindVertexArray(vao_id);

	shprog_build_all();

	if (init_g_audio() != 0)
	{
		return -1;
	}

	g_rg = rg_create_timeseeded(0);

	/* Initialize the ship */
	ship_t ship = {.x = 0.5f, .y = 0.5f, .reload_max = 5};
	GLuint buf_ship_id;
	glGenBuffers(1, &buf_ship_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_ship_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ship_t), &ship, GL_DYNAMIC_DRAW);

	/* Initialize the enemies */
	unsigned int enemy_maximum_number = 256;
	unsigned int enemy_number = 0;
	enemy_t* enemy_array = xcalloc(enemy_maximum_number, sizeof(enemy_t));
	for (unsigned int i = 0; i < 13; i++)
	{
		enemy_t* new_enemy = &enemy_array[enemy_number++];
		new_enemy->x = -0.5f;
		new_enemy->y = -0.5f;
		new_enemy->r = 0.0f;
		new_enemy->g = 1.0f;
		new_enemy->b = 1.0f;
		new_enemy->angle = rg_float(g_rg, 0.0f, TAU);
		new_enemy->speed = rg_float(g_rg, 0.001f, 0.01f);
	}
	GLuint buf_enemies_id;
	glGenBuffers(1, &buf_enemies_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_enemies_id);
	glBufferData(GL_ARRAY_BUFFER, enemy_maximum_number * sizeof(enemy_t),
		enemy_array, GL_DYNAMIC_DRAW);

	unsigned int bullet_maximum_number = 256;
	unsigned int bullet_number = 0;
	bullet_t* bullet_array = xcalloc(bullet_maximum_number, sizeof(bullet_t));

	GLuint buf_bullets_id;
	glGenBuffers(1, &buf_bullets_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_bullets_id);
	glBufferData(GL_ARRAY_BUFFER, bullet_maximum_number * sizeof(bullet_t),
		bullet_array, GL_DYNAMIC_DRAW);

	unsigned int part_maximum_number = 256;
	unsigned int part_number = 0;
	part_t* part_array = xcalloc(part_maximum_number, sizeof(part_t));

	GLuint buf_parts_id;
	glGenBuffers(1, &buf_parts_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_parts_id);
	glBufferData(GL_ARRAY_BUFFER, part_maximum_number * sizeof(part_t),
		part_array, GL_DYNAMIC_DRAW);

	float cursor_x = 0.0f;
	float cursor_y = 0.0f;
	int firing = 0;
	
	int running = 1;
	while (running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					running = 0;
				break;

				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							running = 0;
						break;

						case SDLK_e:
							for (unsigned int i = 0; i < 13; i++)
							{
								enemy_t* new_enemy = &enemy_array[enemy_number++];
								new_enemy->x = -0.5f;
								new_enemy->y = -0.5f;
								new_enemy->r = 0.0f;
								new_enemy->g = 1.0f;
								new_enemy->b = 1.0f;
								new_enemy->angle = rg_float(g_rg, 0.0f, TAU);
								new_enemy->speed = rg_float(g_rg, 0.001f, 0.01f);
							}
							glBindBuffer(GL_ARRAY_BUFFER, buf_enemies_id);
							glBufferData(GL_ARRAY_BUFFER,
								enemy_maximum_number * sizeof(enemy_t),
								enemy_array, GL_DYNAMIC_DRAW);
						break;
					}
				break;

				case SDL_MOUSEMOTION:
					cursor_x =
						((float)event.motion.x - 400.0f) / 400.0f - 1.0f;
					cursor_y =
						-((float)event.motion.y - 800.0f) / 400.0f - 1.0f;
				break;

				case SDL_MOUSEBUTTONDOWN:
					switch (event.button.button)
					{
						case SDL_BUTTON_LEFT:
							firing = 1;
						break;
					}
				break;

				case SDL_MOUSEBUTTONUP:
					switch (event.button.button)
					{
						case SDL_BUTTON_LEFT:
							firing = 0;
						break;
					}
				break;
			}
		}

		#define SHIP_COLLIDE_RADIUS 0.023
		#define ENEMY_RADIUS 0.020
		
		/* Update the ship and firing */
		{
			float vx = cosf(ship.angle) * ship.speed;
			float vy = sinf(ship.angle) * ship.speed;

			float cursor_squaredist =
				square_length(ship.x - cursor_x, ship.y - cursor_y);
			float cursor_dist = sqrtf(cursor_squaredist);

			/* Collision with the cursor */
			if (cursor_dist < SHIP_COLLIDE_RADIUS)
			{
				printf("TODO: die\n");
				play_sound(g_sound_die);
				ship.x = 0.5f;
				ship.y = 0.5f;
				ship.speed = 0.0f;
				goto ship_update_end;
			}
			
			/* Collision with enemies */
			for (unsigned int i = 0; i < enemy_number; i++)
			{
				float dist = length(
					ship.x - enemy_array[i].x, ship.y - enemy_array[i].y);
				if (dist < SHIP_COLLIDE_RADIUS + ENEMY_RADIUS)
				{
					printf("TODO: die\n");
					play_sound(g_sound_die);
					ship.x = 0.5f;
					ship.y = 0.5f;
					ship.speed = 0.0f;
					goto ship_update_end;
				}
			}

			float cursor_angle = atan2f(cursor_y - ship.y, cursor_x - ship.x);

			/* Firing mechanics */
			if (ship.reload > 0)
			{
				ship.reload--;
			}
			else if (firing)
			{
				ship.reload = ship.reload_max;

				play_sound(g_sound_pew);

				#if 0
				#define RECOIL_FACTOR 0.003f
				vx -= RECOIL_FACTOR * cosf(cursor_angle);
				vy -= RECOIL_FACTOR * sinf(cursor_angle);
				#undef RECOIL_FACTOR
				#endif

				bullet_t* new_bullet = &bullet_array[bullet_number++];
				new_bullet->x =
					ship.x + cosf(cursor_angle) * SHIP_COLLIDE_RADIUS;
				new_bullet->y =
					ship.y + sinf(cursor_angle) * SHIP_COLLIDE_RADIUS;
				new_bullet->tail_x = ship.x;
				new_bullet->tail_y = ship.y;
				new_bullet->angle = cursor_angle;
				new_bullet->speed = 0.009f / cursor_dist;
				new_bullet->safe_time = 20;
				if (bullet_number > bullet_maximum_number)
				{
					fprintf(stderr, "TODO: expand the bullet buffer\n");
				}
			}
			
			#define GRAVITY_FACTOR 0.0008f
			vx += GRAVITY_FACTOR * (cursor_x - ship.x) / cursor_dist;
			vy += GRAVITY_FACTOR * (cursor_y - ship.y) / cursor_dist;
			#undef GRAVITY_FACTOR

			ship.x += vx;
			ship.y += vy;

			/* Bounce on the edges of the world */
			#define BOUNCE_FACTOR 0.8f
			if (ship.x < -1.0f)
			{
				ship.x = -1.0f;
				vx *= -BOUNCE_FACTOR;
			}
			else if (ship.x > 1.0f)
			{
				ship.x = 1.0f;
				vx *= -BOUNCE_FACTOR;
			}
			if (ship.y < -1.0f)
			{
				ship.y = -1.0f;
				vy *= -BOUNCE_FACTOR;
			}
			else if (ship.y > 1.0f)
			{
				ship.y = 1.0f;
				vy *= -BOUNCE_FACTOR;
			}
			#undef BOUNCE_FACTOR

			ship.angle = atan2f(vy, vx);
			ship.speed = length(vx, vy);
			ship.draw_angle = cursor_angle;

			ship_update_end:;
		}
		glBindBuffer(GL_ARRAY_BUFFER, buf_ship_id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ship_t), &ship, GL_DYNAMIC_DRAW);

		/* Update the bullets */
		for (unsigned int i = 0; i < bullet_number; i++)
		{
			bullet_t* bullet = &bullet_array[i];

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
				*bullet = bullet_array[--bullet_number];
				continue;
			}

			/* Collision with enemies */
			for (unsigned int j = 0; j < enemy_number; j++)
			{
				enemy_t* enemy = &enemy_array[j];

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
							part_t* new_part = &part_array[part_number++];
							new_part->x = enemy->x;
							new_part->y = enemy->y;
							new_part->r = 1.0f;
							new_part->g = 0.0f;
							new_part->b = 0.0f;
							new_part->radius_max =
								rg_float(g_rg, 0.01f, 0.03f);
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
						*enemy = enemy_array[--enemy_number];

						/* The bullet dies */
						*bullet = bullet_array[--bullet_number];
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
					length(bullet->x - ship.x, bullet->y - ship.y);
				if (ship_dist < SHIP_COLLIDE_RADIUS)
				{
					printf("TODO: die\n");
					play_sound(g_sound_die);
					ship.x = 0.5f;
					ship.y = 0.5f;
					ship.speed = 0.0f;

					/* The bullet dies */
					*bullet = bullet_array[--bullet_number];
					continue;
				}
			}
		}
		glBindBuffer(GL_ARRAY_BUFFER, buf_bullets_id);
		glBufferData(GL_ARRAY_BUFFER, bullet_maximum_number * sizeof(bullet_t),
			bullet_array, GL_DYNAMIC_DRAW);

		/* Update the enemies */
		for (unsigned int i = 0; i < enemy_number; i++)
		{
			enemy_t* enemy = &enemy_array[i];

			float vx = cosf(enemy->angle) * enemy->speed;
			float vy = sinf(enemy->angle) * enemy->speed;

			enemy->x += vx;
			enemy->y += vy;

			if (enemy->x < -1.0f)
			{
				enemy->x = -1.0f;
				vx *= -1.0f;
			}
			else if (enemy->x > 1.0f)
			{
				enemy->x = 1.0f;
				vx *= -1.0f;
			}
			if (enemy->y < -1.0f)
			{
				enemy->y = -1.0f;
				vy *= -1.0f;
			}
			else if (enemy->y > 1.0f)
			{
				enemy->y = 1.0f;
				vy *= -1.0f;
			}

			enemy->angle = atan2f(vy, vx);
			enemy->speed = length(vx, vy);
		}
		glBindBuffer(GL_ARRAY_BUFFER, buf_enemies_id);
		glBufferData(GL_ARRAY_BUFFER, enemy_maximum_number * sizeof(enemy_t),
			enemy_array, GL_DYNAMIC_DRAW);

		#undef SHIP_COLLIDE_RADIUS
		#undef ENEMY_RADIUS

		/* Update the parts */
		for (unsigned int i = 0; i < part_number; i++)
		{
			part_t* part = &part_array[i];

			if (part->life_time == 0)
			{
				/* The particle dies */
				*part = part_array[--part_number];
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
				*part = part_array[--part_number];
				continue;
			}

			part->draw_angle += part->rotation;
		}
		glBindBuffer(GL_ARRAY_BUFFER, buf_parts_id);
		glBufferData(GL_ARRAY_BUFFER, part_maximum_number * sizeof(part_t),
			part_array, GL_DYNAMIC_DRAW);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		/* Render the ship */
		{
			#define ATTRIB_LOCATION_POS ((GLuint)0)
			#define ATTRIB_LOCATION_DRAW_ANGLE ((GLuint)1)

			glViewport(400, 0, 800, 800);
			glUseProgram(g_shprog_draw_ship);
			glEnableVertexAttribArray(ATTRIB_LOCATION_POS);
			glEnableVertexAttribArray(ATTRIB_LOCATION_DRAW_ANGLE);
			
			glBindBuffer(GL_ARRAY_BUFFER, buf_ship_id);
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
		if (enemy_number > 0)
		{
			#define ATTRIB_LOCATION_POS ((GLuint)0)
			#define ATTRIB_LOCATION_COLOR ((GLuint)1)
			#define ATTRIB_LOCATION_ANGLE ((GLuint)2)

			glViewport(400, 0, 800, 800);
			glUseProgram(g_shprog_draw_enemies);
			glEnableVertexAttribArray(ATTRIB_LOCATION_POS);
			glEnableVertexAttribArray(ATTRIB_LOCATION_COLOR);
			glEnableVertexAttribArray(ATTRIB_LOCATION_ANGLE);
			
			glBindBuffer(GL_ARRAY_BUFFER, buf_enemies_id);
			glVertexAttribPointer(ATTRIB_LOCATION_POS, 2, GL_FLOAT,
				GL_FALSE, sizeof(enemy_t), (void*)offsetof(enemy_t, x));
			glVertexAttribPointer(ATTRIB_LOCATION_COLOR, 3, GL_FLOAT,
				GL_FALSE, sizeof(enemy_t), (void*)offsetof(enemy_t, r));
			glVertexAttribPointer(ATTRIB_LOCATION_ANGLE, 1, GL_FLOAT,
				GL_FALSE, sizeof(enemy_t), (void*)offsetof(enemy_t, angle));

			glDrawArrays(GL_POINTS, 0, enemy_number);
			
			glDisableVertexAttribArray(ATTRIB_LOCATION_POS);
			glDisableVertexAttribArray(ATTRIB_LOCATION_COLOR);
			glDisableVertexAttribArray(ATTRIB_LOCATION_ANGLE);
			glUseProgram((GLuint)0);

			#undef ATTRIB_LOCATION_POS
			#undef ATTRIB_LOCATION_COLOR
			#undef ATTRIB_LOCATION_ANGLE
		}

		/* Render the bullets */
		if (bullet_number > 0)
		{
			#define ATTRIB_LOCATION_POS ((GLuint)0)
			#define ATTRIB_LOCATION_SAFE_TIME ((GLuint)1)
			#define ATTRIB_LOCATION_ANGLE ((GLuint)2)
			#define ATTRIB_LOCATION_TAIL_POS ((GLuint)3)

			glViewport(400, 0, 800, 800);
			glUseProgram(g_shprog_draw_bullets);
			glEnableVertexAttribArray(ATTRIB_LOCATION_POS);
			glEnableVertexAttribArray(ATTRIB_LOCATION_SAFE_TIME);
			glEnableVertexAttribArray(ATTRIB_LOCATION_ANGLE);
			glEnableVertexAttribArray(ATTRIB_LOCATION_TAIL_POS);
			
			glBindBuffer(GL_ARRAY_BUFFER, buf_bullets_id);
			glVertexAttribPointer(ATTRIB_LOCATION_POS, 2, GL_FLOAT,
				GL_FALSE, sizeof(bullet_t), (void*)offsetof(bullet_t, x));
			glVertexAttribPointer(ATTRIB_LOCATION_SAFE_TIME, 1, GL_UNSIGNED_INT,
				GL_FALSE, sizeof(bullet_t), (void*)offsetof(bullet_t, safe_time));
			glVertexAttribPointer(ATTRIB_LOCATION_ANGLE, 1, GL_FLOAT,
				GL_FALSE, sizeof(bullet_t), (void*)offsetof(bullet_t, angle));
			glVertexAttribPointer(ATTRIB_LOCATION_TAIL_POS, 2, GL_FLOAT,
				GL_FALSE, sizeof(bullet_t), (void*)offsetof(bullet_t, tail_x));

			glDrawArrays(GL_POINTS, 0, bullet_number);
			
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
		if (part_number > 0)
		{
			#define ATTRIB_LOCATION_POS ((GLuint)0)
			#define ATTRIB_LOCATION_COLOR ((GLuint)1)
			#define ATTRIB_LOCATION_ANGLE ((GLuint)2)
			#define ATTRIB_LOCATION_RADIUS ((GLuint)3)

			glViewport(400, 0, 800, 800);
			glUseProgram(g_shprog_draw_parts);
			glEnableVertexAttribArray(ATTRIB_LOCATION_POS);
			glEnableVertexAttribArray(ATTRIB_LOCATION_COLOR);
			glEnableVertexAttribArray(ATTRIB_LOCATION_ANGLE);
			glEnableVertexAttribArray(ATTRIB_LOCATION_RADIUS);
			
			glBindBuffer(GL_ARRAY_BUFFER, buf_parts_id);
			glVertexAttribPointer(ATTRIB_LOCATION_POS, 2, GL_FLOAT,
				GL_FALSE, sizeof(part_t), (void*)offsetof(part_t, x));
			glVertexAttribPointer(ATTRIB_LOCATION_COLOR, 3, GL_FLOAT,
				GL_FALSE, sizeof(part_t), (void*)offsetof(part_t, r));
			glVertexAttribPointer(ATTRIB_LOCATION_ANGLE, 1, GL_FLOAT,
				GL_FALSE, sizeof(part_t), (void*)offsetof(part_t, draw_angle));
			glVertexAttribPointer(ATTRIB_LOCATION_RADIUS, 1, GL_FLOAT,
				GL_FALSE, sizeof(part_t), (void*)offsetof(part_t, radius));

			glDrawArrays(GL_POINTS, 0, part_number);
			
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

		SDL_GL_SwapWindow(g_window);
	}

	rg_destroy(g_rg);
	cleanup_g_audio();
	glDeleteVertexArrays(1, &vao_id);
	shprog_destroy_all();
	cleanup_g_graphics();
	return 0;
}
