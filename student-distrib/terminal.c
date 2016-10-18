#include "terminal.h"
#include "keyboard_handler.h"
#include "schedule.h"

#define NULL_CHAR '\0'
#define MAX_CHAR 127

uint8_t key_buf[BUFFER_LIMIT][SCREEN_LIMIT];
uint8_t out_buf[BUFFER_LIMIT][SCREEN_LIMIT];
volatile int read_ready = 1;


/*
*   int32_t terminal_read (int32_t fd, uint8_t* usr_buf, int32_t usr_buf_len)
*   DESCRIPTION: copy data to user
*   Inputs: length of data to be read
*   Return Value: none
*	Function: go through all data in out buffer and copy everything into key buffer
*/
int32_t terminal_read (int32_t fd, uint8_t* buf, int32_t nbytes) {
	while (read_ready || (terminal_id != current_terminal_id));									//while readable and displaying index is the same as current_terminal_id, go ahead
	read_ready = 1;
	uint32_t i;
	if (nbytes <= 0)															//end of buffer is reached, return 0
		return 0;
	for (i = 0; i < nbytes && out_buf[i][terminal_id] != NULL_CHAR && i < MAX_CHAR; i++)	//loop through the given length of the buffer when it is no the end or reached limit
		buf[i] = out_buf[i][terminal_id];					     							//transfer all out_buffer to user_buffer value and clear out_buffer

	out_buf[0][terminal_id] = 0;
	buf[i] = NULL_CHAR;
	return i;																		//return the number of bytes read
}

/*
 * terminal_write
 *   DESCRIPTION: The terminal_write function will print the buffer to screen
 *   INPUTS: fd				-- file descriptor indexprint keyboard typed information
 *   OUTPUTS: none
 *   RETURN VALUE: number of write
 *   SIDE EFFECTS: none
 */

int32_t terminal_write(int32_t fd, const uint8_t* buf, int32_t nbytes) {
	if ((buf == NULL) || (nbytes < 0))							//if the buffer given to write is NULL or the end is reached, return -1
		return -1;

	uint32_t i;
	for (i = 0; i < nbytes; i++)
		putc(buf[i], 0);												// print data stored in user buffer to screen
	return nbytes;
}

/*
 * terminal_open
 *   DESCRIPTION: The terminal_open function will open the terminal
 *   INPUTS: command     --  command to open terminal
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */

int32_t terminal_open(const uint8_t * filename) {
	return 0; 						//return 0 on success
}

/*
 * terminal_close
 *   DESCRIPTION: The terminal_close function will close the terminal
 *   INPUTS: fd
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */

int32_t terminal_close(int32_t fd) {
	return -1;		//cannot close terminal
}

/*
* void keybrd_init();
*   Inputs: void
*   Return Value: none
*	Function: Initialize the terminal
*/
void keybrd_init() {
	clear();
	int i, j;
	terminal_id = 0;
	for (j = 0; j < SCREEN_LIMIT; j++) {
		for (i = 0; i < BUFFER_LIMIT; i++) {
			key_buf[i][j] = 0;						// initialize key buffer
			out_buf[i][j] = 0;						// initialize out buffer
		}
	}				// initialize keypressed_count
	return;
}

