#include <stdint.h>

#include "ece391support.h"
#include "ece391syscall.h"
#include "ece391game.h"

int index_random = 0;

game * make_game(int rows, int cols)
{
	int i;
	//printf("make_game\n");
	game * newgame = (game*)ece391_malloc(sizeof(game)); // allocate for game struct first
	//printf("ece391_malloc\n");
	//printf("new game: %x\n", (uint32_t)newgame);
	if (newgame == (game*)0) return (game*)0;
	newgame->rows = rows;
	newgame->cols = cols; // initialize values
	newgame->cells = (int*)ece391_malloc((uint32_t)(rows * cols * sizeof(int))); // allocate dynamic array for cells
	//printf("cells memory: %x\n", (uint32_t)newgame->cells);
	if (newgame->cells == (cell*)0) return (cell*)0;
	for (i = 0; i < rows * cols; i++) {
		//printf("each cell: %x\n", (uint32_t)(newgame->cells + i));
		*(newgame->cells + i) = -1; //initialize the dynamic array
	}
	//printf("finished initialization\n");
	newgame->score = 0;
	return newgame; // return the pointer to the new game
}



void destroy_game(game * cur_game)
{
	ece391_free((uint32_t*)(cur_game->cells)); // free cells first
	ece391_free((uint32_t*)(cur_game)); // free the game struct
	return;
}

cell * get_cell(game * cur_game, int row, int col)
{
	if (row < 0 || row >= cur_game->rows || col < 0 || col >= cur_game->cols) return (cell*)0;
	return (cur_game->cells + row * (cur_game->cols) + col); // convert the 2D index into 1D and return the pointer
}

int move_w(game * cur_game)
{
	int i, r, c;
	int rows = cur_game->rows;
	int cols = cur_game->cols;
	int moved = 0;
	//swap up all the cell
	for (c = 0; c < cols; c++) {
		for (i = 0; i < rows - 1; i++) {
			for (r = 0; r < rows - 1; r++) {
				if (*get_cell(cur_game, r, c) == -1) {
					if (*get_cell(cur_game, r + 1, c) != -1) {
						*get_cell(cur_game, r, c) = *get_cell(cur_game, r + 1, c);
						*get_cell(cur_game, r + 1, c) = -1;
						moved = 1; // record a move
					}
				}
			}
		}
	}
	//merge
	for (c = 0; c < cols; c++) {
		for (r = 0; r < rows - 1; r++) {
			if (*get_cell(cur_game, r, c) == *get_cell(cur_game, r + 1, c) && *get_cell(cur_game, r, c) != -1) {
				*get_cell(cur_game, r, c) = *get_cell(cur_game, r, c) * 2;
				*get_cell(cur_game, r + 1, c) = -1;
				cur_game->score += *get_cell(cur_game, r, c); // increment the score
				moved = 1; // record a move
			}
		}
	}
	//swap up again after the merge
	if (moved) {
		for (c = 0; c < cols; c++) {
			for (i = 0; i < rows - 1; i++) {
				for (r = 0; r < rows - 1; r++) {
					if (*get_cell(cur_game, r, c) == -1) {
						if (*get_cell(cur_game, r + 1, c) != -1) {
							*get_cell(cur_game, r, c) = *get_cell(cur_game, r + 1, c);
							*get_cell(cur_game, r + 1, c) = -1;
							moved = 1;
						}
					}
				}
			}
		}
	}
	if (moved)
		return 1;
	else return 0;
};

int move_s(game * cur_game) //slide down
{

	int i, r, c;
	int rows = cur_game->rows;
	int cols = cur_game->cols;
	int moved = 0;
	//swap down all the cells
	for (c = 0; c < cols; c++) {
		for (i = 0; i < rows - 1; i++) {
			for (r = 0; r < rows - 1; r++) {
				if (*get_cell(cur_game, r + 1, c) == -1) {
					if (*get_cell(cur_game, r, c) != -1) {
						*get_cell(cur_game, r + 1, c) = *get_cell(cur_game, r, c);
						*get_cell(cur_game, r, c) = -1;
						moved = 1; //record the move
					}
				}
			}
		}
	}
	//merge
	for (c = 0; c < cols; c++) {
		for (r = rows - 2; r > -1; r--) {
			if (*get_cell(cur_game, r, c) == *get_cell(cur_game, r + 1, c) && *get_cell(cur_game, r, c) != -1) {
				*get_cell(cur_game, r + 1, c) = *get_cell(cur_game, r, c) * 2;
				*get_cell(cur_game, r, c) = -1;
				cur_game->score += *get_cell(cur_game, r + 1, c); // increment the score
				moved = 1; // record a move
			}
		}
	}
	//swap down again
	if (moved) {
		for (c = 0; c < cols; c++) {
			for (i = 0; i < rows - 1; i++) {
				for (r = 0; r < rows - 1; r++) {
					if (*get_cell(cur_game, r + 1, c) == -1) {
						if (*get_cell(cur_game, r, c) != -1) {
							*get_cell(cur_game, r + 1, c) = *get_cell(cur_game, r, c);
							*get_cell(cur_game, r, c) = -1;
						}
					}
				}
			}
		}
	}
	if (moved)
		return 1;
	else return 0;
};

