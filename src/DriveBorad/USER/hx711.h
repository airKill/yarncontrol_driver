#ifndef __HX711_H__
#define __HX711_H__

#include "stm32f10x.h"

#define HX711_SCK_GPIO_PIN                 GPIO_Pin_1
#define HX711_SCK_GPIO                     GPIOB

#define HX711_DT_GPIO_PIN                 GPIO_Pin_0
#define HX711_DT_GPIO                     GPIOB

#define HX711_SCK_H()     GPIO_SetBits(HX711_SCK_GPIO,HX711_SCK_GPIO_PIN)
#define HX711_SCK_L()     GPIO_ResetBits(HX711_SCK_GPIO,HX711_SCK_GPIO_PIN)

#define READ_HX711_DT()    GPIO_ReadInputDataBit(HX711_DT_GPIO,HX711_DT_GPIO_PIN)
#define HX711_DT_DOUT_H()  GPIO_SetBits(HX711_DT_GPIO,HX711_DT_GPIO_PIN) 

#define WEIGHT_XISHU  (500.0 / 523.0)

#define MAXCOUNT 12

extern unsigned long hx711_buf[MAXCOUNT];
extern unsigned long hx711_sum;

void hx711_init(void);
unsigned long Read_HX711_Count(void);
void delay_hx711(u16 us);
long get_pizhong(void);
void hx711_buf_Init(void);
long double hx711_filter(void);
#endif