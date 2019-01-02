#ifndef _BSP_CLOSESW_H_
#define _BSP_CLOSESW_H

#define RCC_CLOSE_SW 	        (RCC_AHB1Periph_GPIOC)
#define GPIO_PORT_CLOSE_SW      GPIOC
#define GPIO_PIN_CLOSE_SW	 GPIO_Pin_9

#define EXTI_PortSourceSW   EXTI_PortSourceGPIOC
#define EXTI_PinSourceSW    EXTI_PinSource9
#define EXTI_LineSW         EXTI_Line9

void bsp_InitCloseSW(void);
void ConfigExitNVIC(void);
#endif