#include "includes.h"

PRODUCT_PARA product_para;

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


