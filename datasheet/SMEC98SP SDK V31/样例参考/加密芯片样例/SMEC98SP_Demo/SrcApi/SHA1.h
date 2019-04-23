#ifndef __SHA1_H__
#define __SHA1_H__

typedef struct 
{
	unsigned long state[5];
	unsigned long count[2];
	unsigned char buffer[64];
} SHA1_CTX;

 	/****************************************************************************
	*   ���ܣ� SHA1��ʼ��
	*   ���룺 context: SHA1�ṹ��
	*   ����� ��
	*****************************************************************************/
	void SHA1Init( SHA1_CTX * context );  

 	/****************************************************************************
	*   ���ܣ� SHA1���ݸ���
	*   ���룺 context: SHA1�ṹ��
	*		   Dat:SHA1��������
	*		   len:SHA1�������ݳ���
	*   ����� ��
	*****************************************************************************/
	void SHA1Update( SHA1_CTX * context, unsigned char * const dat, unsigned int len );  

 	/****************************************************************************
	*   ���ܣ� SHA1����
	*   ���룺 context: SHA1�ṹ��
	*   ����� digest��	20ֱ��SHA1 hashֵ
	*****************************************************************************/
	void SHA1Final( unsigned char * digest /*[20]*/, SHA1_CTX * context);  

	extern SHA1_CTX	xdata		Sha1_Context;

#endif