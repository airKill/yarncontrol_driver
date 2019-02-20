#ifndef __PARA_H
#define __PARA_H

#include "stm32f4xx.h"

#define W25QXX_ADDR_INFO   0            //Ŀ¼����ռ��W25Q64һ������
#define W25QXX_ADDR_JINGSHA     0x1000      //�����ļ��ӵڶ�������ַ��ʼ
#define W25QXX_ADDR_CHANNENG    0x6000  //����ҳ������
#define W25QXX_ADDR_PEILIAO     0x7000  //����ҳ������
#define W25QXX_ADDR_WEIMI       0x8000  //γ��ҳ������

#define W25QXX_ADDR_RFID_A        0xA000  //���A��Ƭ��������
#define W25QXX_ADDR_RFID_B        0xB000  //���B��Ƭ��������
#define W25QXX_ADDR_RFID_REPAIR   0xC000  //ά�޿�Ƭ��������
//ÿ�α����ļ���ռ�ÿռ䣨BYTE������ƫ������ʵ�����ÿռ�Ϊ1���ļ������ȣ�+ 10���ļ�����+6���洢ʱ�����ڣ�+30�������趨ֵ�� =47
#define JINGSHA_SIZE    100 
#define FILE_COUNT_MAX  10

#define CHANNENG_SIZE    50 
#define WEIMI_SIZE    300 
/******************�ļ������ʽ********************
�ļ�������                1byte   
�ļ��������10Byte��      10byte
���ڣ�YY MM DD HH MM SS�� 6byte
�������������趨ֵ        30byte
**************************************************/
typedef struct
{//�����л�����
  u8 class_enable_onoff;
  u8 class_time_hour;
  u8 class_time_minute;
}CLASS_PARA;

typedef struct
{//���濨Ƭ����
  u16 card_A_count;     //A������
  u16 card_B_count;     //B������
  u16 card_repair_count;//ά�޿�Ƭ����
}CARD_COUNT;

typedef struct
{//ֹͣҳ�����
  u32 stop_time[11];//����ֹͣ���ܵ�ʱ��
  u16 stop_day_count;//����������
  u8 stop_year;//��ʼ������
  u8 stop_month;//��ʼ������
  u8 stop_day;//��ʼ������
}STOP_PAGE_PARA;

typedef struct
{//ֹͣҳ�����
  u8 period_password[10];//����������
  u8 period_password_len;//���������볤��
  u8 period_enable_onoff;//������ʹ�ܿ���
  u8 period_year;//��������
  u8 period_month;//��������
  u8 period_day;//��������
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
  u8 page_enable_onoff[3];//0:��ɴҳ�棬1������ҳ�棬2��γ��ҳ��
  GEAR_RATIO ratio;
  REGIN_IN regin_in;//ϵͳ��¼���뼰��ɴҳ���޸�����
  PERIOD_PAGE_PARA period_para;//������ҳ�����
  CLASS_PARA class_para;//����ҳ�����
  CARD_COUNT card_count;//��Ƭ¼����������
  STOP_PAGE_PARA stop_para;//ֹͣҳ�����
  WEIMI_INFO weimi_info;
}DEVICE_INFO;
extern DEVICE_INFO device_info;

void read_device_info(void);
void para_init(SLAVE_PARA *para);
void read_from_disk(char *diskbuf);
u8* check_cmd(char *str1,char *str2);
#endif

