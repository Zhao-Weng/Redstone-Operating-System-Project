#ifndef _MOUSEDRIVER_H
#define _MOUSEDRIVER_H

#include "lib.h"
#include "x86_desc.h"
#include "types.h"
#include "i8259.h"
#include "keyboard_handler.h"
#include "terminal.h"


//init value of mouse package
#define MOUSE_PORT 0x60
#define STATUS_REGISTER 0x64
#define MOUSE_IRQ 0xc
//#define MOUSE_ID 1
#define LEFT_BUTTON (1 << 0)
#define RIGHT_BUTTON (1 << 1)
#define MIDDLE_BUTTON (1 << 2)
#define MOVEMENT_ONE (1 << 3)//always 1
#define X_SIGN (1 << 4)
#define Y_SIGN (1 << 5)
#define X_OVERFLOW (1 << 6)
#define Y_OVERFLOW (1 << 7)

/*
 * PS/2 Response Codes
 */
#define PS2_RESP_BAT_PASSED     0xAA    /* Returned after reset, when self test succeeds. */
#define PS2_RESP_BAT_FAILED     0xFC    /* Returned after reset, when self test fails */
#define PS2_RESP_ACK            0xFA    /* Acknowledge response. */



typedef struct position {
	int32_t x;
	int32_t y;
} pos_t;

//static pos_t mouse;

uint8_t read_mouse();
void move (int32_t dx, int32_t dy);
void write_mouse(uint8_t data, uint8_t port);
void mouse_return();
extern void mouse_handler();
void init_mouse();


#endif