int move_a(game * cur_game) //slide left
{
	int i, r, c;
	int rows = cur_game->rows;
	int cols = cur_game->cols;
	int temp;
	int moved = 0;
	//swap left all the cells
	for (r = 0; r < rows; r++) {
		for (i = 0; i < cols - 1; i++) {
			for (c = 0; c < cols - 1; c++) {
				if (*get_cell(cur_game, r, c) == -1) {
					if (*get_cell(cur_game, r, c + 1) != -1) {
						temp = *get_cell(cur_game, r, c);
						*get_cell(cur_game, r, c) = *get_cell(cur_game, r, c + 1);
						*get_cell(cur_game, r, c + 1) = temp;
						moved = 1; // record a move
					}
				}
			}
		}
	}
	//merge
	for (r = 0; r < rows; r++) {
		for (c = 0; c < cols - 1; c++) {
			if (*get_cell(cur_game, r, c) == *get_cell(cur_game, r, c + 1) && *get_cell(cur_game, r, c) != -1) {
				*get_cell(cur_game, r, c) = *get_cell(cur_game, r, c) * 2;
				*get_cell(cur_game, r, c + 1) = -1;
				cur_game->score += *get_cell(cur_game, r, c); // increment the score
				moved = 1; // record the move
			}
		}
	}
	//swap again
	if (moved) {
		for (r = 0; r < rows; r++) {
			for (i = 0; i < cols - 1; i++) {
				for (c = 0; c < cols - 1; c++) {
					if (*get_cell(cur_game, r, c) == -1) {
						if (*get_cell(cur_game, r, c + 1) != -1) {
							temp = *get_cell(cur_game, r, c);
							*get_cell(cur_game, r, c) = *get_cell(cur_game, r, c + 1);
							*get_cell(cur_game, r, c + 1) = temp;
						}
					}
				}
			}
		}
	}
	if (moved)
		return 1;
	else return 0;
};

int move_d(game * cur_game) { //slide to the right
	int i, r, c;
	int rows = cur_game->rows;
	int cols = cur_game->cols;
	int temp;
	int moved = 0;
	//swap right all the cells
	for (r = 0; r < rows; r++) {
		for (i = 0; i < cols - 1; i++) {
			for (c = 0; c < cols - 1; c++) {
				if (*get_cell(cur_game, r, c + 1) == -1) {
					if (*get_cell(cur_game, r, c) != -1) {
						temp = *get_cell(cur_game, r, c);
						*get_cell(cur_game, r, c) = *get_cell(cur_game, r, c + 1);
						*get_cell(cur_game, r, c + 1) = temp;
						moved = 1; // record a move
					}
				}
			}
		}
	}
	//merge
	for (r = 0; r < rows; r++) {
		for (c = cols - 2; c > -1; c--) {
			if (*get_cell(cur_game, r, c) == *get_cell(cur_game, r, c + 1) && *get_cell(cur_game, r, c + 1) != -1) {
				*get_cell(cur_game, r, c + 1) = *get_cell(cur_game, r, c + 1) * 2;
				*get_cell(cur_game, r, c) = -1;
				cur_game->score += *get_cell(cur_game, r, c + 1); // increment the score
				moved = 1; // record a move
			}
		}
	}
	//swap again
	if (moved) {
		for (r = 0; r < rows; r++) {
			for (i = 0; i < cols - 1; i++) {
				for (c = 0; c < cols - 1; c++) {
					if (*get_cell(cur_game, r, c + 1) == -1) {
						if (*get_cell(cur_game, r, c) != -1) {
							temp = *get_cell(cur_game, r, c);
							*get_cell(cur_game, r, c) = *get_cell(cur_game, r, c + 1);
							*get_cell(cur_game, r, c + 1) = temp;
						}
					}
				}
			}
		}
	}
	if (moved)
		return 1;
	else return 0;
};

