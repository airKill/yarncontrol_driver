#include "bsp.h"

uint16_t SRC_Buffer[10] = {2000};

void TIM4ConfigPwmOut(u32 freq,u16 num)
{
  uint16_t period;
  //  period = SystemCoreClock / TIM_Prescaler / freq / 2; 
  period = 84;
  TIM4_GPIO_Configuration();
  TIM4_CH1_PWM_Config(period);
  TIM4_PWMDMA_Config(period,num);
}

void TIM4StartPwmOut(void)
{
  TIM_DMACmd(TIM4,TIM_DMA_CC1,ENABLE);
  TIM4->CCER |= 1<<0; //?aTME4 PWM¨º?3?
  TIM_Cmd(TIM4,ENABLE);
  DMA_Cmd(DMA1_Stream0,ENABLE); 
}

void TIM4StopPwmOut(void)
{
  TIM_DMACmd(TIM4,TIM_DMA_CC1,DISABLE);
  TIM4 -> CCER &= ~(1<<0); //?aTME4 PWM¨º?3?
  TIM_Cmd(TIM4,DISABLE);
  DMA_Cmd(DMA1_Stream0,DISABLE);
}

void TIM4_GPIO_Configuration( void )
{
  GPIO_InitTypeDef      GPIO_InitStructure;
  /* Enable the GPIO_LED Clock */
  //  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_TIM4);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD,&GPIO_InitStructure);
}

void TIM4_CH1_PWM_Config(u32 period)
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
  //  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = period / 2;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  //  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
  //  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  //  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
  TIM_OC1Init(TIM4, &TIM_OCInitStructure);
  //------------------------------------------------------------------------------------	
  //  TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Enable);
  //  TIM_ARRPreloadConfig(TIM4,ENABLE);
  
  //	TIM_DMAConfig(TIM4,TIM_DMABase_ARR,TIM_DMABurstLength_3Transfers);
  TIM_DMACmd(TIM4,TIM_DMA_CC1,DISABLE);
  TIM4 -> CCER &= ~(1<<0); //1?¡À?TME4 PWM¨º?3?
  TIM_Cmd(TIM4,DISABLE);
}

void TIM4_PWMDMA_Config(u16 period,u16 cnt)
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
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void DMA1_Stream0_IRQHandler(void)
{	
  if(DMA_GetITStatus(DMA1_Stream0,DMA_IT_TCIF0)==SET)
  {
    DMA_ClearFlag(DMA1_Stream0,DMA_IT_TCIF0);
    TIM4StopPwmOut();
  }
}
