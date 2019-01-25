#ifndef _PAGE_WEIMI_H
#define _PAGE_WEIMI_H

#define ROLLER_D        80.0
#define PI              3.1415926
#define CIRCLE_LENGTH   (PI * ROLLER_D)
#define SERVOMOTOR_DIV  1000

typedef struct
{
  u32 real_wei_count[20];//��ǰγ��
  float wei_cm_set[10];//γ����
  float wei_inch_set[10];//γӢ��
  u32 total_wei_count[20];//������γ��
  u16 step1_speed[10];//��γ����ٶ�
  u16 step2_speed[10];//���ߵ���ٶ�
}WEIMI_PARA;
extern WEIMI_PARA weimi_para;

typedef struct
{
  u8 current_seg;
  u32 current_wei;
  u32 total_wei;
  u32 real_wei_count;
  u16 step1_speed;
  u16 step2_speed;
}MOTOR_PROCESS;
extern MOTOR_PROCESS MotorProcess;

u16 MotorStepCount(DEVICE_INFO *info,WEIMI_PARA *para,u8 num);
void init_weimi_para(WEIMI_PARA *para);
void get_weimi_para(WEIMI_PARA *para,DEVICE_INFO *info,MOTOR_PROCESS *motor);
#endif