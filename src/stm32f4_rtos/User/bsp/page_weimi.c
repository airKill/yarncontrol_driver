#include "includes.h"

WEIMI_PARA weimi_para;
MOTOR_PROCESS MotorProcess;
//u8 isMotorStop = 0;
//����1γʱ���ŷ����������
//info:ϵͳ������������Ͳ���ŷ�������ֱ�
//para:������ò���������γ��������
//num:�κ�
u16 MotorStepCount(DEVICE_INFO *info,WEIMI_PARA *para,u8 num)
{
  u16 stepcount = 0;
  //10mm/��Ͳ�ܳ�=��ͲȦ��
  //��ͲȦ��*���ֱ�1*���ֱ�2=�ŷ����Ȧ��
  //�ŷ����Ȧ��*�ŷ�ϸ����=1cm�ŷ����������
  //1cm�ŷ����������/γ����=1γ������
  stepcount = (u16)(10 / CIRCLE_LENGTH * info->ratio.GEAR1 * info->ratio.GEAR2 * SERVOMOTOR_DIV / para->wei_cm_set[num]);
  return stepcount;
}

void init_weimi_para(WEIMI_PARA *para)
{
  u8 i;
  for(i=0;i<20;i++)
  {
    para->total_wei_count[i] = 0;
  }
  para->wei_cm_set[0] = 10;
    para->wei_inch_set[0] = para->wei_cm_set[0] * 2.54;
  for(i=1;i<20;i++)
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