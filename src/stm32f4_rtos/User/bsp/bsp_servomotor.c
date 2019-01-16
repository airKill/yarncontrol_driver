#include "bsp.h"

u16 DMA1_MEM_LEN = 0;

void bsp_InitServoMotor(void)
{
  bsp_io_ServoMotor();
  DMA1_Stream0_CH2Init();
  TIM4_CH1_PWM_Config();
  DIFF_G_H();
  DIFF_G0_L();
}

void bsp_io_ServoMotor(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_SERVOMOTOR, ENABLE);
  
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_TIM12);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_TIM4);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		/* 设为输出口 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽模式 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 上下拉电阻不使能 */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO口最大速度 */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR1;
  GPIO_Init(GPIO_PORT_SERVOMOTOR1, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR2;
  GPIO_Init(GPIO_PORT_SERVOMOTOR2, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* 设为输出口 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽模式 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* 上下拉电阻不使能 */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO口最大速度 */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR_G;
  GPIO_Init(GPIO_PORT_SERVOMOTOR_G, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR_G0;
  GPIO_Init(GPIO_PORT_SERVOMOTOR_G0, &GPIO_InitStructure);
}

/* 开启一次DMA传输 */
void DMA_PWM_Enable(void)
{
  DMA_Cmd(DMA1_Stream0, DISABLE);
  TIM_SetAutoreload(TIM4,2);/* 由于最后一项是0，所以在最后的时刻ARR会被清零，导致下一次启动无效。*/
  DMA_SetCurrDataCounter(DMA1_Stream0,DMA1_MEM_LEN);
  DMA_Cmd(DMA1_Stream0,ENABLE);
  TIM_Cmd(TIM4, ENABLE);  /* 使能TIM3 */
  TIM_GenerateEvent(TIM4,1);
}	  

//进度反馈，返回剩下的数据量
u16 DMA_send_feedback(void)
{
  return DMA_GetCurrDataCounter(DMA1_Stream0);
}

void TIM4_PWM_Init(u16 arr,u16 psc)
{
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	/* 使能定时器3时钟 */
  
  /* 初始化TIM4 */
  TIM_TimeBaseStructure.TIM_Period = arr; /* 设置在下一个更新事件装入活动的自动重装载寄存器周期的值 */
  TIM_TimeBaseStructure.TIM_Prescaler = psc; /* 设置用来作为TIMx时钟频率除数的预分频值 */
  TIM_TimeBaseStructure.TIM_ClockDivision = 0; /* 设置时钟分割:TDTS = Tck_tim */
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  /*TIM向上计数模式 */
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); /* 根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位 */
  
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; /* 选择定时器模式:TIM脉冲宽度调制模式1 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; /* 比较输出使能 */
  //TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  /* 使能TIM3在CCR1上的预装载寄存器*/
  TIM_OCInitStructure.TIM_Pulse = 100;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; /* 输出极性:TIM输出比较极性高 */
  TIM_OC1Init(TIM4, &TIM_OCInitStructure);  /* 根据T指定的参数初始化外设TIM3 OC1 */
  //TIM_DMACmd(TIM4, TIM_DMA_Update, ENABLE);	/* 如果是要调节占空比就把这行去掉注释，然后注释掉下面那行，再把DMA通道6改为DMA通道3 */
  TIM_DMACmd(TIM4, TIM_DMA_Update, ENABLE);
  TIM_Cmd(TIM4, ENABLE);  /* 使能TIM3 */
}

void TIM4_CH1_PWM_Config(void)
{
  TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
  TIM_OCInitTypeDef    TIM_OCInitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

  TIM_DeInit(TIM4);
  TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 50;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
  
  TIM_OCStructInit(&TIM_OCInitStructure);
  
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;////
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  //  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 20;
  //  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  //  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  //  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  //  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
  TIM_OC1Init(TIM4, &TIM_OCInitStructure);
  //------------------------------------------------------------------------------------	
  //  TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Enable);
  //  TIM_ARRPreloadConfig(TIM4,ENABLE);
  
  //	TIM_DMAConfig(TIM4,TIM_DMABase_ARR,TIM_DMABurstLength_3Transfers);
  TIM_DMACmd(TIM4,TIM_DMA_CC1,DISABLE);
  TIM4 -> CCER &= ~(1<<0); //1?±?TME4 PWMê?3?
  TIM_Cmd(TIM4,DISABLE);
}

void DMA1_Stream0_CH2Init(void)
{
  NVIC_InitTypeDef    NVIC_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
  
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void DMA1_Stream0_CH2_Cmd(void (*Fuc)(uint16_t *,int32_t,int32_t),uint16_t *DataBuf,int32_t BufSize,int32_t MemoryInc)
{
  Fuc(DataBuf,BufSize,MemoryInc);
  DMA_Cmd(DMA1_Stream0,ENABLE);
}

void TIM4_PWMDMA_Config(uint16_t *DataBuf,int32_t BufSize,int32_t MemoryInc)
{
  DMA_InitTypeDef  DMA_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
  
  DMA_InitStructure.DMA_Channel = DMA_Channel_2;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&TIM4->ARR;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)DataBuf;
  DMA_InitStructure.DMA_BufferSize = BufSize;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = MemoryInc;
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
}

void DMA1_Stream0_IRQHandler(void)
{	
  if(DMA_GetITStatus(DMA1_Stream0,DMA_IT_TCIF0)==SET)
  {
    DMA_ClearFlag(DMA1_Stream0,DMA_IT_TCIF0);
    DMA_Cmd(DMA1_Stream0,DISABLE);
    TIM_DMACmd(TIM4,TIM_DMA_CC1,DISABLE);
    TIM4 -> CCER &= ~(1<<0); //关闭TIME4 CH1 输出使能
  }
}


