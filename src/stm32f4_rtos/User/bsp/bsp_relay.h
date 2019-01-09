#ifndef _BSP_RELAY_H_
#define _BSP_RELAY_H

#include "stm32f4xx.h"

#define GPIO_PORT_RELAY  GPIOB
#define GPIO_PIN_RELAY	GPIO_Pin_12

#define RELAY_CLOSE()    GPIO_WriteBit(GPIO_PORT_RELAY,GPIO_PIN_RELAY,Bit_SET)
#define RELAY_OPEN()   GPIO_WriteBit(GPIO_PORT_RELAY,GPIO_PIN_RELAY,Bit_RESET)
void bsp_Init_Relay(void);
#endif