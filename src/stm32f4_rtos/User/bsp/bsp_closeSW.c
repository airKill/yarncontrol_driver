#include "bsp.h"

void bsp_InitCloseSW(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef   EXTI_InitStructure;
  /* ��GPIOʱ�� */
  RCC_AHB1PeriphClockCmd(RCC_CLOSE_SW, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  SYSCFG_EXTILineConfig(EXTI_PortSourceSW, EXTI_PinSourceSW);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		/* ��Ϊ����� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* ���������費ʹ�� */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO������ٶ� */
  
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
    EXTI_ClearITPendingBit(EXTI_LineSW); /* ����жϱ�־λ */
    xSemaphoreGiveFromISR(xSemaphore_pluse, &xHigherPriorityTaskWoken);
    /* ���xHigherPriorityTaskWoken = pdTRUE����ô�˳��жϺ��е���ǰ������ȼ�����ִ�� */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
  else if(EXTI_GetITStatus(EXTI_LineEncoder) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_LineEncoder); /* ����жϱ�־λ */
  }
}
