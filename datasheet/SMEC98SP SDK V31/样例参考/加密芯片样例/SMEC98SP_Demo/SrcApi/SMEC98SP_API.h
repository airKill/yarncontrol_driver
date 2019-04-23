#ifndef __SMEC98SP_I2C_H__
#define __SMEC98SP_I2C_H__

	#define TRUE 				0x01
	#define FALSE 				0x00
	#define SUCCESSFULL 		0x00
	#define FAIL 				0x01
	#define FAIL_ERASE 			0xFC
	
	#define	SIMPLE_DES_EN		0x00
	#define	SIMPLE_DES_DE		0x01
	#define	TRIPLE_DES_EN		0x02
	#define	TRIPLE_DES_DE		0x03

	/****************************************************************************
	*   功能： 读取多字节FLASH
	*   输入： DataLen - 读取长度  FlashAddr - FLASH地址 	 
	*   输出： 
	*****************************************************************************/
	extern unsigned char FlashReadNBytes(unsigned int DataLen, unsigned char *DataAddr , unsigned long FlashAddr);
	
	/****************************************************************************
	*   功能： 写FLASH
	*   输入： FlashAddr - FLASH地址   Len - 写入长度		  
	*   输出： DataAddr - 要写入的数据
	*****************************************************************************/
	extern unsigned char FlashWriteNBytes(unsigned long FlashAddr, unsigned char* DataAddr,unsigned int Len);

	/****************************************************************************
	*   功能： 擦除FLASH, 一次擦除一块(256字节)
	*   输入： FlashAddr - FLASH地址						   
	*   输出： 
	*****************************************************************************/
	extern unsigned char FlashErasePage(unsigned long FlashAddr);
	
	/****************************************************************************
	*   功能： 重新进入编程模式
	*   输入： 
	*   输出： 
	*   备注：调用该函数后，芯片需要重新上电后才能进入编程模式
	*****************************************************************************/
	extern unsigned char ReworkEntry(void);

	/****************************************************************************
	*   功能： 获取随机数
	*   输入： lenght: 随机数长度 
	*   输出： RAM_addr 所取随机数 
	*****************************************************************************/
	extern void Random_Number_Generator_Get (unsigned char * RAM_addr, unsigned char lenght);

	/****************************************************************************
	*   功能：DES运算
	*   输入：DesMode: bit0~bit1: 0x00-des encrypt 0x01-des decrypt 0x02-triple des encrypt 0x03-triple des decrypt
					   bit4=0, ECB mode, else CBC mode
			  KeyData: 8 or 16bytes input
			  DataSource: 8 bytes input
	*              
	*   输出：DataResult: 8 bytes output  
	*****************************************************************************/
	extern void HW_DES(unsigned char DesMode, unsigned char  * KeyData, unsigned char  * DataSource, unsigned char  * DataResult);

	/****************************************************************************
	*   功能：获取芯片ID号
	*   输入：  
	*   输出：chipid - 12字节ID号  
	*****************************************************************************/
	extern void GetChipID(unsigned char * chipid);

#endif