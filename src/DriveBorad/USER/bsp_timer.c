#include "main.h"

void (*s_TIM_CallBack1)(void);
void (*s_TIM_CallBack2)(void);
void (*s_TIM_CallBack3)(void);
void (*s_TIM_CallBack4)(void);

void bsp_InitHardTimer(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  u32 usPeriod;
  u16 usPrescaler;
  u32 uiTIMxCLK;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  
  uiTIMxCLK = SystemCoreClock / 2;
  
  usPrescaler = uiTIMxCLK / 1000;	
  
  usPeriod = 0xFFFFFFFF;
  
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = usPeriod;
  TIM_TimeBaseStructure.TIM_Prescaler = usPrescaler;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  
  TIM_Cmd(TIM3, ENABLE);
}

void bsp_StartHardTimer(u8 _CC, u32 _uiTimeOut, void * _pCallBack)
{
  u32 cnt_now;
  u32 cnt_tar;
  
  if (_uiTimeOut < 5)
  {
    ;
  }
  else
  {
    _uiTimeOut -= 5;
  }
  
  cnt_now = TIM_GetCounter(TIM3);    	
  cnt_tar = cnt_now + _uiTimeOut;			
  if (_CC == 1)
  {
    s_TIM_CallBack1 = (void (*)(void))_pCallBack;
    
    TIM_SetCompare1(TIM3, cnt_tar);      	
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
    TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);	
    
  }
  else if (_CC == 2)
  {
    s_TIM_CallBack2 = (void (*)(void))_pCallBack;
    
    TIM_SetCompare2(TIM3, cnt_tar);      	
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
    TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);	
  }
  else if (_CC == 3)
  {
    s_TIM_CallBack3 = (void (*)(void))_pCallBack;
    
    TIM_SetCompare3(TIM3, cnt_tar);      	
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
    TIM_ITConfig(TIM3, TIM_IT_CC3, ENABLE);	
  }
  else if (_CC == 4)
  {
    s_TIM_CallBack4 = (void (*)(void))_pCallBack;
    
    TIM_SetCompare4(TIM3, cnt_tar);      	
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC4);
    TIM_ITConfig(TIM3, TIM_IT_CC4, ENABLE);	
  }
  else
  {
    return;
  }
}

void TIM_CallBack1(void)
{
  LED1_OFF();
}

void TIM_CallBack2(void)
{
  
}

void TIM_CallBack3(void)
{
  
}




