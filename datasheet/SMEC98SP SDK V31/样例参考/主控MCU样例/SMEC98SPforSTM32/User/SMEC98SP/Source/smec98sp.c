//COPYRIGHT (c) 2018-2025	Sinormous Technology
//�������о�ΰҵ��Ϣ�Ƽ����޹�˾
//www.sinormous.com;
//Author:����ˮ
//QQ:47583353

#include <string.h>
#include "stm32f10x.h"
#include "smec98sp.h"
#include "iic_smec98sp.h"
#include "des.h"			 //����õ�DES���㣬�����
#include "sha1.h"			 //����õ�SHA1���㣬�����




//===========================      ����оƬ SMEC98SP �ӿ�  ===========================//
/*        ����оƬ�ڲ���������б�д���޸�, ����Ϊ���ݼ���оƬ���������д           */
/*
 	1.��ȡSMEC98SP��UID��
 	2.��������оƬ�������
 	3.��֤PIN
 	4.�ڲ���֤
 	5.�ⲿ��֤
 	6.SHA1��ϣ�㷨��֤
 	7.�ؼ��㷨���ڼ���оƬ��
 	8.�����㷨
 	9.���Ķ�����
 	10.������
 	11.д����
*/

#define IIC_ADDR	0x00			//��ӦSMEC98SP�е�ַ

//---------------------------------------------------------
//������: ��ȡ����оƬSMEC98SP��UID
//����˵����
//			pUID - UID���ָ��
//����ֵ˵����
//			0 - �ɹ�
//			1 - ʧ��
//---------------------------------------------------------
unsigned char SMEC_GetUid(unsigned char * pUID)
{
    unsigned char bBuf[128] = {0};
    unsigned char ret;
    unsigned char  len;

    bBuf[0] = 0x83;		        //��UID�����֣��ɸ��ģ������оƬ�г��򱣳�һ��
    bBuf[1] = 0x00;
    bBuf[2] = 0x00;
    bBuf[3] = len = 0x0c;		//��Ҫ��ȡ��UID����

    ret = IIC_WriteWithAddr(IIC_ADDR & 0xFE, bBuf, 4);
    if(ret)                     //����
        return 1;

    //˵��������ȷ��ȡUID, ����оƬ��������90 00 + UID1 ~ UID12����ǰ2�ֽ�Ϊ״̬��, 0x9000 ��ʾִ����ȷ, ������ʾ����
    ret = IIC_ReadWithAddr(IIC_ADDR | 0x1, bBuf, (len + 2));
    if(ret || bBuf[0] != 0x90 || bBuf[1] != 0x00)         //ͨ�Ŵ����״̬�벻Ϊ0x9000
        return 1;

    memcpy(pUID, bBuf + 2, len);
    return 0;		            //��ȡ�ɹ�
}

//---------------------------------------------------------
//������: ��ȡ����оƬSMEC98SP�������
//����˵����
//			pRandom - ��������ָ��
//����ֵ˵����
//			0 - �ɹ�
//			1 - ʧ��
//---------------------------------------------------------
unsigned char SMEC_GetRandom(unsigned char *pRandom)
{
    unsigned char bBuf[128] = {0};
    unsigned char  ret;
    unsigned char  len;

    bBuf[0] = 0x84;		        //�ɸ��ģ������оƬ�г��򱣳�һ��
    bBuf[1] = 0x00;
    bBuf[2] = 0x00;
    bBuf[3] = len = 0x08;	    //��Ҫ��ȡ�����������

    ret = IIC_WriteWithAddr(IIC_ADDR & 0xFE, bBuf, 4);
    if(ret)                     //����
        return 1;

    //����ȷ��ȡ�����, ����оƬ��������90 00 + random1 ~ random8��ǰ2�ֽ�Ϊ״̬��, 0x9000 ��ʾִ����ȷ, ������ʾ����
    ret = IIC_ReadWithAddr(IIC_ADDR | 0x1, bBuf, (len+2));
    if(ret || bBuf[0] != 0x90 || bBuf[1] != 0x00)         //ͨ�Ŵ����״̬�벻Ϊ0x9000
        return 1;

    memcpy(pRandom, bBuf + 2, len);
    return 0;		            //�ɹ�
}

