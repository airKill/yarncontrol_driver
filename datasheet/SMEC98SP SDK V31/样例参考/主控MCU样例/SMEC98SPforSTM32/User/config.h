#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "stm32f10x.h"
#include "Types.h"
//=================================================
// Port define

// Port A
#define LED1				GPIO_Pin_3	// out
#define LED2				GPIO_Pin_4	// out
#define LED3				GPIO_Pin_5	// out
#define LED4				GPIO_Pin_6	// out

#define LED_PORT		GPIOA

#define K1					GPIO_Pin_7
#define K_PORT			GPIOA



#define USART1_TX		        GPIO_Pin_9	// out
#define USART1_RX		        GPIO_Pin_10	// in 




#define __GNUC__



//#define ApplicationAddress 	0x08004000


#endif

