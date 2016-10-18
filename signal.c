#include "x86_desc.h"
#include "lib.h"
#include "signal.h"
#include "types.h"



#define PCB_MASK 0xFFFFE000
#define SIGRETURN_VAL 10





//blcok a signal
void  block_signal (uint32_t esp, int32_t sig_number){
	pcb_t * curr_pcb = (pcb_t *)(esp & PCB_MASK);
	curr_pcb->sig_block = curr_pcb->sig_block | (1 << sig_number);
	return ;
}

//update pending signal; 
void pending_signal(uint32_t esp, int32_t sig_number){
	pcb_t * curr_pcb = (pcb_t *)(esp & PCB_MASK);
	curr_pcb->sig_pending = curr_pcb->pending | (1 << sig_number);
	return ;
}


void do_signal(regs_t * r){
	int sig_number;
	uint32_t flags;
	cli_and_save(flags);
	//prevent race condition.


	if(r->cs == KERNEL_CS) return;
	//is only invoked when the CPU is returning to 
	//user mode. 
	pcb_t * curr_pcb = (pcb_t *)(r->esp & PCB_MASK);
	if(mypcb - >sig_pending == 0) return ;

	for (sig_number = 0;sig_number < SIGNAL_ARRAY_SIZE; sig_number++){
		if(((curr_pcb->sig_pending) >> sig_number) & 1 == 1)
				break; 
	}
	//check which signal is pending


	if((curr_pcb->sig_block) >> sig_number ==1)
		return ;
	//check whether that signal is blocked. 

	curr_pcb -> sig_block = (1<< sig_number) | curr_pcb->sig_block;
	//block the signal to prevent reentraant; 

	curr_pcb->sig_pending = curr_pcb->sig_pending & ~(1<<sig_number);
	//set he signal to be not pending, it is executed. 
	setup_frame(sig_number, curr_pcb, r);

	restore_flags(flags);
	return ;


}

//!!!!!!!!!!!!!!set up the register to help resume execute in 
//user mode. 
int setup_frame(int sig_number, regs_t* r, pcb_t* curr_pcb){

	sig_frame_t* frame_addr = (sig_frame_t *)((r->useresp - sizeof(sig_frame_t))& 0xfffffff8;
	//get the initial address of the frame;
	sig_frame_t frame = frame_addr;
	frame->return_addr = (uint32_t*)(r->useresp - sizeof(uint8_t)*RETURN_CODE_LENGTH);
	//when the user handler finishes, it returns to the return code
	frame->sig_number = sig_number;
	frame->hw_context.ebx = r->ebx;
	frame->hw_context.ecx = r->ecx;
 	frame->hw_context.edx = r->edx;
	frame->hw_context.esi = r->esi;
	frame->hw_context.edi = r->edi;
	frame->hw_context.ebp = r->ebp;
	frame->hw_context.eax = r->eax;
	frame->hw_context.ds = r->ds;
	frame->hw_context.es = r->es;
	frame->hw_context.fs = r->fs;
	frame->hw_context.intro_num = r->int_num;
	frame->hw_context.err_code = r->err_code;
	frame->hw_context.eip = r->eip;
	frame->hw_context.cs = r->cs;
	frame->hw_context.eflags = r->eflags;
	frame->hw_context.useresp = r->useresp;
	frame->hw_context.ss = r->ss;

	static const struct { 
	uint16_t poplmovl;
	uint32_t val;
	uint16_t int80;    
	
} __attribute__((packed)) code = { 
	0xb858,		 /* popl %eax  */
SIGRETURN_VAL,   
	0x80cd,		/* int $0x80 */
	
}; 
			
	memcpy(frame->return_code, &code, RETURN_CODE_LENGTH);
	//copy execute sigreturn; 


	//set up registers for signal handler; 
	//modify regs area of kernel to ensure its control transferred
	//to signal handler when process resumes its execution in User
	//mode 
	r->useresp = (uint32_t)frame_addr;
	r->eip = (uint32_t )curr_pcb->sig_handler_array[sig_number];
	r->eax = SIGRETURN_VAL;
	r->ebx = 0;
	r->ecx = 0;
	r->edx = 0;

 	r->ds = USER_DS;
	r->es = USER_DS;
	r->ss = USER_DS;
	r->cs = USER_CS;

	return 0;
}




int restore_signal_context(sys_reg * r){
	uint32_t flags;
	cli_and_save(flags);


	pcb_t* curr_pcb = (pcb_t * )(r->esp & PCB_MASK);

	sig_frame_t* frame_addr = (sig_frame_t *)((r->useresp - sizeof(sig_frame_t))& 0xfffffff8;
	//get the initial address of the frame;
	sig_frame_t frame = frame_addr;


//restore kernel context
	r->gs = frame->hw_context.gs;
	r->fs = frame->hw_context.fs;
	r->es = frame->hw_context.es;
	r->ds = frame->hw_context.ds;
	
	r->edi = frame->hw_context.edi;
	r->esi = frame->hw_context.esi;
	r->ebp = frame->hw_context.ebp;
	r->esp = frame->hw_context.esp;
	r->ebx = frame->hw_context.ebx;
	r->edx = frame->hw_context.edx;
	r->ecx = frame->hw_context.ecx;

	r->int_num = frame->context.intro_num;
	r->err_code = frame->context.err_code;
	
	r->eip = frame->context.eip;
	r->cs = frame->context.cs;
	r->eflags = frame->context.eflags;
	r->useresp = frame->context.useresp;
	r->ss = frame->context.ss;

	//change it to be not block since signal processing has finished. 
	curr_pcb -> sig_block = curr_pcb -> sig_block & (~(1 << frame->sig_number));
	restore_flags(flags);
	return frame.context.eax;
	
}


int check_unblocked_pending_signal(uint32_t esp){
	pcb_t * curr_pcb = (pcb_t*)(esp & PCB_MASK);
	if(curr_pcb == 0)
		return 0;
	if(curr_pcb -> sig_pending & (~(curr_pcb ->sig_block)) )
		return 1;
	return 0; 
}



