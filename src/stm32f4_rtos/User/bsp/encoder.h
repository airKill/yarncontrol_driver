#ifndef _ENCODER_H_
#define _ENCODER_H

#define RCC_ENCODER 	        (RCC_AHB1Periph_GPIOC)
#define GPIO_PORT_ENCODERA      GPIOC
#define GPIO_PIN_ENCODERA	 GPIO_Pin_7

#define GPIO_PORT_ENCODERB      GPIOC
#define GPIO_PIN_ENCODERB	 GPIO_Pin_6

#define EXTI_PortEncoder            EXTI_PortSourceGPIOC
#define EXTI_SourceEncoder      EXTI_PinSource7
#define EXTI_LineEncoder         EXTI_Line7

#define ENCODER_LINE    600

#define TempLen 10

#define filter_num 12

extern float Freq_value;
extern u16 main_speed;
extern u8 Freq_ptr1;      //滤波计数值1
extern u8 Freq_ptr2;      //溢出计数值2

u16 recursive_average_filter(u16 filter_object);
void Encoder_Cap_Init(void);
u8 Freq_Sample(void);
#endif