#include "lib.h"
#include "terminal.h"
#include "keyboard_handler.h"
#include "systemcall.h"
#include "schedule.h"
#define VIDEO_MEMORY 0xB8000
#define NUMBER_OF_COLS 80
#define NUMBER_OF_ROWS 25
#define ATTRIB 0x7
#define VGA_BASE_ONE 0x3D4		
#define VGA_BASE_TWO 0x3D5
#define SIZE_OF_SCREEN 4096

int screen_x[SCREEN_LIMIT];
int screen_y[SCREEN_LIMIT];

/*
* void clear(void);
*   Inputs: void
*   Return Value: none
*	Function: Clears video memory
*/
void
clear(void) {
	int32_t i;
	// write to registers
	for (i = 0; i < NUMBER_OF_ROWS * NUMBER_OF_COLS; i++) {
		*(uint8_t *)(VIDEO_MEMORY + 2 * SIZE_OF_SCREEN * terminal_id + (i << 1)) = ' ';
		*(uint8_t *)(VIDEO_MEMORY + 2 * SIZE_OF_SCREEN * terminal_id + (i << 1) + 1) = ATTRIB;
	}
	//reset position and cursor
	screen_x[terminal_id] = 0;
	screen_y[terminal_id] = 0;
	update_cursor(0, 0);
}


/*
* update_cursor(int row, int col)
*   Inputs: row and col, position on the screen
*   Return Value: void
*	Function: update the cursor position
*/
void
update_cursor(int row, int col) {
	unsigned short position = (row * NUMBER_OF_COLS) + col;

	// cursor LOW port to vga INDEX register
	outb(0x0F, VGA_BASE_ONE);
	outb((unsigned char)(position & 0xFF), VGA_BASE_TWO);
	// cursor HIGH port to vga INDEX register
	outb(0x0E, VGA_BASE_ONE);
	outb((unsigned char )((position >> 8) & 0xFF), VGA_BASE_TWO);
}

/* Standard printf().
 * Only supports the following format strings:
 * %%  - print a literal '%' character
 * %x  - print a number in hexadecimal
 * %u  - print a number as an unsigned integer
 * %d  - print a number as a signed integer
 * %c  - print a character
 * %s  - print a string
 * %#x - print a number in 32-bit aligned hexadecimal, i.e.
 *       print 8 hexadecimal digits, zero-padded on the left.
 *       For example, the hex number "E" would be printed as
 *       "0000000E".
 *       Note: This is slightly different than the libc specification
 *       for the "#" modifier (this implementation doesn't add a "0x" at
 *       the beginning), but I think it's more flexible this way.
 *       Also note: %x is the only conversion specifier that can use
 *       the "#" modifier to alter output.
 *
*/

/*
*  backspace(void)
*   Inputs: none
*   Return Value: void
*	Function:  perform backspace: clear video memory and buffer
*/
void
backspace(void) {
	// If at the start, go back to the last of the previous line
	if (screen_x[terminal_id] == 0) {
		screen_y[terminal_id]--;
		screen_x[terminal_id] = NUMBER_OF_COLS - 1;		
	}
	// else move backwards
	else
		screen_x[terminal_id]--;							

	// change registers 
	*(uint8_t *)(VIDEO_MEMORY + 2 * SIZE_OF_SCREEN * terminal_id + ((NUMBER_OF_COLS * screen_y[terminal_id] + screen_x[terminal_id]) << 1)) = ' ';
	*(uint8_t *)(VIDEO_MEMORY + 2 * SIZE_OF_SCREEN * terminal_id + ((NUMBER_OF_COLS * screen_y[terminal_id] + screen_x[terminal_id]) << 1) + 1) = ATTRIB;	
	//update the cursor to the previous position
	update_cursor(screen_y[terminal_id], screen_x[terminal_id]);											
}

