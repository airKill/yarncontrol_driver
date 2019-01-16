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
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		/* ��Ϊ����� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* ���������費ʹ�� */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO������ٶ� */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR1;
  GPIO_Init(GPIO_PORT_SERVOMOTOR1, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR2;
  GPIO_Init(GPIO_PORT_SERVOMOTOR2, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* ��Ϊ����� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* ���������費ʹ�� */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO������ٶ� */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR_G;
  GPIO_Init(GPIO_PORT_SERVOMOTOR_G, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR_G0;
  GPIO_Init(GPIO_PORT_SERVOMOTOR_G0, &GPIO_InitStructure);
}

/* ����һ��DMA���� */
void DMA_PWM_Enable(void)
{
  DMA_Cmd(DMA1_Stream0, DISABLE);
  TIM_SetAutoreload(TIM4,2);/* �������һ����0������������ʱ��ARR�ᱻ���㣬������һ��������Ч��*/
  DMA_SetCurrDataCounter(DMA1_Stream0,DMA1_MEM_LEN);
  DMA_Cmd(DMA1_Stream0,ENABLE);
  TIM_Cmd(TIM4, ENABLE);  /* ʹ��TIM3 */
  TIM_GenerateEvent(TIM4,1);
}	  

//���ȷ���������ʣ�µ�������
u16 DMA_send_feedback(void)
{
  return DMA_GetCurrDataCounter(DMA1_Stream0);
}

void TIM4_PWM_Init(u16 arr,u16 psc)
{
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	/* ʹ�ܶ�ʱ��3ʱ�� */
  
  /* ��ʼ��TIM4 */
  TIM_TimeBaseStructure.TIM_Period = arr; /* ��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ */
  TIM_TimeBaseStructure.TIM_Prescaler = psc; /* ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ */
  TIM_TimeBaseStructure.TIM_ClockDivision = 0; /* ����ʱ�ӷָ�:TDTS = Tck_tim */
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  /*TIM���ϼ���ģʽ */
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); /* ����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ */
  
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; /* ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ1 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; /* �Ƚ����ʹ�� */
  //TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  /* ʹ��TIM3��CCR1�ϵ�Ԥװ�ؼĴ���*/
  TIM_OCInitStructure.TIM_Pulse = 100;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; /* �������:TIM����Ƚϼ��Ը� */
  TIM_OC1Init(TIM4, &TIM_OCInitStructure);  /* ����Tָ���Ĳ�����ʼ������TIM3 OC1 */
  //TIM_DMACmd(TIM4, TIM_DMA_Update, ENABLE);	/* �����Ҫ����ռ�ձȾͰ�����ȥ��ע�ͣ�Ȼ��ע�͵��������У��ٰ�DMAͨ��6��ΪDMAͨ��3 */
  TIM_DMACmd(TIM4, TIM_DMA_Update, ENABLE);
  TIM_Cmd(TIM4, ENABLE);  /* ʹ��TIM3 */
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
  TIM4 -> CCER &= ~(1<<0); //1?��?TME4 PWM��?3?
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
    TIM4 -> CCER &= ~(1<<0); //�ر�TIME4 CH1 ���ʹ��
  }
}


