#include "game.h"
#include "malloc.h"
#include "terminal.h"
#include "keyboard_handler.h"
#include "systemcall.h"

int start_2048()
{
	sti();
	int rows, cols;
	char buf[200];
	clear();

	printf("2048: Enter dimensions (rowscolumns), for example: 4 4\n");
	printf("max 9 rows and 9 columns:\n");
	
	while (1) {
		terminal_read(0, buf, 10);
		if (buf[0] > 48 && buf[0] < 58 && buf[2] > 48 && buf[2] < 58) {
			//printf("%c %c\n", buf[0], buf[2]);
			rows = buf[0] - 48;
			cols = buf[2] - 48;
			break;
		}
		else
			printf("please enter dimensions in the correct form");
	}
	//printf("game has been made\n");

	game * cur_game = make_game(rows, cols); //make new game entity called cur_game and insert the first random tile
	//printf("current game: %d\n", (uint32_t)cur_game);
	if ( cur_game == NULL ) {
		printf("Bad game pointer\n");
		return 2;
	}
	//printf("%lf\n",((double)malloc_usable_size(cur_game->cells))/sizeof(int));
	//printf("before rand_new tile\n");
	rand_new_tile(cur_game);
	//printf("rand_new_tile\n");
	print_game(cur_game);
	//printf("print_game\n");
	char c;
	do {
		terminal_read(0, buf, 10);
		c = buf[0]; //get character input; does not require a return keypress after input.

	} while ( process_turn(c, cur_game ) == 1); //call process_turn and check if successful


	printf("\n391OS>");
	return 0;
}
