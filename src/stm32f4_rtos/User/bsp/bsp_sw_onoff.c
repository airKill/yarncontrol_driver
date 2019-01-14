#include "bsp.h"

void bsp_Init_SW_ONOFF(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* ��GPIOʱ�� */
  RCC_AHB1PeriphClockCmd(RCC_SW_ONOFF, ENABLE);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		/* ��Ϊ����� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* ���������費ʹ�� */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO������ٶ� */
  
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
