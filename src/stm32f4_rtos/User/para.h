#ifndef __PARA_H
#define __PARA_H

#include "stm32f4xx.h"

#define W25QXX_ADDR_INFO   0            //目录数据占用W25Q64一个扇区
#define W25QXX_ADDR_JINGSHA   0x1000      //保存文件从第二扇区地址开始
#define W25QXX_ADDR_CHANNENG    0x6000  //产能页面数据
#define W25QXX_ADDR_PEILIAO    0x7000  //产能页面数据
#define W25QXX_ADDR_RFID_A        0xA000  //班次A卡片保存数据
#define W25QXX_ADDR_RFID_B        0xB000  //班次B卡片保存数据
#define W25QXX_ADDR_RFID_REPAIR   0xC000  //维修卡片保存数据
//每次保存文件，占用空间（BYTE），即偏移量，实际所用空间为1（文件名长度）+ 10（文件名）+6（存储时间日期）+30（张力设定值） =47
#define FILE_SIZE    100 
#define FILE_COUNT_MAX  10
/******************文件储存格式********************
文件名长度                1byte   
文件名（最大10Byte）      10byte
日期（YY MM DD HH MM SS） 6byte
各驱动板张力设定值        30byte
**************************************************/
typedef struct
{
  u8 isfirst;
  u8 device_id[6];
  u8 device_id_len;
  u8 system_state;
  u8 password[6];
  u8 password_len;
  u8 page_count_all;         
  u8 page_count_select;  
  u8 onoff[30];
  u8 file_select[10];
  u8 class_time_hour;
  u8 class_time_minute;
  u16 card_A_count;     //A班数量
  u16 card_B_count;     //B班数量
  u16 card_repair_count;//维修卡片数量
  u32 stop_time[11];
}DEVICE_INFO;
extern DEVICE_INFO device_info;

void read_device_info(void);
void para_init(SLAVE_PARA *para);
#endif

