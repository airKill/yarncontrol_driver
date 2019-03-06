#include "bsp.h"

uint16_t SRC_Buffer[10] = {2000};

void TIM4_CH1_ConfigPwmOut(u32 freq,u16 num)
{
  uint16_t period;
  //  period = SystemCoreClock / TIM_Prescaler / freq / 2; 
  period = freq;
  TIM4_CH1_GPIO_Configuration();
  TIM4_CH1_PWMDMA_Config(period,num);
}

void TIM4_CH1_StartPwmOut(void)
{
  TIM_DMACmd(TIM4,TIM_DMA_CC1,ENABLE);
  TIM4->CCER |= 1<<0; //?aTME4 PWMê?3?
//  TIM_Cmd(TIM4,ENABLE);
  DMA_Cmd(DMA1_Stream0,ENABLE); 
}

void TIM4_CH1_StopPwmOut(void)
{
  TIM_DMACmd(TIM4,TIM_DMA_CC1,DISABLE);
  TIM4 -> CCER &= ~(1<<0); //?aTME4 PWMê?3?
//  TIM_Cmd(TIM4,DISABLE);
  DMA_Cmd(DMA1_Stream0,DISABLE);
}

void TIM4_CH1_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_TIM4);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//PD
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOD,&GPIO_InitStructure);
}

void DIFF_G_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR_G;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIO_PORT_SERVOMOTOR_G,&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR_G0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIO_PORT_SERVOMOTOR_G0,&GPIO_InitStructure);
  
  DIFF_G_H();
  DIFF_G0_L();
}  

void TIM4_PWM_Config(u32 period,u8 dir)
{
  TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
  TIM_OCInitTypeDef    TIM_OCInitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);////
  
  //------------------------------------------------------------------------------------	
  TIM_DeInit(TIM4);
  TIM_TimeBaseStructure.TIM_Prescaler = 1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = period - 1;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
  
  TIM_OCStructInit(&TIM_OCInitStructure);
  
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;////
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = period / 2;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC1Init(TIM4, &TIM_OCInitStructure);

  TIM_DMACmd(TIM4,TIM_DMA_CC1,DISABLE);
  TIM4 -> CCER &= ~(1<<0); //

  //PP信号控制方向
  //TIM_OCMode_PWM1:逆时针 TIM_OCMode_PWM2:顺时针
  TIM_OCInitStructure.TIM_OCMode = dir;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = period / 2;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC2Init(TIM4, &TIM_OCInitStructure);

  TIM_DMACmd(TIM4,TIM_DMA_CC2,DISABLE);
  TIM4 -> CCER &= ~(1<<4); //
  
  TIM_Cmd(TIM4,DISABLE);
}

void TIM4_CH1_PWMDMA_Config(u16 period,u16 cnt)
{
  u8 i;
  for(i=0;i<10;i++) 
    SRC_Buffer[i] = period;
  DMA_InitTypeDef  DMA_InitStructure;
  NVIC_InitTypeDef    NVIC_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
  
  DMA_DeInit(DMA1_Stream0);  
  while (DMA_GetCmdStatus(DMA1_Stream0) != DISABLE);
  
  DMA_InitStructure.DMA_Channel = DMA_Channel_2;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&TIM4->ARR;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)SRC_Buffer;
  DMA_InitStructure.DMA_BufferSize = cnt;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA1_Stream0,&DMA_InitStructure);
  
  DMA_ITConfig(DMA1_Stream0,DMA_IT_TC,ENABLE);
  
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void TIM4_CH2_ConfigPwmOut(u32 freq,u16 num)
{
  uint16_t period;
  //  period = SystemCoreClock / TIM_Prescaler / freq / 2; 
  period = freq;
  TIM4_CH2_GPIO_Configuration();
  TIM4_CH2_PWMDMA_Config(period,num);
}

void TIM4_CH2_StartPwmOut(void)
{
  TIM_DMACmd(TIM4,TIM_DMA_CC2,ENABLE);
  TIM4->CCER |= 1<<4; //?aTME4 PWMê?3?
  TIM_Cmd(TIM4,ENABLE);
  DMA_Cmd(DMA1_Stream3,ENABLE); 
}

void TIM4_CH2_StopPwmOut(void)
{
  TIM_DMACmd(TIM4,TIM_DMA_CC2,DISABLE);
  TIM4 -> CCER &= ~(1<<4); //?aTME4 PWMê?3?
  TIM_Cmd(TIM4,DISABLE);
  DMA_Cmd(DMA1_Stream3,DISABLE);
}

void TIM4_CH2_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_TIM4);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//PP
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOD,&GPIO_InitStructure);
}

void TIM4_CH2_PWMDMA_Config(u16 period,u16 cnt)
{
  u8 i;
  for(i=0;i<10;i++) 
    SRC_Buffer[i] = period;
  DMA_InitTypeDef  DMA_InitStructure;
  NVIC_InitTypeDef    NVIC_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
  
  DMA_DeInit(DMA1_Stream3);  
  while (DMA_GetCmdStatus(DMA1_Stream3) != DISABLE);
  
  DMA_InitStructure.DMA_Channel = DMA_Channel_2;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&TIM4->ARR;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)SRC_Buffer;
  DMA_InitStructure.DMA_BufferSize = cnt;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA1_Stream3,&DMA_InitStructure);
  
  DMA_ITConfig(DMA1_Stream3,DMA_IT_TC,ENABLE);
  
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void ServoMotorRunning(u16 stepnum)
{
  TIM4_CH1_PWMDMA_Config(FREQ_500KHZ,stepnum);
  TIM4_CH1_StartPwmOut();
  TIM4_CH2_PWMDMA_Config(FREQ_500KHZ,stepnum);
  TIM4_CH2_StartPwmOut();
}

void DMA1_Stream0_IRQHandler(void)
{
  if(DMA_GetITStatus(DMA1_Stream0,DMA_IT_TCIF0)==SET)
  {
    DMA_ClearFlag(DMA1_Stream0,DMA_IT_TCIF0);
    TIM4_CH1_StopPwmOut();
  }
}

void DMA1_Stream3_IRQHandler(void)
{
  if(DMA_GetITStatus(DMA1_Stream3,DMA_IT_TCIF3)==SET)
  {
    DMA_ClearFlag(DMA1_Stream3,DMA_IT_TCIF3);
    TIM4_CH2_StopPwmOut();
  }
}

