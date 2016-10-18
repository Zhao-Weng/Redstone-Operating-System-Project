#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"

//paging initialization
void paging_init();
void switch_pd(uint32_t addr);
void switch_video_pd(uint32_t addr, int terminal_id);

#endif /* _RTC_H */