int legal_move_check(game * cur_game)
{
	int rows = cur_game->rows; int cols = cur_game->cols;
	int i, r, c;
	//if any -1 exists, possible legal move exists
	for (i = 0; i < rows * cols; i++) {
		if (*(cur_game->cells + i) == -1) return 1;
	}
	//check available merge in horizontal direction
	for (c = 0; c < cols; c++) {
		for (r = 0; r < rows - 1; r++) {
			if (*get_cell(cur_game, r, c) == *get_cell(cur_game, r + 1, c)) return 1;
		}
	}
	//check available merge in vertical direction
	for (r = 0; r < rows; r++) {
		for (c = 0; c < cols - 1; c++) {
			if (*get_cell(cur_game, r, c) == *get_cell(cur_game, r, c + 1)) return 1;
		}
	}
	//printf("illegal move\n");
	return 0;
}


void remake_game(game ** _cur_game_ptr, int new_rows, int new_cols)
{
	//clear the memory
	destroy_game(* _cur_game_ptr);
	//make a new one
	*_cur_game_ptr = make_game(new_rows, new_cols);
	return;
}

/*! code below is provided and should not be changed */

void rand_new_tile(game * cur_game)
/*! insert a new tile into a random empty cell. First call rand()%(rows*cols) to get a random value between 0 and (rows*cols)-1.
*/
{

	cell * cell_ptr;
	cell_ptr = 	cur_game->cells;

	if (cell_ptr == (cell*) 0) {
		ece391_fdputs (1, (uint8_t*)"Bad Cell Pointer.\n");
		//exit(0);
	}


	//check for an empty cell
	int emptycheck = 0;
	int i;

	for (i = 0; i < ((cur_game->rows) * (cur_game->cols)); i++) {
		if ((*cell_ptr) == -1) {
			emptycheck = 1;
			break;
		}
		cell_ptr += 1;
	}
	if (emptycheck == 0) {
		ece391_fdputs (1, (uint8_t*)"Error: Trying to insert into no a board with no empty cell. The function rand_new_tile() should only be called after tiles have succesfully moved, meaning there should be at least 1 open spot.\n");
		//exit(0);
	}
	int random_numbers[10] = {199, 123, 213, 333, 4123, 3213, 3332, 978, 404, 243};
	int ind, row, col;
	int num;
	do {
		index_random++;
		index_random = index_random % 10;
		ind = random_numbers[index_random] % ((cur_game->rows) * (cur_game->cols));
		col = ind % (cur_game->cols);
		row = ind / cur_game->cols;
	} while ( *get_cell(cur_game, row, col) != -1);
	//*get_cell(cur_game, row, col) = 2;
	num = random_numbers[index_random] % 20;
	if (num <= 1) {
		*get_cell(cur_game, row, col) = 4; // 1/10th chance
	}
	else {
		*get_cell(cur_game, row, col) = 2;// 9/10th chance
	}
}

char* convert_number(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}

int print_game(game * cur_game)
{
	cell * cell_ptr;
	cell_ptr = 	cur_game->cells;
	char number_string[10];
	int rows = cur_game->rows;
	int cols = cur_game->cols;
	int i, j;
	convert_number(cur_game->score, number_string);
	ece391_fdputs (1, (uint8_t*)"\n\n\nscore:");
	ece391_fdputs( 1, number_string);
	ece391_fdputs(1, "\n");


	ece391_fdputs (1, (uint8_t*)"|"); // topleft box char
	for (i = 0; i < cols * 5; i++)
		ece391_fdputs (1, (uint8_t*)"-"); // top box char
	ece391_fdputs (1, (uint8_t*)"|\n"); //top right char


	for (i = 0; i < rows; i++) {
		ece391_fdputs (1, (uint8_t*)"|"); // side box char
		for (j = 0; j < cols; j++) {
			if ((*cell_ptr) == -1 ) { //print asterisks
				ece391_fdputs (1, (uint8_t*)" **  ");
			}
			else {
				switch ( *cell_ptr ) { //print colored text
				case 2:
					ece391_fdputs (1, (uint8_t*)"  2  ");
					break;
				case 4:
					ece391_fdputs (1, (uint8_t*)"  4  ");
					break;
				case 8:
					ece391_fdputs (1, (uint8_t*)"  8  ");
					break;
				case 16:
					ece391_fdputs (1, (uint8_t*)"  16 ");
					break;
				case 32:
					ece391_fdputs (1, (uint8_t*)"  32 ");
					break;
				case 64:
					ece391_fdputs (1, (uint8_t*)"  64 ");
					break;
				case 128:
					ece391_fdputs (1, (uint8_t*)"  128");
					break;
				case 256:
					ece391_fdputs (1, (uint8_t*)"  256");
					break;
				case 512:
					ece391_fdputs (1, (uint8_t*)"  512");
					break;
				case 1024:
					ece391_fdputs (1, (uint8_t*)" 1024");
					break;
				case 2048:
					ece391_fdputs (1, (uint8_t*)" 2048");
					break;
				case 4096:
					ece391_fdputs (1, (uint8_t*)" 4096");
					break;
				case 8192:
					ece391_fdputs (1, (uint8_t*)" 8192");
					break;
				default:
					ece391_fdputs (1, (uint8_t*)"  X  ");

				}

			}
			cell_ptr++;
		}
		ece391_fdputs (1, (uint8_t*)"|\n"); //print right wall and newline
	}

	ece391_fdputs (1, (uint8_t*)"-"); // print bottom left char
	for (i = 0; i < cols * 5; i++)
		ece391_fdputs (1, (uint8_t*)"-"); // bottom char
	ece391_fdputs (1, (uint8_t*)"|\n"); //bottom right char

	return 0;
}

