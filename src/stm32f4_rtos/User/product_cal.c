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

//�������
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

//����ÿ�׳�Ʒ����
float final_per_meter(PRODUCT_PARA *para)
{
  float weight;
  weight = para->latitude_weight + para->longitude_weight + para->rubber_weight;
  return weight;
}

//��������ɲ���
float product_complete_meter(PRODUCT_PARA *para)
{
  float complete_meter;
  complete_meter = para->product_a + para->product_b;//����ɲ���=A�����+B�����
  return complete_meter;
}

//����δ��ɲ���
float product_uncomplete_meter(PRODUCT_PARA *para)
{
  float uncomplete_meter;
  float complete_meter;
  complete_meter = para->product_a + para->product_b;//����ɲ���=A�����+B�����
  uncomplete_meter = para->total_meter_set - complete_meter;//δ��ɲ���=�ܲ���-����ɲ���
  return uncomplete_meter;
}

//����ÿǧγ����
u32 count_per_kilo(PRODUCT_PARA *para)
{
  u32 count;
  count = para->pulse_count / 1000;
  return count;
}

//�������������
float product_complete_kilo(PRODUCT_PARA *para)
{
  float weight;
  weight = final_per_meter(para) * product_complete_meter(para) / para->loss;//���������=ÿ�׳�Ʒ����*��ɲ���/���
  return weight;
}

//����δ�������
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

u8 get_class_time(RTC_TIME *time,PRODUCT_PARA *para)
{
  u8 num;
  u32 dat,set_value;
  dat = (time->hour * 10000) + (time->minute * 100) + time->second;
  set_value = (para->class_time_hour * 10000) + (para->class_time_minute * 100);//�趨�Ļ���ʱ��
  if((dat >= set_value) && (dat <= (set_value + 120000)))//ʱ����08:00:00~20:00:00֮��
  {
    num = CLASS_A;
  }
  else
    num = CLASS_B;
  return num;
}

//����:��һ������������Ƿ�����ͬ����
//card:��Ƭ��ţ�4�ֽ�
//buf_lib:����
//buf_len:���鳤��
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
  card_A_buf = mymalloc(SRAMIN,product_para.card_A_count);
  card_B_buf = mymalloc(SRAMIN,product_para.card_B_count);
  card_repair_buf = mymalloc(SRAMIN,product_para.card_repair_count);
  if(card_A_buf != NULL)
    W25QXX_Read((u8 *)&card_A_buf,(u32)W25QXX_ADDR_RFID_A,product_para.card_A_count);//��ȡA�࿨������
  if(card_B_buf != NULL)
    W25QXX_Read((u8 *)&card_B_buf,(u32)W25QXX_ADDR_RFID_B,product_para.card_B_count);//��ȡB�࿨������
  if(card_repair_buf != NULL)
    W25QXX_Read((u8 *)&card_repair_buf,(u32)W25QXX_ADDR_RFID_REPAIR,product_para.card_repair_count);//��ȡά���࿨������
  if(is_same_data(id,card_A_buf,product_para.card_A_count))
  {
    type = FUNC_CLASS_A;
  }
  else if(is_same_data(id,card_B_buf,product_para.card_B_count))
  {
    type = FUNC_CLASS_B;
  }
  else if(is_same_data(id,card_repair_buf,product_para.card_repair_count))
  {
    type = FUNC_REPAIR;
  }
  else
  {//δע�Ῠ
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
  if(type == FUNC_CLASS_A)
  {
    card_buf = mymalloc(SRAMIN,product_para.card_A_count);
    W25QXX_Read((u8 *)&card_buf,(u32)W25QXX_ADDR_RFID_A,product_para.card_A_count);
    card_buf[product_para.card_A_count] = id;
    product_para.card_A_count++;
    W25QXX_Write((u8 *)&card_buf,(u32)W25QXX_ADDR_RFID_A,product_para.card_A_count);
  }
  else if(type == FUNC_CLASS_B)
  {
    card_buf = mymalloc(SRAMIN,product_para.card_B_count);
    W25QXX_Read((u8 *)&card_buf,(u32)W25QXX_ADDR_RFID_B,product_para.card_B_count);
    card_buf[product_para.card_A_count] = id;
    product_para.card_B_count++;
    W25QXX_Write((u8 *)&card_buf,(u32)W25QXX_ADDR_RFID_B,product_para.card_B_count);
  }
  else if(type == FUNC_REPAIR)
  {
    card_buf = mymalloc(SRAMIN,product_para.card_repair_count);
    W25QXX_Read((u8 *)&card_buf,(u32)W25QXX_ADDR_RFID_REPAIR,product_para.card_repair_count);
    card_buf[product_para.card_A_count] = id;
    product_para.card_repair_count++;
    W25QXX_Write((u8 *)&card_buf,(u32)W25QXX_ADDR_RFID_REPAIR,product_para.card_repair_count);
  }
  myfree(SRAMIN,card_buf);
}


