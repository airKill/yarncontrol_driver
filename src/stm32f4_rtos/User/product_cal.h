#ifndef __PRODUCT_CAL_H
#define __PRODUCT_CAL_H

#include "stm32f4xx.h"

typedef struct
{
  u32 pulse_count; //������
  float product_a;        //A�����       
  float product_b;        //B�����
  u32 working_time;      //�ܿ���ʱ��
  u32 idle_time;         //��ͣ��ʱ��
  float latitude_weight;   //��ɴ����/��
  float longitude_weight;  //γɴ����/��
  float rubber_weight;     //������/��
  float final_weight;    //��Ʒ����/��
  u8 loom_num;          //֯������
  u32 total_meter_set;   //��������
  u32 total_weitht_set;  //����������
  u8 kaidu_set;         //��������
  float weimi_set;         //γ������
  u16 weimi_dis_set;     //γ����ʾ����
  u16 loss;             //���
}PRODUCT_PARA;
extern PRODUCT_PARA product_para;
void init_product_para(PRODUCT_PARA *para);
float product_per_meter(PRODUCT_PARA *para);
float final_per_meter(PRODUCT_PARA *para);
float product_complete_meter(PRODUCT_PARA *para);
float product_uncomplete_meter(PRODUCT_PARA *para);
u32 count_per_kilo(PRODUCT_PARA *para);
float product_complete_kilo(PRODUCT_PARA *para);
float product_uncomplete_kilo(PRODUCT_PARA *para);
float get_float_1bit(float data);
#endif