#include "bsp.h"

void bsp_InitCloseSW(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef   EXTI_InitStructure;
  /* 打开GPIO时钟 */
  RCC_AHB1PeriphClockCmd(RCC_CLOSE_SW, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  SYSCFG_EXTILineConfig(EXTI_PortSourceSW, EXTI_PinSourceSW);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		/* 设为输出口 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽模式 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 上下拉电阻不使能 */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO口最大速度 */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_CLOSE_SW;
  GPIO_Init(GPIO_PORT_CLOSE_SW, &GPIO_InitStructure);
  
  EXTI_InitStructure.EXTI_Line = EXTI_LineSW;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  ConfigExitNVIC();
}

void ConfigExitNVIC(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void EXTI9_5_IRQHandler(void)
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  if(EXTI_GetITStatus(EXTI_LineSW) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_LineSW); /* 清除中断标志位 */
    xSemaphoreGiveFromISR(xSemaphore_pluse, &xHigherPriorityTaskWoken);
    /* 如果xHigherPriorityTaskWoken = pdTRUE，那么退出中断后切到当前最高优先级任务执行 */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
  else if(EXTI_GetITStatus(EXTI_LineEncoder) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_LineEncoder); /* 清除中断标志位 */
  }
}
