#ifndef __BSP_ADC_H
#define __BSP_ADC_H	

#include "stm32f4xx.h"

#define GPIO_PORT_ADC_POWER  GPIOB
#define GPIO_PIN_ADC_POWER	GPIO_Pin_1

#define ADC_POWER_CHANNEL       ADC_Channel_9

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//ADC 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 
 							   
void Adc_Init(void); 				//ADC通道初始化
u16  Get_Adc(u8 ch); 				//获得某个通道值 
u16 Get_Adc_Average(u8 times);//得到某个通道给定次数采样的平均值  
#endif 















