#include "includes.h"

void bsp_Init_encoder(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef   EXTI_InitStructure;
  /* 打开GPIO时钟 */
  RCC_AHB1PeriphClockCmd(RCC_ENCODER, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  SYSCFG_EXTILineConfig(EXTI_PortEncoder, EXTI_SourceEncoder);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		/* 设为输出口 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽模式 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 上下拉电阻不使能 */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO口最大速度 */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ENCODER;
  GPIO_Init(GPIO_PORT_ENCODER, &GPIO_InitStructure);
  
  EXTI_InitStructure.EXTI_Line = EXTI_LineEncoder;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}