//---------------------------------------------------------
//������: ��֤����оƬSMEC98SP��PIN��
//����˵����
//			pbPin - PIN��
//          bPinLen - PIN����
//����ֵ˵����
//			0 - �ɹ�
//			1 - ʧ��
//---------------------------------------------------------
unsigned char SMEC_CheckPin(unsigned char *pbPin, unsigned char bPinLen)
{
    unsigned char bBuf[128] = {0};
    unsigned char  ret;

    bBuf[0] = 0x70;		        //�ɸ��ģ������оƬ�г��򱣳�һ��
    bBuf[1] = 0x00;
    bBuf[2] = 0x00;
    bBuf[3] = bPinLen;	    //��Ҫ��ȡ�����������
    memcpy(&bBuf[4], pbPin, bPinLen);

    ret = IIC_WriteWithAddr(IIC_ADDR & 0xFE, bBuf, bPinLen + 4);
    if(ret)                     //����
        return 1;

    //����ȷ��֤PIN ����оƬ���� 90 00
    ret = IIC_ReadWithAddr(IIC_ADDR | 0x1, bBuf, 2);
    if(ret || bBuf[0] != 0x90 || bBuf[1] != 0x00)         //ͨ�Ŵ����״̬�벻Ϊ0x9000
        return 1;

    return 0;		            //��֤PIN�ɹ�
}

//---------------------------------------------------------
//������: SHA1��֤
//����˵����
//			pbSha1Key  	- SHA1��֤��Կ��ʵ��Ϊ��SHA1�����ǰ������, ������оƬ��SMEC98SP�ڲ�����, ������I2C��·�ϴ���
//			bSha1KeyLen - SHA1��֤��Կ����
//			pbRandom    - SHA1���������
//			bRandomLen  - �������ݳ���
//����ֵ˵����
//			0 - �ɹ�
//			1 - ʧ��
//˵��:����оƬ�ȶ�pbSha1Key��HASH(ժҪ)����,Ȼ���ٶ�pbRandom������HASH����. ��������SMEC98SP����ֵ���Ƚ�.
//      pbSha1Key�ֱ��������оƬ��SMEC98SP��,��������·�ϴ���.�����������оƬ�����оƬ��pbSha1Key��һ��, ���޷���֤�ɹ�
//---------------------------------------------------------
unsigned char SMEC_Sha1Auth(unsigned char *pbSha1Key, unsigned char bSha1KeyLen, unsigned char *pbRandom, unsigned char bRandomLen)
{
    unsigned char bBuf[128] = {0};
    unsigned char ret;
    unsigned char bHashData1[20] = {0}, bHashData2[20] = {0};

    bBuf[0] = 0x71;		        //�ɸ��ģ������оƬ�г��򱣳�һ��
    bBuf[1] = 0x00;
    bBuf[2] = 0x00;
    bBuf[3] = bRandomLen;  		//8�ֽ����������
    memcpy(&bBuf[4], pbRandom, bRandomLen);

    ret = IIC_WriteWithAddr(IIC_ADDR & 0xFE, bBuf, bRandomLen + 4);
    if(ret)                     //����
        return 1;

    //��ִ����ȷ������״̬��90 00 + 20�ֽ�HASH���
    ret = IIC_ReadWithAddr(IIC_ADDR | 0x1, bBuf, 20 + 2);
    if(ret || bBuf[0] != 0x90 || bBuf[1] != 0x00)         //ͨ�Ŵ����״̬�벻Ϊ0x9000
        return 1;
    memcpy(bHashData1, bBuf + 2, 20);

    SHA1Init( &Sha1_Context );
    SHA1Update(&Sha1_Context, pbSha1Key, bSha1KeyLen);		//�Ȱ�pbSha1Key��������
    SHA1Update(&Sha1_Context, pbRandom, bRandomLen);
    SHA1Final(bHashData2, &Sha1_Context);					//������

    if(memcmp(bHashData1, bHashData2, 20))					//�������оƬ�����оƬ�����һ��,����֤ʧ��
        return 1;
    return 0;
}

