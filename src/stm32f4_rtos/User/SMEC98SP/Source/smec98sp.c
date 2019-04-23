//COPYRIGHT (c) 2018-2025	Sinormous Technology
//�������о�ΰҵ��Ϣ�Ƽ����޹�˾
//www.sinormous.com;
//Author:����ˮ
//QQ:47583353

#include <string.h>
#include "bsp.h"
#include <stdbool.h>



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

unsigned short Get_Adc_RandomSeek(void)
{
  unsigned char Count;
  unsigned short ADC_RandomSeek = 0;
  
  for(Count = 0; Count < 4; Count++) 
  {
    ADC_RegularChannelConfig(ADC1, SEMC_ADC_CHANNEL, 1, ADC_SampleTime_480Cycles );  //ADC1,ADCͨ��,480������,��߲���ʱ�������߾�ȷ��
    ADC_SoftwareStartConv(ADC1);		//ʹ��ָ����ADC1�����ת����������	
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�̨���y��a???����?
    ADC_RandomSeek <<= 4;
    ADC_RandomSeek += ADC_GetConversionValue(ADC1) & 0x000f;		/*2��?��4��?ADC��??�̡�???��?��?2��?����?�̨�????��??��3��16??����?a??����*/
  }
  return ADC_RandomSeek;
}

void semc_adc_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  ADC_InitTypeDef       ADC_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��GPIOAʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //ʹ��ADC1ʱ��
  
  //�ȳ�ʼ��ADC1ͨ��5 IO��
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SEMC_ADC;//PA4 ͨ��4
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//ģ������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//����������
  GPIO_Init(GPIO_PORT_SEMC_ADC, &GPIO_InitStructure);//��ʼ��  
  
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1��λ
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//��λ����	 
  
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//���������׶�֮����ӳ�5��ʱ��
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMAʧ��
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);//��ʼ��
  
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12λģʽ
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;//��ɨ��ģʽ	
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//�ر�����ת��
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//��ֹ������⣬ʹ���������
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���	
  ADC_InitStructure.ADC_NbrOfConversion = 1;//1��ת���ڹ��������� Ҳ����ֻת����������1 
  ADC_Init(ADC1, &ADC_InitStructure);//ADC��ʼ��
  
  ADC_Cmd(ADC1, ENABLE);//����ADת����	
}

void GetStm32Uid(unsigned char *pSTM32_UID)
{
  pSTM32_UID[0] =  *(unsigned char*)(0x1FFF7A10);
  pSTM32_UID[1] =  *(unsigned char*)(0x1FFF7A11);
  pSTM32_UID[2] =  *(unsigned char*)(0x1FFF7A12);
  pSTM32_UID[3] =  *(unsigned char*)(0x1FFF7A13);
  pSTM32_UID[4] =  *(unsigned char*)(0x1FFF7A14);
  pSTM32_UID[5] =  *(unsigned char*)(0x1FFF7A15);
  pSTM32_UID[6] =  *(unsigned char*)(0x1FFF7A16);
  pSTM32_UID[7] =  *(unsigned char*)(0x1FFF7A17);
  pSTM32_UID[8] =  *(unsigned char*)(0x1FFF7A18);
  pSTM32_UID[9] =  *(unsigned char*)(0x1FFF7A19);
  pSTM32_UID[10] = *(unsigned char*)(0x1FFF7A1a);
  pSTM32_UID[11] = *(unsigned char*)(0x1FFF7A1b);
}

void SMEC_Test(void)
{
  /*?��???��??,2??��?��I2C???����?��?��?,?����?��1��?��?��?����.��|?????��??��?�����?���,����?1?��??D???��????ao��,��?1��?��???��?t???????D?����??��?? */
  unsigned char InternalKey[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};//?��2?��??��?��??,��?D?o��SMEC98SP��???
  unsigned char ExternalKey[16] = {0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F};//��a2?��??��?��??,��?D?o��SMEC98SP��???
  unsigned char SHA1_Key[16] = {0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F};   //1t?��??������??��?��??,��?D?o��SMEC98SP��???
  unsigned char MKey[16] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F};   //?��???��??,��?����2������1y3��?��??,��?D?o��SMEC98SP��???
  
  unsigned char Pin[8] = {0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc};			//Pin��??��?��??,��?D?o��SMEC98SP��???
  
  unsigned char bStm32Uid[12] = {0};			//��?��?STM32��?UID
  unsigned char bSmec98spUid[12] = {0};		//��?��?SMEC98SP��?UID
  unsigned short RandomSeek;					//???����y??����
  unsigned char bRandom[8] = {0};				//��?��????����y
  unsigned char bSessionKey[8] = {0};			//��?��?1y3��?��??,1y3��?��???a��������2��������??��??
  unsigned char bDataBuf[64] = {0};
  unsigned char ret, bLen;
  unsigned short i, j;
  
  SMEC_I2cInit();
  /*��?��?ADCD��??��y??2������???����y*/
  semc_adc_Init();									//?����??��?��3��D��?D
  RandomSeek = Get_Adc_RandomSeek();			//��?��?ADCD��??��y??2������???����y
  
  /*??��?STM32��?UID*/
  GetStm32Uid(bStm32Uid);
  printf("GetStm32Uid: ");
//  PrintHex(bStm32Uid, 12);
  printf("\r\n");
  
  /*??��?SMEC98SP��?UID*/
  ret = SMEC_GetUid(bSmec98spUid);
  if(ret)
  {
    printf("SMEC_GetUid -> Error !\r\n");
    while(1);
  }
  printf("SMEC_GetUid: ");
