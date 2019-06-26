#include "main.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//MODBUS Э�����������	  
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2017/8/16
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2017-2027
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 


m_protocol_dev_typedef m_ctrl_dev;	//����modbus������


//����һ֡���ݣ���������洢��fx����
//ע�⣺���������õ�malloc��fx����ָ�������ڴ棬��������fx��һ��Ҫ�ͷ��ڴ棡��
//����������ڴ�й¶������
//fx��ָ֡�� 
//����ֵ�����������0��OK��������������롣
m_result mb_unpack_frame(m_frame_typedef *fx)
{
  u16 rxchkval = 0;   	 		//���յ���У��ֵ 
  u16 calchkval = 0;			//����õ���У��ֵ	
  u8 datalen = 0; 				//��Ч���ݳ��� 
  fx->datalen = 0; 				//���ݳ�������
  if((m_ctrl_dev.rxlen > M_MAX_FRAME_LENGTH) || (m_ctrl_dev.rxlen < M_MIN_FRAME_LENGTH))
  {
    m_ctrl_dev.rxlen = 0;			//���rxlen
    m_ctrl_dev.frameok = 0;		//���framok��ǣ��Ա��´ο����������� 
    return MR_FRAME_FORMAT_ERR;//֡��ʽ����
  }
  datalen = m_ctrl_dev.rxbuf[3];
  calchkval = mc_check_sum(m_ctrl_dev.rxbuf,datalen + 4);
  rxchkval = m_ctrl_dev.rxbuf[datalen + 4];
//  switch(m_ctrl_dev.checkmode)
//  {
//  case M_FRAME_CHECK_SUM:							//У���
//    calchkval = mc_check_sum(m_ctrl_dev.rxbuf,datalen + 4);
//    rxchkval = m_ctrl_dev.rxbuf[datalen + 4];
//    break;
//  case M_FRAME_CHECK_XOR:							//���У��
//    calchkval = mc_check_xor(m_ctrl_dev.rxbuf,datalen + 4);
//    rxchkval = m_ctrl_dev.rxbuf[datalen + 4];
//    break;
//  case M_FRAME_CHECK_CRC8:						//CRC8У��
//    calchkval = mc_check_crc8(m_ctrl_dev.rxbuf,datalen + 4);
//    rxchkval = m_ctrl_dev.rxbuf[datalen + 4];
//    break;
//  case M_FRAME_CHECK_CRC16:						//CRC16У��
//    calchkval = mc_check_crc16(m_ctrl_dev.rxbuf,datalen + 4);
//    rxchkval = ((u16)m_ctrl_dev.rxbuf[datalen + 4] << 8) + m_ctrl_dev.rxbuf[datalen + 5];
//    break;
//  } 	
  m_ctrl_dev.rxlen = 0;			//���rxlen
  m_ctrl_dev.frameok = 0;		//���framok��ǣ��Ա��´ο����������� 
  if(calchkval == rxchkval)		//У������
  {
    fx->address = m_ctrl_dev.rxbuf[0];
    fx->function = m_ctrl_dev.rxbuf[1];
    fx->reg = m_ctrl_dev.rxbuf[2];
    fx->datalen = m_ctrl_dev.rxbuf[3];
    if(fx->datalen)
    {
//      fx->data = mymalloc(SRAMIN,fx->datalen);		//�����ڴ�
      for(datalen=0;datalen<fx->datalen;datalen++)
      {
        fx->data[datalen] = m_ctrl_dev.rxbuf[4 + datalen];		//��������
      }
    }
    fx->chkval = rxchkval;	//��¼У��ֵ		
  }
  else 
    return MR_FRAME_CHECK_ERR;
  return MR_OK;	
}


