#ifndef __PRODUCT_CAL_H
#define __PRODUCT_CAL_H

#include "stm32f4xx.h"
#include "protocol.h"
#include "para.h"

#define PLUSE_STOP    0
#define PLUSE_IDLE    1
#define PLUSE_WORK    2

#define PEILIAO_SIZE    50

typedef enum 
{
  CLASS_A = 0,       //A班
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
  SYS_REPLACE_PAN,       //替换盘头
  SYS_STOP
}SYSTEM_STATE;

typedef struct
{
  float product_a;        //A班产量       
  float product_b;        //B班产量
  u32 product_uncomplete;        //未完成产量       
  u32 product_complete;          //已完成产量
  u32 weicount_kilowei;         //纬数/千纬
  u32 total_work_time;      //总开机时间
  u32 total_stop_time;         //总停机时间
  u32 weight_uncomplete;        //未完成重量       
  u32 weight_complete;          //已完成重量
  u16 speed;            //机器速度   
}PRODUCT_PARA;
extern PRODUCT_PARA product_para;

typedef struct
{
  u32 latitude_weight;   //经纱重量/米，设置重量的10倍
  u32 longitude_weight;  //纬纱重量/米
  u32 rubber_weight;     //橡胶重量/米
  u32 final_weight;    //成品重量/米
  u32 total_meter_set;   //总米设置
  u32 total_weitht_set;  //总重量设置
  u32 complete_meter;
  u32 complete_work_time;
  u32 kaidu_set;         //开度设置
  u32 weimi_set;         //纬密设置
  u32 add_meter_set;       //补单数量    
  u16 weimi_dis_set;       //纬密显示设置
  u8 loom_num;          //织机条数
  u8 loss;             //损耗
}PEILIAO_PARA;
extern PEILIAO_PARA peiliao_para;

extern u8 work_idle;
extern u16 work_idle_time;

extern u32 total_meter_gross;
extern u32 total_weight_gross;
extern u8 plan_complete,old_plan_complete;
extern const char system_state_dis[20][20];

void init_product_para(PRODUCT_PARA *para,PEILIAO_PARA peiliao);
void init_peiliao_para(PEILIAO_PARA *para);
float product_per_meter(PEILIAO_PARA para,u32 pluse);
u32 final_per_meter(PEILIAO_PARA para);
u32 product_complete_meter(PRODUCT_PARA para);
u32 product_uncomplete_meter(PRODUCT_PARA para,PEILIAO_PARA peiliao);
u32 count_per_kilo(u32 pluse);
u32 product_complete_kilo(PRODUCT_PARA para,PEILIAO_PARA peiliao);
u32 product_uncomplete_kilo(PRODUCT_PARA para,PEILIAO_PARA peiliao);
float get_float_1bit(float data);
u8 get_class_time(RTC_TIME *time,DEVICE_INFO *para);
u8 is_same_data(u32 card,u32 *buf_lib,u16 buf_len);
u8 get_card_type(u32 id);
void inc_card_type(u32 id,u8 type);
u8 get_period_state(RTC_TIME *current,DEVICE_INFO *set);
u16 ChannengMQTTPackage(u8 *buf);
u16 PeiliaoMQTTPackage(u8 *buf);
u16 SystemStateMQTTPackage(u8 *buf);
#endif