/*
* scroll_screen(int running_terminal)
*   Inputs: running terminal to display the screen
*   Return Value: void
*	Function: enable scrolling, according to different type of terminals
*/
void
scroll_screen(int running_terminal) {
	if (running_terminal == 0) {
		int32_t i;
		// shift video memory by one row up except the last row
		for (i = 0; i < (NUMBER_OF_ROWS - 1) * NUMBER_OF_COLS; i++) {								
			*(uint8_t *)(VIDEO_MEMORY + 2 * SIZE_OF_SCREEN * current_terminal_id + (i << 1)) = *(uint8_t *)(VIDEO_MEMORY + 2 * SIZE_OF_SCREEN * current_terminal_id + ((i + NUMBER_OF_COLS) << 1));
			*(uint8_t *)(VIDEO_MEMORY + 2 * SIZE_OF_SCREEN * current_terminal_id + (i << 1) + 1) = ATTRIB;
		}
		// fill with empty space for the last row 
		for (i = (NUMBER_OF_ROWS - 1) * NUMBER_OF_COLS; i < NUMBER_OF_ROWS * NUMBER_OF_COLS; i++)				
		{
			*(uint8_t *)(VIDEO_MEMORY + 2 * SIZE_OF_SCREEN * current_terminal_id + (i << 1)) = ' ';
			*(uint8_t *)(VIDEO_MEMORY + 2 * SIZE_OF_SCREEN * current_terminal_id + (i << 1) + 1) = ATTRIB;
		}
		// drecrese row count
		screen_y[current_terminal_id]--;
		return;
	}
	else {
		int32_t i;
		// shift the  video memory by one row up except the last row
		for (i = 0; i < (NUMBER_OF_ROWS - 1) * NUMBER_OF_COLS; i++) {								
			*(uint8_t *)(VIDEO_MEMORY + 2 * SIZE_OF_SCREEN * terminal_id + (i << 1)) = *(uint8_t *)(VIDEO_MEMORY + 2 * SIZE_OF_SCREEN * terminal_id + ((i + NUMBER_OF_COLS) << 1));
			*(uint8_t *)(VIDEO_MEMORY + 2 * SIZE_OF_SCREEN * terminal_id + (i << 1) + 1) = ATTRIB;
		}
		// fill with empty space for the last row 
		for (i = (NUMBER_OF_ROWS - 1) * NUMBER_OF_COLS; i < NUMBER_OF_ROWS * NUMBER_OF_COLS; i++)				
		{
			*(uint8_t *)(VIDEO_MEMORY + 2 * SIZE_OF_SCREEN * terminal_id + (i << 1)) = ' ';
			*(uint8_t *)(VIDEO_MEMORY + 2 * SIZE_OF_SCREEN * terminal_id + (i << 1) + 1) = ATTRIB;
		}
		// decrease the row count
		screen_y[terminal_id]--;														
		return;
	}
}


/*
*   void print_new_line(int running_terminal)
*   Inputs: running_terminal for which terminal to display on
*   Return Value: void
*	Function: call scroll_screen function and display correvtly
*/
void
print_new_line(int running_terminal) {
	// update indices
	if (running_terminal == 0) {
		screen_y[current_terminal_id]++;
		// handle the last line
		if (screen_y[current_terminal_id] == NUMBER_OF_ROWS)
			scroll_screen(running_terminal);
		return;
	}
	else {
		screen_y[terminal_id]++;
		// handle the last line
		if (screen_y[terminal_id] == NUMBER_OF_ROWS)
			scroll_screen(running_terminal);
		return;
	}
}

/* Standard printf().
 * Only supports the following format strings:
 * %%  - print a literal '%' character
 * %x  - print a number in hexadecimal
 * %u  - print a number as an unsigned integer
 * %d  - print a number as a signed integer
 * %c  - print a character
 * %s  - print a string
 * %#x - print a number in 32-bit aligned hexadecimal, i.e.
 *       print 8 hexadecimal digits, zero-padded on the left.
 *       For example, the hex number "E" would be printed as
 *       "0000000E".
 *       Note: This is slightly different than the libc specification
 *       for the "#" modifier (this implementation doesn't add a "0x" at
 *       the beginning), but I think it's more flexible this way.
 *       Also note: %x is the only conversion specifier that can use
 *       the "#" modifier to alter output.
 * */
