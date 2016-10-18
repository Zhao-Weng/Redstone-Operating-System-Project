#ifndef _INTERRUPT_HANDLER_H
#define _INTERRUPT_HANDLER_H

#include "lib.h"



//index for the displaying terminal
extern uint32_t terminal_id;


extern void keyboard_handler(void);
void switch_terminal (uint32_t target_screen, uint32_t screen_size);

		/* reset mouse device */
#define VIDEO 0xB8000








#endif
