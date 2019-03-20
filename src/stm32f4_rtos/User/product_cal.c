#include "includes.h"

volatile PRODUCT_PARA product_para;
volatile PEILIAO_PARA peiliao_para;

u32 total_meter_gross = 0;
u32 total_weight_gross = 0;
u8 plan_complete = 0,old_plan_complete = 0;

u8 work_idle = PLUSE_STOP;
u16 work_idle_time = 0;

const char system_state_dis[20][20] = {
  {"系统正常"},
  {"待转机"},
  {"转机"},
  {"待PPC处理"},
  {"待料"},
  {"缺料"},
  {"清洁卫生"},
  {"工艺调校"},
  {"织机调校"},
  {"维修"},
  {"待QAD处理"},
  {"替换盘头"},   
  {"系统停止"},
};

void init_product_para(volatile PRODUCT_PARA *para,volatile PEILIAO_PARA peiliao)
{
  u32 meter,weight;
  meter = (u32)(peiliao.total_meter_set * 10 * (1 + peiliao.loss / 100.0));
  weight = (u32)(peiliao.total_weitht_set * 10 * (1 + peiliao.loss / 100.0));
  para->product_a = 0;
  para->product_b = 0;
  para->product_complete = 0;
  para->product_uncomplete = meter - para->product_complete;
  para->weicount_kilowei = 0;
  para->speed = 0;
  para->total_work_time = 0;
  para->total_stop_time = 0;
  para->weight_complete = 0;
  para->weight_uncomplete = weight - para->weight_complete;
}

void init_peiliao_para(volatile PEILIAO_PARA *para)
{
  para->latitude_weight = 1000;
  para->longitude_weight = 1000;
  para->rubber_weight = 1000;
  para->final_weight = para->latitude_weight + para->longitude_weight + para->rubber_weight;
  para->loom_num = 1;
  para->loss = 10;
  para->total_meter_set = 10000;
  para->total_weitht_set = 10000;
  para->kaidu_set = 100;
  para->weimi_set = 100;
  para->add_meter_set = 0;
  para->weimi_dis_set = 1000;
}

//计算产量
float product_per_meter(volatile PEILIAO_PARA para,u32 pluse)
{
  float meter;
  if(para.kaidu_set > 0)
  {
//    meter = (pluse / para->weimi_set / 100 / para->kaidu_set) * para->loom_num;
    meter = ((float)pluse / para.weimi_set / para.kaidu_set) * para.loom_num;
  }
  else if(para.kaidu_set == 0)
  {
    meter = (float)pluse / para.weimi_set / 10 * para.loom_num;
//    meter = pluse / para->weimi_set / 100 * para->loom_num;
  }
  return meter;
}

//计算每米成品重量
u32 final_per_meter(volatile PEILIAO_PARA para)
{
  u32 weight;
  weight = para.latitude_weight + para.longitude_weight + para.rubber_weight;
  return weight;
}

//计算已完成产量
u32 product_complete_meter(volatile PRODUCT_PARA para)
{
  u32 complete_meter;
  complete_meter = para.product_a + para.product_b;//已完成产量=A班产量+B班产量
  return complete_meter;
}

//计算未完成产量
u32 product_uncomplete_meter(volatile PRODUCT_PARA para,volatile PEILIAO_PARA peiliao)
{
  u32 uncomplete_meter;
  u32 complete_meter;
  complete_meter = para.product_a + para.product_b;//已完成产量=A班产量+B班产量
  uncomplete_meter = peiliao.total_meter_set - complete_meter;//未完成产量=总产量-已完成产量
  return uncomplete_meter;
}

//计算每千纬数量
u32 count_per_kilo(u32 pluse)
{
  u32 count;
  count = pluse / 1000;
  return count;
}

//计算已完成重量
u32 product_complete_kilo(volatile PRODUCT_PARA para,volatile PEILIAO_PARA peiliao)
{
  float weight;
  weight = final_per_meter(peiliao) / 1000 * product_complete_meter(para);//已完成重量=每米成品重量*完成产量
  return weight;
}

//计算未完成重量
u32 product_uncomplete_kilo(volatile PRODUCT_PARA para,volatile PEILIAO_PARA peiliao)
{
  float weight;
  weight = peiliao.total_weitht_set - product_complete_kilo(para,peiliao);
  return weight;
}

float get_float_1bit(float data)
{
  float tmp;
  tmp = (float)((int)(data * 10)) / 10;
  return tmp;
}

u8 get_class_time(RTC_TIME *time,DEVICE_INFO *para)
{
  u8 num;
  u32 dat,set_value;
  dat = (time->hour * 10000) + (time->minute * 100) + time->second;
  set_value = (para->class_para.class_time_hour * 10000) + (para->class_para.class_time_minute * 100);//设定的换班时间
  if(set_value >= 120000)
  {//换班时间大于12点，则下班时间为凌晨
    if(((dat >= set_value) && (dat <= 235959)) || (dat < (set_value - 120000)))
    {//A班时间为：设定时间~凌晨00:00:00~（设定时间 - 12：00:00）
      num = CLASS_A;
    }
    else
      num = CLASS_B;
  }
  else
  {
    if((dat >= set_value) && (dat <= (set_value + 120000)))
    {
      num = CLASS_A;
    }
    else
      num = CLASS_B;
  }
  return num;
}

//是否超过试用期
//over：返回值，1：超期，0：未超期
u8 get_period_state(RTC_TIME *current,DEVICE_INFO *set)
{
  u8 over = 0;
  u32 current_date;
  u32 set_date;
  current_date = current->year * 10000 + current->month * 100 + current->day;
  set_date = set->period_para.period_year * 10000 + set->period_para.period_month * 100 + set->period_para.period_day;
  if(current_date >= set_date)
  {
    over = 1;
  }
  else
    over = 0;
  return over;
}

