#include "rtc.h"
#include "terminal.h"
#include "i8259.h"
#include "keyboard_handler.h"
#include "paging.h"
#include "pit.h"
#include "schedule.h"
#include "systemcall.h"
#include "2048.h"


#define KEYBRD_DATA_PORT 0x60
#define BUFF_END '\0'
#define BUFFER_LIMIT 128
#define BUFF_SIZE_LIMIT 127
#define VIDEO_MEM 0xB8000

#define CTRL_PRESS 0x1D
#define LEFT_SHIFT_PRESS 0x2A
#define RIGHT_SHIFT_PRESS 0x36
#define ALT_PRESS 0x38
#define CAPSLOCK_PRESS 0x3A
#define bit_mask 0x80
#define F1 0x3B
#define F2 0x3C
#define F3 0x3D

#define SCREEN_SIZE 4096
#define VGA_REG_SWITCH1 0x3D4	//VGA register used to switch termianl
#define VGA_REG_SWITCH2 0x3D5
#define HIGHBIT 0x0C
#define LOWBIT 0x0D
#define DISPLAY 0xFF
#define ATTRIB 0x7
uint32_t terminal_id = 0;
uint32_t key_buff_size[SCREEN_LIMIT] = {0, 0, 0};
static int32_t s;
static uint8_t temp;
static uint8_t pre_ATTRIB;
/*
 * Table for scancode used for US keyboard
 * from online documentation
 */
static unsigned char kbdus[128] =
{
	0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
	'9', '0', '-', '=', '\b', /* Backspace */
	'\t',     /* Tab */
	'q', 'w', 'e', 'r', /* 19 */
	't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
	0,      /* 29   - Control */
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
	'\'', '`',   0,    /* Left shift */
	'\\', 'z', 'x', 'c', 'v', 'b', 'n',      /* 49 */
	'm', ',', '.', '/',   0,        /* Right shift */
	'*',
	0,  /* Alt */
	' ',  /* Space bar */
	0,  /* Caps lock */
	0,  /* 59 - F1 key ... > */
	0,   0,   0,   0,   0,   0,   0,   0,
	0,  /* < ... F10 */
	0,  /* 69 - Num lock*/
	0,  /* Scroll Lock */
	0,  /* Home key */
	0,  /* Up Arrow */
	0,  /* Page Up */
	'-',
	0,  /* Left Arrow */
	0,
	0,  /* Right Arrow */
	'+',
	0,  /* 79 - End key*/
	0,  /* Down Arrow */
	0,  /* Page Down */
	0,  /* Insert Key */
	0,  /* Delete Key */
	0,   0,   0,
	0,  /* F11 Key */
	0,  /* F12 Key */
	0,  /* All other keys are undefined */
};

static uint32_t ctrl_pressed = 0;											//initialze value for the flags
static uint32_t shift_pressed = 0;
static uint32_t capslock_press = 0;
static uint32_t alt_pressed = 0;


/*
*   void switch_terminal (uint32_t target_screen, uint32_t screen_size)
*   Inputs: target_screen, scrren_size
*   Return Value: void
*   Function: print out button pushed on screen
*/
void switch_terminal (uint32_t target_screen, uint32_t screen_size)
{
	//calculating the high byte and display to the screen
	uint8_t high_byte = (screen_size * target_screen >> 8) & DISPLAY;
	outb(HIGHBIT, VGA_REG_SWITCH1);
	outb(high_byte, VGA_REG_SWITCH2);
	//calculating the low byte and display to the screen
	uint8_t low_byte = (screen_size * target_screen) & DISPLAY;
	outb(LOWBIT, VGA_REG_SWITCH1);
	outb(low_byte, VGA_REG_SWITCH2);

	*(uint8_t *)(VIDEO_MEM + 2 * SCREEN_SIZE * terminal_id + (s << 1)) = temp;
	*(uint8_t *)(VIDEO_MEM + 2 * SCREEN_SIZE * terminal_id + (s << 1) + 1) = pre_ATTRIB;
	terminal_id = target_screen;

	//call paging and set up new pages for video mem
	switch_video_pd(VIDEO_MEM + 2 * SCREEN_SIZE * terminal_id, terminal_id);
	send_eoi(1);
}

/*
* void keyboard_handler(void);
*   Inputs: void
*   Return Value: none
*/

