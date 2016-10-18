#include "schedule.h"
#include "systemcall.h"
#include "keyboard_handler.h"

#define SIZE_OF_SCREEN 0x1000
#define UNUSED -1

/*
 *   initialize_schedule
 *   inputs: N/A
 *   return: N/A
 *	 functionality: initialize necessary data structure for schedule
 */
void initialize_schedule() {
	int i;
	sche_ready = 1;
	next_sche_terminal = 0;
	current_terminal_id = UNUSED;
	//set each PID in array to UNUSED flag
	for (i = 0; i < TERMINAL_MAXIMUM; i++)
		terminal_process_info[i] = UNUSED;
}
/*
 *   run_a_new_shell
 *   inputs: N/A
 *   return: N/A
 *	 functionality: start a new shell
 */
void run_a_new_shell() {
	sche_ready = 1;
	current_terminal_id = next_sche_terminal;
	send_eoi(0);
	execute((uint8_t *) "shell");
}

/*
 *   schedule_handler
 *   inputs: N/A
 *   return: N/A
 *	 functionality: switch between currently running process
 */
void schedule_handler() {
	uint32_t new_user_paging;
	//update the next schedule terminal index
	if (next_sche_terminal >= TERMINAL_MAXIMUM - 1)
		next_sche_terminal = (current_terminal_id + 1) % TERMINAL_MAXIMUM;
	else
		next_sche_terminal = current_terminal_id + 1;

	//swtich video page mapping for user programs
	switch_video_pd(VIDEO_MEMORY + 2 * SIZE_OF_SCREEN * next_sche_terminal, next_sche_terminal);
	//get the correct PCB pointer
	if (terminal_process_info[next_sche_terminal] == UNUSED && pid == UNUSED) {
		curr_process = (pcb_t *) (BA_MB - BA_KB * (next_sche_terminal + 1));
	} else {
		curr_process = (pcb_t *) (BA_MB - BA_KB * (terminal_process_info[current_terminal_id] + 1));
	}
	//save current esp and ebp
	asm volatile("movl %%ebp, %0" : "=r" (curr_process->sche_ebp));
	asm volatile("movl %%esp, %0" : "=r" (curr_process->sche_esp));

	//if we are trying to run a new shell
	if (terminal_process_info[next_sche_terminal] == UNUSED)
		run_a_new_shell();
	else {
		//calculate the pcb pointer using pid stored
		curr_process = (pcb_t *) (BA_MB - BA_KB * (terminal_process_info[next_sche_terminal] + 1));
		new_user_paging = BA_MB + F_MB * (curr_process -> PID);
		//swtich page mapping for user programs
		switch_pd(new_user_paging);
		current_terminal_id = next_sche_terminal;
	}

	//restore the esp, ebp, ss0 and esp0 to the process we are switching to
	send_eoi(0);
	//move esp and ebp to the registers
	asm volatile("movl %0, %%esp" : : "b" (curr_process->sche_esp));
	asm volatile("movl %0, %%ebp" : : "b" (curr_process->sche_ebp));
	tss.ss0 = KERNEL_DS;
	tss.esp0 = BA_MB - 4 - BA_KB * (curr_process->PID);
}
