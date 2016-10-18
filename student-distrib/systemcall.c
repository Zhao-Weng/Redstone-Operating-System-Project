#include "keyboard_handler.h"
#include "systemcall.h"
#include "schedule.h"

#define F_KB 0x1000
#define F_MB 0x0400000
#define BA_MB 0x0800000
#define BA_KB 0x2000
#define USER_PROGRAM_PAGING 0x08000000
#define USER_PROGRAM_END 0x08400000
#define USER_PROGRAM_ADDR 0x08048000
#define USER_PROGRAM_OFF 0x00048000
#define USER_PROGRAM_ESP 0x0083FFFFC
#define IRET_MASK 0x200
#define BUF_LEN 32
#define VIDEO_MEM 0xB8000
#define VIDEO_DIR_OFFSET 37
#define BUFFER_SIZE 1024
#define MIN_FD_NUM 2
#define MAX_FD_NUM 7
#define SOUND_FREQ 1193182
#define QUIET 30000
#define SOUND_PORT_DATA 0x42
#define SOUND_PORT_CMD 0x43
#define SOUND_IO_PORT 0x61
//data get from @yicheng senior thesis
int freq[256] = {130, 130, 130, 130, 195, 195, 195, 195, 261, 261, 261, 261, 311, 311, 311, 311, 391, 391, 391, 391, 311, 311, 311, 311, 261, 261, 261, 261, 195, 195, 195, 195, 130, 130, 130, 130, 195, 195, 195, 195, 261, 261, 261, 261, 311, 311, 311, 311, 97, 97, 97, 97, 146, 146, 146, 146, 195, 195, 195, 195, 233, 233, 233, 233, 103, 103, 103, 103, 155, 155, 155, 155, 207, 207, 207, 207, 261, 261, 261, 261, 311, 311, 311, 311, 261, 261, 261, 261, 207, 207, 207, 207, 155, 155, 155, 155, 103, 103, 103, 103, 155, 155, 155, 155, 261, 261, 261, 261, 311, 311, 311, 311, 207, 207, 207, 207, 349, 349, 349, 349, 207, 207, 207, 207, 391, 391, 391, 391, 349, 116, 349, 116, 174, 174, 174, 174, 233, 233, 233, 233, 293, 293, 293, 293, 174, 174, 174, 174, 233, 233, 233, 233, 293, 293, 293, 293, 349, 349, 349, 349, 155, 155, 155, 155, 233, 233, 233, 233, 311, 311, 311, 311, 349, 349, 349, 349, 97, 97, 97, 97, 146, 146, 146, 146, 195, 195, 195, 195, 233, 233, 233, 233, 293, 103, 293, 103, 155, 155, 155, 155, 207, 207, 207, 207, 311, 311, 311, 311, 207, 207, 207, 207, 155, 155, 155, 155, 261, 103, 261, 103, 207, 207, 207, 207, 155, 155, 155, 155, 261, 261, 261, 261, 311, 311, 311, 311, 466, 466, 466, 466, 155, 155, 155, 155, 415, 415, 415, 415, 207, 207, 207, 207, 391, 391, 391, 391};
int dur[256] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 50, 50, 50, 50, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 50, 50, 50, 50, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 50, 50, 50, 50, 100, 100, 100, 100, 100, 100, 100, 100, 50, 50, 50, 50, 50, 50, 50, 50, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};

typedef int32_t func();

//file operation
int32_t rtc_file_op[4] = {(int32_t) rtc_read, (int32_t) rtc_write, (int32_t) rtc_open, (int32_t) rtc_close};
int32_t terminal_file_op[4] = {(int32_t) terminal_read, (int32_t) terminal_write, (int32_t) terminal_open, (int32_t) terminal_close};
int32_t directory_file_op[4] = {(int32_t) dir_read, (int32_t) dir_write, (int32_t) dir_open, (int32_t) dir_close};
int32_t file_file_op[4] = {(int32_t) file_read, (int32_t) file_write, (int32_t) file_open, (int32_t) file_close};

