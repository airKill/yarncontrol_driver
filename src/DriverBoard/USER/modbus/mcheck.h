#ifndef __MCHECK_H
#define __MCHECK_H 
#include "stm32f10x.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//MODBUS 校验检查代码	  
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2017/8/16
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2017-2027
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 



u8 mc_check_sum(u8* buf,u16 len);  	 	//和校验 所有字节之和为0
u8 mc_check_xor(u8* buf, u16 len);		//异或校验，所有字节异或
u8 mc_check_crc8(u8 *buf,u16 len);		//CRC8校验
u16 mc_check_crc16(u8 *buf,u16 len);	//CRC16校验
#endif













