#include "includes.h"

WEIMI_PARA weimi_para;
MOTOR_PROCESS MotorProcess;
u16 speed_zhu = 0;
u16 speed_zhu1 = 0;
u8 servomotor_guodu = 0;//伺服电机过渡调速标志
u8 stepmotor_guodu[3] = {0,0,0};

u8 servomotor_dir = FORWARD;
u8 servomotor_mode = AUTO;
u8 is_stop = 0,old_is_stop = 0xff;
u8 max_seg = 0,valid_seg[4] = {0,0,0,0};

u8 fault_weimi_flag = 0;

const float SPEED_RADIO[3] = {SPEED_RADIO12,SPEED_RADIO12,SPEED_RADIO3};

//u16 servomotor_step = 0;
//u8 isMotorStop = 0;
//返回1纬时，伺服电机脉冲数
//info:系统参数，包含滚筒和伺服电机齿轮比
//para:电机配置参数，包含纬厘米设置
//num:段号
//u16 MotorStepCount(DEVICE_INFO *info,WEIMI_PARA *para,u8 num)
//{
//  u16 stepcount = 0;
//  //10mm/滚筒周长=滚筒圈数
//  //滚筒圈数*齿轮比1*齿轮比2=伺服电机圈数
//  //伺服电机圈数*伺服细分数=1cm伺服电机脉冲数
//  //1cm伺服电机脉冲数/纬厘米=1纬脉冲数
//  stepcount = (u16)(10 / CIRCLE_LENGTH * info->ratio.GEAR1 * info->ratio.GEAR2 * SERVOMOTOR_DIV / para->wei_cm_set[num]);
//  return stepcount;
//}

void init_weimi_para(WEIMI_PARA *para,PEILIAO_PARA peiliao)
{
  u8 i;
  for(i=0;i<20;i++)
  {
    para->real_wei_count[i] = 0;
    if((i % 2) == 0)
      para->total_wei_count[i] = 0;//段号纬循环设置
    else
      para->total_wei_count[i] = 0;//过渡纬循环设置
  }
  
  para->total_wei_count[0] = 50;
  para->total_wei_count[1] = 20;
  
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
//  motor->current_seg = info->weimi_info.reg;
//  motor->songwei_seg[0] = info->weimi_info.songwei_seg[0];
//  motor->songwei_seg[1] = info->weimi_info.songwei_seg[1];
//  motor->songwei_seg[2] = info->weimi_info.songwei_seg[2];
//  motor->current_wei = info->weimi_info.count;
//  motor->song_current_wei[0] = info->weimi_info.songwei_count[0];
//  motor->song_current_wei[1] = info->weimi_info.songwei_count[1];
//  motor->song_current_wei[2] = info->weimi_info.songwei_count[2];
  motor->total_wei = para->total_wei_count[motor->current_seg *2 + servomotor_guodu];
  motor->real_wei_count = para->real_wei_count[motor->current_seg];
  motor->song_total_wei[0] = para->total_wei_count[motor->songwei_seg[0] * 2 + stepmotor_guodu[0]];
  motor->song_total_wei[1] = para->total_wei_count[motor->songwei_seg[1] * 2 + stepmotor_guodu[1]];
  motor->song_total_wei[2] = para->total_wei_count[motor->songwei_seg[2] * 2 + stepmotor_guodu[2]];
  motor->wei_cm_set = para->wei_cm_set[motor->current_seg];
  motor->step_factor[0] = para->step_factor[0][motor->current_seg / 2];
  motor->step_factor[1] = para->step_factor[1][motor->current_seg / 2];
  motor->step_factor[2] = para->step_factor[2][motor->current_seg / 2];
}

u16 get_main_speed(float freq)
{
  u16 speed;
  speed = (u16)(freq / 600.0 * 60);//编码器频率/线数600*60=主轴转速/分钟
  return speed;
}

//将转速转换为伺服电机步数
//输入：speed 转/分钟
//输出：count 步数
u32 get_sevro_step(float speed)
{
  u16 freq;
  u32 count;
  if(speed > 0)
  {
//    freq = speed / 60 * 1000;
//    count = 4000000 / freq;
    count = 240000 / speed;
  }
  else
    count = 0;
  return count;
}

void reset_seg_to_1(void)
{
  MotorProcess.current_seg = 0;
  MotorProcess.current_wei = 1;
  MotorProcess.songwei_seg[0] = 0;
  MotorProcess.songwei_seg[1] = 0;
  MotorProcess.songwei_seg[2] = 0;
  MotorProcess.song_current_wei[0] = 1;
  MotorProcess.song_current_wei[1] = 1;
  MotorProcess.song_current_wei[2] = 1;
  MotorProcess.total_wei = weimi_para.total_wei_count[0];
  MotorProcess.song_total_wei[0] = weimi_para.total_wei_count[0];
  MotorProcess.song_total_wei[1] = weimi_para.total_wei_count[0];
  MotorProcess.song_total_wei[2] = weimi_para.total_wei_count[0];
  servomotor_guodu = 0;
  stepmotor_guodu[0] = 0;
  stepmotor_guodu[1] = 0;
  stepmotor_guodu[2] = 0;
  write_bkp_para(&MotorProcess);
  fault_weimi_flag = 0;
  Sdwe_disDigi(PAGE_WEIMI_REALWEI_1,1,4);
}

//将转速转换为步进电机步数
//输入：speed 转/分钟
//输出：count 步数
u32 from_speed_step(float speed)
{
  u16 freq;
  u32 count;
  if(speed > 0)
  {
//    freq = speed / 60 * 360 / 1.8 * 16;
//    freq = speed * 3200 / 60;//步进电机细分数3200
//    count = 4000000 / freq;
    count = 75000 / speed;
  }
  else
    count = 0;
  return count;
}

u8 get_valid_seg(WEIMI_PARA para)
{
  u8 seg = 0;
  u8 i;
  for(i=0;i<10;i++)
  {
//    if((weimi_para.total_wei_count[i] == 0) || (weimi_para.wei_cm_set[i / 2] == 0))
    if((weimi_para.wei_cm_set[i] == 0))
    {//段号中纬循环和纬厘米都设置，则有效
      seg = i;
      break;
    }
  }
  if(i >= 10)
    seg = 10;
  return seg;
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
    {//段号中速比设置不为0则有效
      seg = i;
      break;
    }
  }
  if(i >= 10)
    seg = 10;
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
    {//段号中速比设置不为0则有效
      seg = i;
      break;
    }
  }
  if(i >= 10)
    seg = 10;
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
    {//段号中速比设置不为0则有效
      seg = i;
      break;
    }
  }
  if(i >= 10)
    seg = 10;
  return seg;
}

u16 WeimiMQTTPackage(u8 *buf)
{
  u8 i;
  u16 length = 0;
  WEIMI_MQTT weimi_mqtt;
  u8 name_len;
  name_len = device_info.device_id_len + SlavePara.filename_len;
  buf[0] = name_len;//第一字节为货号长度
  length = 1;
  memcpy(buf + length,device_info.device_id,device_info.device_id_len);//设备ID放在最前面
  length = length + device_info.device_id_len;
  memcpy(buf + length,SlavePara.filename,SlavePara.filename_len);//货号名称放在中间
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
  buf[0] = name_len;//第一字节为货号长度
  length = 1;
  memcpy(buf + length,device_info.device_id,device_info.device_id_len);//设备ID放在最前面
  length = length + device_info.device_id_len;
  memcpy(buf + length,SlavePara.filename,SlavePara.filename_len);//货号名称放在中间
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