int pid = -1;		//indicate the current pid
uint8_t global_return_status = -1; 	//to store the status
int process_state[];
uint8_t status = 0;



pcb_t * get_pcb_addr(){
	return (pcb_t*)(BA_MB - BA_KB*(terminal_process_info[current_terminal_id]+1)) ;
}


void sing_song (int freq_number, uint32_t duration){

	uint16_t divv;
	if(freq_number!=0)
		divv = SOUND_FREQ/freq_number;
	else
		divv = SOUND_FREQ/QUIET;
	//send the freqency.
	outb(182, SOUND_PORT_CMD);//182 = b8
	outb((uint8_t)(divv & 0xff), SOUND_PORT_DATA); //get lower bit
	outb((uint8_t)((divv >> 8)& 0xff), SOUND_PORT_DATA); //get high bit

	
	uint8_t tmp = inb(SOUND_IO_PORT);
	if (tmp != (tmp | 3)){
		outb(tmp | 3, SOUND_IO_PORT);
	}
	int32_t i = duration;
	//duration is millisecond
	int32_t j = (int)65535*2;	//somehow too fast
	//65553 is 16bit cpu_speed
	
	for (i = duration; i > 0; i--){
		while (j > 0){
			j--;
		}
		j = (int)65535*2;
	}


	uint8_t temp = inb(SOUND_IO_PORT) & 0xFC;
	outb(temp, SOUND_IO_PORT);
}

void song()
{
		int i;
		
		for(i=0; i<256; i++)
		{
			sing_song(freq[i], dur[i]);
		}
		
}

/*
*   int32_t halt (uint8_t status)
*   Inputs: status
*   Return Value: never return or 0 on success
*	Function: halt the current process
*/
int32_t halt (uint8_t status) {

	pcb_t * curr_process = (pcb_t *) (BA_MB - BA_KB * (terminal_process_info[current_terminal_id] + 1));
	tss.esp0 = curr_process->esp0;
	tss.ss0 = curr_process->ss0;
	pid = curr_process->PID;

	clear_file_descriptors();
	process_state[pid] = 0;
	pid = curr_process->parent;
	terminal_process_info[current_terminal_id] = pid;

	if (pid == -1) {
		execute((uint8_t *) "shell");
	}
	//reset the paging
	global_return_status = status;
	uint32_t addr = BA_MB + F_MB * pid;
	switch_pd(addr);


	//jump to parent process
	asm volatile (
	    "movl %0, %%esp			\n\
        		movl %1, %%ebp			\n\
        		jmp *%2"
	    :
	    :"r"(curr_process->esp), "r"(curr_process->ebp), "r" (curr_process->return_add)
	);

	return 0;
}


/*
*   void halt_fd()
*   Inputs: none
*   Return Value: none
*	Function: halt the current fd
*/

void clear_file_descriptors() {
	int i;
	for (i = MIN_FD_NUM; i < MAX_FILE_NUM; i++)
		(get_pcb_addr())->file_array[i].flags = 0;
}

/*
*   int32_t execute(const uint8_t * command)
*   Inputs: the command to execute
*   Return Value: the status on success and -1 on failure or error condition
*	Function: execute the given command
*/

