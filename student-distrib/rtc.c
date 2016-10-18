#include "i8259.h"
#include "lib.h"
#include "systemcall.h"


#define REG_C_OFF 0x0C
#define NMI_PORT 0x70
#define RTC_RAM 0x71
#define REG_B_ON 0x40
#define REG_A 0x8A
#define REG_B 0x8B
#define REG_C 0x8C
#define RTC_IRQ 0x28


volatile int busy_rtc;

/*
*   void rtc_initialization
*   Inputs: none		
*   Return Value: none
*	Function: initialize the rtc and set the interrupt rate
*/
void rtc_initialization(void){
	//mask the interrupt during initialization
	//cli();
	outb(REG_B, NMI_PORT);	//select Register B, and disable NMI
	char prev = inb(RTC_RAM);	//read the current value of Register B
	outb(REG_B, NMI_PORT);	//reset the index again
	outb(prev | REG_B_ON, RTC_RAM);	//write the previous value ORed with 0x40. This turns on bit 6 of Register B
	//set the rate of interrupt to 4Hz
	int rate = 15; 
	outb(REG_A, NMI_PORT);
	prev = inb(RTC_RAM);
	outb(REG_A, NMI_PORT);
	outb((prev & 0xF0) | rate, RTC_RAM);
	//sti();
	enable_irq(RTC_IRQ);
}


/*
*   void rtc_handler(void);
*   Inputs: void
*   Return Value: none
*   Function: real time clock
*/
void rtc_handler(void){

	outb(REG_C_OFF, NMI_PORT); 	//select Register C
	inb(RTC_RAM);	//throw away the content
	if(busy_rtc) 
		busy_rtc = 0;	//next interrupt occurred, set flag to 0
	send_eoi(8);	//send EOI with RTC IRQ number: 8

}

int32_t rtc_open(const uint8_t* filename){
	rtc_initialization();
	return 0;
}

int32_t rtc_close(int32_t fd){
	disable_irq(RTC_IRQ);//disable rtc
	return 0;
}



int32_t rtc_read(int32_t fd, int32_t * buf, int32_t nbytes){
	busy_rtc=1;
	while(busy_rtc);
	return 0;
}

int32_t rtc_write(int32_t fd,const uint32_t * buf, int32_t nbytes){
	int rate;
	switch(*buf){
		case 2:
			rate = 15;
			break;
		case 4:
			rate = 14;
			break;
		case 8:
			rate = 13;
			break;
		case 16:
			rate = 12;
			break;
		case 32:
			rate = 11;
			break;
		case 64:
			rate = 10;
			break;
		case 128:
			rate = 9;
			break;
		case 256:
			rate = 8;
			break;
		case 512:
			rate = 7;
			break;
		case 1024:
			rate =6;
			break;
		default:
			return -1;
	}

	
	//disable_irq(0x28);
	outb(0x8A, 0x70);
	char pre = inb(0x71);
	outb(0x8A, 0x70);
	outb((pre & 0xF0) | rate , 0x71);
	//enable_irq(0x28);
	return 0;
}



