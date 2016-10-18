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
#define MOUSE_IRQ 0x2c
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

/*
 * PS/2 Mouse Commands
 */
#define PS2_MOUSE_SETSCALE1_1   0xE6    /* Set scaling 1:1, mouse response = ACK. */
#define PS2_MOUSE_SETSCALE2_1   0xE7    /* Set scaling 2:1, mouse response = ACK. */
#define PS2_MOUSE_SETRES        0xE8    /* Set resolution, mouse response = ACK, host sends resaolution, mouse response = ACK. */
#define PS2_MOUSE_STATUSREQ     0xE9    /* Status request, mouse response = ACK, then 3 byte status packet. */
#define PS2_MOUSE_SETSTREAM     0xEA    /* Set stream mode, mouse response = ACK, then enters stream mode. */
#define PS2_MOUSE_READDATA      0xEB    /* Read data, mouse response = ACK, then movement data packet. */
#define PS2_MOUSE_RESETWRAP     0xEC    /* Reset wrap mode, back to previous mode, mouse response = ACK. */
#define PS2_MOUSE_SETWRAP       0xEE    /* Set wrap mode, mouse response = ACK. */
#define PS2_MOUSE_SETREMOTE     0xF0    /* Set remote mode, mouse response = ACK. */
#define PS2_MOUSE_GETID         0xF2    /* response = ACK, then ID: 0x00 is standard mouse. */
#define PS2_MOUSE_SETRATE       0xF3    /* Set sample rate, mouse respons = ACK, host sends sample rate, mouse response = ACK.*/
#define PS2_MOUSE_ENABLE        0xF4    /* Enable data reporting, mouse response = ACK. */
#define PS2_MOUSE_DISABLE       0xF5    /* Disable data reporting, mouse response = ACK. */
#define PS2_MOUSE_SETDEFAULTS   0xF6    /* Set default (sample rate = 100, resolution = 4counts/mm, scaling 1:1, disable reporting), response = ACK. */
#define PS2_MOUSE_RESEND        0xFE    /* Resend request for last byte, mouse response = last byte. */
#define PS2_MOUSE_RESET         0xFF    /* Reset mouse, response = 0xAA ("OK") or 0xFC ("Failed"). */
/* Mouse Movement Data Packet
 *
 *             Bit 7     Bit 6    Bit 5    Bit 4   Bit 3     Bit 2       Bit 1      Bit 0
 *        +---------------------------------------------------------------------------------+
 * Byte 1 | Y overfl | X overfl | Y sign | X sign | '1' | Middle Btn | Right Btn | Left Btn |
 * Byte 2 | X movement                                                                      |
 * Byte 3 | Y movement                                                                      |
 *        +---------------------------------------------------------------------------------+
 */


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



