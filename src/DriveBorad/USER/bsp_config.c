#include "main.h"

u8 UART1_RX_BUF[UART1_MAX_RECV_LEN];
u16 Uart1_rx_count = 0;

u8 UART2_RX_BUF[UART2_MAX_RECV_LEN];
u16 Uart2_rx_count = 0;

u8 UART3_RX_BUF[UART3_MAX_RECV_LEN];
u16 Uart3_rx_count = 0;

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1,(u16)ch);
  
  /* Loop until transmit data register is empty */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}
  
  return ch;
}

void UART1_TO_PC(u8 *buf,u16 len)
{
  u16 i;
  for(i=0;i<len;i++)
  {
    USART_SendData(USART1,(u16)buf[i]);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
  }
}

void RS485_TO_PC(u8 *buf,u16 len)
{
  u16 i;
  RS485_TX();
  for(i=0;i<len;i++)
  {
    USART_SendData(USART2,(u16)buf[i]);
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
  }
    vTaskDelay(1);
  RS485_RX();
}


void Printf_Init(void)
{
  USART_InitTypeDef USART_InitStructure;   
  GPIO_InitTypeDef GPIO_InitStructure;  
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Configure USART Rx as alternate function  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);	/* USART configuration */ 
  
  USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
  USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);
  
  USART_Cmd(USART1, ENABLE); /* Enable USART */
  USART_ClearFlag(USART1, USART_FLAG_TC);
}

void RS485_Init(void)
{
  USART_InitTypeDef USART_InitStructure;   
  GPIO_InitTypeDef GPIO_InitStructure;  
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = RS485_EN_PIN;
  GPIO_Init(RS485_EN_PORT, &GPIO_InitStructure);
  
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStructure);	/* USART configuration */ 
  
  USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);
  USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
  
  USART_Cmd(USART2, ENABLE); /* Enable USART */
  USART_ClearFlag(USART2, USART_FLAG_TC);
  
  RS485_RX();
}

void DMA_Config(void)
{
  DMA_InitTypeDef DMA_InitStructure; 
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	
  
  DMA_DeInit(DMA1_Channel6);  
  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART2->DR;  
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)UART2_RX_BUF;  
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  
  DMA_InitStructure.DMA_BufferSize = UART2_MAX_RECV_LEN; 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
  DMA_Init(DMA1_Channel6, &DMA_InitStructure);  
  
  DMA_SetCurrDataCounter(DMA1_Channel6,UART2_MAX_RECV_LEN);
  DMA_Cmd(DMA1_Channel6, ENABLE);
} 

void LED_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;  
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = LED1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LED1_PORT, &GPIO_InitStructure);
}

void TIM2_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//
  
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  
  TIM_TimeBaseStructure.TIM_Period = (1000 - 1);	//
  TIM_TimeBaseStructure.TIM_Prescaler = (72 - 1);	//
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;	// 
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);	//
  
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  
  TIM_Cmd(TIM2, ENABLE);  //计数器使能
}

void InterruptConfig(void)
{   
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel =USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void GPIO_ToggleBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  GPIOx->ODR ^= GPIO_Pin;
}

void watchdog(void)
{
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//使能写入PR和RLR
  IWDG_SetPrescaler(IWDG_Prescaler_256);  //写入PR预分频值 看门狗频率40KHZ/256=156HZ 6.4ms
  IWDG_SetReload(200);  //写入RLR  2000×6.4 = 12.8
  IWDG_ReloadCounter(); //reload
  IWDG_Enable();//KR写入0xCCCC
}

//喂独立看门狗
void IWDG_Feed(void)
{
  IWDG_ReloadCounter();//reload
}

void bsp_init(void)
{
  LED_init();
  delay_init(72);
  InterruptConfig();
  Printf_Init();
  bsp_InitKey();
//  bsp_InitHardTimer();
  
  hx711_init();
  motor_init();
  RS485_Init();
  DMA_Config();
  Adc_Init();
  switch_init();
  device_addr = read_switch();
  readPara();
  mb_init(0);
  NULLMASS = device_info.hx711_offset;
  watchdog();
}


