#ifndef _SCHEDULE_H
#define _SCHEDULE_H

#include "lib.h"
#include "rtc.h"
#include "terminal.h"
#include "filesystem.h"
#include "types.h"
#include "paging.h"
#include "x86_desc.h"
#define F_MB 0x0400000
#define BA_MB 0x0800000
#define BA_KB 0x2000
#define VIDEO_MEMORY 0xB8000
#define TERMINAL_MAXIMUM 3

void initialize_schedule();
extern void schedule_handler();
int terminal_process_info[3];
int current_terminal_id;
int next_sche_terminal;
int sche_ready;
pcb_t * curr_process;

#endif
