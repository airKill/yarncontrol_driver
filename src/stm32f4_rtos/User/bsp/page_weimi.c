#include "includes.h"

WEIMI_PARA weimi_para;

//返回1纬时，伺服电机脉冲数
//info:系统参数，包含滚筒和伺服电机齿轮比
//para:电机配置参数，包含纬厘米设置
//num:段号
u16 MotorStepCount(DEVICE_INFO *info,WEIMI_PARA *para,u16 pulse,u8 num)
{
  u16 stepcount = 0;
  //10mm/滚筒周长=滚筒圈数
  //滚筒圈数*齿轮比1*齿轮比2=伺服电机圈数
  //伺服电机圈数*伺服细分数=1cm伺服电机脉冲数
  //1cm伺服电机脉冲数/纬厘米*10=1纬脉冲数
  stepcount = (u16)(10 / CIRCLE_LENGTH * info->ratio.GEAR1 * info->ratio.GEAR2 * SERVOMOTOR_DIV / para->wei_cm_set[num] * 10);
  return stepcount;
}