#ifndef _PAGE_WEIMI_H
#define _PAGE_WEIMI_H

#define ROLLER_D        80.0
#define PI              3.1415926
#define CIRCLE_LENGTH   (PI * ROLLER_D)
#define SERVOMOTOR_DIV  1000

#define FORWARD 0
#define BACKWARD 1

#define AUTO 0
#define MANUAL 1

#define WEIMI_SIZE      300

typedef struct
{
  u32 real_wei_count[20];//当前纬号
  float wei_cm_set[10];//纬厘米
  float wei_inch_set[10];//纬英寸
  u32 total_wei_count[20];//设置总纬号
  u16 step1_factor[10];//送纬1速度比
  u16 step2_factor[10];//送纬2速度比
  u16 step3_factor[10];//底线速度比
}WEIMI_PARA;
extern WEIMI_PARA weimi_para;

typedef struct
{
  u32 wei_circle[10];//纬循环
  float wei_cm[10];//纬厘米
  u32 wei_guodu[10];//纬过渡
}WEIMI_MQTT;

typedef struct
{
  u16 step1_factor[10];//送纬1速度比
  u16 step2_factor[10];//送纬2速度比
  u16 step3_factor[10];//底线速度比
}WEISHA_MQTT;

typedef struct
{
  u8 current_seg;
  u32 current_wei;
  u32 total_wei;
  u32 real_wei_count;
  u16 step1_factor;
  u16 step2_factor;
  u16 step3_factor;
}MOTOR_PROCESS;
extern MOTOR_PROCESS MotorProcess;

extern u16 speed_zhu;
extern u8 step_motor_adjust;
extern u16 servomotor_step;
extern u8 servomotor_dir;
extern u8 servomotor_mode;
extern u8 is_stop,old_is_stop;
extern u8 valid_seg;

u16 MotorStepCount(DEVICE_INFO *info,WEIMI_PARA *para,u8 num);
void init_weimi_para(WEIMI_PARA *para,PEILIAO_PARA *peiliao);
void get_weimi_para(WEIMI_PARA *para,DEVICE_INFO *info,MOTOR_PROCESS *motor);
u16 get_main_speed(float freq);
u32 from_speed_step(float speed);
u8 get_valid_seg(WEIMI_PARA *para);
u16 WeimiMQTTPackage(u8 *buf);
u16 WeishaMQTTPackage(u8 *buf);
#endif