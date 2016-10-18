
/*
*   void pit_eoi()
*   Inputs: void
*   Return Value: void
*	Function: send eoi when pit is done
*/


#include "pit.h"
#include "lib.h"
#include "types.h"
#include "systemcall.h"
#include "i8259.h"

#define  base_frq      1193180
#define  channel_zero  0x40
#define  mask_8        0xFF
#define  eight         8
#define  command_port  0x43
#define  command_byte  0x36
#define  sound_port    0x61
#define  channel_two   0x42
#define  sound_mask    0xFC
#define  command_mask  0xb6
#define PIT_IRQ     0x20
#define ini_pit_freq 100


void pit_initialization(int32_t frequency)  
{//source:http://www.jamesmolloy.co.uk/tutorial_html/5.-IRQs%20and%20the%20PIT.html
	
	int divisor = base_frq / frequency;   // get the divior
   	outb(command_byte, command_port);     //out command byte

  
   uint8_t l = (uint8_t)(divisor & mask_8);      // get low 8 bits
   uint8_t h = (uint8_t)((divisor>>eight) & mask_8 ); //get high 8 bits

  
   outb(l, channel_zero);  //out low bit and high bit
   outb(h, channel_zero);
   
}







