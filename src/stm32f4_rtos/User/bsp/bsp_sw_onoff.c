#include "bsp.h"

void bsp_Init_SW_ONOFF(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* 打开GPIO时钟 */
  RCC_AHB1PeriphClockCmd(RCC_SW_ONOFF, ENABLE);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		/* 设为输出口 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽模式 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 上下拉电阻不使能 */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO口最大速度 */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SW_ONOFF;
  GPIO_Init(GPIO_PORT_SW_ONOFF, &GPIO_InitStructure);
}

u8 GetDeviceState(void)
{
  u8 tmp;
  if(Read_SW_ONOFF() == 1)
  {
    tmp = 0;
  }
  else if(Read_SW_ONOFF() == 0)
  {
    tmp = 1;
  }
  return tmp;
}
