#ifndef _MALLOC_H
#define _MALLOC_H

void init_dynamic_memory();
uint32_t * malloc(uint32_t size);
uint32_t free(uint32_t * arg);

#endif
