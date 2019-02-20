#include "includes.h"

u8 rs485_rev_buf[50];
u16 rs485_rev_len = 0;

SLAVE_PARA SlavePara;
RTC_TIME rtc_time;
JINGSHA_FILE JingSha_File;

u8 input_password_buf[10];
u8 input_password_len = 0;

u8 file_select[10] = {0};
u8 read_file_select[10] = {0};

DISK_FILE Disk_File;//U盘文件

void rs485_control_onoff(u8 num,u8 onoff)
{
  u8 sendbuf[10];
  sendbuf[0] = 0xA5;
  sendbuf[1] = num + 1;//将0——29存储号转换为1——30地址号
  sendbuf[2] = 1;
  sendbuf[3] = 0;
  sendbuf[4] = onoff;
  sendbuf[5] = (sendbuf[0] + sendbuf[1] + sendbuf[2] + sendbuf[3] + sendbuf[4]) & 0xff;
  RS485_sendbuf(sendbuf,6);
}

void rs485_set_value(u8 num,u16 weight)
{
  u8 sendbuf[10];
  sendbuf[0] = 0xA5;
  sendbuf[1] = num + 1;
  sendbuf[2] = 2;
  sendbuf[3] = (weight & 0xff00) >> 8;
  sendbuf[4] = weight & 0xff;
  sendbuf[5] = (sendbuf[0] + sendbuf[1] + sendbuf[2] + sendbuf[3] + sendbuf[4]) & 0xff;
  RS485_sendbuf(sendbuf,6);
}

void rs485_get_weight(u8 num)
{
  u8 sendbuf[10];
  sendbuf[0] = 0xA5;
  sendbuf[1] = num + 1;
  sendbuf[2] = 3;
  sendbuf[3] = 0;
  sendbuf[4] = 0;
  sendbuf[5] = (sendbuf[0] + sendbuf[1] + sendbuf[2] + sendbuf[3] + sendbuf[4]) & 0xff;
  RS485_sendbuf(sendbuf,6);
}

void rs485_start_stop(u8 num,u16 state)
{
  u8 sendbuf[10];
  sendbuf[0] = 0xA5;
  sendbuf[1] = num;
  sendbuf[2] = 4;
  sendbuf[3] = 0;
  sendbuf[4] = state;
  sendbuf[5] = (sendbuf[0] + sendbuf[1] + sendbuf[2] + sendbuf[3] + sendbuf[4]) & 0xff;
  RS485_sendbuf(sendbuf,6);
}

u8 get_alive_num(u8 *value,u8 *index_num)
{
  u8 i,num = 0;
  for(i=0;i<30;i++)
  {
    if(value[i] == 1)
    {
      index_num[num] = i;
      num++;
    }
  }
  return num;
}

void DiskFile_init(void)
{
  u8 i,j;
  Disk_File.filenum = 0;
  for(i=0;i<10;i++)
  {
    Disk_File.fileselect[i] = 0;
    for(j=0;j<20;j++)
        Disk_File.filename[i][j] = 0;
  }  
}

 