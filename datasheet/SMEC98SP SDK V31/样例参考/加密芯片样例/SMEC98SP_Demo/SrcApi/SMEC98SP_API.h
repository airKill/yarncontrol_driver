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
	*   ���ܣ� ��ȡ���ֽ�FLASH
	*   ���룺 DataLen - ��ȡ����  FlashAddr - FLASH��ַ 	 
	*   ����� 
	*****************************************************************************/
	extern unsigned char FlashReadNBytes(unsigned int DataLen, unsigned char *DataAddr , unsigned long FlashAddr);
	
	/****************************************************************************
	*   ���ܣ� дFLASH
	*   ���룺 FlashAddr - FLASH��ַ   Len - д�볤��		  
	*   ����� DataAddr - Ҫд�������
	*****************************************************************************/
	extern unsigned char FlashWriteNBytes(unsigned long FlashAddr, unsigned char* DataAddr,unsigned int Len);

	/****************************************************************************
	*   ���ܣ� ����FLASH, һ�β���һ��(256�ֽ�)
	*   ���룺 FlashAddr - FLASH��ַ						   
	*   ����� 
	*****************************************************************************/
	extern unsigned char FlashErasePage(unsigned long FlashAddr);
	
	/****************************************************************************
	*   ���ܣ� ���½�����ģʽ
	*   ���룺 
	*   ����� 
	*   ��ע�����øú�����оƬ��Ҫ�����ϵ����ܽ�����ģʽ
	*****************************************************************************/
	extern unsigned char ReworkEntry(void);

	/****************************************************************************
	*   ���ܣ� ��ȡ�����
	*   ���룺 lenght: ��������� 
	*   ����� RAM_addr ��ȡ����� 
	*****************************************************************************/
	extern void Random_Number_Generator_Get (unsigned char * RAM_addr, unsigned char lenght);

	/****************************************************************************
	*   ���ܣ�DES����
	*   ���룺DesMode: bit0~bit1: 0x00-des encrypt 0x01-des decrypt 0x02-triple des encrypt 0x03-triple des decrypt
					   bit4=0, ECB mode, else CBC mode
			  KeyData: 8 or 16bytes input
			  DataSource: 8 bytes input
	*              
	*   �����DataResult: 8 bytes output  
	*****************************************************************************/
	extern void HW_DES(unsigned char DesMode, unsigned char  * KeyData, unsigned char  * DataSource, unsigned char  * DataResult);

	/****************************************************************************
	*   ���ܣ���ȡоƬID��
	*   ���룺  
	*   �����chipid - 12�ֽ�ID��  
	*****************************************************************************/
	extern void GetChipID(unsigned char * chipid);

#endif