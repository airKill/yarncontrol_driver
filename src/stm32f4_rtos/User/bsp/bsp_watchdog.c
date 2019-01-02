#include "bsp.h"

void watchdog(void)
{
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//使能写入PR和RLR
  IWDG_SetPrescaler(IWDG_Prescaler_32);  //写入PR预分频值 看门狗频率32KHZ/32=1kHZ 1ms
  IWDG_SetReload(1000);  //写入RLR  100ms
  IWDG_ReloadCounter(); //reload
  IWDG_Enable();//KR写入0xCCCC
}

//喂独立看门狗
void IWDG_Feed(void)
{
  IWDG_ReloadCounter();//reload
}