//---------------------------------------------------------
//������: �ڲ���֤
//����˵����
//			pbKey - �ڲ���֤����Կ
//          pbRandom - �ڲ���֤���������
//����ֵ˵����
//			0 - �ɹ�
//			1 - ʧ��
//˵��:����оƬ��SMEC98SP����8�ֽ������, SMEC98SP�����������3DES���ܺ��͸�����оƬ, ������оƬ�жϻ������ݵĺϷ���
//---------------------------------------------------------
unsigned char SMEC_IntrAuth(unsigned char *pbKey, unsigned char *pbRandom)
{
    unsigned char bBuf[128] = {0};
    unsigned char ret;
    unsigned char bTmp[8] = {0};

    bBuf[0] = 0x88;		        //�ɸ��ģ������оƬ�г��򱣳�һ��
    bBuf[1] = 0x00;
    bBuf[2] = 0x00;
    bBuf[3] = 8;	    		//8�ֽ����������
    memcpy(&bBuf[4], pbRandom, 8);

    ret = IIC_WriteWithAddr(IIC_ADDR & 0xFE, bBuf, 8 + 4);
    if(ret)                     //����
        return 1;

    //ִ����ȷ������90 00+�������������
    ret = IIC_ReadWithAddr(IIC_ADDR | 0x1, bBuf, 8+2);
    if(ret || bBuf[0] != 0x90 || bBuf[1] != 0x00)         //ͨ�Ŵ����״̬�벻Ϊ0x9000
        return 1;

    TripleDES(pbRandom, pbKey, bTmp);
    if(memcmp(bBuf + 2, bTmp, 8))//�ȶԷ��ؽ�������Ƿ���Ԥ�ڵ�һ��
        return 1;

    return 0;		            //�ڲ���֤��ȷ
}

//---------------------------------------------------------
//������: �ⲿ��֤
//����˵����
//			pbKey - �ⲿ��֤��Կ
//����ֵ˵����
//			0 - �ɹ�
//			1 - ʧ��
//˵��:����оƬ�Ȼ�ȡSMEC98SP��8�ֽ������, Ȼ����bKey���������3DES���ܣ��ٽ������͸�SMEC98SP
//---------------------------------------------------------
unsigned char SMEC_ExtrAuth(unsigned char *pbKey)
{
    unsigned char bBuf[128] = {0};
    unsigned char ret;
    unsigned char bCryptData[8] = {0}, bRandom[8] = {0};

    if(SMEC_GetRandom(bRandom))//��ȡ8�ֽ������
        return 1;
    TripleDES(bRandom, pbKey, bCryptData);	//���������3DES����

    bBuf[0] = 0x82;		        //�ɸ��ģ������оƬ�г��򱣳�һ��
    bBuf[1] = 0x00;
    bBuf[2] = 0x00;
    bBuf[3] = 8;	    		//8�ֽ����������
    memcpy(&bBuf[4], bCryptData, 8);

    ret = IIC_WriteWithAddr(IIC_ADDR & 0xFE, bBuf, 8 + 4);
    if(ret)                     //����
        return 1;

    //��ִ����ȷ������״̬��90 00
    ret = IIC_ReadWithAddr(IIC_ADDR | 0x1, bBuf, 2);
    if(ret || bBuf[0] != 0x90 || bBuf[1] != 0x00)         //ͨ�Ŵ����״̬�벻Ϊ0x9000
        return 1;

    return 0;		            //�ⲿ��֤��ȷ
}

//---------------------------------------------------------
//������: ��Flash
//����˵����
//			pbData - ��ȡ���ݴ�ŵ�ַ
//			bLen  -  ��ȡ�����ݳ���
//����ֵ˵����
//			0 - �ɹ�
//			1 - ʧ��
//˵��:
//---------------------------------------------------------
unsigned char SMEC_ReadFlash(unsigned short wAddr, unsigned char *pbData, unsigned char bLen)
{
    unsigned char bBuf[128] = {0};
    unsigned char ret;

    bBuf[0] = 0xB0;		        //�ɸ��ģ������оƬ�г��򱣳�һ��
    bBuf[1] = (unsigned char)(wAddr>>8);
    bBuf[2] = (unsigned char) wAddr;
    bBuf[3] = bLen;	    		//��Ҫ��ȡ���ݳ���

    ret = IIC_WriteWithAddr(IIC_ADDR & 0xFE, bBuf, 4);
    if(ret)                     //����
        return 1;

    //��ִ����ȷ������״̬��90 00 + ����
    ret = IIC_ReadWithAddr(IIC_ADDR | 0x1, bBuf, bLen + 2);
    if(ret || bBuf[0] != 0x90 || bBuf[1] != 0x00)         //ͨ�Ŵ����״̬�벻Ϊ0x9000
        return 1;

    memcpy(pbData, bBuf + 2, bLen);
    return 0;		            //��ȷ
}

