#ifndef __SMEC98SP_I2C_HARD_API_H__
#define __SMEC98SP_I2C_HARD_API_H__
//-----------------------------------------------------------------------------
// COPYRIGHT (c) 2001-2008	Sinormous Technology
// File: SMEC98SP_I2C_hard_api.h
//-----------------------------------------------------------------------------
// ABSTRACT:
// Test application for SMEC98SP-I2C
//-----------------------------------------------------------------------------
// MODIFICATIONS:
// Author	Date		Description
// 
//-----------------------------------------------------------------------------

	// Change CPU frequency function
	#define F_OSC_30M 	0x00	// HS /1	30 MHz
	#define F_OSC_20M 	0x01	// MS / 1	20 MHz
	#define F_OSC_15M 	0x02	// HS / 2	15MHz
	#define F_OSC_10M 	0x03	// MS / 2	10MHz
	#define F_OSC_7_5M 	0x04	// HS / 4	7.5MHz
	#define F_OSC_5M 		0x05	// MS / 4	5MHz
	#define F_OSC_3_75M 0x06	// HS / 8	3,75 MHz
	#define F_OSC_2_50 	0x07	// MS / 8	2,5 MHz
	
	/****************************************************************************
	*   功能：设置加密芯片时钟频率
	*   输入：Freq - 加密芯片时钟频率, 最大30MHz
	*   输出：无
	*   备注：
	*****************************************************************************/
	extern void Set_Frequency(unsigned char Freq);

	/****************************************************************************
	*   功能： I2C数据传输，查询方式，发送/接收I2C总线上数据。
	*   参数：无
	*   全局变量：I2C_Buf，I2C_send_bytes，I2C_State，I2C_count_bytes 
	*   输入： I2C_Buf - I2C write/read 的数据存放起始地址。	 
			   I2C_send_bytes - 若I2C总线命令为read，加密芯片需要发送的数据长度。

	*   输出： I2C_State - 当前I2C 命令总类。
	*             - 0x01  I2C write操作
	*	          - 0x02  I2C read操作
	*	       I2C_Buf - 若I2C总线命令为write，接收到的数据存放起始地址。
	*          I2C_count_bytes - 若I2C总线命令为write，接收到的数据长度。
	*   备注：I2C总线上数据传输的地址需跟I2CADR一致，否则不受影响。
	*****************************************************************************/
	extern void i2c_xfer(void);

#endif