int32_t execute(const uint8_t * command) {
	cli();
	int k, i;
	int temp_pid;
	int file_type;
	int eip = 0;
	dentry_t temp;
	int32_t command_len = 0;
	int32_t argument_lem = 0;
	uint8_t executable_file[128];
	uint8_t command_args[128];
	uint32_t esp, ebp;
	uint8_t USER_EIP[4];
	uint8_t ELF_check_bytes[4];


	if (command == NULL)
		return -1;
	//store the esp and ebp
	asm volatile (
	    "movl %%esp, %0		\n 		\
        		 movl %%ebp, %1"
	    :"=r"(esp), "=r"(ebp)
	    :
	    :"memory"
	);

	for (k = 0; k < BUF_LEN; k++) {
		executable_file[k] = '\0';
		command_args[k] = '\0';
	}
	if (command[0] == '\0' || command[0] == ' ')
		return -1;

	while (command[command_len] != '\0' && command[command_len] != ' ' && command[command_len] != '\n') {
		executable_file[command_len] = command[command_len];
		command_len++;
	}

	executable_file[command_len] = '\0';

	if (command_len > BUF_LEN)
		return -1;

	command_len++;

	//store the argument
	while (command[command_len + argument_lem] != '\0' && command[command_len + argument_lem] != '\n') {
		command_args[argument_lem] = command[command_len + argument_lem];
		argument_lem++;
	}

	if (argument_lem > 32)
		return -1;

	//chech whether it is executable
	file_type = read_dentry_by_name(executable_file, &temp);
	if (file_type == -1)
		return -1;

	//check if it is ELF
	file_type = read_data(temp.inode_num, 0, ELF_check_bytes, 4);
	if (file_type == -1)

		return -1;

	if (ELF_check_bytes[0] != 0x7f || ELF_check_bytes[1] != 0x45 || ELF_check_bytes[2] != 0x4c || ELF_check_bytes[3] != 0x46)
		return -1;
	//handling maximum number of shells
	if (pid == 5)

		return -1;

	if (sche_ready == 1) {
		temp_pid = terminal_process_info[current_terminal_id];
		int i;
		for (i = 0; i < 7; i++) {
			if (process_state[i] == 0) {
				process_state[i] = 1;
				pid = i;
				break;
			}
		}
		terminal_process_info[current_terminal_id] = pid;
	}
	else {
		temp_pid = terminal_process_info[terminal_id];
		int i;
		for (i = 0; i < 7; i++) {
			if (process_state[i] == 0) {
				process_state[i] = 1;
				pid = i;
				break;
			}
		}
		terminal_process_info[terminal_id] = pid;
	}
	sche_ready = 0;

	//set up paging
	uint32_t addr = BA_MB + F_MB * pid;
	switch_pd(addr);

	read_data(temp.inode_num, 0, (uint8_t *) USER_PROGRAM_ADDR, F_MB - USER_PROGRAM_OFF);
	read_data(temp.inode_num, 24, USER_EIP, 4);

	for (i = 0; i < 4; i++) {
		eip |= (USER_EIP[i] << (i * 8)); //bits 27-24
	}
	//get the pcb with corressponding kernel address
	pcb_t * create_process = (pcb_t *) (BA_MB - BA_KB * (pid + 1)); //get the current pcb
	construct_pcb_entries(create_process, temp_pid);

	//getting the arg
	for (i = 0; i < argument_lem; i++) {
		create_process->pcb_exe_args[i] = command_args[i];
	}
	for (i = argument_lem; i < 32; i++) {
		create_process->pcb_exe_args[i] = '\0';
	}
	create_process->pcb_arg_size = argument_lem;

	//store the parent esp and ebp
	create_process->esp = esp;
	create_process->ebp = ebp;


	//store the return address for halt
	asm volatile (

	    "leal halt_ret, %%eax		\n 	 	\
        		 movl %%eax, %0"
	    :"=m"(create_process->return_add)
	    :
	    :"eax", "memory"
	);


	//set the ss0 and esp0
	tss.ss0 = KERNEL_DS;
	tss.esp0 = BA_MB - BA_KB * pid - 4;

	asm volatile (
	    "movw %0, %%ax 				\n 		\
		 movw %%ax, %%ds 			\n 		\
		 movw %%ax, %%es 			\n 		\
		 movw %%ax, %%fs 			\n 		\
		 movw %%ax, %%gs 			\n 		\
		 pushl %0				\n 		\
		 pushl %1 	\n 		\
		 pushfl 				\n  	\
		 popl %%eax 			\n 		\
		 orl %2, %%eax 			\n 		\
		 pushl %%eax 			\n 		\
		 pushl %3 		\n 		\
		 pushl %4 	 					"
	    :
	    :"i"(USER_DS), "i"(USER_PROGRAM_ESP), "r"(IRET_MASK), "i"(USER_CS), "r"(eip)
	    :"memory", "cc", "eax");


	asm volatile("iret");

	//halt will jump to here
	asm volatile("halt_ret:");

	return global_return_status;
}