//功能:从一个数组里查找是否有相同的数
//card:卡片编号，4字节
//buf_lib:数组
//buf_len:数组长度
u8 is_same_data(u32 card,u32 *buf_lib,u16 buf_len)
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

u8 get_card_type(u32 id)
{
  u32 *card_A_buf;
  u32 *card_B_buf;
  u32 *card_repair_buf;
  u8 type;
  card_A_buf = mymalloc(SRAMIN,256);
  card_B_buf = mymalloc(SRAMIN,256);
  card_repair_buf = mymalloc(SRAMIN,256);
  if(card_A_buf != NULL)
    W25QXX_Read((u8 *)card_A_buf,(u32)W25QXX_ADDR_RFID_A,device_info.card_count.card_A_count * 4);//读取A班卡缓冲区
  if(card_B_buf != NULL)
    W25QXX_Read((u8 *)card_B_buf,(u32)W25QXX_ADDR_RFID_B,device_info.card_count.card_B_count * 4);//读取B班卡缓冲区
  if(card_repair_buf != NULL)
    W25QXX_Read((u8 *)card_repair_buf,(u32)W25QXX_ADDR_RFID_REPAIR,device_info.card_count.card_repair_count * 4);//读取维护班卡缓冲区
  if(is_same_data(id,card_A_buf,device_info.card_count.card_A_count))
  {
    type = FUNC_CLASS_A;
  }
  else if(is_same_data(id,card_B_buf,device_info.card_count.card_B_count))
  {
    type = FUNC_CLASS_B;
  }
  else if(is_same_data(id,card_repair_buf,device_info.card_count.card_repair_count))
  {
    type = FUNC_REPAIR;
  }
  else
  {//未注册卡
    type = FUNC_IDLE;
  }
  myfree(SRAMIN,card_A_buf);
  myfree(SRAMIN,card_B_buf);
  myfree(SRAMIN,card_repair_buf);
  return type;
}

void inc_card_type(u32 id,u8 type)
{
  u32 *card_buf;
  card_buf = mymalloc(SRAMIN,256);
  if(type == FUNC_CLASS_A)
  {
    W25QXX_Read((u8 *)card_buf,(u32)W25QXX_ADDR_RFID_A,device_info.card_count.card_A_count * 4);//读取A板卡数据库
    card_buf[device_info.card_count.card_A_count] = id;//将此卡写入缓冲区
    device_info.card_count.card_A_count++;//A班卡数量加1
    W25QXX_Write((u8 *)card_buf,(u32)W25QXX_ADDR_RFID_A,device_info.card_count.card_A_count * 4);//将新的缓冲区写入数据库
    W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));//保存A班卡数量
  }
  else if(type == FUNC_CLASS_B)
  {
    W25QXX_Read((u8 *)card_buf,(u32)W25QXX_ADDR_RFID_B,device_info.card_count.card_B_count * 4);
    card_buf[device_info.card_count.card_B_count] = id;
    device_info.card_count.card_B_count++;
    W25QXX_Write((u8 *)card_buf,(u32)W25QXX_ADDR_RFID_B,device_info.card_count.card_B_count * 4);
    W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
  }
  else if(type == FUNC_REPAIR)
  {
    W25QXX_Read((u8 *)card_buf,(u32)W25QXX_ADDR_RFID_REPAIR,device_info.card_count.card_repair_count * 4);
    card_buf[device_info.card_count.card_repair_count] = id;
    device_info.card_count.card_repair_count++;
    W25QXX_Write((u8 *)card_buf,(u32)W25QXX_ADDR_RFID_REPAIR,device_info.card_count.card_repair_count * 4); 
    W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
  }
  myfree(SRAMIN,card_buf);
}

u16 ChannengMQTTPackage(u8 *buf)
{
  u8 i;
  u16 length = 0;
  u8 name_len;
  name_len = device_info.device_id_len + SlavePara.filename_len;
  buf[0] = name_len;//第一字节为货号长度
  length = 1;
  memcpy(buf + length,device_info.device_id,device_info.device_id_len);//设备ID放在最前面
  length = length + device_info.device_id_len;
  memcpy(buf + length,SlavePara.filename,SlavePara.filename_len);//货号名称放在中间
  length = length + SlavePara.filename_len;
  
  memcpy(buf + length,(u8 *)&product_para,sizeof(product_para));
  length = length + sizeof(product_para);
  return length;
}

u16 PeiliaoMQTTPackage(u8 *buf)
{
  u8 i;
  u16 length = 0;
  u8 name_len;
  name_len = device_info.device_id_len + SlavePara.filename_len;
  buf[0] = name_len;//第一字节为货号长度
  length = 1;
  memcpy(buf + length,device_info.device_id,device_info.device_id_len);//设备ID放在最前面
  length = length + device_info.device_id_len;
  memcpy(buf + length,SlavePara.filename,SlavePara.filename_len);//货号名称放在中间
  length = length + SlavePara.filename_len;
  
  memcpy(buf + length,(u8 *)&peiliao_para,sizeof(peiliao_para));
  length = length + sizeof(peiliao_para);
  return length;
}

u16 SystemStateMQTTPackage(u8 *buf)
{
  u16 length = 0;
  u8 name_len;
  name_len = device_info.device_id_len + SlavePara.filename_len;
  buf[0] = name_len;//第一字节为货号长度
  length = 1;
  memcpy(buf + length,device_info.device_id,device_info.device_id_len);//设备ID放在最前面
  length = length + device_info.device_id_len;
  memcpy(buf + length,SlavePara.filename,SlavePara.filename_len);//货号名称放在中间
  length = length + SlavePara.filename_len;
  buf[length] = device_info.system_state;
  length = length + 1;
  return length;
}
