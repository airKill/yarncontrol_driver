#include "includes.h"

void bsp_Init_Relay(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* ��GPIOʱ�� */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* ��Ϊ����� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* ���������費ʹ�� */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO������ٶ� */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_RELAY;
  GPIO_Init(GPIO_PORT_RELAY, &GPIO_InitStructure);
  
  RELAY_CLOSE();
}