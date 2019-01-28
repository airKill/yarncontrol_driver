#ifndef _ENCODER_H_
#define _ENCODER_H

#define RCC_ENCODER 	        (RCC_AHB1Periph_GPIOC)
#define GPIO_PORT_ENCODER      GPIOC
#define GPIO_PIN_ENCODER	 GPIO_Pin_7

#define EXTI_PortEncoder            EXTI_PortSourceGPIOC
#define EXTI_SourceEncoder      EXTI_PinSource7
#define EXTI_LineEncoder         EXTI_Line7

#define ENCODER_LINE    600

#define TempLen 10

extern float Freq_value;
extern u16 main_speed;

void Encoder_Cap_Init(void);
void Freq_Sample(void);
#endif