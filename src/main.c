
#include "window.h"
#include "dbgmsg.h"
#include "shaders.h"
#include "utils.h"

struct enemy_t
{
	float x, y;
	float r, g, b;
	float angle;
};
typedef struct enemy_t enemy_t;

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

	unsigned int enemy_maximum_number = 8;

	enemy_t* enemy_array = xmalloc(enemy_maximum_number * sizeof(enemy_t));
	for (unsigned int i = 0; i < enemy_maximum_number; i++)
	{
		enemy_array[i].x = (float)i / (float)enemy_maximum_number;
		enemy_array[i].y = enemy_array[i].x / 2.0f;
		enemy_array[i].r = (float)i / (float)enemy_maximum_number;
		enemy_array[i].g = 1.0f - enemy_array[i].r;
		enemy_array[i].b = 0.2f;
		enemy_array[i].angle = TAU * (float)i / (float)enemy_maximum_number;
	}

	GLuint buf_enemies_id;
	glGenBuffers(1, &buf_enemies_id);
	glBindBuffer(GL_ARRAY_BUFFER, buf_enemies_id);
	glBufferData(GL_ARRAY_BUFFER, enemy_maximum_number * sizeof(enemy_t),
		enemy_array, GL_DYNAMIC_DRAW);
	
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
			}
		}

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

			glDrawArrays(GL_POINTS, 0, enemy_maximum_number);
			
			glDisableVertexAttribArray(ATTRIB_LOCATION_POS);
			glDisableVertexAttribArray(ATTRIB_LOCATION_COLOR);
			glDisableVertexAttribArray(ATTRIB_LOCATION_ANGLE);
			glUseProgram((GLuint)0);

			#undef ATTRIB_LOCATION_POS
			#undef ATTRIB_LOCATION_COLOR
			#undef ATTRIB_LOCATION_ANGLE
			#undef ATTRIB_LOCATION_OLDPOS
		}

		SDL_GL_SwapWindow(g_window);
	}

	glDeleteVertexArrays(1, &vao_id);
	cleanup_g_graphics();
	return 0;
}
