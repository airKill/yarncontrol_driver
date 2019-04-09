#ifndef __BSP_ADC_H
#define __BSP_ADC_H	

#include "stm32f4xx.h"

#define GPIO_PORT_ADC_POWER  GPIOB
#define GPIO_PIN_ADC_POWER	GPIO_Pin_1

#define ADC_POWER_CHANNEL       ADC_Channel_9

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//ADC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 
 							   
void Adc_Init(void); 				//ADCͨ����ʼ��
u16  Get_Adc(u8 ch); 				//���ĳ��ͨ��ֵ 
u16 Get_Adc_Average(u8 times);//�õ�ĳ��ͨ����������������ƽ��ֵ  
#endif 