//���һ֡���ݣ������� 
//fx��ָ����Ҫ�����֡  
void mb_packsend_frame(m_frame_typedef *fx)
{  
  u16 i;
  u16 calchkval=0;			//����õ���У��ֵ	
  u16 framelen=0;				//������֡����
  u8 sendbuf[10];				//���ͻ�����
  if(fx->reg == REG_VALUE_WEIGHT)
    fx->datalen = 5;
  framelen = 5 + fx->datalen;
  sendbuf[0] = fx->address;
  sendbuf[1] = fx->function;
  sendbuf[2] = fx->reg;
  sendbuf[3] = fx->datalen; 
  for(i=0;i<fx->datalen;i++)
  {
    sendbuf[4 + i] = fx->data[i];
  }	
  calchkval=mc_check_sum(sendbuf,fx->datalen+4); 
  sendbuf[4 + fx->datalen] = calchkval & 0XFF;
  RS485_TO_PC(sendbuf,framelen);	//������һ֡����
}

//��ʼ��modbus
//checkmode��У��ģʽ��0,У���;1,���;2,CRC8;3,CRC16
//����ֵ��0,�ɹ�;����,�������
m_result mb_init(u8 checkmode)
{
//  m_ctrl_dev.rxbuf = mymalloc(SRAMIN,M_MAX_FRAME_LENGTH);	//��������֡���ջ���
  m_ctrl_dev.rxlen = 0;
  m_ctrl_dev.frameok = 0;
  m_ctrl_dev.checkmode = checkmode;
  if(m_ctrl_dev.rxbuf)
    return MR_OK;
  else 
    return MR_MEMORY_ERR;
}

//����MODBUS,�ͷ��ڴ�
void mb_destroy(void)
{
  myfree(SRAMIN,m_ctrl_dev.rxbuf);	//��������֡���ջ���
  m_ctrl_dev.rxlen = 0;
  m_ctrl_dev.frameok = 0; 
}

void modbus_action(m_frame_typedef *fx,u16 weight)
{
  if(fx->address == device_addr)
  {
    LED1_ON();
//    bsp_StartHardTimer(1,50000,(void *)TIM_CallBack1);
    if(fx->function == FUNC_WRITE)
    {//д�Ĵ���
      if(fx->reg == REG_ONOFF)
      {
        device_info.onoff = (fx->data[0] << 8) + fx->data[1];
        __set_PRIMASK(1);
        Write_flash(USER_FLASH_ADDR,(u16*)&device_info,sizeof(device_info) / 2);
        __set_PRIMASK(0);
        if(device_info.onoff == 0)
        {
          Device_Process = PROCESS_STOP;
          motor_dir = MOTOR_STOP;
          motor_control(motor_dir);
        }
        printf("onoff is %d\r\n",device_info.onoff);
      }
      else if(fx->reg == REG_SET_WEIGHT)
      {
        device_info.weight_value = (fx->data[0] << 8) + fx->data[1];
        __set_PRIMASK(1);
        Write_flash(USER_FLASH_ADDR,(u16*)&device_info,sizeof(device_info) / 2);
        __set_PRIMASK(0);
        printf("SET_WEIGHT is %d\r\n",device_info.weight_value);
      }
      mb_packsend_frame(fx);
    }
    else if(fx->function == FUNC_READ)
    {
      if(fx->reg == REG_VALUE_WEIGHT)
      {
        fx->data[0] = (weight & 0xff00) >> 8;
        fx->data[1] = weight & 0xff;
        fx->data[2] = (device_info.weight_value & 0xff00) >> 8;
        fx->data[3] = device_info.weight_value & 0xff;
        fx->data[4] = device_info.onoff;
        mb_packsend_frame(fx);
        printf("read weight is %d\r\n",weight);
      }
    }
  }
  else if(fx->address == BROADCAST)
  {//�㲥��Ӧ��
    if(fx->function == FUNC_WRITE)
    {
      if(fx->reg == REG_START)
      {
        rev_start_stop = (fx->data[0] << 8) + fx->data[1];
        if(rev_start_stop == 0)
        {
          start_time_flag = 0;
          start_stop = 0;
          start_time = 0;
        }
        else
        {
          start_time_flag = 1;
        }
      }
      else if(fx->reg == REG_RESET)
      {
        Device_Process = PROCESS_RESET;
      }
    }
  }
}


















