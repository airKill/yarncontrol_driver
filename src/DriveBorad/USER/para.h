#ifndef __PARA_H
#define __PARA_H

//#include "sys.h"	  
#include "stm32f10x.h"

#define USER_FLASH_ADDR   0x0800f000

#define	R_60	12408ul	
#define	R_55  14905ul
#define	R_57  13843ul
#define	R_46	20911ul	
#define	R_20	62520ul	
#define	R_15	78759ul	
#define	R_10	99997ul	
#define	R_TMP	(R_60)

#define TEMP_MAX  (3.3 * R_TMP/(51000ul + R_TMP))

#define	SYS_VOLTAGE		330ul		// ��������ѹ����λ0.01v

#define	VOLTAGE_TO_AD(v)	((u16)((((v*0xFFFul*2ul)/(11UL*SYS_VOLTAGE))+1)/2))	   // ��ѹֵ��Ӧ��ADֵ����ѹ��λ0.01v������1M�Լ�100K��ѹ���������������봦��
#define	CURRENT_TO_AD(a)	((u16)((((a*0xFFFul*3ul*2ul)/(10UL*SYS_VOLTAGE))+1)/2))	 	// ������Ӧ��ADֵ��5��ŷ��������������������봦��������λ����    (�ָ�Ϊ3��ŷ��

#define	LOWEST_VOLTAGE_VALUE	VOLTAGE_TO_AD(1550)
#define	LOW_VOLTAGE_VALUE		VOLTAGE_TO_AD(1700)
#define	OVER_CURRENT_VALUE		CURRENT_TO_AD(180)		// ����ֵ   (ԭ��80��

typedef struct
{
  u8 isfirst;
  u8 onoff;
  u32 weight_value;
  float hx711_xishu;
  u32 hx711_offset;
  u16 precision;
//  u8 onoff;
//  u16 weight;
}DEVICE_INFO;

extern DEVICE_INFO device_info;

void readPara(void);
void Para_default(void);
#endif


