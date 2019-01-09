#ifndef __PARA_H
#define __PARA_H

#include "stm32f4xx.h"

#define W25QXX_ADDR_INFO   0            //Ŀ¼����ռ��W25Q64һ������
#define W25QXX_ADDR_JINGSHA   0x1000      //�����ļ��ӵڶ�������ַ��ʼ
#define W25QXX_ADDR_CHANNENG    0x6000  //����ҳ������
#define W25QXX_ADDR_PEILIAO    0x7000  //����ҳ������
#define W25QXX_ADDR_RFID_A        0xA000  //���A��Ƭ��������
#define W25QXX_ADDR_RFID_B        0xB000  //���B��Ƭ��������
#define W25QXX_ADDR_RFID_REPAIR   0xC000  //ά�޿�Ƭ��������
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
  u16 card_A_count;     //A������
  u16 card_B_count;     //B������
  u16 card_repair_count;//ά�޿�Ƭ����
  u32 stop_time[11];
}DEVICE_INFO;
extern DEVICE_INFO device_info;

void read_device_info(void);
void para_init(SLAVE_PARA *para);
#endif

