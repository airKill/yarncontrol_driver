#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include "stm32f4xx.h"

#define SLAVE_MAX_COUNT 32

typedef struct
{
  u8 filename_len;         
  u8 filename[10];
  u8 onoff[30];         //当前页面驱动板工作开关
  u16 value_set[30];    //当前页面设定张力，单位g
  u16 value_sample[30]; //当前页面张力，单位g
}SLAVE_PARA;
extern SLAVE_PARA SlavePara;

typedef struct
{
  u8 filename_len;         
  u8 filename[20];   
  u8 year;         //年
  u8 month;   
  u8 day; 
  u8 week; 
  u8 hour; 
  u8 minute; 
  u8 second;
  u16 weight_value[30];
}JINGSHA_FILE;
extern JINGSHA_FILE JingSha_File;

typedef struct
{
  u8 year;         //年
  u8 month;   
  u8 day; 
  u8 week; 
  u8 hour; 
  u8 minute; 
  u8 second; 
}RTC_TIME;
extern RTC_TIME rtc_time;

typedef struct
{
  u8 filenum;//U盘文件数量，最大10
  u8 fileselect[10];//U盘文件选择标记
  u8 filename[10][20];//U盘文件名
}DISK_FILE;
extern DISK_FILE Disk_File;

extern u8 input_password_buf[10];
extern u8 input_password_len;

extern u8 rs485_rev_buf[50];
extern u16 rs485_rev_len;

extern u8 file_select[10];
extern u8 read_file_select[10];

void rs485_revProtocol(u8 *buf,u16 len);
void rs485_control_onoff(u8 num,u8 onoff);
void rs485_set_value(u8 num,u16 weight);
void rs485_get_weight(u8 num);
void rs485_start_stop(u8 num,u16 state);
u8 get_alive_num(u8 *value,u8 *index_num);
void DiskFile_init(void);
void init_jingsha_para(JINGSHA_FILE *para);
#endif

