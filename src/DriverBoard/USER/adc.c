#include "main.h"

float value_current = 0.0;

u8 isCurrentOver = 0;
u8 old_isCurrentOver = 0;
u8 A_sample_count = 0;

void Adc_Init(void)  
{     
  ADC_InitTypeDef ADC_InitStructure;   
  GPIO_InitTypeDef GPIO_InitStructure;  
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
  
  RCC_ADCCLKConfig(RCC_PCLK2_Div2);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  ADC_DeInit(ADC1);
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_28Cycles5 );
  
  ADC_Cmd(ADC1, ENABLE);

  ADC_ResetCalibration(ADC1);
  
  while(ADC_GetResetCalibrationStatus(ADC1));
  
  ADC_StartCalibration(ADC1);
  
  while(ADC_GetCalibrationStatus(ADC1));
}         

u16 Get_Adc(u8 ch)
{
  ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_28Cycles5 );
  
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
  
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
  
  return ADC_GetConversionValue(ADC1);
}

u16 Average_filter(void)
{
  u8 count;
  u16 sum = 0;
  for(count=0;count<A_SAMPLE_MAXCOUNT;count++)
  {
    sum += Get_Adc(ADC_CURRENT_CHANNEL);
    vTaskDelay(2);
  }
  return (sum / A_SAMPLE_MAXCOUNT);
}

