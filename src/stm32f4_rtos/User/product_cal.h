#ifndef __PRODUCT_CAL_H
#define __PRODUCT_CAL_H

#include "stm32f4xx.h"

typedef struct
{
  u32 pulse_count; //脉冲数
  float product_a;        //A班产量       
  float product_b;        //B班产量
  u32 working_time;      //总开机时间
  u32 idle_time;         //总停机时间
  float latitude_weight;   //经纱重量/米
  float longitude_weight;  //纬纱重量/米
  float rubber_weight;     //橡胶重量/米
  float final_weight;    //成品重量/米
  u8 loom_num;          //织机条数
  u32 total_meter_set;   //总米设置
  u32 total_weitht_set;  //总重量设置
  u8 kaidu_set;         //开度设置
  float weimi_set;         //纬密设置
  u16 weimi_dis_set;     //纬密显示设置
  u16 loss;             //损耗
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