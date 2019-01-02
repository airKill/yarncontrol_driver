#include "bsp.h"

void watchdog(void)
{
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//ʹ��д��PR��RLR
  IWDG_SetPrescaler(IWDG_Prescaler_32);  //д��PRԤ��Ƶֵ ���Ź�Ƶ��32KHZ/32=1kHZ 1ms
  IWDG_SetReload(1000);  //д��RLR  100ms
  IWDG_ReloadCounter(); //reload
  IWDG_Enable();//KRд��0xCCCC
}

//ι�������Ź�
void IWDG_Feed(void)
{
  IWDG_ReloadCounter();//reload
}