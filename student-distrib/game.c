/*
Zefan Xu
ECE 220 MP7

This file contains functions that helps running the algorithm of 2048.
Data associated with each game is stored in struct game, which consists
of number of rows, columns and score, as well as a dynamic array of cells.

make_game allocate dynamic memory needed by this struct and initialize the
default value of each cell to -1

destroy_game frees all the memory associated with a particular game struct

get_cell takes a pair of 2D index and return the pointer to that cell

move_w moves up and merges the cells based on the rules of 2048

move_a moves left and merges the cells based on the rules of 2048

move_d moves right and merges the cells based on the rules of 2048

move_s moves down and merges the cells based on the rules of 2048

legal_move_check returns 1 if there are still legal moves available,
otherwise returns 0

remake_game first frees memory associated with the old game, and then
allocate memory and initialized default cell values for the new game

I don't know what other functions do, because I did't write them,
nor did I read them, and I don't care.
*/
#include "game.h"

int index_random = 0;
extern int current_terminal_id;

game * make_game(int rows, int cols)
{
	int i;
	//printf("make_game\n");
	game * newgame = (game*)malloc(sizeof(game)); // allocate for game struct first
	//printf("malloc\n");
	//printf("new game: %x\n", (uint32_t)newgame);
	if (newgame == NULL) return NULL;
	newgame->rows = rows;
	newgame->cols = cols; // initialize values
	newgame->cells = (int*)malloc((uint32_t)(rows * cols * sizeof(int))); // allocate dynamic array for cells
	//printf("cells memory: %x\n", (uint32_t)newgame->cells);
	if (newgame->cells == NULL) return NULL;
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
	free((uint32_t*)(cur_game->cells)); // free cells first
	free((uint32_t*)(cur_game)); // free the game struct
	return;
}

cell * get_cell(game * cur_game, int row, int col)
{
	if (row < 0 || row >= cur_game->rows || col < 0 || col >= cur_game->cols) return NULL;
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

	if (cell_ptr == NULL) {
		printf("Bad Cell Pointer.\n");
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
		printf("Error: Trying to insert into no a board with no empty cell. The function rand_new_tile() should only be called after tiles have succesfully moved, meaning there should be at least 1 open spot.\n");
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

int print_game(game * cur_game)
{
	cell * cell_ptr;
	cell_ptr = 	cur_game->cells;

	int rows = cur_game->rows;
	int cols = cur_game->cols;
	int i, j;

	printf("\n\n\nscore:%d\n", cur_game->score);


	printf("|"); // topleft box char
	for (i = 0; i < cols * 5; i++)
		printf("-"); // top box char
	printf("|\n"); //top right char


	for (i = 0; i < rows; i++) {
		printf("|"); // side box char
		for (j = 0; j < cols; j++) {
			if ((*cell_ptr) == -1 ) { //print asterisks
				printf(" **  ");
			}
			else {
				switch ( *cell_ptr ) { //print colored text
				case 2:
					printf("  2  ");
					break;
				case 4:
					printf("  4  ");
					break;
				case 8:
					printf("  8  ");
					break;
				case 16:
					printf("  16 ");
					break;
				case 32:
					printf("  32 ");
					break;
				case 64:
					printf("  64 ");
					break;
				case 128:
					printf("  128");
					break;
				case 256:
					printf("  256");
					break;
				case 512:
					printf("  512");
					break;
				case 1024:
					printf(" 1024");
					break;
				case 2048:
					printf(" 2048");
					break;
				case 4096:
					printf(" 4096");
					break;
				case 8192:
					printf(" 8192");
					break;
				default:
					printf("  X  ");

				}

			}
			cell_ptr++;
		}
		printf("|\n"); //print right wall and newline
	}

	printf("-"); // print bottom left char
	for (i = 0; i < cols * 5; i++)
		printf("-"); // bottom char
	printf("|\n"); //bottom right char

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
		//if (move_success) printf("s is pressed\n");
		break;
	case 'd':
		move_success = move_d(cur_game);
		break;
	case 'q':
		destroy_game(cur_game);
		printf("\nQuitting..\n");
		return 0;
		break;
	/*	case 'n':
			//get row and col input for new game
	dim_prompt: printf("NEW GAME: Enter dimensions (rows columns):");
			while (NULL == fgets(buf, 200, stdin)) {
				printf("\nProgram Terminated.\n");
				return 0;
			}

			if (2 != sscanf(buf, "%d%d%1s", &rows, &cols, garbage) ||
			        rows < 0 || cols < 0) {
				printf("Invalid dimensions.\n");
				goto dim_prompt;
			}

			remake_game(&cur_game, rows, cols);

			move_success = 1;
	*/
	default: //any other input
		printf("Invalid Input. Valid inputs are: w, a, s, d, q, n.\n");
	}




	if (move_success == 1) { //if movement happened, insert new tile and print the game.
		rand_new_tile(cur_game);
		print_game(cur_game);
	}

	if ( legal_move_check(cur_game) == 0) { //check if the newly spawned tile results in game over.
		printf("Game Over!\n");
		destroy_game(cur_game);
		return 0;
	}
	return 1;
}
