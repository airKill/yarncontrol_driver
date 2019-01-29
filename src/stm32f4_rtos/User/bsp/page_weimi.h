#ifndef _PAGE_WEIMI_H
#define _PAGE_WEIMI_H

#define ROLLER_D        80.0
#define PI              3.1415926
#define CIRCLE_LENGTH   (PI * ROLLER_D)
#define SERVOMOTOR_DIV  1000

typedef struct
{
  u32 real_wei_count[20];//当前纬号
  float wei_cm_set[10];//纬厘米
  float wei_inch_set[10];//纬英寸
  u32 total_wei_count[20];//设置总纬号
  u16 step1_speed[10];//送纬电机速度
  u16 step2_speed[10];//底线电机速度
  float step1_factor[10];
  float step2_factor[10];
}WEIMI_PARA;
extern WEIMI_PARA weimi_para;

typedef struct
{
  u8 current_seg;
  u32 current_wei;
  u32 total_wei;
  u32 real_wei_count;
  float step1_factor;
  float step2_factor;
}MOTOR_PROCESS;
extern MOTOR_PROCESS MotorProcess;

extern u16 speed_zhu;

u16 MotorStepCount(DEVICE_INFO *info,WEIMI_PARA *para,u8 num);
void init_weimi_para(WEIMI_PARA *para);
void get_weimi_para(WEIMI_PARA *para,DEVICE_INFO *info,MOTOR_PROCESS *motor);
u16 get_main_speed(float freq);
u16 from_speed_step(u16 speed);
#endif