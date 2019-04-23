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
	void SHA1Init( void * context );  

 	/****************************************************************************
	*   功能： SHA1数据更新
	*   输入： context: SHA1结构体
	*		   Dat:SHA1输入数据
	*		   len:SHA1输入数据长度
	*   输出： 无
	*****************************************************************************/
	void SHA1Update( void * context, unsigned char * const dat, unsigned int len ); 


	void SHA1Final( unsigned char * digest /*[20]*/, void * context); 

        void SHA1Transform( unsigned long * state /*[5]*/, unsigned char * buffer /*[64]*/ );

	extern SHA1_CTX	Sha1_Context;

#endif
