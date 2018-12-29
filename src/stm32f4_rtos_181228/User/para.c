#include "includes.h"

DEVICE_INFO device_info;

void read_device_info(void)
{
  u8 i;
  __set_PRIMASK(1);
  W25QXX_Read((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
  __set_PRIMASK(0);
  if(device_info.isfirst != 0xaa)
  {
    device_info.isfirst = 0xaa;
    device_info.master_id = 0x01;
    device_info.page_count_all = 0;
    device_info.page_count_select = 0;
    for(i=0;i<30;i++)
    {
      device_info.onoff[i] = 0;
    }
    for(i=0;i<10;i++)
    {
      device_info.file_select[i] = 0;
    }
    __set_PRIMASK(1);
    W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
    __set_PRIMASK(0);
  }
  if(device_info.page_count_all == 0)
  {
    File_info.filename_len = 0;
    for(i=0;i<10;i++)
    {
      File_info.filename[i] = 0;
    }
    for(i=0;i<30;i++)
    {
      File_info.weight_value[i] = 0;
    }
    File_info.year = 18;
    File_info.month = 1;
    File_info.day = 1;
    File_info.week = 1;
    File_info.hour = 1;
    File_info.minute = 1;
    File_info.second = 1;
  }
  else
  {
    __set_PRIMASK(1);
    W25QXX_Read((u8 *)&File_info,(u32)(W25QXX_ADDR_FILE + FILE_SIZE * device_info.page_count_select),sizeof(File_info));
    __set_PRIMASK(0);
  }
}

void para_init(SLAVE_PARA *para)
{
  u8 i;
  for(i=0;i<30;i++)
  {
    para->onoff[i] = device_info.onoff[i];
    para->value_set[i] = File_info.weight_value[i];
    para->value_sample[i] = 0;
  }
  para->filename_len = File_info.filename_len;
  for(i=0;i<File_info.filename_len;i++)
  {
    para->filename[i] = File_info.filename[i];
  }
  for(i=0;i<10;i++)
    file_select[i] = device_info.file_select[i];
}
