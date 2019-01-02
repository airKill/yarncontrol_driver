#include "main.h"

unsigned long hx711_buf[MAXCOUNT];
unsigned long hx711_sum = 0;
u8 hx711_i;

void hx711_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* 使能(开启)KEY1引脚对应IO端口时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  /* 设定KEY1对应引脚IO编号 */
  GPIO_InitStructure.GPIO_Pin = HX711_SCK_GPIO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(HX711_SCK_GPIO, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = HX711_DT_GPIO_PIN;
  GPIO_Init(HX711_DT_GPIO, &GPIO_InitStructure);
  
  HX711_SCK_L(); 
}

unsigned long Read_HX711_Count(void)
{
  unsigned long Count;
  unsigned char i;
  HX711_SCK_L(); 
  delay_hx711(1);
  Count = 0;
  while(READ_HX711_DT()); 
  delay_hx711(1);
  for (i=0; i<24; i++)
  {
    HX711_SCK_H(); 
    Count = Count << 1; 
    delay_hx711(1);
    HX711_SCK_L(); 
    delay_hx711(1);
    if(READ_HX711_DT()) 
      Count++;
  }
  HX711_SCK_H();
  Count = Count ^ 0x800000;
  delay_hx711(1);
  HX711_SCK_L();
  delay_hx711(1);
  return(Count);
}

void hx711_buf_Init(void)   
{
  int j=0;  
  for(j=0;j<MAXCOUNT;j++)
  {
    hx711_buf[j] = Read_HX711_Count() / 100;
    hx711_sum += hx711_buf[j];
  }
}

//long double hx711_filter(void)  
//{
//  unsigned long temp = 0,max = 0,min = 0;
//  int j=0;
//  
//  temp = Read_HX711_Count() / 100;
//
//  hx711_sum = hx711_sum + temp - hx711_buf[hx711_i]; 
//  hx711_buf[hx711_i] = temp;
//  hx711_i++;
//  if(hx711_i == MAXCOUNT)
//    hx711_i=0;
//  
//  max = hx711_buf[0];
//  min = hx711_buf[0];
//  for(j=0;j<MAXCOUNT;j++)
//  {
//    if(max < hx711_buf[j])	
//      max = hx711_buf[j];
//    if(min > hx711_buf[j])	
//      min = hx711_buf[j];
//  }
//  return (hx711_sum - max - min) / (MAXCOUNT - 2);	
//}


long double hx711_filter(void) 
{
  char count,i,j;
  unsigned long value_buf[MAXCOUNT];
  unsigned long sum = 0,temp = 0;
  for(count=0;count<MAXCOUNT;count++)
  {
    value_buf[count] = Read_HX711_Count() / 100;
    vTaskDelay(10);
  }
  for(j=0;j<MAXCOUNT - 1;j++)
  {
    for(i=0;i<MAXCOUNT - j;i++)
    {
      if(value_buf[i] > value_buf[i + 1])
      {
        temp = value_buf[i];
        value_buf[i] = value_buf[i + 1]; 
        value_buf[i + 1] = temp;
      }
    }
  }
  for(count=1;count<MAXCOUNT - 1;count++)
    sum += value_buf[count];
  return (unsigned long)(sum / (MAXCOUNT - 2));
}

void delay_hx711(u16 us)
{
  u16 i;
  for(i=0;i<us;i++)
  {
    __NOP();
    __NOP();
    __NOP();
  }
}
