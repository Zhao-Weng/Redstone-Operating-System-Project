#include "lib.h"
#include "types.h"

#define DYNAMIC_ADDR_START 0x0800000
#define KB_1 1024
#define KB_4 0x1000

//the internal data structure to keep track of how much memory has been used
uint8_t dynamic_memory_table[KB_4];

/*
 * initi_dynamic_memory
 * input N/A
 * output N/A
 * function: initialize the internal data structure to keep track of memory
 */
void init_dynamic_memory() {
	int i = 0;
	for (i = 0; i < KB_4; i++) {
		//set all dynamic space to be unallocated
		dynamic_memory_table[i] = 0;
	}
}
/*
 * malloc
 * input N/A
 * output N/A
 * function: allocate dynamic memory
 */
uint32_t * malloc(uint32_t size) {
	uint8_t KB_size = (uint8_t)((size / KB_1) + 1);
	int new_index = 0;
	int check_space_index, check_space_flag;
	//looking for next available spot
	while (new_index < KB_4) {
		if (dynamic_memory_table[new_index] != 0) 
			//jump few spot ahead
			new_index += dynamic_memory_table[new_index];
		else {
			//check if we have enough consecutive availble space
			check_space_flag = 0;
			for (check_space_index = new_index; check_space_index < new_index + KB_size; check_space_index++) {
				if (dynamic_memory_table[check_space_index]) check_space_flag = 1;
			}
			if (!check_space_flag)
				break;
		}
	}
	//all the space has been allocated
	if (new_index >= KB_4) return NULL;

	dynamic_memory_table[new_index] = KB_size;

	//calculte the actual address and return to user
	return (uint32_t*)(DYNAMIC_ADDR_START + new_index * KB_1);
}
/*
 * free
 * input N/A
 * output N/A
 * function: free dynamic memory
 */
uint32_t free(uint32_t * arg){
	//calculate the index for array
	int index = (((uint32_t)arg) - DYNAMIC_ADDR_START) / KB_1;
	//mark as freed 
	dynamic_memory_table[index] = 0;
	return 0;
}

