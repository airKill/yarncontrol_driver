#include "includes.h"

PRODUCT_PARA product_para;

void init_product_para(PRODUCT_PARA *para)
{
  para->system_state = 0;
  para->product_a = 0;
  para->product_b = 0;
  para->total_work_time = 0;
  para->total_stop_time = 0;
  para->latitude_weight = 10;
  para->longitude_weight = 10;
  para->rubber_weight = 10;
  para->final_weight = final_per_meter(para);
  para->loom_num = 10;
  para->total_meter_set = 100;
  para->total_weitht_set = 100;
  para->kaidu_set = 10;
  para->weimi_set = 100;
  para->weimi_dis_set = 500;
  para->loss = 10;
}

//计算产量
float product_per_meter(PRODUCT_PARA *para)
{
  float meter;
  if(para->kaidu_set > 0)
  {
    meter = (para->pulse_count / para->weimi_set / para->kaidu_set) * para->loom_num;
  }
  else if(para->kaidu_set == 0)
  {
    meter = para->pulse_count / para->weimi_set;
  }
  return meter;
}

//计算每米成品重量
float final_per_meter(PRODUCT_PARA *para)
{
  float weight;
  weight = para->latitude_weight + para->longitude_weight + para->rubber_weight;
  return weight;
}

//计算已完成产量
float product_complete_meter(PRODUCT_PARA *para)
{
  float complete_meter;
  complete_meter = para->product_a + para->product_b;//已完成产量=A班产量+B班产量
  return complete_meter;
}

//计算未完成产量
float product_uncomplete_meter(PRODUCT_PARA *para)
{
  float uncomplete_meter;
  float complete_meter;
  complete_meter = para->product_a + para->product_b;//已完成产量=A班产量+B班产量
  uncomplete_meter = para->total_meter_set - complete_meter;//未完成产量=总产量-已完成产量
  return uncomplete_meter;
}

//计算每千纬数量
u32 count_per_kilo(PRODUCT_PARA *para)
{
  u32 count;
  count = para->pulse_count / 1000;
  return count;
}

//计算已完成重量
float product_complete_kilo(PRODUCT_PARA *para)
{
  float weight;
  weight = final_per_meter(para) * product_complete_meter(para) / para->loss;//已完成重量=每米成品重量*完成产量/损耗
  return weight;
}

//计算未完成重量
float product_uncomplete_kilo(PRODUCT_PARA *para)
{
  float weight;
  weight = para->total_weitht_set - product_complete_kilo(para);
  return weight;
}

float get_float_1bit(float data)
{
  float tmp;
  tmp = (float)((int)(data * 10)) / 10;
  return tmp;
}

u8 get_class_time(RTC_TIME *time)
{
  u8 num;
  u32 dat;
  dat = (time->hour * 10000) + (time->minute * 100) + time->second;
  if((dat >= 80000) && (dat <= 200000))//时间在08:00:00~20:00:00之间
  {
    num = CLASS_A;
  }
  else
    num = CLASS_B;
  return num;
}

//功能:从一个数组里查找是否有相同的数
//card:卡片编号，4字节
//buf_lib:数组
//buf_len:数组长度
u8 get_card_function(u32 card,u32 *buf_lib,u16 buf_len)
{
  u8 same = 0;
  u16 i;
  for(i=0;i<buf_len;i++)
  {
    if(card == buf_lib[i])
      same = 1;
  }
  return same;
}