int32_t
printf(int8_t *format, ...)
{

	/* Pointer to the format string */
	int8_t* buf = format;

	/* Stack pointer for the other parameters */
	int32_t* esp = (void *)&format;
	esp++;

	while (*buf != '\0') {
		switch (*buf) {
		case '%':
		{
			int32_t alternate = 0;
			buf++;

format_char_switch:
			/* Conversion specifiers */
			switch (*buf) {
			/* Print a literal '%' character */
			case '%':
				putc('%', 0);
				break;

			/* Use alternate formatting */
			case '#':
				alternate = 1;
				buf++;
				/* Yes, I know gotos are bad.  This is the
				 * most elegant and general way to do this,
				 * IMHO. */
				goto format_char_switch;

			/* Print a number in hexadecimal form */
			case 'x':
			{
				int8_t conv_buf[64];
				if (alternate == 0) {
					itoa(*((uint32_t *)esp), conv_buf, 16);
					puts(conv_buf, 0);
				} else {
					int32_t starting_index;
					int32_t i;
					itoa(*((uint32_t *)esp), &conv_buf[8], 16);
					i = starting_index = strlen(&conv_buf[8]);
					while (i < 8) {
						conv_buf[i] = '0';
						i++;
					}
					puts(&conv_buf[starting_index], 0);
				}
				esp++;
			}
			break;

			/* Print a number in unsigned int form */
			case 'u':
			{
				int8_t conv_buf[36];
				itoa(*((uint32_t *)esp), conv_buf, 10);
				puts(conv_buf, 0);
				esp++;
			}
			break;

			/* Print a number in signed int form */
			case 'd':
			{
				int8_t conv_buf[36];
				int32_t value = *((int32_t *)esp);
				if (value < 0) {
					conv_buf[0] = '-';
					itoa(-value, &conv_buf[1], 10);
				} else {
					itoa(value, conv_buf, 10);
				}
				puts(conv_buf, 0);
				esp++;
			}
			break;

			/* Print a single character */
			case 'c':
				putc( (uint8_t) * ((int32_t *)esp) , 0);
				esp++;
				break;

			/* Print a NULL-terminated string */
			case 's':
				puts( *((int8_t **)esp), 0);
				esp++;
				break;

			default:
				break;
			}

		}
		break;

		default:
			putc(*buf, 0);
			break;
		}
		buf++;
	}

	return (buf - format);
}

/*
* int32_t puts(int8_t* s, running_terminal);
*   Inputs: int_8* s = pointer to a string of characters, and running_terminal for which terminal to print on
*   Return Value: Number of bytes written
*	Function: Output a string to the console, 1 for running and 0 for displaying
*/

int32_t
puts(int8_t* s, int running_terminal)
{
	register int32_t index = 0;
	while (s[index] != '\0') {
		if (running_terminal == 0) {
			putc(s[index], 0);
			index++;
		}
		else {
			putc(s[index], 1);
			index++;
		}
	}

	return index;
}

