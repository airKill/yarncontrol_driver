/**
  ****************** (C) COPYRIGHT 2017 Nova  *******************
  * @file     drv_adc.c
  * @auther   sunzhen
  * @version  V1.0.0
  * @date     2017.3.10
  * @brief    This file provides the functions of drv_adc.
  *
  ***************************************************************
  * @attention
  *
  * History<br>
  * version   date            auther        description<br>
  * v1.0.0    2017.3.10       sunzhen       Initial Version<br>
  ***************************************************************
  */



#ifdef __cplusplus
extern "C" {
#endif

#include "drv_adc.h"

  
vu16 AD_Value[_ADC_NUMBER_OF_CHANNEL];

/************************************************************************
** Name   : ADC_Drv_Init
** brief  : ADC driver initialization.
** input  : None
** output : None
** Return : None
************************************************************************/
void ADC_Drv_Init(void)
{
  ADC_InitTypeDef   ADC_InitStructure;
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(_ADC1_APB2Periph_GPIOx | _ADC1_APB2Periph_ADCx, ENABLE);
  RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
  
  /* Configure ADC pin as analog input ******************************/
  GPIO_InitStructure.GPIO_Pin = _ADC1_GPIO_PINx|_ADC2_GPIO_PINx|_ADC3_GPIO_PINx|_ADC4_GPIO_PINx;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;

  GPIO_Init(_ADC1_GPIOx, &GPIO_InitStructure);

  ADC_DeInit(ADC1);

  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = _ADC_NUMBER_OF_CHANNEL;
  ADC_Init(ADC1, &ADC_InitStructure);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5 );
  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5 );
  ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_239Cycles5 );
  ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_239Cycles5 );

  ADC_DMACmd(ADC1, ENABLE);

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
  /* Enable ADC1 reset calibaration register */
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));

  _ADC_DMA_Configuration();
  
  /* Start ADC1 Software Conversion */
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);  
  DMA_Cmd(DMA1_Channel1, ENABLE);
  
}



/************************************************************************
** Name   : ADC_Drv_DeInit
** brief  : ADC driver deinitialization.
** input  : None
** output : None
** Return : None
************************************************************************/
void ADC_Drv_DeInit(void)
{
 // ADC_DeInit();
}


/************************************************************************
** Name   : ADC_Drv_GetSampling
** brief  : get a sampling via the adc.
** input  : EN_ADC_t adcx: object of a adc.
** output : None
** Return : None
************************************************************************/
u16 ADC_Drv_GetSampling(EN_ADC_t adcx,u8 channel)
{

  return AD_Value[channel];
}






/*ADC DMA config*/
void _ADC_DMA_Configuration(void)
{
  /* ADC1 DMA1 Channel Config */
  DMA_InitTypeDef DMA_InitStructure;
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)AD_Value;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = _ADC_NUMBER_OF_CHANNEL;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
}



#ifdef __cplusplus
}
#endif



