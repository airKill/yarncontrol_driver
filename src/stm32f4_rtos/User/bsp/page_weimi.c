#include "includes.h"

WEIMI_PARA weimi_para;
MOTOR_PROCESS MotorProcess;
u16 speed_zhu = 0,old_speed_zhu = 0;
u8 servomotor_guodu = 0;//�ŷ�������ɵ��ٱ�־
u8 stepmotor_guodu[3] = {0,0,0};

u8 servomotor_dir = FORWARD;
u8 servomotor_mode = AUTO;
u8 is_stop = 0,old_is_stop = 0xff;
u8 max_seg = 0,valid_seg[4] = 0;

u8 fault_weimi_flag = 0;

const float SPEED_RADIO[3] = {SPEED_RADIO12,SPEED_RADIO12,SPEED_RADIO3};

u16 servomotor_step = 0;
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

void init_weimi_para(WEIMI_PARA *para,PEILIAO_PARA peiliao)
{
  u8 i;
  for(i=0;i<20;i++)
  {
    para->real_wei_count[i] = 0;
    if((i % 2) == 0)
      para->total_wei_count[i] = 50;//�κ�γѭ������
    else
      para->total_wei_count[i] = 20;//����γѭ������
  }
  para->wei_cm_set[0] = peiliao.weimi_set / 10.0;
  para->wei_inch_set[0] = para->wei_cm_set[0] * 2.54;
  for(i=1;i<10;i++)
  {
    para->wei_cm_set[i] = 0;
    para->wei_inch_set[i] = 0;
  }
  para->step_factor[0][0] = 50;
  para->step_factor[1][0] = 50;
  para->step_factor[2][0] = 50;
  for(i=1;i<10;i++)
  {
    para->step_factor[0][i] = 0;
    para->step_factor[1][i] = 0;
    para->step_factor[2][i] = 0;
  }
}

void get_weimi_para(WEIMI_PARA *para,DEVICE_INFO *info,MOTOR_PROCESS *motor)
{
  motor->current_seg = info->weimi_info.reg;
  motor->songwei_seg[0] = info->weimi_info.songwei_seg[0];
  motor->songwei_seg[1] = info->weimi_info.songwei_seg[1];
  motor->songwei_seg[2] = info->weimi_info.songwei_seg[2];
  motor->current_wei = info->weimi_info.count;
  motor->song_current_wei[0] = info->weimi_info.count;
  motor->song_current_wei[1] = info->weimi_info.count;
  motor->song_current_wei[2] = info->weimi_info.count;
  motor->total_wei = para->total_wei_count[motor->current_seg];
  motor->real_wei_count = para->real_wei_count[motor->current_seg];
  motor->song_total_wei[0] = para->total_wei_count[motor->songwei_seg[0] * 2];
  motor->song_total_wei[1] = para->total_wei_count[motor->songwei_seg[1] * 2];
  motor->song_total_wei[2] = para->total_wei_count[motor->songwei_seg[2] * 2];
  motor->step_factor[0] = para->step_factor[0][motor->current_seg / 2];
  motor->step_factor[1] = para->step_factor[1][motor->current_seg / 2];
  motor->step_factor[2] = para->step_factor[2][motor->current_seg / 2];
}

u16 get_main_speed(float freq)
{
  u16 speed;
  speed = (u16)(freq / 600.0 * 60);//������Ƶ��/����600*60=����ת��/����
  return speed;
}
//��ת��ת��Ϊ�����������
//���룺speed ת/����
//�����count ����
u32 from_speed_step(float speed)
{
  u16 freq;
  u32 count;
  freq = speed / 60 * 360 / 1.8 * 16;
  count = 4000000 / freq;
  return count;
}

u8 get_valid_seg(WEIMI_PARA para)
{
  u8 seg = 0;
  u8 i;
  for(i=0;i<20;i++)
  {
//    if((weimi_para.total_wei_count[i] == 0) || (weimi_para.wei_cm_set[i / 2] == 0))
    if((weimi_para.wei_cm_set[i / 2] == 0))
    {//�κ���γѭ����γ���׶����ã�����Ч
      seg = i;
      break;
    }
  }
  return seg / 2;
}

