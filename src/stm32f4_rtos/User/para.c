#include "includes.h"

DEVICE_INFO device_info;

void read_device_info(void)
{
  u8 i;
  W25QXX_Read((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
  W25QXX_Read((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
//  W25QXX_Read((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
//  W25QXX_Read((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI,sizeof(weimi_para));
  if(device_info.isfirst != 0xaa)
  {
    device_info.isfirst = 0xaa;
    sprintf((char *)device_info.device_id,"%s","jx1234");
    device_info.device_id_len = 6;
    device_info.system_state = 0;
    device_info.page_count_all = 0;
    device_info.page_count_select = 0;
    //换班默认参数设置
    device_info.class_para.class_enable_onoff = 1;//默认开启A/B换班
    device_info.class_para.class_time_hour = 8;//默认换班时间08:00:00
    device_info.class_para.class_time_minute = 0;
    //卡片录入默认参数设置
    device_info.card_count.card_A_count = 0;
    device_info.card_count.card_B_count = 0;
    device_info.card_count.card_repair_count = 0;
    //页面功能默认参数设置
    device_info.func_onoff.jingsha = 1;
    device_info.func_onoff.channeng = 1;
    device_info.func_onoff.weimi = 1;
    
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
      device_info.stop_para.stop_time[i] = 0;
    }
    device_info.weimi_info.reg = 0;
    device_info.weimi_info.count = 0;
    //试用期默认参数设置
    device_info.period_para.period_enable_onoff = 0;//默认试用期关闭
    device_info.period_para.period_year = 20;//默认试用期限2020年
    device_info.period_para.period_month = 1;//默认试用期限1月
    device_info.period_para.period_day = 1;//默认试用期限1日
    memset(device_info.period_para.period_password,0,10);
    strcpy((char *)device_info.period_para.period_password,"232323");
    device_info.period_para.period_password_len = strlen((char const *)device_info.period_para.period_password);
    //系统登录密码默认设置
    memset(device_info.regin_in.password,0,6);
    strcpy((char *)device_info.regin_in.password,"111111");
    device_info.regin_in.password_len = strlen((char const *)device_info.regin_in.password);;  
    W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
    init_product_para(&product_para,&peiliao_para);
    W25QXX_Write((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
    init_peiliao_para(&peiliao_para);
    W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
    init_weimi_para(&weimi_para);
    W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI,sizeof(weimi_para));
  }
  if(device_info.page_count_all == 0)
  {
    JingSha_File.filename_len = 0;
    for(i=0;i<10;i++)
    {
      JingSha_File.filename[i] = 0;
    }
    for(i=0;i<30;i++)
    {
      JingSha_File.weight_value[i] = 0;
    }
    JingSha_File.year = 18;
    JingSha_File.month = 1;
    JingSha_File.day = 1;
    JingSha_File.week = 1;
    JingSha_File.hour = 1;
    JingSha_File.minute = 1;
    JingSha_File.second = 1;
  }
  else
  {
    W25QXX_Read((u8 *)&JingSha_File,(u32)(W25QXX_ADDR_JINGSHA + JINGSHA_SIZE * device_info.page_count_select),sizeof(JingSha_File));//经纱保存数据
    W25QXX_Read((u8 *)&peiliao_para,(u32)(W25QXX_ADDR_PEILIAO + PEILIAO_SIZE * device_info.page_count_select),sizeof(peiliao_para));//胚料保存数据
    W25QXX_Read((u8 *)&weimi_para,(u32)(W25QXX_ADDR_WEIMI + WEIMI_SIZE * device_info.page_count_select),sizeof(weimi_para));//纬密保存数据
  }
}

void para_init(SLAVE_PARA *para)
{
  u8 i;
  for(i=0;i<30;i++)
  {
    para->onoff[i] = device_info.onoff[i];
    para->value_set[i] = JingSha_File.weight_value[i];
    para->value_sample[i] = 0;
  }
  para->filename_len = JingSha_File.filename_len;
  for(i=0;i<JingSha_File.filename_len;i++)
  {
    para->filename[i] = JingSha_File.filename[i];
  }
  for(i=0;i<10;i++)
    file_select[i] = device_info.file_select[i];
}
