#include "includes.h"

DEVICE_INFO device_info;

void read_device_info(void)
{
  u8 i;
  W25QXX_Read((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
  W25QXX_Read((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
  W25QXX_Read((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
  if(device_info.isfirst != 0xab)
  {
    device_info.isfirst = 0xaa;
    sprintf(device_info.device_id,"%s","jx1234");
    device_info.device_id_len = 6;
    device_info.system_state = 0;
    device_info.page_count_all = 0;
    device_info.page_count_select = 0;
    device_info.class_enable_onoff = 1;//默认开启A/B换班
    device_info.class_time_hour = 8;//默认换班时间08:00:00
    device_info.class_time_minute = 0;
    device_info.card_A_count = 0;
    device_info.card_B_count = 0;
    device_info.card_repair_count = 0;
    device_info.page_enable_onoff[0] = 1;
    device_info.page_enable_onoff[1] = 1;
    device_info.page_enable_onoff[2] = 1;
    for(i=0;i<30;i++)
    {
      device_info.onoff[i] = 0;
    }
    for(i=0;i<10;i++)
    {
      device_info.file_select[i] = 0;
    }
    for(i=0;i<11;i++)
    {
      device_info.stop_time[i] = 0;
    }
    device_info.period_enable_onoff = 0;//默认试用期关闭
    device_info.period_year = 20;//默认试用期限2020年
    device_info.period_month = 1;//默认试用期限1月
    device_info.period_day = 1;//默认试用期限1日
    memset(device_info.period_password,0,10);
    strcpy(device_info.period_password,"admin1");
    device_info.period_password_len = strlen(device_info.period_password);;
    W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
    init_product_para(&product_para);
    W25QXX_Write((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
    init_peiliao_para(&peiliao_para);
    W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
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
    W25QXX_Read((u8 *)&File_info,(u32)(W25QXX_ADDR_JINGSHA + FILE_SIZE * device_info.page_count_select),sizeof(File_info));
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
