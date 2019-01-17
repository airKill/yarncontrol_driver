#ifndef __PRODUCT_CAL_H
#define __PRODUCT_CAL_H

#include "stm32f4xx.h"
#include "protocol.h"
#include "para.h"

#define PLUSE_STOP    0
#define PLUSE_IDLE    1
#define PLUSE_WORK    2

typedef enum 
{
  CLASS_A = 0,       //A��
  CLASS_B,
}CLASS_NUM;

typedef enum
{
  CARD_DISABLE= 0,
  READ_PERMISSION,
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
  SYS_REPLACE_PAN,       //�滻��ͷ
  SYS_STOP
}SYSTEM_STATE;

typedef struct
{
  float product_a;        //A�����       
  float product_b;        //B�����
  float product_uncomplete;        //δ��ɲ���       
  float product_complete;          //����ɲ���
  u32 weicount_kilowei;         //γ��/ǧγ
  u16 speed;            //�����ٶ�   
  u32 total_work_time;      //�ܿ���ʱ��
  u32 total_stop_time;         //��ͣ��ʱ��
  float weight_uncomplete;        //δ�������       
  float weight_complete;          //���������
}PRODUCT_PARA;
extern PRODUCT_PARA product_para;

typedef struct
{
  float latitude_weight;   //��ɴ����/��
  float longitude_weight;  //γɴ����/��
  float rubber_weight;     //������/��
  float final_weight;    //��Ʒ����/��
  u8 loom_num;          //֯������
  u8 loss;             //���
  u32 total_meter_set;   //��������
  u32 total_weitht_set;  //����������
  float kaidu_set;         //��������
  float weimi_set;         //γ������
  u32 add_meter_set;       //��������    
  u16 weimi_dis_set;       //γ����ʾ����
}PEILIAO_PARA;
extern PEILIAO_PARA peiliao_para;

extern u8 work_idle;
extern u16 work_idle_time;

extern u32 total_meter_gross;
extern u32 total_weight_gross;
extern u8 plan_complete,old_plan_complete;
extern const char system_state_dis[20][20];

void init_product_para(PRODUCT_PARA *para);
void init_peiliao_para(PEILIAO_PARA *para);
float product_per_meter(PEILIAO_PARA *para,u32 pluse);
float final_per_meter(PEILIAO_PARA *para);
float product_complete_meter(PRODUCT_PARA *para);
float product_uncomplete_meter(PRODUCT_PARA *para,PEILIAO_PARA *peiliao);
u32 count_per_kilo(u32 pluse);
float product_complete_kilo(PRODUCT_PARA *para,PEILIAO_PARA *peiliao);
float product_uncomplete_kilo(PRODUCT_PARA *para,PEILIAO_PARA *peiliao);
float get_float_1bit(float data);
u8 get_class_time(RTC_TIME *time,DEVICE_INFO *para);
u8 is_same_data(u32 card,u32 *buf_lib,u16 buf_len);
u8 get_card_type(u32 id);
void inc_card_type(u32 id,u8 type);
u8 get_period_state(RTC_TIME *current,DEVICE_INFO *set);

#endif
