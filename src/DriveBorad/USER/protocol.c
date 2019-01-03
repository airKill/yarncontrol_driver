#include "main.h"

u8 message0 = 0;
u8 rev_buf[10];
u8 rev_len = 0;

u16 heart_cnt = 0;//心跳时间，超过一定时间未收到主机查询命令，认为断开连接，停止工作

MOTOR motor;

void rs485_revProtocol(u8 *buf,u16 len)
{
  u8 checksum = 0,i;
  u16 value;
  if((buf[0] == 0xA5) && (len == 6))
  {//指令格式正确
    for(i=0;i<5;i++)
      checksum = checksum + buf[i];
    if(buf[5] == checksum)
    {//校验和正确
      if(buf[1] == device_addr)
      {
        if(buf[2] == 1)
        {//从机开关应答
          motor.onoff = buf[4];
          rs485_ack_onoff(device_addr,motor.onoff);
        }
        else if(buf[2] == 2)
        {//从机设置张力应答
          motor.weight = (buf[3] << 8) + buf[4];
          rs485_ack_set_value(device_addr,motor.weight);
        }
        else if(buf[2] == 3)
        {//从机查询张力应答
          value = (u16)load_value;
          rs485_ack_weight(device_addr,value);
          heart_cnt = 0;
        }
      }
      else if(buf[1] == 0xFF)
      {
        if(buf[2] == 4)
        {//从机启停应答
          start_stop = buf[4];
          rs485_ack_start_stop(device_addr,start_stop);
        }
      }
    }
  }
}

void rs485_ack_onoff(u8 num,u8 onoff)
{
  u8 sendbuf[10];
  sendbuf[0] = 0xA6;
  sendbuf[1] = num;
  sendbuf[2] = 1;
  sendbuf[3] = 0;
  sendbuf[4] = onoff;
  sendbuf[5] = (sendbuf[0] + sendbuf[1] + sendbuf[2] + sendbuf[3] + sendbuf[4]) & 0xff;
  RS485_TO_PC(sendbuf,6);
}

void rs485_ack_set_value(u8 num,u16 weight)
{
  u8 sendbuf[10];
  sendbuf[0] = 0xA6;
  sendbuf[1] = num;
  sendbuf[2] = 2;
  sendbuf[3] = (weight & 0xff00) >> 8;
  sendbuf[4] = weight & 0xff;
  sendbuf[5] = (sendbuf[0] + sendbuf[1] + sendbuf[2] + sendbuf[3] + sendbuf[4]) & 0xff;
  RS485_TO_PC(sendbuf,6);
}

void rs485_ack_weight(u8 num,u16 weight)
{
  u8 sendbuf[10];
  sendbuf[0] = 0xA6;
  sendbuf[1] = num;
  sendbuf[2] = 3;
  sendbuf[3] = (weight & 0xff00) >> 8;
  sendbuf[4] = weight & 0xff;
  sendbuf[5] = (sendbuf[0] + sendbuf[1] + sendbuf[2] + sendbuf[3] + sendbuf[4]) & 0xff;
  RS485_TO_PC(sendbuf,6);
}

void rs485_ack_start_stop(u8 num,u16 state)
{
  u8 sendbuf[10];
  sendbuf[0] = 0xA6;
  sendbuf[1] = num;
  sendbuf[2] = 4;
  sendbuf[3] = 0;
  sendbuf[4] = state;
  sendbuf[5] = (sendbuf[0] + sendbuf[1] + sendbuf[2] + sendbuf[3] + sendbuf[4]) & 0xff;
  RS485_TO_PC(sendbuf,6);
}

void uart1_debug(void)
{
  if(uart1_check_cmd("get weight_value"))
  {
    printf("weight_value is %d\r\n",device_info.weight_value);
  }
  else if(uart1_check_cmd("get start_stop"))
  {
    printf("start_stop is %d\r\n",start_stop);
  }
  else if(uart1_check_cmd("get Device_Process"))
  {
    printf("Device_Process is %d\r\n",Device_Process);
  }
}

u8* uart1_check_cmd(char *str)
{
  char *strx = 0;
  strx = strstr((const char*)UART1_RX_BUF,(const char*)str);
  return (u8*)strx;
}
