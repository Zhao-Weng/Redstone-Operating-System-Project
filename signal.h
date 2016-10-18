#ifndef _SIGNAL_H
#define _SIGNAL_H

#include "types.h"

#define DIV_ZERO 0
#define SEGFAULT 1
#define INTERRUPT 2
#define ALARM 3
#define USRE1 4
#define RETURN_CODE_LENGTH 8


typedef struct sig_frame{
	int32_t return_addr;
	int32_t sig_number;
	hw_context_t hw_context;
	uint8_t return_code[RETURN_CODE_LENGTH];
}sig_frame_t;




// extern int sig_initialization();
// extern int sig_ingore();
// extern int sig_kill();



//extern void do_sig_systemcall(sys_regs_t* r);




void  block_signal (uint32_t esp, int32_t sig_number);
void pending_signal(uint32_t esp, int32_t sig_number);

void do_signal(regs_t * r);
int setup_frame(int sig_number, regs_t* r, pcb_t* curr_pcb);
int restore_signal_context(sys_reg * r);
int check_unblocked_pending_signal(uint32_t esp);























#endif