/*
*   void construct_pcb_entries(pcb_t * temp)
*   Inputs: current pcb
*   Return Value: none
*	Function: build the pcb
*/

void construct_pcb_entries(pcb_t * pcb_pointer, int new_pid) {

	pcb_pointer->PID = pid; //store the current pid


	//store the parent's esp0 and ss0
	pcb_pointer->esp0 = tss.esp0;
	pcb_pointer->ss0 = tss.ss0;
	pcb_pointer->file_array[1].f_op = (int32_t *) terminal_file_op;
	pcb_pointer->file_array[1].inode = 0;
	pcb_pointer->file_array[1].file_position = 0;
	pcb_pointer->file_array[1].flags = 1;
	pcb_pointer->file_array[0].f_op = (int32_t *) terminal_file_op;
	pcb_pointer->file_array[0].inode = 0;
	pcb_pointer->file_array[0].file_position = 0;
	pcb_pointer->file_array[0].flags = 1;


	//store parent's pid
	pcb_pointer->parent = new_pid;

}


/*
*   int32_t read(int32_t fd, uint8_t * buf, int32_t nbytes)
*   Inputs: fd, buffer and number of bytes to be read
*   Return Value: return each function on success and -1 on failure
*	Function: call the rtc/filesystem/terminal read function
*/

int32_t read(int32_t fd, uint8_t * buf, int32_t nbytes) {
	// function * function_read;
	sti();
	if (fd > MAX_FD_NUM || fd < 0 || fd == 1 || buf == NULL) 	//check if it is a valid fd, and it cannot do stdout when read
		return -1;

	if ((get_pcb_addr())->file_array[fd].flags == 0) 	//check if it is in-use
		return -1;
	return ((func *)((get_pcb_addr())->file_array[fd].f_op[0]))((int32_t) fd, (uint8_t *) buf, (int32_t) nbytes);		//call the read function of correspond file_type

}

/*
*   int32_t write(int32_t fd, const uint8_t * buf, int32_t nbytes)
*   Inputs: fd, buffer and number of bytes to be written
*   Return Value: return each function on success and -1 on failure
*	Function: call the rtc/filesystem/terminal write function
*/

int32_t write(int32_t fd, const uint8_t * buf, int32_t nbytes) {

	if (fd > MAX_FD_NUM || fd < 0 || fd == 0 || buf == NULL) 	//check if it is a valid fd, and it cannot do stdin when write
		return -1;

	if ((get_pcb_addr())->file_array[fd].flags == 0)		//check if it is in-use
		return -1;
	return ((func *)((get_pcb_addr())->file_array[fd].f_op[1]))((int32_t) fd, (uint8_t *) buf, (int32_t) nbytes);		//call the write function of correspond file_type

}

/*
*   int32_t open(const uint8_t * filename)
*   Inputs: the filename
*   Return Value: return fd number on success and -1 on failure
*	Function: call open function according to file type
*/