//---------------------------------------------------------
//������: ���Ķ�
//����˵����
//			pbSeesionKey  - ���������ݵ���Կ(8�ֽ�)
//			pbData		  - ���Ľ��ܺ�����
//			bLen   		  - ��ȡ�����ݳ���
//����ֵ˵����
//			0 - �ɹ�
//			1 - ʧ��
//˵��:
//---------------------------------------------------------
unsigned char SMEC_CryptReadFlash(unsigned char *pbSeesionKey, unsigned short wAddr, unsigned char *pbData, unsigned char bLen)
{
    unsigned char bBuf[128] = {0};
    unsigned char ret;
    unsigned char len, i;

    len = (bLen + 7) / 8 * 8;	//��bLenת��Ϊ8������

    bBuf[0] = 0xB1;		        //�ɸ��ģ������оƬ�г��򱣳�һ��
    bBuf[1] = (unsigned char)(wAddr>>8);
    bBuf[2] = (unsigned char) wAddr;
    bBuf[3] = len;	    		//��Ҫ��ȡ���������ݳ���, 8��������

    ret = IIC_WriteWithAddr(IIC_ADDR & 0xFE, bBuf, 4);
    if(ret)                     //����
        return 1;

    //��ִ����ȷ������״̬��90 00 + ��������
    ret = IIC_ReadWithAddr(IIC_ADDR | 0x1, bBuf, len + 2);
    if(ret || bBuf[0] != 0x90 || bBuf[1] != 0x00)         //ͨ�Ŵ����״̬�벻Ϊ0x9000
        return 1;

    for(i = 0; i < len; i += 8)
    {
        decrypt(&bBuf[2 + i], pbSeesionKey, &bBuf[2 + i]); 			//����ȡ�����������ݽ���
    }

    memcpy(pbData, bBuf + 2, bLen);
    return 0;		            //��ȷ
}

//---------------------------------------------------------
//������: дFlash
//����˵����
//			pbData - д�����ݴ�ŵ�ַ
//			bLen  -  д������ݳ���
//����ֵ˵����
//			0 - �ɹ�
//			1 - ʧ��
//˵��:
//---------------------------------------------------------
unsigned char SMEC_WriteFlash(unsigned short wAddr, unsigned char *pbData, unsigned char bLen)
{
    unsigned char bBuf[128] = {0};
    unsigned char ret;

    bBuf[0] = 0xD6;		        //�ɸ��ģ������оƬ�г��򱣳�һ��
    bBuf[1] = (unsigned char)(wAddr>>8);
    bBuf[2] = (unsigned char) wAddr;
    bBuf[3] = bLen;	    		//��Ҫ��ȡ���ݳ���
    memcpy(&bBuf[4], pbData, bLen);

    ret = IIC_WriteWithAddr(IIC_ADDR & 0xFE, bBuf, bLen + 4);
    if(ret)                     //����
        return 1;

    //��ִ����ȷ������״̬��90 00
    ret = IIC_ReadWithAddr(IIC_ADDR | 0x1, bBuf, 2);
    if(ret || bBuf[0] != 0x90 || bBuf[1] != 0x00)         //ͨ�Ŵ����״̬�벻Ϊ0x9000
        return 1;
    return 0;		            //��ȷ
}

//---------------------------------------------------------
//������: ���ü���оƬ�ڲ�����Բ�ܳ��㷨
//����˵����
//			pbInput 	 	- ��������
//			bInputLen 		- �������ݳ���
//			pbOutput	  	- ����оƬ�����,�������
//			pbOutputLen	  	- ������ݳ���
//����ֵ˵����
//			0 - �ɹ�
//			1 - ʧ��
//˵��: �� MCU �е�һ���ֹؼ����룬�������оƬ�����У�����Ҫ�õ�SMEC98SP�е��㷨ʱ��
//      ��MCU ��SMEC98SP ����ָ�SMEC98SP ����ָ����ڲ����У����ؽ����MCU��
//---------------------------------------------------------
unsigned char SMEC_CircleAlg(unsigned char *pbInput, unsigned char bInputLen, unsigned char *pbOutput, unsigned char *pbOutputLen)
{
    unsigned char bBuf[128] = {0};
    unsigned char ret;

    bBuf[0] = 0x72;		        //�ɸ��ģ������оƬ�г��򱣳�һ��
    bBuf[1] = 0x00;
    bBuf[2] = 0x00;
    bBuf[3] = bInputLen;
    memcpy(&bBuf[4], pbInput, bInputLen);

    ret = IIC_WriteWithAddr(IIC_ADDR & 0xFE, bBuf, 4 + bInputLen);
    if(ret)                     //����
        return 1;

    //��ִ����ȷ������״̬��90 00 + ������, ���������е��㷨Ϊ ����Բ�ܳ�, ֻ����1���ֽڵ����
    ret = IIC_ReadWithAddr(IIC_ADDR | 0x1, bBuf, 1 + 2);
    if(ret || bBuf[0] != 0x90 || bBuf[1] != 0x00)         //ͨ�Ŵ����״̬�벻Ϊ0x9000
        return 1;

    *pbOutputLen = 1;	 		//�����е��㷨Ϊ ����Բ�ܳ�, ֻ����1���ֽڵ����
    memcpy(pbOutput, bBuf + 2, *pbOutputLen);
    return 0;		            //��ȷ
}

