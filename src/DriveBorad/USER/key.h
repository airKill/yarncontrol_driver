#ifndef __KEY_H
#define __KEY_H

//#include "sys.h"	  
#include "stm32f10x.h"

#define KEY1_PORT       GPIOB
#define KEY1_PIN        GPIO_Pin_4

#define KEY_PORT       GPIOB
#define KEY_PIN        GPIO_Pin_13

#define READ_KEY()    GPIO_ReadInputDataBit(KEY_PORT,KEY_PIN)

void Key_init(void);
u8 readKey(void);
void ButtonCheck(void);

#endif