int32_t open(const uint8_t * filename) {

	int32_t fd;
	dentry_t temp;
	// function * function_open;

	if (filename == NULL)
		return -1;
	for (fd = MIN_FD_NUM; fd < MAX_FILE_NUM; fd++) {
		if ((get_pcb_addr())->file_array[fd].flags == 0)
			break;
	}

	if (fd == MAX_FILE_NUM)
		return -1;
	uint32_t file_type;
	file_type = read_dentry_by_name(filename, &temp);	//check if it is a valid filename

	if (file_type == -1)	//not a valid filename
		return -1;

	//initialization of different file_type
	switch (temp.file_type) {
	case 0:
		(get_pcb_addr())->file_array[fd].f_op = (int32_t *) rtc_file_op;
		(get_pcb_addr())->file_array[fd].inode = 0;
		(get_pcb_addr())->file_array[fd].file_position = 0;
		(get_pcb_addr())->file_array[fd].flags = 1;
		((func *)((get_pcb_addr())->file_array[fd].f_op[2]))((uint8_t *) filename);		//call the open function of the corresponding file_type
		return fd;

	case 1:
		(get_pcb_addr())->file_array[fd].f_op = (int32_t *) directory_file_op;
		(get_pcb_addr())->file_array[fd].inode = 0;
		(get_pcb_addr())->file_array[fd].file_position = 0;
		(get_pcb_addr())->file_array[fd].flags = 1;
		((func *)((get_pcb_addr())->file_array[fd].f_op[2]))((uint8_t *) filename);		//call the open function of the corresponding file_type
		return fd;

	case 2:
		(get_pcb_addr())->file_array[fd].f_op = (int32_t *) file_file_op;
		(get_pcb_addr())->file_array[fd].inode = temp.inode_num;
		(get_pcb_addr())->file_array[fd].file_position = 0;
		(get_pcb_addr())->file_array[fd].flags = 1;
		((func *)((get_pcb_addr())->file_array[fd].f_op[2]))((uint8_t *) filename);		//call the open function of the corresponding file_type
		return fd;

	default:
		return -1;
	}

}



/*
*   int32_t close(int32_t fd)
*   Inputs: the fd number
*   Return Value: return fd number on success and -1 on failure
*	Function: call close function on the given fd number
*/

int32_t close(int32_t fd) {

	// function * function_close;

	if (fd < 2 || fd > MAX_FD_NUM)		//check if it is a valid fd
		return -1;

	if ((get_pcb_addr())->file_array[fd].flags == 0) 	//check if it is in-use
		return -1;

	((func *)((get_pcb_addr())->file_array[fd].f_op[3]))((int32_t) fd);
	// printf("hello\n");
	(get_pcb_addr())->file_array[fd].file_position = 0;	//reset the file_position
	(get_pcb_addr())->file_array[fd].flags = 0;		//reset the flags
	(get_pcb_addr())->file_array[fd].f_op = NULL;		//reset the f_op


	return 0;		//successfully return
}

/*
*   int32_t getargs(uint8_t * buf, int32_t nbytes)
*   Inputs: buffer and number of bytes
*   Return Value: return 0 on success and -1 on failure
*	Function: get the argument from the keyboard buffer
*/
int32_t getargs(uint8_t * buf, int32_t nbytes) {

	if (buf == NULL)
		return -1;
	if ((get_pcb_addr())->pcb_arg_size > nbytes)					//check the arg size
		return -1;
	int i;
	for (i = 0; i < BUFFER_SIZE; i++)							//reinitialize the buffer
		buf[i] = '\0';
	for (i = 0; i < (get_pcb_addr())->pcb_arg_size; i++)			//put the arg into the buffer
		buf[i] = (get_pcb_addr())->pcb_exe_args[i];
	return 0;													//return 0 on success
}

/*
*   int32_t vidmap(uint8_t ** screen_start)
*   Inputs: where the video_mem start
*   Return Value: return 0 on success and -1 on failure
*	Function: map video memory to other virtual address
*/
int32_t vidmap(uint8_t ** arg) {

	if (arg < (uint8_t **) USER_PROGRAM_PAGING || arg >= (uint8_t **) USER_PROGRAM_END)
		return -1;												//check the screen address
	switch_video_pd(VIDEO_MEM + F_KB * 2 * current_terminal_id, current_terminal_id);										//map video mem to virtual address
	(*arg) = (uint8_t *) ((0xB8 + current_terminal_id * 2) * F_KB); //assign the address to screen start

	return 0;													//return 0 on success

}

int32_t set_handler(int32_t signum, void * arg) {
	return -1;
}

int32_t sigreturn(void) {
	return -1;
}
