#ifndef __PRODUCT_CAL_H
#define __PRODUCT_CAL_H

#include "stm32f4xx.h"
#include "protocol.h"

typedef enum 
{
  CLASS_A = 0,       //A班
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
  SYS_NORMAL = 0,       //系统正常
  SYS_WAIT_TRANSFER,    //待转机
  SYS_TRANSFER,         //转机
  SYS_WAIT_PPC,         //待PPC处理
  SYS_WAIT_MATERIAL,    //待料
  SYS_NO_MATERIAL,      //缺料
  SYS_CLEAN,            //清洁卫生
  SYS_TECH_ADJUST,      //工艺调校
  SYS_DEVICE_ADJUST,    //织机调校
  SYS_REPAIR,           //维修
  SYS_WAIT_QAD,         //待QAD处理
  SYS_REPLACE_PAN       //替换盘头
}SYSTEM_STATE;

typedef struct
{
  u8 system_state;
  u32 stop_time[11];
  u32 pulse_count; //脉冲数
  float product_a;        //A班产量       
  float product_b;        //B班产量
  u32 total_work_time;      //总开机时间
  u32 total_stop_time;         //总停机时间
  float latitude_weight;   //经纱重量/米
  float longitude_weight;  //纬纱重量/米
  float rubber_weight;     //橡胶重量/米
  float final_weight;    //成品重量/米
  u8 loom_num;          //织机条数
  u32 total_meter_set;   //总米设置
  u32 total_weitht_set;  //总重量设置
  float kaidu_set;         //开度设置
  float weimi_set;         //纬密设置
  u16 weimi_dis_set;     //纬密显示设置
  u16 loss;             //损耗
  u16 speed;            //机器速度   
  u16 card_A_count;     //A班数量
  u16 card_B_count;     //B班数量
  u16 card_repair_count;//维修卡片数量
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
