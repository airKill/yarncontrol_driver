#ifndef __PARA_H
#define __PARA_H

#include "stm32f4xx.h"

#define W25QXX_ADDR_INFO   0            //目录数据占用W25Q64一个扇区
#define W25QXX_ADDR_FILE   4096      //保存文件从第二扇区地址开始

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
  u8 master_id;
  u8 password[6];
  u8 password_len;
  u8 page_count_all;         
  u8 page_count_select;  
  u8 onoff[30];
  u8 file_select[10];
}DEVICE_INFO;
extern DEVICE_INFO device_info;

void read_device_info(void);
void para_init(SLAVE_PARA *para);
#endif

