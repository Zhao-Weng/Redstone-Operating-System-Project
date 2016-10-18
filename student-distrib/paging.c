#include "paging.h"

#define ONEKB 1024	
#define FOURKB 4096	
#define not_present 0x00000002	
#define present 0x00000003	
#define FOURMB_USER 0x87
#define FOURMB_PRESENT 0x83
#define video 184	//video memory
#define USER_PAGE 32
#define VIDEO_DIR_OFFSET 37
#define USER_not_present 0x07

uint32_t pd_array[ONEKB] __attribute__((aligned(FOURKB)));
uint32_t kernel_page_table[ONEKB] __attribute__((aligned(FOURKB)));

/*
*   void paging_initialization
*   Inputs: none
*   Return Value: none
*	Function: initialize the paging, map video memory and kernel page
*/
void switch_pd(uint32_t addr){

	pd_array[USER_PAGE] = addr | FOURMB_USER; 	//set to user level
	int32_t cr3;
	//TLB flusH
	asm volatile (												
 		"movl %%cr3, %0		\n	 	 	\
 		 movl %0, %%cr3"					\
 		: 	"=r"(cr3)							\
 		: 	"r"(cr3)								\
 		: "memory", "cc");			


}



/*
*   void switch_video_pd(uint32_t addr, int terminal_id)
*   Inputs: the address to map, and the offset to the displaying terminal	
*   Return Value: none
*	Function: map video meory paging to the corresponding physical address
*/
void switch_video_pd(uint32_t addr, int terminal_id){
	//set video paging with user level access
	kernel_page_table[video + terminal_id * 2] = addr | USER_not_present; 	
	int32_t cr3;
	//TLB flusH
	asm volatile (												
 		"movl %%cr3, %0		\n	 	 	\
 		 movl %0, %%cr3"					\
 		: 	"=r"(cr3)							\
 		: 	"r"(cr3)								\
 		: "memory", "cc");			

		
		
		
}

/*
*   void paging_init()
*   Inputs: none		
*   Return Value: none
*	Function: initialize data structure related to paging
*/
void paging_init(){

 	int i;	
 	for(i = 0; i < ONEKB; i++){
 		pd_array[i] = not_present;
 		if(i == video)
 			kernel_page_table[i] = (i*FOURKB) | 0x07;
 		else
 			kernel_page_table[i] = (i*FOURKB) | not_present;
 	}
 	//set up dynamic allocated space paging
	pd_array[2] = 2 * ONEKB * FOURKB | FOURMB_USER; 	
	
 	//set up kernel space video paging
 	pd_array[0] = ((unsigned int)kernel_page_table) | 0x07;

 	//set up kernel space paging
 	pd_array[1] = ONEKB * FOURKB | FOURMB_PRESENT; 

	//set up CR0 CR3 CR4
 	asm volatile (
 		"movl $pd_array, %%eax 	\n 	\
 		movl %%eax, %%cr3 				\n	\
 		movl %%cr4, %%eax				\n	\
 		orl $0x00000010, %%eax			\n	\
 		movl %%eax, %%cr4				\n	\
 		movl %%cr0, %%eax				\n	\
 		orl $0x80000000, %%eax			\n	\
 		movl %%eax, %%cr0"					\
 		: 									\
 		: "g"(pd_array)				\
 		: "memory", "cc", "eax");			

 
 }