void keyboard_handler(void)
{
	//get the scancode
	uint8_t scancode = inb(KEYBRD_DATA_PORT);
	uint8_t key;
	//printf("enter keyboard handler\n");
	if (scancode == LEFT_SHIFT_PRESS)
		shift_pressed = 1;
	if (scancode == LEFT_SHIFT_PRESS + bit_mask)
		shift_pressed = 0;
	if (scancode == RIGHT_SHIFT_PRESS)
		shift_pressed = 1;
	if (scancode == RIGHT_SHIFT_PRESS + bit_mask)
		shift_pressed = 0;
	if (scancode == CTRL_PRESS)
		ctrl_pressed = 1;
	if (scancode == CTRL_PRESS + bit_mask)
		ctrl_pressed = 0;
	if (scancode == ALT_PRESS)
		alt_pressed = 1;
	if (scancode == ALT_PRESS + bit_mask)
		alt_pressed = 0;
	if (scancode == CAPSLOCK_PRESS)
		capslock_press = 1 - capslock_press;

	if ((scancode == F1) && (alt_pressed == 1))
		switch_terminal(0, SCREEN_SIZE);
	if ((scancode == F2) && (alt_pressed == 1))
		switch_terminal(1, SCREEN_SIZE);
	if ((scancode == F3) && (alt_pressed == 1))
		switch_terminal(2, SCREEN_SIZE);
	//scancode & 0x80 means that key is released. ! means press.
	if (!(scancode & bit_mask)) {
		key = kbdus[scancode];
		if (shift_pressed == 1)
		{
			if (key == '1')
				key = '!';
			else if (key == '2')
				key = '@';
			else if (key == '3')
				key = '$';
			else if (key == '4')
				key = '$';
			else if (key == '5')
				key = '%';
			else if (key == '6')
				key = '^';
			else if (key == '7')
				key = '&';
			else if (key == '8')
				key = '*';
			else if (key == '9')
				key = '(';
			else if (key == '0')
				key = ')';
			else if (key == '-')
				key = '_';
			else if (key == '=')
				key = '+';
			else if (key == ',')
				key = '<';
			else if (key == '.')
				key = '.';
			else if (key == '/')
				key = '?';
			else if (key >= 'a' && key <= 'z')
				key = key - 32;
			else
				;
		}

		if (capslock_press == 1)
		{
			if (key >= 'a' && key <= 'z')
				key = key - 32;
		}
		// keyboard buff implement
		// how to put a NULL at the end??

		if (((key == 'L') || (key == 'l')) && ctrl_pressed == 1) {
			clear ();									//clear screen
			send_eoi(1);
			return;
		}
		if (key == 's' && alt_pressed == 1)
		{
			song();
			send_eoi(1);
			return;
		}

		//edge case
		if (scancode != 0x0E && key != '\n' && (key >= ' ' && key <= '~') && key != '\0') {
			if (key_buff_size[terminal_id] == BUFF_SIZE_LIMIT)
				;
			else {
				key_buf[key_buff_size[terminal_id]][terminal_id] = key;
				key_buff_size[terminal_id] ++;
				//print would cause cursor error
				putc (key, 1);
			}
		}
		//backspace implement
		if (scancode == 0x0E) {
			if (key_buff_size[terminal_id] > 0) {
				key_buff_size[terminal_id] --;
				backspace();
				key_buf[key_buff_size[terminal_id]][terminal_id] = BUFF_END;
			}
		}
		//enter case
		if (key == '\n') {

			if (key_buff_size[terminal_id] < BUFF_SIZE_LIMIT) {
				key_buf[key_buff_size[terminal_id]][terminal_id] = '\n';

				key_buf[key_buff_size[terminal_id] + 1][terminal_id] = BUFF_END;

				key_buff_size[terminal_id] = key_buff_size[terminal_id] + 1;
				//putc (key, 1);
			}
			else
				key_buf[key_buff_size[terminal_id]][terminal_id] = BUFF_END;		//store to the buffer
			uint32_t j;
			for (j = 0; j < key_buff_size[terminal_id] + 1; j++)
			{
				out_buf[j][terminal_id] = key_buf[j][terminal_id];					//move all key_buffer value to out_buffer
				key_buf[j][terminal_id] = '\0'; //clear buff
			}
			key_buff_size[terminal_id] = 0;
			read_ready = 0;   //send read signal
			putc (key , 1);
		}

	}
	send_eoi(1);    //send eoi when  done
	return;
}



