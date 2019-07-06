#include "main.h"

DEVICE_INFO device_info;

void readPara(void)
{
  Read_flash((u16 *)USER_FLASH_ADDR,(u16 *)&device_info,sizeof(device_info)/2);
  if(device_info.isfirst != 0x55)    //0x5a²»±£´æ£¬0x55±£´æ
  {
    device_info.isfirst = 0x55;
    device_info.onoff = 1;
    device_info.weight_value = 0;
    device_info.hx711_offset = 84945;
    device_info.hx711_xishu = 500.0 / 523.0;
    device_info.precision = 300;
    __set_PRIMASK(1);
    Write_flash(USER_FLASH_ADDR,(u16*)&device_info,sizeof(device_info) / 2);
    __set_PRIMASK(0);
  }
}
