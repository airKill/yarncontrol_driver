#ifndef __SHA1_H__
#define __SHA1_H__

typedef struct 
{
	unsigned long state[5];
	unsigned long count[2];
	unsigned char buffer[64];
} SHA1_CTX;

 	/****************************************************************************
	*   功能： SHA1初始化
	*   输入： context: SHA1结构体
	*   输出： 无
	*****************************************************************************/
	void SHA1Init( SHA1_CTX * context );  

 	/****************************************************************************
	*   功能： SHA1数据更新
	*   输入： context: SHA1结构体
	*		   Dat:SHA1输入数据
	*		   len:SHA1输入数据长度
	*   输出： 无
	*****************************************************************************/
	void SHA1Update( SHA1_CTX * context, unsigned char * const dat, unsigned int len );  

 	/****************************************************************************
	*   功能： SHA1运算
	*   输入： context: SHA1结构体
	*   输出： digest：	20直接SHA1 hash值
	*****************************************************************************/
	void SHA1Final( unsigned char * digest /*[20]*/, SHA1_CTX * context);  

	extern SHA1_CTX	xdata		Sha1_Context;

#endif