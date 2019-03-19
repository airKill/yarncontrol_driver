#include "includes.h"

void bsp_Init_Relay(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* 打开GPIO时钟 */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* 设为输出口 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽模式 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* 上下拉电阻不使能 */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO口最大速度 */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_RELAY;
  GPIO_Init(GPIO_PORT_RELAY, &GPIO_InitStructure);
  
  RELAY_CLOSE();
}