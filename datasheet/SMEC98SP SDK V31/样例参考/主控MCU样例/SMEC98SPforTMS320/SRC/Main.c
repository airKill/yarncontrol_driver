//###########################################################################
//
// FILE:    SMEC98SPforTMS320
//
// TITLE:   SMEC98SP����оƬ��DSP�е�Ӧ��
// �������о�ΰҵ��Ϣ�Ƽ����޹�˾
// www.sinormous.com;
// Author:����ˮ
// QQ:47583353

#include "DSP2833x_Device.h"     // DSP2833x ͷ�ļ�
#include "DSP2833x_Examples.h"   // DSP2833x �������ͷ�ļ�
#include <ctime>

#include <stdlib.h>
#include "iic_smec98sp.h"
#include "smec98sp.h"

/*
  1.��ȡSMEC98SP��UID��, ��ȡMCU�����
  2.��֤PIN
  3.���ⲿ��֤
  4.SHA1=>ǰ������^�����
  5.���Ķ�
  6.������
  7.д����
  8.�����㷨(PA������->�����ͼ���оƬ, ���ķ���)

  ���ֱ������,�뽫print�ĵ�����Ϣȥ��
*/

void SMEC_Test(void)
{
	/*������Կ,������I2C��·�ϴ���,����ʹ��ͬһ��.Ӧ�ý���Կ��ɢ�洢,��ֹ����оƬ���ƽ��,���������ڶ����������ҵ���Կ */
	unsigned char InternalKey[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};//�ڲ���֤��Կ,�����SMEC98SPһ��
	unsigned char ExternalKey[16] = {0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F};//�ⲿ��֤��Կ,�����SMEC98SPһ��
	unsigned char SHA1_Key[16] = {0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F};   //��ϣ�㷨��֤��Կ,�����SMEC98SPһ��
	unsigned char MKey[16] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F};   //������Կ,���ڲ���������Կ,�����SMEC98SPһ��
	unsigned char Pin[8] = {0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc};			//Pin��֤��Կ,�����SMEC98SPһ��

	unsigned char bSmec98spUid[12] = {0};		//���SMEC98SP��UID
	unsigned short RandomSeek;					//���������
	unsigned char bRandom[8] = {0};				//��������
	unsigned char bSessionKey[8] = {0};			//��Ź�����Կ,������ԿΪ��ʱ��������Կ
	unsigned char bDataBuf[64] = {0};
	unsigned char ret, bLen;
	unsigned short i, j;


	RandomSeek = (unsigned short)time(0);	//RandomSeekһ��Ҫʹ������ı���,��������MCU��������������ǹ̶�ֲ

	/*��ȡSMEC98SP��UID*/
	ret = SMEC_GetUid(bSmec98spUid);
	if(ret)
	{
		//printf("SMEC_GetUid -> Error !\r\n");
		while(1);
	}
	//printf("SMEC_GetUid: ");
    //PrintHex(bSmec98spUid, 12);
	//printf("\r\n");

	/*�������RandomSeek������һ���������(SMEC98SP��UID����, ʹ�ü�ʹ��ͬ�����,��ͬ��SMEC98SP���������Ҳ��ͬ)*/
	for(i = 0; i < 6; i += 2)
	{
		/*ʹRandomSeek��SMEC98SP��UID���*/
		j = (bSmec98spUid[i] << 8) + bSmec98spUid[i + 1];
		RandomSeek ^= j;
	}
	srand(RandomSeek);
	//printf("RandomSeek: %04x \r\n", RandomSeek);

	/*PIN����֤*/
	ret = SMEC_CheckPin(Pin, (unsigned char)sizeof(Pin));
	if(ret)
	{
		//printf("SMEC_CheckPin -> Error !\r\n");
		while(1);
	}
	//printf("SMEC_CheckPin OK !\r\n");

	/*�ڲ���֤, ����оƬ��SMEC98SP����оƬ�Ϸ����ж�*/
	for(i = 0; i < 8; i ++)
	{
		bRandom[i] = (unsigned char) rand();
	}
	ret = SMEC_IntrAuth(InternalKey, bRandom);
	if(ret)
	{
		//printf("SMEC_IntrAuth -> Error !\r\n");
		while(1);
	}
	//printf("SMEC_IntrAuth OK !\r\n");

	/*�ⲿ��֤, SMEC98SP����оƬ������оƬ�Ϸ����ж�*/
	ret = SMEC_ExtrAuth(ExternalKey);
	if(ret)
	{
		//printf("SMEC_ExtrAuth -> Error !\r\n");
		while(1);
	}
