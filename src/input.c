
#include "input.h"
#include "window.h"

void apply_input_commands(commands_t* commands)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				commands->is_quitting = 1;
			break;

			case SDL_KEYDOWN:
			case SDL_KEYUP:
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						commands->is_escaping = event.type == SDL_KEYDOWN;
					break;

					case SDLK_e:
						commands->debug_spawn_enemies = event.type == SDL_KEYDOWN;
					break;
				}
			break;

			case SDL_MOUSEMOTION:
				commands->inwindow_cursor_x = event.motion.x;
				commands->inwindow_cursor_y = event.motion.y;
			break;

			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
					case SDL_BUTTON_LEFT:
						commands->is_firing = 1;
					break;
				}
			break;

			case SDL_MOUSEBUTTONUP:
				switch (event.button.button)
				{
					case SDL_BUTTON_LEFT:
						commands->is_firing = 0;
					break;
				}
			break;
		}
	}
}
