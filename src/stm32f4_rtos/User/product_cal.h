#ifndef __PRODUCT_CAL_H
#define __PRODUCT_CAL_H

#include "stm32f4xx.h"
#include "protocol.h"

typedef enum 
{
  CLASS_A = 0,       //A��
  CLASS_B,
}CLASS_NUM;

typedef enum
{
  READ_PERMISSION = 0,
  WRITE_PERMISSION,
  WRITE_INC_A,
  WRITE_DEC_A,
  WRITE_INC_B,
  WRITE_DEC_B,
  WRITE_INC_REPAIR,
  WRITE_DEC_REPAIR,
}CARD_FUNC;

typedef enum 
{
  SYS_NORMAL = 0,       //ϵͳ����
  SYS_WAIT_TRANSFER,    //��ת��
  SYS_TRANSFER,         //ת��
  SYS_WAIT_PPC,         //��PPC����
  SYS_WAIT_MATERIAL,    //����
  SYS_NO_MATERIAL,      //ȱ��
  SYS_CLEAN,            //�������
  SYS_TECH_ADJUST,      //���յ�У
  SYS_DEVICE_ADJUST,    //֯����У
  SYS_REPAIR,           //ά��
  SYS_WAIT_QAD,         //��QAD����
  SYS_REPLACE_PAN       //�滻��ͷ
}SYSTEM_STATE;

typedef struct
{
  u8 system_state;
  u32 stop_time[11];
  u32 pulse_count; //������
  float product_a;        //A�����       
  float product_b;        //B�����
  u32 total_work_time;      //�ܿ���ʱ��
  u32 total_stop_time;         //��ͣ��ʱ��
  float latitude_weight;   //��ɴ����/��
  float longitude_weight;  //γɴ����/��
  float rubber_weight;     //������/��
  float final_weight;    //��Ʒ����/��
  u8 loom_num;          //֯������
  u32 total_meter_set;   //��������
  u32 total_weitht_set;  //����������
  float kaidu_set;         //��������
  float weimi_set;         //γ������
  u16 weimi_dis_set;     //γ����ʾ����
  u16 loss;             //���
  u16 speed;            //�����ٶ�   
  u16 card_A_count;     //A������
  u16 card_B_count;     //B������
  u16 card_repair_count;//ά�޿�Ƭ����
  u8 class_time_hour;
  u8 class_time_minute;
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
u8 get_class_time(RTC_TIME *time,PRODUCT_PARA *para);
u8 is_same_data(u32 card,u32 *buf_lib,u16 buf_len);
u8 get_card_type(u32 id);
void inc_card_type(u32 id,u8 type);
#endif
