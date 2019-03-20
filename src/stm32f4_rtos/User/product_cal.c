#include "includes.h"

volatile PRODUCT_PARA product_para;
volatile PEILIAO_PARA peiliao_para;

u32 total_meter_gross = 0;
u32 total_weight_gross = 0;
u8 plan_complete = 0,old_plan_complete = 0;

u8 work_idle = PLUSE_STOP;
u16 work_idle_time = 0;

const char system_state_dis[20][20] = {
  {"ϵͳ����"},
  {"��ת��"},
  {"ת��"},
  {"��PPC����"},
  {"����"},
  {"ȱ��"},
  {"�������"},
  {"���յ�У"},
  {"֯����У"},
  {"ά��"},
  {"��QAD����"},
  {"�滻��ͷ"},   
  {"ϵͳֹͣ"},
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

//�������
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

//����ÿ�׳�Ʒ����
u32 final_per_meter(volatile PEILIAO_PARA para)
{
  u32 weight;
  weight = para.latitude_weight + para.longitude_weight + para.rubber_weight;
  return weight;
}

//��������ɲ���
u32 product_complete_meter(volatile PRODUCT_PARA para)
{
  u32 complete_meter;
  complete_meter = para.product_a + para.product_b;//����ɲ���=A�����+B�����
  return complete_meter;
}

//����δ��ɲ���
u32 product_uncomplete_meter(volatile PRODUCT_PARA para,volatile PEILIAO_PARA peiliao)
{
  u32 uncomplete_meter;
  u32 complete_meter;
  complete_meter = para.product_a + para.product_b;//����ɲ���=A�����+B�����
  uncomplete_meter = peiliao.total_meter_set - complete_meter;//δ��ɲ���=�ܲ���-����ɲ���
  return uncomplete_meter;
}

//����ÿǧγ����
u32 count_per_kilo(u32 pluse)
{
  u32 count;
  count = pluse / 1000;
  return count;
}

//�������������
u32 product_complete_kilo(volatile PRODUCT_PARA para,volatile PEILIAO_PARA peiliao)
{
  float weight;
  weight = final_per_meter(peiliao) / 1000 * product_complete_meter(para);//���������=ÿ�׳�Ʒ����*��ɲ���
  return weight;
}

//����δ�������
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
  set_value = (para->class_para.class_time_hour * 10000) + (para->class_para.class_time_minute * 100);//�趨�Ļ���ʱ��
  if(set_value >= 120000)
  {//����ʱ�����12�㣬���°�ʱ��Ϊ�賿
    if(((dat >= set_value) && (dat <= 235959)) || (dat < (set_value - 120000)))
    {//A��ʱ��Ϊ���趨ʱ��~�賿00:00:00~���趨ʱ�� - 12��00:00��
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

//�Ƿ񳬹�������
//over������ֵ��1�����ڣ�0��δ����
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
  card_A_buf = mymalloc(SRAMIN,256);
  card_B_buf = mymalloc(SRAMIN,256);
  card_repair_buf = mymalloc(SRAMIN,256);
  if(card_A_buf != NULL)
    W25QXX_Read((u8 *)card_A_buf,(u32)W25QXX_ADDR_RFID_A,device_info.card_count.card_A_count * 4);//��ȡA�࿨������
  if(card_B_buf != NULL)
    W25QXX_Read((u8 *)card_B_buf,(u32)W25QXX_ADDR_RFID_B,device_info.card_count.card_B_count * 4);//��ȡB�࿨������
  if(card_repair_buf != NULL)
    W25QXX_Read((u8 *)card_repair_buf,(u32)W25QXX_ADDR_RFID_REPAIR,device_info.card_count.card_repair_count * 4);//��ȡά���࿨������
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
  card_buf = mymalloc(SRAMIN,256);
  if(type == FUNC_CLASS_A)
  {
    W25QXX_Read((u8 *)card_buf,(u32)W25QXX_ADDR_RFID_A,device_info.card_count.card_A_count * 4);//��ȡA�忨���ݿ�
    card_buf[device_info.card_count.card_A_count] = id;//���˿�д�뻺����
    device_info.card_count.card_A_count++;//A�࿨������1
    W25QXX_Write((u8 *)card_buf,(u32)W25QXX_ADDR_RFID_A,device_info.card_count.card_A_count * 4);//���µĻ�����д�����ݿ�
    W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));//����A�࿨����
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
  buf[0] = name_len;//��һ�ֽ�Ϊ���ų���
  length = 1;
  memcpy(buf + length,device_info.device_id,device_info.device_id_len);//�豸ID������ǰ��
  length = length + device_info.device_id_len;
  memcpy(buf + length,SlavePara.filename,SlavePara.filename_len);//�������Ʒ����м�
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
  buf[0] = name_len;//��һ�ֽ�Ϊ���ų���
  length = 1;
  memcpy(buf + length,device_info.device_id,device_info.device_id_len);//�豸ID������ǰ��
  length = length + device_info.device_id_len;
  memcpy(buf + length,SlavePara.filename,SlavePara.filename_len);//�������Ʒ����м�
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
  buf[0] = name_len;//��һ�ֽ�Ϊ���ų���
  length = 1;
  memcpy(buf + length,device_info.device_id,device_info.device_id_len);//�豸ID������ǰ��
  length = length + device_info.device_id_len;
  memcpy(buf + length,SlavePara.filename,SlavePara.filename_len);//�������Ʒ����м�
  length = length + SlavePara.filename_len;
  buf[length] = device_info.system_state;
  length = length + 1;
  return length;
}