int process_turn(const char input_char, game* cur_game) //returns 1 if legal move is possible after input is processed
{
	int move_success = 0;

	switch ( input_char ) {
	case 'w':
		move_success = move_w(cur_game);
		break;
	case 'a':
		move_success = move_a(cur_game);
		break;
	case 's':
		move_success = move_s(cur_game);
		//if (move_success) ece391_fdputs (1, (uint8_t*)"s is pressed\n");
		break;
	case 'd':
		move_success = move_d(cur_game);
		break;
	case 'q':
		destroy_game(cur_game);
		ece391_fdputs (1, (uint8_t*)"\nQuitting..\n");
		return 0;
		break;
	/*	case 'n':
			//get row and col input for new game
	dim_prompt: ece391_fdputs (1, (uint8_t*)"NEW GAME: Enter dimensions (rows columns):");
			while (NULL == fgets(buf, 200, stdin)) {
				ece391_fdputs (1, (uint8_t*)"\nProgram Terminated.\n");
				return 0;
			}

			if (2 != sscanf(buf, "%d%d%1s", &rows, &cols, garbage) ||
			        rows < 0 || cols < 0) {
				ece391_fdputs (1, (uint8_t*)"Invalid dimensions.\n");
				goto dim_prompt;
			}

			remake_game(&cur_game, rows, cols);

			move_success = 1;
	*/
	default: //any other input
		ece391_fdputs (1, (uint8_t*)"Invalid Input. Valid inputs are: w, a, s, d, q, n.\n");
	}




	if (move_success == 1) { //if movement happened, insert new tile and print the game.
		rand_new_tile(cur_game);
		print_game(cur_game);
	}

	if ( legal_move_check(cur_game) == 0) { //check if the newly spawned tile results in game over.
		ece391_fdputs (1, (uint8_t*)"Game Over!\n");
		destroy_game(cur_game);
		return 0;
	}
	return 1;
}

int main()
{
	int rows, cols;
	char buf[200];
	ece391_fdputs (1, (uint8_t*)(uint8_t*)"2048: Enter dimensions (rowscolumns), for example: 4 4\n");
	ece391_fdputs (1, (uint8_t*)(uint8_t*)"max 9 rows and 9 columns:\n");
	while (1) {
		ece391_read (0, buf, 10);
		if (buf[0] > 48 && buf[0] < 58 && buf[2] > 48 && buf[2] < 58) {
			//ece391_fdputs (1, (uint8_t*)(uint8_t*)"%c %c\n", buf[0], buf[2]);
			rows = buf[0] - 48;
			cols = buf[2] - 48;
			break;
		}
		else
			ece391_fdputs (1, (uint8_t*)(uint8_t*)"please enter dimensions in the correct form");
	}
	//ece391_fdputs (1, (uint8_t*)(uint8_t*)"game has been made\n");

	game * cur_game = make_game(rows, cols); //make new game entity called cur_game and insert the first random tile
	//ece391_fdputs (1, (uint8_t*)(uint8_t*)"current game: %d\n", (uint32_t)cur_game);
	if ( cur_game == (game*)0 ) {
		ece391_fdputs (1, (uint8_t*)(uint8_t*)"Bad game pointer\n");
		return 2;
	}
	//ece391_fdputs (1, (uint8_t*)(uint8_t*)"%lf\n",((double)malloc_usable_size(cur_game->cells))/sizeof(int));
	//ece391_fdputs (1, (uint8_t*)(uint8_t*)"before rand_new tile\n");
	rand_new_tile(cur_game);
	//ece391_fdputs (1, (uint8_t*)(uint8_t*)"rand_new_tile\n");
	print_game(cur_game);
	//ece391_fdputs (1, (uint8_t*)(uint8_t*)"print_game\n");
	char c;
	do {
		ece391_read (0, buf, 10);
		c = buf[0]; //get character input; does not require a return keypress after input.

	} while ( process_turn(c, cur_game ) == 1); //call process_turn and check if successful

	return 0;
}
