#ifndef _SOUNDBLASTER_H
#define _SOUNDBLASTER_H
#include "types.h"

uint16_t sound_blaster_inb(uint16_t);
uint sound_blaster_oub(uint8_t data, uint16_t port);

int DSP_Reset();
void DSP_Write();
uint16_t DSP_Read();

void sampleing_rate_setting(uint16_t rate);
int transfer_length(int channel,uint16_t tranfer_len);
int DMA_data_transfer(int channel, uint8_t modebit, uint16_t tranfer_len);
int enable_sound_playing(int DMA, int channel);
int disable_sound_playing(int DMA, int channel)

















#endif


