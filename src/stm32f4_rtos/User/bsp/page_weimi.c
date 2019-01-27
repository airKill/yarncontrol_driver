#include "includes.h"

WEIMI_PARA weimi_para;
MOTOR_PROCESS MotorProcess;

//u8 isMotorStop = 0;
//返回1纬时，伺服电机脉冲数
//info:系统参数，包含滚筒和伺服电机齿轮比
//para:电机配置参数，包含纬厘米设置
//num:段号
u16 MotorStepCount(DEVICE_INFO *info,WEIMI_PARA *para,u8 num)
{
  u16 stepcount = 0;
  //10mm/滚筒周长=滚筒圈数
  //滚筒圈数*齿轮比1*齿轮比2=伺服电机圈数
  //伺服电机圈数*伺服细分数=1cm伺服电机脉冲数
  //1cm伺服电机脉冲数/纬厘米=1纬脉冲数
  stepcount = (u16)(10 / CIRCLE_LENGTH * info->ratio.GEAR1 * info->ratio.GEAR2 * SERVOMOTOR_DIV / para->wei_cm_set[num]);
  return stepcount;
}

void init_weimi_para(WEIMI_PARA *para)
{
  u8 i;
  for(i=0;i<20;i++)
  {
    para->real_wei_count[i] = 0;
    if((i % 2) == 0)
      para->total_wei_count[i] = 1000;//段号纬循环设置
    else
      para->total_wei_count[i] = 20;//过渡纬循环设置
  }
  para->wei_cm_set[0] = 10;
  para->wei_inch_set[0] = para->wei_cm_set[0] * 2.54;
  for(i=1;i<10;i++)
  {
    para->wei_cm_set[i] = 0;
    para->wei_inch_set[i] = 0;
  }
  for(i=0;i<10;i++)
  {
    para->step1_speed[i] = 0;
    para->step2_speed[i] = 0;
  }
}

void get_weimi_para(WEIMI_PARA *para,DEVICE_INFO *info,MOTOR_PROCESS *motor)
{
  motor->current_seg = info->weimi_info.reg;
  motor->current_wei = info->weimi_info.count;
  motor->total_wei = para->total_wei_count[motor->current_seg];
  motor->real_wei_count = para->real_wei_count[motor->current_seg];
  motor->step1_speed = para->step1_speed[motor->current_seg / 2];
  motor->step2_speed = para->step2_speed[motor->current_seg / 2];
}