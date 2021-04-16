
#ifndef GRAVINYON_HEADER_INPUT_
#define GRAVINYON_HEADER_INPUT_

struct commands_t
{
	/* General commands */
	int is_escaping;
	int is_quitting; /* TODO: replace all these ints by some bitfields */

	/* Debugging commands */
	int debug_spawn_enemies;

	/* Cursor position */
	int inwindow_cursor_x, inwindow_cursor_y;

	/* Game commands */
	int is_firing;
};
typedef struct commands_t commands_t;

void apply_input_commands(commands_t* commands);

#endif /* GRAVINYON_HEADER_INPUT_ */
