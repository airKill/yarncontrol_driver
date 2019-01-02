
/**
  ****************** (C) COPYRIGHT 2017 Nova  *******************
  * @file     drv_adc.h
  * @auther   sunzhen
  * @version  V1.0.0
  * @date     2017.3.10
  * @brief    This file provides the functions of drv_adc.
  *
  ***************************************************************
  * @attention
  *
  * History<br>
  * version   date            auther         description<br>
  * v1.0.0    2017.3.10       sunzhen        Initial Version<br>
  ***************************************************************
  */


#ifndef __DRV_ADC_H_
#define __DRV_ADC_H_


#ifdef __cplusplus
 extern "C" {
#endif

#include "sys_type.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
   

#define _ADC1_ADCx                        ADC1
#define _ADC1_CHANNEL                     0
#define _ADC1_GPIOx                       GPIOA
#define _ADC1_GPIO_PINx                   GPIO_Pin_0
#define _ADC1_APB2Periph_GPIOx            RCC_APB2Periph_GPIOA
#define _ADC1_APB2Periph_ADCx             RCC_APB2Periph_ADC1

#define _ADC2_ADCx                        ADC1
#define _ADC2_CHANNEL                     1
#define _ADC2_GPIOx                       GPIOA
#define _ADC2_GPIO_PINx                   GPIO_Pin_1
#define _ADC2_AHB1Periph_GPIOx            RCC_AHB1Periph_GPIOA
#define _ADC2_APB2Periph_ADCx             RCC_APB2Periph_ADC1

#define _ADC3_ADCx                        ADC1
#define _ADC3_CHANNEL                     2
#define _ADC3_GPIOx                       GPIOA
#define _ADC3_GPIO_PINx                   GPIO_Pin_2
#define _ADC3_AHB1Periph_GPIOx            RCC_AHB1Periph_GPIOA
#define _ADC3_APB2Periph_ADCx             RCC_APB2Periph_ADC1

#define _ADC4_ADCx                        ADC1
#define _ADC4_CHANNEL                     3
#define _ADC4_GPIOx                       GPIOA
#define _ADC4_GPIO_PINx                   GPIO_Pin_3
#define _ADC4_AHB1Periph_GPIOx            RCC_AHB1Periph_GPIOA
#define _ADC4_APB2Periph_ADCx             RCC_APB2Periph_ADC1




#define _ADC_NUMBER_OF_CHANNEL            4
   
typedef enum
{
  EN_ADC1 = 0x00,
  EN_ADC2,
  EN_ADC3,
  EN_ADC4,

}EN_ADC_t;


typedef struct _ADC
{
  EN_ADC_t index;
}_ADC_t;



void ADC_Drv_Init(void);
void ADC_Drv_DeInit(void);
u16 ADC_Drv_GetSampling(EN_ADC_t adcx,u8 channel);
void _ADC_DMA_Configuration(void);





#ifdef __cplusplus
}
#endif

#endif   /* __DRV_UART_H_ */
   


