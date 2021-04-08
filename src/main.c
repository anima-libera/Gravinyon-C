
#include "window.h"
#include "dbgmsg.h"
#include "shaders.h"
#include "utils.h"

struct ship_t
{
	float x, y;
	float draw_angle;
	float angle, speed;
};
typedef struct ship_t ship_t;

struct enemy_t
{
	float x, y;
	float r, g, b;
	float angle, speed;
};
typedef struct enemy_t enemy_t;

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

	ship_t ship = {.x = 0.5f, .y = 0.5f};

	GLuint buf_ship_id;
	glGenBuffers(1, &buf_ship_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_ship_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ship_t), &ship, GL_DYNAMIC_DRAW);

	unsigned int enemy_maximum_number = 256;
	unsigned int enemy_number = 3;
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

	float cursor_x = 0.0f;
	float cursor_y = 0.0f;
	
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
			}
		}

		{
			float vx = cosf(ship.angle) * ship.speed;
			float vy = sinf(ship.angle) * ship.speed;

			float cursor_squaredist =
				square_length(ship.x - cursor_x, ship.y - cursor_y);
			float cursor_dist = sqrtf(cursor_squaredist);

			#define SHIP_RADIUS 0.023
			if (cursor_dist < SHIP_RADIUS)
			{
				printf("TODO: die\n");
				ship.x = 0.5f;
				ship.y = 0.5f;
				ship.speed = 0.0f;
				goto ship_update_end;
			}
			#define ENEMY_RADIUS 0.020
			for (unsigned int i = 0; i < enemy_number; i++)
			{
				float dist = length(
					ship.x - enemy_array[i].x, ship.y - enemy_array[i].y);
				if (dist < SHIP_RADIUS + ENEMY_RADIUS)
				{
					printf("TODO: die\n");
					ship.x = 0.5f;
					ship.y = 0.5f;
					ship.speed = 0.0f;
					goto ship_update_end;
				}
			}
			#undef ENEMY_RADIUS
			#undef SHIP_RADIUS
			
			#define GRAVITY_FACTOR 0.0008f
			vx += GRAVITY_FACTOR * (cursor_x - ship.x) / cursor_dist;
			vy += GRAVITY_FACTOR * (cursor_y - ship.y) / cursor_dist;
			#undef GRAVITY_FACTOR

			ship.x += vx;
			ship.y += vy;

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
			ship.draw_angle = atan2f(ship.y - cursor_y, ship.x - cursor_x);

			ship_update_end:;
		}
		glBindBuffer(GL_ARRAY_BUFFER, buf_ship_id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ship_t), &ship, GL_DYNAMIC_DRAW);

		for (unsigned int i = 0; i < enemy_number; i++)
		{
			float vx = cosf(enemy_array[i].angle) * enemy_array[i].speed;
			float vy = sinf(enemy_array[i].angle) * enemy_array[i].speed;

			enemy_array[i].x += vx;
			enemy_array[i].y += vy;

			if (enemy_array[i].x < -1.0f)
			{
				enemy_array[i].x = -1.0f;
				vx *= -1;
			}
			else if (enemy_array[i].x > 1.0f)
			{
				enemy_array[i].x = 1.0f;
				vx *= -1;
			}

			if (enemy_array[i].y < -1.0f)
			{
				enemy_array[i].y = -1.0f;
				vy *= -1;
			}
			else if (enemy_array[i].y > 1.0f)
			{
				enemy_array[i].y = 1.0f;
				vy *= -1;
			}

			enemy_array[i].angle = atan2f(vy, vx);
			enemy_array[i].speed = length(vx, vy);
		}
		glBindBuffer(GL_ARRAY_BUFFER, buf_enemies_id);
		glBufferData(GL_ARRAY_BUFFER, enemy_maximum_number * sizeof(enemy_t),
			enemy_array, GL_DYNAMIC_DRAW);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

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

		SDL_GL_SwapWindow(g_window);
	}

	glDeleteVertexArrays(1, &vao_id);
	cleanup_g_graphics();
	return 0;
}
