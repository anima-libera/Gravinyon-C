
#include "window.h"
#include "dbgmsg.h"
#include "shaders.h"
#include "utils.h"

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
	float r, g, b;
	float angle, speed;
};
typedef struct bullet_t bullet_t;

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

	/* Initialize the ship */
	ship_t ship = {.x = 0.5f, .y = 0.5f, .reload_max = 5};
	GLuint buf_ship_id;
	glGenBuffers(1, &buf_ship_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_ship_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ship_t), &ship, GL_DYNAMIC_DRAW);

	/* Initialize the enemies */
	unsigned int enemy_maximum_number = 256;
	unsigned int enemy_number = 10;
	enemy_t* enemy_array = xcalloc(enemy_maximum_number, sizeof(enemy_t));
	for (unsigned int i = 0; i < enemy_number; i++)
	{
		enemy_array[i].x = 0.0f;
		enemy_array[i].y = 0.0f;
		enemy_array[i].r = (float)i / (float)enemy_number;
		enemy_array[i].g = 1.0f - enemy_array[i].r;
		enemy_array[i].b = (float)(i % 2);
		enemy_array[i].angle = TAU * (float)i / (float)enemy_number;
		enemy_array[i].speed = 0.004f;
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

				#if 0
				#define RECOIL_FACTOR 0.003f
				vx -= RECOIL_FACTOR * cosf(cursor_angle);
				vy -= RECOIL_FACTOR * sinf(cursor_angle);
				#undef RECOIL_FACTOR
				#endif

				bullet_array[bullet_number].x =
					ship.x + cosf(cursor_angle) * SHIP_COLLIDE_RADIUS;
				bullet_array[bullet_number].y =
					ship.y + sinf(cursor_angle) * SHIP_COLLIDE_RADIUS;
				bullet_array[bullet_number].tail_x = ship.x;
				bullet_array[bullet_number].tail_y = ship.y;
				bullet_array[bullet_number].r = 1.0f;
				bullet_array[bullet_number].g = 0.0f;
				bullet_array[bullet_number].b = 0.0f;
				bullet_array[bullet_number].angle = cursor_angle;
				bullet_array[bullet_number].speed = 0.009f / cursor_dist;
				bullet_number++;
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
			bullet_array[i].tail_x =
				(bullet_array[i].x + 4.0f * bullet_array[i].tail_x) / 5.0f;
			bullet_array[i].tail_y =
				(bullet_array[i].y + 4.0f * bullet_array[i].tail_y) / 5.0f;

			float vx = cosf(bullet_array[i].angle) * bullet_array[i].speed;
			float vy = sinf(bullet_array[i].angle) * bullet_array[i].speed;

			bullet_array[i].x += vx;
			bullet_array[i].y += vy;

			/* Out of the world */
			if (bullet_array[i].tail_x < -1.0f ||
				bullet_array[i].tail_x > 1.0f ||
				bullet_array[i].tail_y < -1.0f ||
				bullet_array[i].tail_y > 1.0f)
			{
				/* The bullet dies */
				bullet_array[i] = bullet_array[--bullet_number];
				continue;
			}

			/* Collision with enemies */
			for (unsigned int j = 0; j < enemy_number; j++)
			{
				float bullet_length = length(
					bullet_array[i].x - bullet_array[i].tail_x,
					bullet_array[i].y - bullet_array[i].tail_y);
				if (bullet_length == 0.0f)
				{
					bullet_length = 0.0001f;
				}

				#define STEP 0.010
				for (float s = 0.0f; s <= bullet_length; s += STEP)
				{
					float s_unit = s / bullet_length;

					float s_x =
						bullet_array[i].tail_x * s_unit +
						bullet_array[i].x * (1.0f - s_unit);
					float s_y =
						bullet_array[i].tail_y * s_unit +
						bullet_array[i].y * (1.0f - s_unit);

					float dist = length(
						enemy_array[j].x - s_x, enemy_array[j].y - s_y);
					if (dist < ENEMY_RADIUS)
					{
						/* The enemy dies */
						enemy_array[j] = enemy_array[--enemy_number];

						/* The bullet dies */
						bullet_array[i] = bullet_array[--bullet_number];
						continue;
					}
				}
				#undef STEP
			}

			/* Collision with the ship */
			float ship_dist = length(
				bullet_array[i].x - ship.x, bullet_array[i].y - ship.y);
			if (ship_dist < SHIP_COLLIDE_RADIUS)
			{
				printf("TODO: die\n");
				ship.x = 0.5f;
				ship.y = 0.5f;
				ship.speed = 0.0f;

				/* The bullet dies */
				bullet_array[i] = bullet_array[--bullet_number];
				continue;
			}
		}
		glBindBuffer(GL_ARRAY_BUFFER, buf_bullets_id);
		glBufferData(GL_ARRAY_BUFFER, bullet_maximum_number * sizeof(bullet_t),
			bullet_array, GL_DYNAMIC_DRAW);

		/* Update the enemies */
		for (unsigned int i = 0; i < enemy_number; i++)
		{
			float vx = cosf(enemy_array[i].angle) * enemy_array[i].speed;
			float vy = sinf(enemy_array[i].angle) * enemy_array[i].speed;

			enemy_array[i].x += vx;
			enemy_array[i].y += vy;

			if (enemy_array[i].x < -1.0f)
			{
				enemy_array[i].x = -1.0f;
				vx *= -1.0f;
			}
			else if (enemy_array[i].x > 1.0f)
			{
				enemy_array[i].x = 1.0f;
				vx *= -1.0f;
			}
			if (enemy_array[i].y < -1.0f)
			{
				enemy_array[i].y = -1.0f;
				vy *= -1.0f;
			}
			else if (enemy_array[i].y > 1.0f)
			{
				enemy_array[i].y = 1.0f;
				vy *= -1.0f;
			}

			enemy_array[i].angle = atan2f(vy, vx);
			enemy_array[i].speed = length(vx, vy);
		}
		glBindBuffer(GL_ARRAY_BUFFER, buf_enemies_id);
		glBufferData(GL_ARRAY_BUFFER, enemy_maximum_number * sizeof(enemy_t),
			enemy_array, GL_DYNAMIC_DRAW);

		#undef SHIP_COLLIDE_RADIUS
		#undef ENEMY_RADIUS

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
			#define ATTRIB_LOCATION_COLOR ((GLuint)1)
			#define ATTRIB_LOCATION_ANGLE ((GLuint)2)
			#define ATTRIB_LOCATION_TAIL_POS ((GLuint)3)

			glViewport(400, 0, 800, 800);
			glUseProgram(g_shprog_draw_bullets);
			glEnableVertexAttribArray(ATTRIB_LOCATION_POS);
			glEnableVertexAttribArray(ATTRIB_LOCATION_COLOR);
			glEnableVertexAttribArray(ATTRIB_LOCATION_ANGLE);
			glEnableVertexAttribArray(ATTRIB_LOCATION_TAIL_POS);
			
			glBindBuffer(GL_ARRAY_BUFFER, buf_bullets_id);
			glVertexAttribPointer(ATTRIB_LOCATION_POS, 2, GL_FLOAT,
				GL_FALSE, sizeof(bullet_t), (void*)offsetof(bullet_t, x));
			glVertexAttribPointer(ATTRIB_LOCATION_COLOR, 3, GL_FLOAT,
				GL_FALSE, sizeof(bullet_t), (void*)offsetof(bullet_t, r));
			glVertexAttribPointer(ATTRIB_LOCATION_ANGLE, 1, GL_FLOAT,
				GL_FALSE, sizeof(bullet_t), (void*)offsetof(bullet_t, angle));
			glVertexAttribPointer(ATTRIB_LOCATION_TAIL_POS, 2, GL_FLOAT,
				GL_FALSE, sizeof(bullet_t), (void*)offsetof(bullet_t, tail_x));

			glDrawArrays(GL_POINTS, 0, bullet_number);
			
			glDisableVertexAttribArray(ATTRIB_LOCATION_POS);
			glDisableVertexAttribArray(ATTRIB_LOCATION_COLOR);
			glDisableVertexAttribArray(ATTRIB_LOCATION_ANGLE);
			glDisableVertexAttribArray(ATTRIB_LOCATION_TAIL_POS);
			glUseProgram((GLuint)0);

			#undef ATTRIB_LOCATION_POS
			#undef ATTRIB_LOCATION_COLOR
			#undef ATTRIB_LOCATION_ANGLE
			#undef ATTRIB_LOCATION_TAIL_POS
		}

		SDL_GL_SwapWindow(g_window);
	}

	glDeleteVertexArrays(1, &vao_id);
	cleanup_g_graphics();
	return 0;
}
