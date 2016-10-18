#ifndef _SYSTEMCALL_H
#define _SYSTEMCALL_H

#include "rtc.h"
#include "lib.h"
#include "types.h"
#include "terminal.h"
#include "x86_desc.h"
#include "paging.h"
#include "filesystem.h"

#define MAX_FILE_NUM 8




typedef struct file_t{

	int32_t * f_op;
	int32_t inode;
	uint32_t file_position;
	uint32_t flags;	//check if it is in-use

} file_t;

//pcb structure
typedef struct pcb{

	file_t file_array[MAX_FILE_NUM];
	int parent;
	uint8_t pcb_exe_args[32];
	int32_t pcb_arg_size;
	uint8_t PID;
	uint32_t esp0;	//parent's esp0
	uint8_t ss0;	//parent's ss0
	uint32_t return_add;	//return address for halt 
	uint32_t esp;	//parent's esp
	uint32_t ebp;	//parent's ebp
	uint32_t sche_esp;
	uint32_t sche_ebp;
	uint32_t sche_esp0;
	uint32_t sche_ss0;
	// uint32_t sche_add;
} pcb_t;

int pid;
uint8_t global;
int process_state[7];


int32_t halt (uint8_t status);
int32_t execute(const uint8_t * command);
int32_t read(int32_t fd, uint8_t * buf, int32_t nbytes);
int32_t write(int32_t fd, const uint8_t * buf, int32_t nbytes);
int32_t open(const uint8_t * filename);
int32_t close(int32_t fd);
int32_t getargs(uint8_t * buf, int32_t nbytes);
int32_t vidmap(uint8_t ** screen_start);
int32_t set_handler(int32_t signum, void * arg);
int32_t sigreturn(void);

void construct_pcb_entries(pcb_t * pcb_pointer, int new_pid);
void clear_file_descriptors();

void sing_song (int freq_number, uint32_t duration);
void song();





#endif
