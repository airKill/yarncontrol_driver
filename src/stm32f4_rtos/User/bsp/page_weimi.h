#ifndef _PAGE_WEIMI_H
#define _PAGE_WEIMI_H

#define ROLLER_D        80.0
#define PI              3.1415926
#define CIRCLE_LENGTH   (PI * ROLLER_D)
#define SERVOMOTOR_DIV  1000

typedef struct
{
  u32 real_wei_count[10];
  u32 real_total_wei[10];
  float wei_cm_set[10];
  float wei_inch_set[10];
  u32 total_wei_count[10];
  u32 median_wei_count[10];
  u16 step1_speed[10];
  u16 step2_speed[10];
}WEIMI_PARA;
extern WEIMI_PARA weimi_para;

u16 MotorStepCount(DEVICE_INFO *info,WEIMI_PARA *para,u16 pulse,u8 num);

#endif