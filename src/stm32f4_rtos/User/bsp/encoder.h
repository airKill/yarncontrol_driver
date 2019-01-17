#ifndef _ENCODER_H_
#define _ENCODER_H

#define RCC_ENCODER 	        (RCC_AHB1Periph_GPIOC)
#define GPIO_PORT_ENCODER      GPIOC
#define GPIO_PIN_ENCODER	 GPIO_Pin_7

#define EXTI_PortEncoder            EXTI_PortSourceGPIOC
#define EXTI_SourceEncoder      EXTI_PinSource7
#define EXTI_LineEncoder         EXTI_Line7

void bsp_Init_encoder(void);

#endif