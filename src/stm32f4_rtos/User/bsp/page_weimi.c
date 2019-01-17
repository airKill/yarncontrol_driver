#include "includes.h"

WEIMI_PARA weimi_para;

//����1γʱ���ŷ����������
//info:ϵͳ������������Ͳ���ŷ�������ֱ�
//para:������ò���������γ��������
//num:�κ�
u16 MotorStepCount(DEVICE_INFO *info,WEIMI_PARA *para,u16 pulse,u8 num)
{
  u16 stepcount = 0;
  //10mm/��Ͳ�ܳ�=��ͲȦ��
  //��ͲȦ��*���ֱ�1*���ֱ�2=�ŷ����Ȧ��
  //�ŷ����Ȧ��*�ŷ�ϸ����=1cm�ŷ����������
  //1cm�ŷ����������/γ����*10=1γ������
  stepcount = (u16)(10 / CIRCLE_LENGTH * info->ratio.GEAR1 * info->ratio.GEAR2 * SERVOMOTOR_DIV / para->wei_cm_set[num] * 10);
  return stepcount;
}