//---------------------------------------------------------
//������: ���ü���оƬ�ڲ�����Բ�ܳ��㷨, ���ù�����Կ���ܺ�,����·�ϴ���
//����˵����
//			pbSeesionKey  	- ������Կ
//			pbInput 	 	- ��������
//			bInputLen 		- �������ݳ���
//			pbOutput	  	- ����оƬ�����,�������
//			pbOutputLen	  	- ������ݳ���
//����ֵ˵����
//			0 - �ɹ�
//			1 - ʧ��
//˵��: �� MCU �е�һ���ֹؼ����룬�������оƬ�����У�����Ҫ�õ�SMEC98SP�е��㷨ʱ��
//      ��MCU ��SMEC98SP ����ָ�SMEC98SP ����ָ����ڲ����У����ؽ����MCU��
//---------------------------------------------------------
unsigned char SMEC_CircleAlgCrypt(unsigned char *pbSeesionKey, unsigned char *pbInput, unsigned char bInputLen, unsigned char *pbOutput, unsigned char *pbOutputLen)
{
    unsigned char bBuf[128] = {0}, bTmp[8] = {0};
    unsigned char ret;

    bBuf[0] = 0x73;		        //�ɸ��ģ������оƬ�г��򱣳�һ��
    bBuf[1] = 0x00;
    bBuf[2] = 0x00;
    bBuf[3] = 8;				//
    memcpy(bTmp, pbInput, bInputLen);
    encrypt(bTmp, pbSeesionKey, &bBuf[4]);				   //���˿����ݼ���,����SMEC98SP,��������

    ret = IIC_WriteWithAddr(IIC_ADDR & 0xFE, bBuf, 8 + 4);
    if(ret)                     //����
        return 1;

    //��ִ����ȷ������״̬��90 00 + ������������, ���������е��㷨Ϊ ����Բ�ܳ�, ֻ����1���ֽڵ��������
    ret = IIC_ReadWithAddr(IIC_ADDR | 0x1, bBuf, 8 + 2);
    if(ret || bBuf[0] != 0x90 || bBuf[1] != 0x00)         //ͨ�Ŵ����״̬�벻Ϊ0x9000
        return 1;

    decrypt(&bBuf[2], pbSeesionKey, bTmp);					//����������������
    *pbOutputLen = bInputLen;	 							//�����е��㷨Ϊ ����Բ�ܳ�, ֻ����1���ֽڵ����.����ݾ����㷨�޸�, ������������������ݳ���
    memcpy(pbOutput, bTmp, *pbOutputLen);
    return 0;		            //��ȷ
}

