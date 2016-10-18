#include "soundblaster.h"
#include "lib.h"


#define RESET_PORT 0x6
#define READ_BUFFER_STATUS_PORT 0xE
#define READ_DATA_PORT 0xA
#define WRITE_DATA_PORT 0xC
#define ACK_PORT 0xF
#define BASE_PORT 0x220

#define SAMPLEING_OUTPUT 0x41


#define pagechannel_0  0x87
#define pagechannel_1  0x83
#define pagechannel_2  0x81
#define pagechannel_3  0x82
#define pagechannel_5  0x8B
#define pagechannel_6  0x89
#define pagechannel_7  0x8A


#define DMAC1_SINGLE_MASK 0xA
#define DMAC2_SINGLE_MASK 0x6




sound_blaster_outb(uint8_t data, uint16_t port){
	outb(data, port+ BASE_PORT );
}
sound_blaster_inb(uint16_t port){
	return inb(BASE_PORT + port);
}





// Write a 1 to the reset port (2x6)
// Wait for 3 microseconds
// Write a 0 to the reset port (2x6)
// Poll the read-buffer status port (2xE) until bit 7 is set
// Poll the read data port (2xA) until you receive an AA
// The DSP usually takes about 100 microseconds to initialized itself. After this period of time, 
// if the return value is not AA or there is no data at all, 
// then the SB card may not be installed or an incorrect I/O address is being used.
int DSP_Reset(){
	int i;
	uint16_t read;
	outb(1, RESET_PORT);
	i = 10050;
	while( i > 0){
		i--;
	}
	outb(0, RESET_PORT);
	volatile int j = 1;
	//prevent compiler falsely optimization
	while(j){
		read = sound_blaseter_inb(READ_BUFFER_STATUS_PORT);
		if(read == 0x7) break;
	}	
	while(j){
		read = sound_blater_inb(READ_DATA_PORT);
		if(read == 0xAA) break;
	}
	return 0;
 
}



// To write a byte to the SB16, the following procedure should be used:

//     Read the write-buffer status port (2xC) until bit 7 is cleared
//     Write the value to the write port (2xC) 

void DSP_Write(){
	volatile int j;
	uint16_t read;
	while(j){
		read = sound_blaster_inb(WRITE_BUFFER_STATUS_PORT);
		if(read == 0x7) break ;
	}
	sound_blaster_outb(read, WRITE_DATA_PORT);


}

// To read a byte from the SB16, the following procedure should be used:

//     Read the read-buffer status port (2xE) until bit 7 is set
//     Read the value from the read port (2xA) 

uint16_t DSP_Read(){
	volatile int j;
	uint16_t read;
	while(j){
		read = sound_blaster_inb(READ_BUFFER_STATUS_PORT);
		if(read == 0x7) break;
	}
	return sound_blaster_inb(READ_DATA_PORT);

}

int transfer_length(int channel,uint16_t tranfer_len){
	
}


int write_pageport(int channel, uint16_t linear_addr){
linear_addr = linear_addr / 65536;
	switch(channel){
	case 0:
	{
		sb_outb(linear_addr,pagechannel_0);
		return 0;
	}
	case 1:
	{
		sb_outb(linear_addr,pagechannel_1);
		return 0;
	}
	case 2:
	{
		sb_outb(linear_addr,pagechannel_2);
		return 0;
	}
	case 3:
	{
		sb_outb(linear_addr,pagechannel_3);
		return 0;
	}
	case 5:
	{
		sb_outb(linear_addr,pagechannel_5);
		return 0;
	}
	case 6:
	{
		sb_outb(linear_addr,pagechannel_6);
		return 0;
	}
	case 7:
	{
		sb_outb(linear_addr,pagechannel_7);
		return 0;
	}
	default:
	{
		return -1;
	}
 }
}


int transferlength(int channel,uint16_t tranfer_len)
{
	tranfer_len --;
	uint8_t lo_len = (tranfer_len & 0x00FF);
	uint8_t hi_len = (tranfer_len & 0xFF00)>>8;
	switch(channel){
	case 0:
	
		sb_outb(lo_len,countchannel_0);
		sb_outb(hi_len,countchannel_0);
		return 0;
	
	case 1:
	
		sb_outb(lo_len,countchannel_1);
		sb_outb(hi_len,countchannel_1);
		return 0;
	
	case 2:
	
		sb_outb(lo_len,countchannel_2);
		sb_outb(hi_len,countchannel_2);
		return 0;
	
	case 3:
	
		sb_outb(lo_len,countchannel_3);
		sb_outb(hi_len,countchannel_3);
		return 0;
	
	case 4:
	
		sb_outb(lo_len,countchannel_4);
		sb_outb(hi_len,countchannel_4);
		return 0;
	
	case 5:
	
		sb_outb(lo_len,countchannel_5);
		sb_outb(hi_len,countchannel_5);
		return 0;
	
	case 6:
	
		sb_outb(lo_len,countchannel_6);
		sb_outb(hi_len,countchannel_6);
		return 0;
	
	case 7:
	
		sb_outb(lo_len,countchannel_7);
		sb_outb(hi_len,countchannel_7);
		return 0;
	
	default:return -1;}
	
		
	
}



// Disable the sound card DMA channel by setting the appropriate mask bit

//   Port[MaskPort] := 4 + (Channel mod 4)
int disable_sound_blaster(int channel,int DMA){
	uint8_t maskport_input ;
	//initiliaze the maskpot_input bits
	maskpot_input = 4 + channel % 4;
	if(DMA == 1) {
		sb_outb( maskpot_input, DMA1_mask);
	}
}



















// Unlike earlier Sound Blasters, the SB16 is programmed 
// with actual sampling rates instead of time constants. 
// On the SB16, the sampling rate is set using DSP commands 41h and 42h. 
// Command 41h is used for output and 42h is used for input. 
// I have heard that on the SB16, both these command currently 
// do the same thing, but I would recommend using the individual 
// commands to guarantee compatibility with future sound cards. 
// The procedure for setting the sampling rate is:

//     Write the command (41h for output rate, 42h for input rate)
//     Write the high byte of the sampling rate (56h for 22050 hz)
//     Write the low byte of the sampling rate (22h for 22050 hz) 
void sampling_rate_setting(uint16_t rate){
	uint8_t lo = rate & 0x00FF;
	uint8_t hi = rate & 0xFF00;
	DSP_Write(SAMPLEING_OUTPUT);
	DSP_Write(hi);
	DSP_Write(lo);

}


