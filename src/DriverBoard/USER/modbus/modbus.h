#ifndef __MODBUS_H
#define __MODBUS_H 
#include "stm32f10x.h"
#include "mcheck.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//MODBUS 协议层驱动代码	  
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2017/8/16
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2017-2027
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define M_MAX_FRAME_LENGTH	20		//最大帧长度为261字节 地址+功能+帧号+数据长度+数据+CRC(CRC16有2字节)
#define M_MIN_FRAME_LENGTH	5		//最小帧长度为5字节 地址+功能+帧号+数据长度+CRC8/XOR/SUM

//校验方式宏定义
#define M_FRAME_CHECK_SUM	0		//校验和
#define M_FRAME_CHECK_XOR	1		//异或校验
#define M_FRAME_CHECK_CRC8	2		//CRC8校验
#define M_FRAME_CHECK_CRC16	3		//CRC16校验

#define FUNC_WRITE   1
#define FUNC_READ   3

#define REG_START       0
#define REG_ONOFF       1
#define REG_SET_WEIGHT      2
#define REG_VALUE_WEIGHT      3
#define REG_RESET       4

#define BROADCAST       0xff

//返回结果：错误类型定义
typedef enum
{
  MR_OK=0,						//正常
  MR_FRAME_FORMAT_ERR = 1,		//帧格式错误	 
  MR_FRAME_CHECK_ERR = 2,			//校验值错位 
  MR_MEMORY_ERR = 3,				//内存错误 
}m_result;

//类modbus帧定义
__packed typedef struct
{ 
  u8 address;						//设备地址：0，广播地址；1~255，设备地址。
  u8 function;					//帧功能，0~255
  u8 reg;						//帧编号
  u8 datalen;						//有效数据长度
  u8 data[10];						//数据存储区
  u16 chkval;						//校验值 
}m_frame_typedef;

//类modbus协议管理器
typedef  struct
{  	
  u8 rxbuf[10];						//接收缓存区
  u16 rxlen;						//接收数据的长度
  u8 frameok;						//一帧数据接收完成标记：0，还没完成；1，完成了一帧数据的接收
  u8 checkmode;					//校验模式：0,校验和;1,异或;2,CRC8;3,CRC16	
}m_protocol_dev_typedef; 

extern m_protocol_dev_typedef m_ctrl_dev;			//定义modbus控制器

m_result mb_unpack_frame(m_frame_typedef *fx);		//解析一帧数据
void mb_packsend_frame(m_frame_typedef *fx);		//打包一帧数据，并发送
m_result mb_init(u8 checkmode);						//初始化MODBUS,申请内存
void mb_destroy(void);								//结束MODBUS,释放内存
void modbus_action(m_frame_typedef *fx,u16 weight);
#endif













