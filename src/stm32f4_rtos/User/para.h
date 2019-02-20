#ifndef __PARA_H
#define __PARA_H

#include "stm32f4xx.h"

#define W25QXX_ADDR_INFO   0            //目录数据占用W25Q64一个扇区
#define W25QXX_ADDR_JINGSHA     0x1000      //保存文件从第二扇区地址开始
#define W25QXX_ADDR_CHANNENG    0x6000  //产能页面数据
#define W25QXX_ADDR_PEILIAO     0x7000  //产能页面数据
#define W25QXX_ADDR_WEIMI       0x8000  //纬密页面数据

#define W25QXX_ADDR_RFID_A        0xA000  //班次A卡片保存数据
#define W25QXX_ADDR_RFID_B        0xB000  //班次B卡片保存数据
#define W25QXX_ADDR_RFID_REPAIR   0xC000  //维修卡片保存数据
//每次保存文件，占用空间（BYTE），即偏移量，实际所用空间为1（文件名长度）+ 10（文件名）+6（存储时间日期）+30（张力设定值） =47
#define JINGSHA_SIZE    100 
#define FILE_COUNT_MAX  10

#define CHANNENG_SIZE    50 
#define WEIMI_SIZE    300 
/******************文件储存格式********************
文件名长度                1byte   
文件名（最大10Byte）      10byte
日期（YY MM DD HH MM SS） 6byte
各驱动板张力设定值        30byte
**************************************************/
typedef struct
{//换班切换参数
  u8 class_enable_onoff;
  u8 class_time_hour;
  u8 class_time_minute;
}CLASS_PARA;

typedef struct
{//保存卡片参数
  u16 card_A_count;     //A班数量
  u16 card_B_count;     //B班数量
  u16 card_repair_count;//维修卡片数量
}CARD_COUNT;

typedef struct
{//停止页面参数
  u32 stop_time[11];//各个停止功能的时间
  u16 stop_day_count;//运行总天数
  u8 stop_year;//开始运行年
  u8 stop_month;//开始运行月
  u8 stop_day;//开始运行日
}STOP_PAGE_PARA;

typedef struct
{//停止页面参数
  u8 period_password[10];//试用期密码
  u8 period_password_len;//试用期密码长度
  u8 period_enable_onoff;//试用期使能开关
  u8 period_year;//试用期年
  u8 period_month;//试用期月
  u8 period_day;//试用期日
}PERIOD_PAGE_PARA;;

typedef struct
{
  u8 password[6];
  u8 password_len;
}REGIN_IN;

typedef struct
{
  u8 jingsha;
  u8 channeng;
  u8 weimi;
}FUNCTION_ONOFF;

typedef struct
{
  u8 GEAR1;
  u8 GEAR2;
}GEAR_RATIO;

typedef struct
{
  u8 reg;
  u32 count;
}WEIMI_INFO;

typedef struct
{
  u8 isfirst;
  u8 device_id[6];
  u8 device_id_len;
  u8 system_state;
  u8 page_count_all;         
  u8 page_count_select;  
  u8 onoff[30];
  u8 file_select[10];
  FUNCTION_ONOFF func_onoff; 
  u8 page_enable_onoff[3];//0:经纱页面，1：产能页面，2：纬密页面
  GEAR_RATIO ratio;
  REGIN_IN regin_in;//系统登录密码及经纱页面修改密码
  PERIOD_PAGE_PARA period_para;//试用期页面参数
  CLASS_PARA class_para;//换班页面参数
  CARD_COUNT card_count;//卡片录入数量保存
  STOP_PAGE_PARA stop_para;//停止页面参数
  WEIMI_INFO weimi_info;
}DEVICE_INFO;
extern DEVICE_INFO device_info;

void read_device_info(void);
void para_init(SLAVE_PARA *para);
void read_from_disk(char *diskbuf);
u8* check_cmd(char *str1,char *str2);
#endif