//---------------------------------------------------------
//������: ����������Կ
//����˵����
//			pbMKey 	 		- ������Կ,���ڲ���������Կ, 16�ֽ�
//			pbRandom 		- ����оƬ�������, 8�ֽ�
//			pbSeesionKey  	- ������Կ
//����ֵ˵����
//			0 - �ɹ�
//			1 - ʧ��
//˵��: �ɹ�����������, ����оƬ����8�ֽ������, ����������Կ������RAM��
//      ������Կ�Ĳ���:  pbMKey �� (pbRandom ^ SMEC98SP�����)��3DES��������, ���ý��Ϊ pbSeesionKey(8�ֽ�)
//---------------------------------------------------------
unsigned char SMEC_GenSessionKey(unsigned char *pbMKey, unsigned char *pbRandom, unsigned char *pbSeesionKey)
{
    unsigned char bBuf[128] = {0}, bTmp[8] = {0};
    unsigned char ret;
    unsigned char i;

    bBuf[0] = 0xA0;		        //�ɸ��ģ������оƬ�г��򱣳�һ��
    bBuf[1] = 0x00;
    bBuf[2] = 0x00;
    bBuf[3] = 8;
    memcpy(&bBuf[4], pbRandom, 8);

    ret = IIC_WriteWithAddr(IIC_ADDR & 0xFE, bBuf, 8 + 4);
    if(ret)                     //����
        return 1;

    //��ִ����ȷ������״̬��90 00 + 8�ֽ������
    ret = IIC_ReadWithAddr(IIC_ADDR | 0x1, bBuf, 8 + 2);
    if(ret || bBuf[0] != 0x90 || bBuf[1] != 0x00)         //ͨ�Ŵ����״̬�벻Ϊ0x9000
        return 1;

    for(i = 0; i < 8; i ++)
    {
        bTmp[i] = pbRandom[i] ^ bBuf[2 + i]; 				//��SMEC98SP�������������оƬ��������������
    }

    TripleDES(bTmp, pbMKey, pbSeesionKey);					//���������Կ
    return 0;		            //��ȷ
}

//---------------------------------------------------------
//������: �˿���������
//����˵����
//			pbSeesionKey  	- ������Կ
//			pbGpioInput  	- ��Ҫ�����IO����������
//			bPortDataLen	- IO�����ݳ���
//			pbGpioOutput	- IO��"����"��Ľ��
//����ֵ˵����
//			0 - �ɹ�
//			1 - ʧ��
//˵��: �����㷨(PA������->�����ͼ���оƬ, ���ķ���)
//  ��Ժܶ����������,û��"�ؼ��㷨"���Դ���ڼ���оƬ��,���ǹ�����һ���㷨:
//  PA�˿�����2�ֽ�, �ù�����Կ���ܺ�,�͸�SMEC98SP,��SMEC98SP���ܺ�ȡ��,���ɹ�����Կ���ܻ��͸�����оƬ.�����͹����һ���㷨
//  ��PA = 0x0000, �ù�����Կ�����͸�SMEC98SP, SMEC98SP���ܺ�õ�0x0000, ȡ����Ϊ0xFFFF, ���ù�����Կ���ܸ�����оƬ. ����оƬ���ܺ�õ�0xFFFF
//  ����, �ж�IO�����ݷ�ʽ,ֻҪ��֮ǰ�෴�Ϳ���. ����˵PA0 �ߵ�ƽ�����Ķ���, ���������������,���ж�PA0Ϊ�͵�ƽȥ��
//  ����ÿ���ϵ�,������Կ����ʱ������, �����Ǳ仯��, ������ʹPA��������ͬ, ����·��ͨѶ������Ҳ�ǲ�ͬ��, ������оƬ�������ǻ���"������"��������,
//  �Ӷ��������ƽ��Ѷ�, ���Է�ֹ"��ֵ��"����
//---------------------------------------------------------
unsigned char SMEC_GpioAlg(unsigned char *pbSeesionKey, unsigned char *pbGpioInput, unsigned char bGpioDataLen, unsigned char *pbGpioOutput)
{
    unsigned char bBuf[128] = {0}, bTmp[8] = {0};
    unsigned char ret;

    bBuf[0] = 0xA2;		        //�ɸ��ģ������оƬ�г��򱣳�һ��
    bBuf[1] = 0x00;
    bBuf[2] = 0x00;
    bBuf[3] = 8;
    memcpy(bTmp, pbGpioInput, bGpioDataLen);
    encrypt(bTmp, pbSeesionKey, &bBuf[4]);				   //���˿����ݼ���,����SMEC98SP,����"����"

    ret = IIC_WriteWithAddr(IIC_ADDR & 0xFE, bBuf, 8 + 4);
    if(ret)                     //����
        return 1;

    //��ִ����ȷ������״̬��90 00 + ��"����"���Ķ˿���������
    ret = IIC_ReadWithAddr(IIC_ADDR | 0x1, bBuf, 8 + 2);
    if(ret || bBuf[0] != 0x90 || bBuf[1] != 0x00)         //ͨ�Ŵ����״̬�벻Ϊ0x9000
        return 1;
    decrypt(&bBuf[2], pbSeesionKey, bTmp);					//��"����"�������
    memcpy(pbGpioOutput, bTmp, bGpioDataLen);

    return 0;		            //��ȷ
}


