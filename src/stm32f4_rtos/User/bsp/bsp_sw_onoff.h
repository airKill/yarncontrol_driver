#ifndef _BSP_SW_ONOFF_H_
#define _BSP_SW_ONOFF_H

#define RCC_SW_ONOFF 	        (RCC_AHB1Periph_GPIOC)
#define GPIO_PORT_SW_ONOFF      GPIOC
#define GPIO_PIN_SW_ONOFF	 GPIO_Pin_8
//#define RCC_SW_ONOFF 	        (RCC_AHB1Periph_GPIOA)
//#define GPIO_PORT_SW_ONOFF      GPIOA
//#define GPIO_PIN_SW_ONOFF	 GPIO_Pin_10

#define Read_SW_ONOFF()    GPIO_ReadInputDataBit(GPIO_PORT_SW_ONOFF,GPIO_PIN_SW_ONOFF)

#define RCC_SERVO_ENABLE 	        (RCC_AHB1Periph_GPIOC)
#define GPIO_PORT_SERVO_ENABLE      GPIOC
#define GPIO_PIN_SERVO_ENABLE	 GPIO_Pin_8
#define SERVO_ENABLE()    GPIO_WriteBit(GPIO_PORT_SERVO_ENABLE,GPIO_PIN_SERVO_ENABLE,Bit_SET)
#define SERVO_DISABLE()   GPIO_WriteBit(GPIO_PORT_SERVO_ENABLE,GPIO_PIN_SERVO_ENABLE,Bit_RESET)

void bsp_Init_SW_ONOFF(void);
u8 GetDeviceState(void);
void bsp_Init_SEVRO_EN(void);
#endif