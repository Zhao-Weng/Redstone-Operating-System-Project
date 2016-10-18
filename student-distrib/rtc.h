#ifndef _RTC_H
#define _RTC_H
#include "types.h"



extern void rtc_initialization();
int32_t rtc_open(const uint8_t* filename);
int32_t rtc_close(int32_t fd);
int32_t rtc_read(int32_t fd, int32_t * buf, int32_t nbytes);
int32_t rtc_write(int32_t fd,const uint32_t * buf, int32_t nbytes);

#endif



