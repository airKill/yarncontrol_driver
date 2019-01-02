#ifndef __MPORT_H
#define __MPORT_H 
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//MODBUS 接口层驱动代码	  
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2017/8/16
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2017-2027
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

 
void mp_init(u32 pclk2,u32 bound);			//初始化接口
void mp_send_data(u8* buf, u16 len);		//发送数据 

#endif


















