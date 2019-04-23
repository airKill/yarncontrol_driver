#ifndef __GLOBAL_H__
#define __GLOBAL_H__
	#define byte				unsigned char 
	#define word				unsigned int 
	#define dword				unsigned long 

	#define BYTE				unsigned char 
	#define WORD				unsigned int 
	#define DWORD				unsigned long 

	#define	DEMO_VERSION		0x21

	#define I2C_MEMSIZE        	255
	
	#define I2C_INS             I2C_Buf[0]
	#define I2C_P1              I2C_Buf[1]
	#define I2C_P2              I2C_Buf[2]
	#define I2C_P3              I2C_Buf[3]

	extern word data			SW;

	extern byte data  			I2C_State;
	extern byte data  			I2C_count_bytes;
	extern byte xdata 			I2C_Buf[256];
	extern byte xdata			Random_Number[8];

#endif	
