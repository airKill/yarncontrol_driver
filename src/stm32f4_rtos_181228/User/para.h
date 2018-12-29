#ifndef __PARA_H
#define __PARA_H

#include "stm32f4xx.h"

#define W25QXX_ADDR_INFO   0            //Ŀ¼����ռ��W25Q64һ������
#define W25QXX_ADDR_FILE   4096      //�����ļ��ӵڶ�������ַ��ʼ

//ÿ�α����ļ���ռ�ÿռ䣨BYTE������ƫ������ʵ�����ÿռ�Ϊ1���ļ������ȣ�+ 10���ļ�����+6���洢ʱ�����ڣ�+30�������趨ֵ�� =47
#define FILE_SIZE    100 
#define FILE_COUNT_MAX  10
/******************�ļ������ʽ********************
�ļ�������                1byte   
�ļ��������10Byte��      10byte
���ڣ�YY MM DD HH MM SS�� 6byte
�������������趨ֵ        30byte
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