/*
*   void putc(uint8_t c, int running_terminal)
*   Inputs: uint_8* c = character to print and running_terminal for which terminal to display on
*   Return Value: void
*	Function: Output a character to the console, and 1 for running, 0 for displaying
*/
void
putc(uint8_t c, int running_terminal)
{
	if (!running_terminal) {
		if (c == '\n' || c == '\r') {
			screen_y[current_terminal_id]++;
			// handle right bound case 
			if (screen_y[current_terminal_id] == NUMBER_OF_ROWS)
				scroll_screen(0);																
			screen_x[current_terminal_id] = 0;
			update_cursor (screen_y[current_terminal_id], screen_x[current_terminal_id]);
		} else {
			*(uint8_t *)(VIDEO_MEMORY + 2 * SIZE_OF_SCREEN * current_terminal_id + ((NUMBER_OF_COLS * screen_y[current_terminal_id] + screen_x[current_terminal_id]) << 1)) = c;
			*(uint8_t *)(VIDEO_MEMORY + 2 * SIZE_OF_SCREEN * current_terminal_id + ((NUMBER_OF_COLS * screen_y[current_terminal_id] + screen_x[current_terminal_id]) << 1) + 1) = ATTRIB;
			screen_x[current_terminal_id]++;
			// handle right bound case 
			if (screen_x[current_terminal_id] == NUMBER_OF_COLS)										
				print_new_line(0);
			// update screen position for a certain terminal 
			screen_x[current_terminal_id] %= NUMBER_OF_COLS;
			screen_y[current_terminal_id] = (screen_y[current_terminal_id] + (screen_x[current_terminal_id] / NUMBER_OF_COLS)) % NUMBER_OF_ROWS;
			update_cursor (screen_y[current_terminal_id], screen_x[current_terminal_id]);
		}
	}
	else {
		if (c == '\n' || c == '\r') {
			screen_y[terminal_id]++;
			// handle buttom bound case
			if (screen_y[terminal_id] == NUMBER_OF_ROWS)
				scroll_screen(1);
			screen_x[terminal_id] = 0;
			update_cursor (screen_y[terminal_id], screen_x[terminal_id]);
		} else {
			*(uint8_t *)(VIDEO_MEMORY + 2 * SIZE_OF_SCREEN * terminal_id + ((NUMBER_OF_COLS * screen_y[terminal_id] + screen_x[terminal_id]) << 1)) = c;
			*(uint8_t *)(VIDEO_MEMORY + 2 * SIZE_OF_SCREEN * terminal_id + ((NUMBER_OF_COLS * screen_y[terminal_id] + screen_x[terminal_id]) << 1) + 1) = ATTRIB;
			screen_x[terminal_id]++;
			// handle buttom bound case
			if (screen_x[terminal_id] == NUMBER_OF_COLS)
				print_new_line(1);
			// update screen position for a certain terminal 
			screen_x[terminal_id] %= NUMBER_OF_COLS;
			screen_y[terminal_id] = (screen_y[terminal_id] + (screen_x[terminal_id] / NUMBER_OF_COLS)) % NUMBER_OF_ROWS;
			update_cursor (screen_y[terminal_id], screen_x[terminal_id]);
		}
	}
}

/*
* int8_t* itoa(uint32_t value, int8_t* buf, int32_t radix);
*   Inputs: uint32_t value = number to convert
*			int8_t* buf = allocated buffer to place string in
*			int32_t radix = base system. hex, oct, dec, etc.
*   Return Value: number of bytes written
*	Function: Convert a number to its ASCII representation, with base "radix"
*/