//  PrintHex(bSmec98spUid, 12);
  printf("\r\n");
  
  /*?????����yRandomSeek��??����?��?��????����|����(��?STM32��?UID, SMEC98SP��?UID���¡㨮?��, ��1��??�䨺1?����??��????,2?��?��?STM32,SMEC98SP???����y??������22?��?)*/
  for(i = 0; i < 6; i += 2)
  {
    /*��1RandomSeek��?STM32��?UID?��1?*/
    j = (bStm32Uid[i] << 8) + bStm32Uid[i + 1];
    RandomSeek ^= j;
    
    /*��1RandomSeek��?SMEC98SP��?UID?��1?*/
    j = (bSmec98spUid[i] << 8) + bSmec98spUid[i + 1];
    RandomSeek ^= j;
  }
  srand(RandomSeek);
  printf("RandomSeek: %04x \r\n", RandomSeek);
  
  /*PIN???��?��*/
  ret = SMEC_CheckPin(Pin, (unsigned char)sizeof(Pin));
  if(ret)
  {
    printf("SMEC_CheckPin -> Error !\r\n");
    while(1);
  }
  printf("SMEC_CheckPin OK !\r\n");
  
  /*?��2?��??��, ?��??D?????SMEC98SP?��?��D???o?����D??D??*/
  for(i = 0; i < 8; i ++)
  {
    bRandom[i] = (unsigned char) rand();
  }
  ret = SMEC_IntrAuth(InternalKey, bRandom);
  if(ret)
  {
    printf("SMEC_IntrAuth -> Error !\r\n");
    while(1);
  }
  printf("SMEC_IntrAuth OK !\r\n");
  
//  /*��a2?��??��, SMEC98SP?��?��D??????��??D???o?����D??D??*/
//  ret = SMEC_ExtrAuth(ExternalKey);
//  if(ret)
//  {
//    printf("SMEC_ExtrAuth -> Error !\r\n");
//    while(1);
//  }
//  printf("SMEC_ExtrAuth OK !\r\n");
//  
//  /*SHA1?a��a??������??��, ��y?Y3��?��?����??o����?��*/
//  for(i = 0; i < 16; i ++)
//  {
//    bDataBuf[i] = (unsigned char) rand();
//  }
//  ret = SMEC_Sha1Auth(SHA1_Key, (unsigned char)sizeof(SHA1_Key), bDataBuf, 16);
//  if(ret)
//  {
//    printf("SMEC_Sha1Auth -> Error !\r\n");
//    while(1);
//  }
//  printf("SMEC_Sha1Auth OK !\r\n");
//  
//  /*�̡¨�??��?��D????��2??????2?��3��??����*/
//  bDataBuf[0] = 0x02;
//  ret = SMEC_CircleAlg(bDataBuf, 1, bDataBuf, &bLen);
//  if(ret)
//  {
//    printf("SMEC_CircleAlg -> Error !\r\n");
//    while(1);
//  }
//  printf("SMEC_CircleAlg OK, C = %02x !\r\n", bDataBuf[0]);
//  
//  /*2������1y3��?��??,��?����o��D?��?Flash��y?Y?��?��?��,?��11?����?"???����y?Y????"*/
//  for(i = 0; i < 8; i ++)
//  {
//    bRandom[i] = (unsigned char) rand();
//  }
//  ret = SMEC_GenSessionKey(MKey, bRandom, bSessionKey);
//  if(ret)
//  {
//    printf("SMEC_GenSessionKey -> Error !\r\n");
//    while(1);
//  }
//  printf("SMEC_GenSessionKey OK !\r\n");
//  
//  /*?��???����?Flash��y?Y*/
//  ret = SMEC_CryptReadFlash(bSessionKey, 0x0000, bDataBuf, 16);
//  if(ret)
//  {
//    printf("SMEC_CryptReadFlash -> Error !\r\n");
//    while(1);
//  }
//  printf("SMEC_CryptReadFlash OK:\r\n");
////  PrintHex(bDataBuf, 16);
//  
//  /*?����?Flash��y?Y*/
//  ret = SMEC_ReadFlash(0x0000, bDataBuf, 16);
//  if(ret)
//  {
//    printf("SMEC_ReadFlash -> Error !\r\n");
//    while(1);
//  }
//  printf("SMEC_ReadFlash OK:\r\n");
////  PrintHex(bDataBuf, 16);
//  
//  /*D��Flash��y?Y*/
//  for(i = 0; i < 16; i ++)
//  {
//    bDataBuf[i] = (unsigned char) i;
//  }
//  ret = SMEC_WriteFlash(0x0000, bDataBuf, 16);
//  if(ret)
//  {
//    printf("SMEC_WriteFlash -> Error !\r\n");
//    while(1);
//  }
//  printf("SMEC_WriteFlash OK!\r\n");
//  
//  /*11?��"???����y?Y????", ?����?��?����?����?PA~PC???����y?Y*/
//  bDataBuf[0] = 0x00;
//  bDataBuf[1] = 0x00;
//  ret = SMEC_GpioAlg(bSessionKey, bDataBuf,2, bDataBuf);
//  if(ret)
//  {
//    printf("SMEC_GpioAlg -> Error !\r\n");
//    while(1);
//  }
//  printf("SMEC_GpioAlg OK:\r\n");
////  PrintHex(bDataBuf, 2);
//  
//  /*�̡¨�??��?��D????��2??????2?��3��??����,2��?��???��???����?��?��?*/
//  bDataBuf[0] = 0x02;
//  ret = SMEC_CircleAlgCrypt(bSessionKey, bDataBuf, 1, bDataBuf, &bLen);
//  if(ret)
//  {
//    printf("SMEC_CircleAlgCrypt -> Error !\r\n");
//    while(1);
//  }
//  printf("SMEC_CircleAlgCrypt OK, C = %02x !\r\n", bDataBuf[0]);
}

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


