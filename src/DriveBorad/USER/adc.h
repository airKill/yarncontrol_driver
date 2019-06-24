#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

#define ADC_CURRENT_CHANNEL       ADC_Channel_0

#define SAMPLE_CURRENT_MAX 0.8
#define A_SAMPLE_MAXCOUNT 10

extern u8 A_sample_count;
extern float value_current;
extern u8 isCurrentOver;
extern u8 old_isCurrentOver;

void Adc_Init(void);
u16 Get_Adc(u8 ch);
u16 Average_filter(void);
#endif