int8_t*
itoa(uint32_t value, int8_t* buf, int32_t radix)
{
	static int8_t lookup[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	int8_t *newbuf = buf;
	int32_t i;
	uint32_t newval = value;

	/* Special case for zero */
	if (value == 0) {
		buf[0] = '0';
		buf[1] = '\0';
		return buf;
	}

	/* Go through the number one place value at a time, and add the
	 * correct digit to "newbuf".  We actually add characters to the
	 * ASCII string from lowest place value to highest, which is the
	 * opposite of how the number should be printed.  We'll reverse the
	 * characters later. */
	while (newval > 0) {
		i = newval % radix;
		*newbuf = lookup[i];
		newbuf++;
		newval /= radix;
	}

	/* Add a terminating NULL */
	*newbuf = '\0';

	/* Reverse the string and return */
	return strrev(buf);
}

/*
* int8_t* strrev(int8_t* s);
*   Inputs: int8_t* s = string to reverse
*   Return Value: reversed string
*	Function: reverses a string s
*/

int8_t*
strrev(int8_t* s)
{
	register int8_t tmp;
	register int32_t beg = 0;
	register int32_t end = strlen(s) - 1;

	while (beg < end) {
		tmp = s[end];
		s[end] = s[beg];
		s[beg] = tmp;
		beg++;
		end--;
	}

	return s;
}

/*
* uint32_t strlen(const int8_t* s);
*   Inputs: const int8_t* s = string to take length of
*   Return Value: length of string s
*	Function: return length of string s
*/

uint32_t
strlen(const int8_t* s)
{
	register uint32_t len = 0;
	while (s[len] != '\0')
		len++;

	return len;
}

/*
* void* memset(void* s, int32_t c, uint32_t n);
*   Inputs: void* s = pointer to memory
*			int32_t c = value to set memory to
*			uint32_t n = number of bytes to set
*   Return Value: new string
*	Function: set n consecutive bytes of pointer s to value c
*/

void*
memset(void* s, int32_t c, uint32_t n)
{
	c &= 0xFF;
	asm volatile("                  \n\
			.memset_top:            \n\
			testl   %%ecx, %%ecx    \n\
			jz      .memset_done    \n\
			testl   $0x3, %%edi     \n\
			jz      .memset_aligned \n\
			movb    %%al, (%%edi)   \n\
			addl    $1, %%edi       \n\
			subl    $1, %%ecx       \n\
			jmp     .memset_top     \n\
			.memset_aligned:        \n\
			movw    %%ds, %%dx      \n\
			movw    %%dx, %%es      \n\
			movl    %%ecx, %%edx    \n\
			shrl    $2, %%ecx       \n\
			andl    $0x3, %%edx     \n\
			cld                     \n\
			rep     stosl           \n\
			.memset_bottom:         \n\
			testl   %%edx, %%edx    \n\
			jz      .memset_done    \n\
			movb    %%al, (%%edi)   \n\
			addl    $1, %%edi       \n\
			subl    $1, %%edx       \n\
			jmp     .memset_bottom  \n\
			.memset_done:           \n\
			"
	             :
	             : "a"(c << 24 | c << 16 | c << 8 | c), "D"(s), "c"(n)
	             : "edx", "memory", "cc"
	            );

	return s;
}

/*
* void* memset_word(void* s, int32_t c, uint32_t n);
*   Inputs: void* s = pointer to memory
*			int32_t c = value to set memory to
*			uint32_t n = number of bytes to set
*   Return Value: new string
*	Function: set lower 16 bits of n consecutive memory locations of pointer s to value c
*/

/* Optimized memset_word */
void*
memset_word(void* s, int32_t c, uint32_t n)
{
	asm volatile("                  \n\
			movw    %%ds, %%dx      \n\
			movw    %%dx, %%es      \n\
			cld                     \n\
			rep     stosw           \n\
			"
	             :
	             : "a"(c), "D"(s), "c"(n)
	             : "edx", "memory", "cc"
	            );

	return s;
}

/*
* void* memset_dword(void* s, int32_t c, uint32_t n);
*   Inputs: void* s = pointer to memory
*			int32_t c = value to set memory to
*			uint32_t n = number of bytes to set
*   Return Value: new string
*	Function: set n consecutive memory locations of pointer s to value c
*/

void*
memset_dword(void* s, int32_t c, uint32_t n)
{
	asm volatile("                  \n\
			movw    %%ds, %%dx      \n\
			movw    %%dx, %%es      \n\
			cld                     \n\
			rep     stosl           \n\
			"
	             :
	             : "a"(c), "D"(s), "c"(n)
	             : "edx", "memory", "cc"
	            );

	return s;
}

/*
* void* memcpy(void* dest, const void* src, uint32_t n);
*   Inputs: void* dest = destination of copy
*			const void* src = source of copy
*			uint32_t n = number of byets to copy
*   Return Value: pointer to dest
*	Function: copy n bytes of src to dest
*/

void*
memcpy(void* dest, const void* src, uint32_t n)
{
	asm volatile("                  \n\
			.memcpy_top:            \n\
			testl   %%ecx, %%ecx    \n\
			jz      .memcpy_done    \n\
			testl   $0x3, %%edi     \n\
			jz      .memcpy_aligned \n\
			movb    (%%esi), %%al   \n\
			movb    %%al, (%%edi)   \n\
			addl    $1, %%edi       \n\
			addl    $1, %%esi       \n\
			subl    $1, %%ecx       \n\
			jmp     .memcpy_top     \n\
			.memcpy_aligned:        \n\
			movw    %%ds, %%dx      \n\
			movw    %%dx, %%es      \n\
			movl    %%ecx, %%edx    \n\
			shrl    $2, %%ecx       \n\
			andl    $0x3, %%edx     \n\
			cld                     \n\
			rep     movsl           \n\
			.memcpy_bottom:         \n\
			testl   %%edx, %%edx    \n\
			jz      .memcpy_done    \n\
			movb    (%%esi), %%al   \n\
			movb    %%al, (%%edi)   \n\
			addl    $1, %%edi       \n\
			addl    $1, %%esi       \n\
			subl    $1, %%edx       \n\
			jmp     .memcpy_bottom  \n\
			.memcpy_done:           \n\
			"
	             :
	             : "S"(src), "D"(dest), "c"(n)
	             : "eax", "edx", "memory", "cc"
	            );

	return dest;
}

/*
* void* memmove(void* dest, const void* src, uint32_t n);
*   Inputs: void* dest = destination of move
*			const void* src = source of move
*			uint32_t n = number of byets to move
*   Return Value: pointer to dest
*	Function: move n bytes of src to dest
*/

/* Optimized memmove (used for overlapping memory areas) */
void*
memmove(void* dest, const void* src, uint32_t n)
{
	asm volatile("                  \n\
			movw    %%ds, %%dx      \n\
			movw    %%dx, %%es      \n\
			cld                     \n\
			cmp     %%edi, %%esi    \n\
			jae     .memmove_go     \n\
			leal    -1(%%esi, %%ecx), %%esi    \n\
			leal    -1(%%edi, %%ecx), %%edi    \n\
			std                     \n\
			.memmove_go:            \n\
			rep     movsb           \n\
			"
	             :
	             : "D"(dest), "S"(src), "c"(n)
	             : "edx", "memory", "cc"
	            );

	return dest;
}

/*
* int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n)
*   Inputs: const int8_t* s1 = first string to compare
*			const int8_t* s2 = second string to compare
*			uint32_t n = number of bytes to compare
*	Return Value: A zero value indicates that the characters compared
*					in both strings form the same string.
*				A value greater than zero indicates that the first
*					character that does not match has a greater value
*					in str1 than in str2; And a value less than zero
*					indicates the opposite.
*	Function: compares string 1 and string 2 for equality
*/

int32_t
strncmp(const int8_t* s1, const int8_t* s2, uint32_t n)
{
	int32_t i;
	for (i = 0; i < n; i++) {
		if ( (s1[i] != s2[i]) ||
		        (s1[i] == '\0') /* || s2[i] == '\0' */ ) {

			/* The s2[i] == '\0' is unnecessary because of the short-circuit
			 * semantics of 'if' expressions in C.  If the first expression
			 * (s1[i] != s2[i]) evaluates to false, that is, if s1[i] ==
			 * s2[i], then we only need to test either s1[i] or s2[i] for
			 * '\0', since we know they are equal. */

			return s1[i] - s2[i];
		}
	}
	return 0;
}

/*
* int8_t* strcpy(int8_t* dest, const int8_t* src)
*   Inputs: int8_t* dest = destination string of copy
*			const int8_t* src = source string of copy
*   Return Value: pointer to dest
*	Function: copy the source string into the destination string
*/

int8_t*
strcpy(int8_t* dest, const int8_t* src)
{
	int32_t i = 0;
	while (src[i] != '\0') {
		dest[i] = src[i];
		i++;
	}

	dest[i] = '\0';
	return dest;
}

/*
* int8_t* strcpy(int8_t* dest, const int8_t* src, uint32_t n)
*   Inputs: int8_t* dest = destination string of copy
*			const int8_t* src = source string of copy
*			uint32_t n = number of bytes to copy
*   Return Value: pointer to dest
*	Function: copy n bytes of the source string into the destination string
*/

int8_t*
strncpy(int8_t* dest, const int8_t* src, uint32_t n)
{
	int32_t i = 0;
	while (src[i] != '\0' && i < n) {
		dest[i] = src[i];
		i++;
	}

	while (i < n) {
		dest[i] = '\0';
		i++;
	}

	return dest;
}

