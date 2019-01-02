#include "main.h"

void hc08_protocol(void)
{
  u8 buf[50];
  float vol;
  float temp;
  if(hc08_check_cmd("#get device info"))
  {
    ValueToActual((u16 *)&ADC_Value,ActualValue);
    vol = ActualValue[BATTERY];
    temp = Get_Temp(ADC_Value.temp);
    sprintf(buf,"#key_cnt is %d,vol is %.2f,temp is %.2f\r\n",device_info.key_count,vol,temp);
    UART3_TO_PC(buf,strlen(buf));
  }
  else if(hc08_check_cmd("#set keycount reset."))
  {
    device_info.key_count = 0;
    Write_flash(USER_FLASH_ADDR,(u16*)&device_info,sizeof(device_info) / 2);
    UART3_TO_PC("#set keycount reset ok.\r\n",strlen("#set keycount reset ok.\r\n"));
  }
}

u8* hc08_check_cmd(char *str)
{
  char *strx = 0;
  strx = strstr((const char*)UART3_RX_BUF,(const char*)str);
  return (u8*)strx;
}