u8 get_songwei0_maxseg(WEIMI_PARA para)
{
  u8 seg = 0;
  u8 i;
  if(weimi_para.step_factor[0][0] == 0)
    return 0;
  for(i=0;i<10;i++)
  {
    if(weimi_para.step_factor[0][i] == 0)
    {//�κ����ٱ����ò�Ϊ0����Ч
      seg = i;
      break;
    }
  }
  return seg;
}

u8 get_songwei1_maxseg(WEIMI_PARA para)
{
  u8 seg = 0;
  u8 i;
  if(weimi_para.step_factor[1][0] == 0)
    return 0;
  for(i=0;i<10;i++)
  {
    if(weimi_para.step_factor[1][i] == 0)
    {//�κ����ٱ����ò�Ϊ0����Ч
      seg = i;
      break;
    }
  }
  return seg;
}

u8 get_songwei2_maxseg(WEIMI_PARA para)
{
  u8 seg = 0;
  u8 i;
  if(weimi_para.step_factor[2][0] == 0)
    return 0;
  for(i=0;i<10;i++)
  {
    if(weimi_para.step_factor[2][i] == 0)
    {//�κ����ٱ����ò�Ϊ0����Ч
      seg = i;
      break;
    }
  }
  return seg;
}

u16 WeimiMQTTPackage(u8 *buf)
{
  u8 i;
  u16 length = 0;
  WEIMI_MQTT weimi_mqtt;
  u8 name_len;
  name_len = device_info.device_id_len + SlavePara.filename_len;
  buf[0] = name_len;//��һ�ֽ�Ϊ���ų���
  length = 1;
  memcpy(buf + length,device_info.device_id,device_info.device_id_len);//�豸ID������ǰ��
  length = length + device_info.device_id_len;
  memcpy(buf + length,SlavePara.filename,SlavePara.filename_len);//�������Ʒ����м�
  length = length + SlavePara.filename_len;
  
  for(i=0;i<10;i++)
  {
    weimi_mqtt.wei_circle[i] = weimi_para.total_wei_count[i * 2];
    weimi_mqtt.wei_cm[i] = weimi_para.wei_cm_set[i];
    weimi_mqtt.wei_guodu[i] = weimi_para.total_wei_count[i * 2 + 1];
  }
  memcpy(buf + length,(u8 *)&weimi_mqtt,sizeof(WEIMI_MQTT));
  length = length + sizeof(WEIMI_MQTT);
  return length;
}

u16 WeishaMQTTPackage(u8 *buf)
{
  u8 i;
  u16 length = 0;
  WEISHA_MQTT weisha_mqtt;
  u8 name_len;
  name_len = device_info.device_id_len + SlavePara.filename_len;
  buf[0] = name_len;//��һ�ֽ�Ϊ���ų���
  length = 1;
  memcpy(buf + length,device_info.device_id,device_info.device_id_len);//�豸ID������ǰ��
  length = length + device_info.device_id_len;
  memcpy(buf + length,SlavePara.filename,SlavePara.filename_len);//�������Ʒ����м�
  length = length + SlavePara.filename_len;
  
  for(i=0;i<10;i++)
  {
    weisha_mqtt.step_factor[0][i] = weimi_para.step_factor[0][i];
    weisha_mqtt.step_factor[1][i] = weimi_para.step_factor[1][i];
    weisha_mqtt.step_factor[2][i] = weimi_para.step_factor[2][i];
  }
  memcpy(buf + length,(u8 *)&weisha_mqtt,sizeof(WEISHA_MQTT));
  length = length + sizeof(WEISHA_MQTT);
  return length;
}

u8 get_max_type(u8 *buf)
{
  u8 i;
  u8 index = 0;
  u8 max = 0;
  for(i=0;i<4;i++)
  {
    if(buf[i] > max)
    {
      max = buf[i];
    }
  }
  for(i=0;i<4;i++)
  {
    if(max == buf[i])
    {
      index = i;
      break;
    }
  }
  return index;
}