//	printf("SMEC_ExtrAuth OK !\r\n");

	/*SHA1ժҪ�㷨��֤, ���ݳ��ȿ��Լ��趨*/
	for(i = 0; i < 16; i ++)
	{
		bDataBuf[i] = (unsigned char) rand();
		//bDataBuf[i] = (unsigned char) i;
	}
	ret = SMEC_Sha1Auth(SHA1_Key, (unsigned char)sizeof(SHA1_Key), bDataBuf, 16);
	if(ret)
	{
		//printf("SMEC_Sha1Auth -> Error !\r\n");
		while(1);
	}
	//printf("SMEC_Sha1Auth OK !\r\n");


	/*���ü���оƬ�ڲ�����Բ�ܳ��㷨*/
	bDataBuf[0] = 0x02;
	ret = SMEC_CircleAlg(bDataBuf, 1, bDataBuf, &bLen);
	if(ret)
	{
		//printf("SMEC_CircleAlg -> Error !\r\n");
		while(1);
	}
	//printf("SMEC_CircleAlg OK, C = %02x !\r\n", bDataBuf[0]);

	/*����������Կ,���ں�����Flash���ݼ��ܶ�,�������"�˿���������"*/
	for(i = 0; i < 8; i ++)
	{
		bRandom[i] = (unsigned char) rand();
	}
	ret = SMEC_GenSessionKey(MKey, bRandom, bSessionKey);
	if(ret)
	{
		//printf("SMEC_GenSessionKey -> Error !\r\n");
		while(1);
	}
	//printf("SMEC_GenSessionKey OK !\r\n");

	/*���Ķ�ȡFlash����*/
	ret = SMEC_CryptReadFlash(bSessionKey, 0x0000, bDataBuf, 16);
	if(ret)
	{
		//printf("SMEC_CryptReadFlash -> Error !\r\n");
		while(1);
	}
	//printf("SMEC_CryptReadFlash OK:\r\n");
    //PrintHex(bDataBuf, 16);

	/*��ȡFlash����*/
	ret = SMEC_ReadFlash(0x0000, bDataBuf, 16);
	if(ret)
	{
		//printf("SMEC_ReadFlash -> Error !\r\n");
		while(1);
	}
	//printf("SMEC_ReadFlash OK:\r\n");
    //PrintHex(bDataBuf, 16);

	/*дFlash����*/
	for(i = 0; i < 16; i ++)
	{
		bDataBuf[i] = (unsigned char) i;
	}
	ret = SMEC_WriteFlash(0x0000, bDataBuf, 16);
	if(ret)
	{
		//printf("SMEC_WriteFlash -> Error !\r\n");
		while(1);
	}
 	//printf("SMEC_WriteFlash OK!\r\n");

	/*����"�˿���������", ������ʵ�ʵ�PA~PC�˿�����*/
	bDataBuf[0] = 0x00;
	bDataBuf[1] = 0x00;
	ret = SMEC_GpioAlg(bSessionKey, bDataBuf,2, bDataBuf);
	if(ret)
	{
		//printf("SMEC_GpioAlg -> Error !\r\n");
		while(1);
	}
 	//printf("SMEC_GpioAlg OK:\r\n");
 //   PrintHex(bDataBuf, 2);

	/*���ü���оƬ�ڲ�����Բ�ܳ��㷨,����������·�ϴ���*/
	bDataBuf[0] = 0x02;
	ret = SMEC_CircleAlgCrypt(bSessionKey, bDataBuf, 1, bDataBuf, &bLen);
	if(ret)
	{
		//printf("SMEC_CircleAlgCrypt -> Error !\r\n");
		while(1);
	}
	//printf("SMEC_CircleAlg OK, C = %02x !\r\n", bDataBuf[0]);

}

void main(void)
{
   InitSysCtrl();


   SMEC_I2cInit();
   SMEC_Test();

   while(1);
}

//===========================================================================
// No more.
//===========================================================================
