
#ifndef GRAVINYON_HEADER_INPUT_
#define GRAVINYON_HEADER_INPUT_

struct commands_t
{
	/* General commands */
	int is_escaping;
	int is_quitting; /* TODO: replace all these ints by some bitfields */

	/* Debugging commands */
	int debug_spawn_enemies;

	/* Game commands */
	int is_firing;
	float cursor_x, cursor_y;
};
typedef struct commands_t commands_t;

#endif /* GRAVINYON_HEADER_INPUT_ */
