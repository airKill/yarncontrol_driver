#ifndef _BSP_SW_ONOFF_H_
#define _BSP_SW_ONOFF_H

#define RCC_SW_ONOFF 	        (RCC_AHB1Periph_GPIOA)
#define GPIO_PORT_SW_ONOFF      GPIOA
#define GPIO_PIN_SW_ONOFF	 GPIO_Pin_9    //UART1 TX作为开机输入信号GPIO

#define Read_SW_ONOFF()    GPIO_ReadInputDataBit(GPIO_PORT_SW_ONOFF,GPIO_PIN_SW_ONOFF)

void bsp_Init_SW_ONOFF(void);
u8 GetDeviceState(void);
#endif