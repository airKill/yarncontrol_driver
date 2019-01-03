#include "main.h"

EXTI_InitTypeDef   EXTI_InitStructure;
GPIO_InitTypeDef   GPIO_InitStructure;
NVIC_InitTypeDef   NVIC_InitStructure;

void Key_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;  
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
//  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = KEY_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(KEY_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = KEY1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(KEY1_PORT, &GPIO_InitStructure);
}

u8 readKey(void)
{
  u8 State;
  if(READ_KEY() == 0)
    State = 1;
  else
    State = 0;
  return State;
}

// 按键检测，定时调用，连续两次检测到低电平认为是按下，连续两次松开认为是松开
void ButtonCheck(void)
{
  // 按键检测
  if(readKey())
  {
    if(m_flgLastButton)
    {
      g_flgButton = 1;
      g_wUnbuttonConter = 0;
      if(g_wButtonCounter != 0XFFFF)
      {
        g_wButtonCounter++;
      }
    }
    m_flgLastButton = 1;
  }
  else
  {
    if(!m_flgLastButton)
    {
      g_flgButton = 0;
      g_wButtonCounter = 0;
      if(g_wUnbuttonConter != 0xFFFFFFFF)
      {
        g_wUnbuttonConter++;
      }
    }
    m_flgLastButton = 0;
  }
}