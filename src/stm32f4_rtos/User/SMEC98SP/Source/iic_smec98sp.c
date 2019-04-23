#include "stm32f4xx.h"
#include <stdbool.h>
//SMEC98SP IIC��������IO��ģ��IICЭ��
#include "bsp.h"

#define IIC_NOACK         TRUE
#define IIC_ACK           FALSE
#define HIGHT             TRUE
#define LOW               FALSE

#define IIC_DELAYTIME     2			 //50
#define ACKCHECKTIME      2000


//===========================      ��SMEC98SPͨѶ�ĹܽŶ���      ===========================//
#define IIC_SCL_PIN			GPIO_Pin_6			//��ͬIO�޸�����
#define IIC_SDA_PIN			GPIO_Pin_7			//��ͬIO�޸�����
#define IIC_PORT			GPIOB				//��ͬIO�޸�����


//===========================      I2CͨѶ���      ===========================//
//---------------------------------------------------------
//�������ܣ�ģ��˿ڻ������ó�ʼ��
//����˵����
//			void
//����ֵ˵����
//			void
//---------------------------------------------------------
void SMEC_I2cInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* ��Ϊ����� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;		/* ��Ϊ����ģʽ */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* ���������費ʹ�� */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO������ٶ� */
  
  GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;
  GPIO_Init(IIC_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* ��Ϊ����� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* ���������費ʹ�� */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO������ٶ� */
  
  GPIO_InitStructure.GPIO_Pin = IIC_SCL_PIN;
  GPIO_Init(IIC_PORT, &GPIO_InitStructure);
}

//�����ź���Ϊ����ģʽ
void SdaInputMode(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		/* ��Ϊ����� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;		/* ��Ϊ����ģʽ */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* ���������費ʹ�� */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO������ٶ� */
  
  GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;
  GPIO_Init(IIC_PORT, &GPIO_InitStructure);
}

//�����ź���Ϊ���ģʽ
void SdaOutputMode(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* ��Ϊ����� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;		/* ��Ϊ����ģʽ */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* ���������費ʹ�� */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO������ٶ� */
  
  GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;
  GPIO_Init(IIC_PORT, &GPIO_InitStructure);
}

void IIC_Delay(unsigned int times)
{
  u16 loop,loop1;
  for(loop=0; loop<times; loop++)
    for(loop1=times; loop1>0; loop1--);
}

//ʱ���ź�����ߵ�ƽ
void IIC_SetSclPin(void)
{
  GPIO_WriteBit(IIC_PORT,IIC_SCL_PIN,Bit_SET);
}

//ʱ���ź�����͵�ƽ
void IIC_ClrSclPin(void)
{
  GPIO_WriteBit(IIC_PORT,IIC_SCL_PIN,Bit_RESET);
}

//�����ź�����ߵ�ƽ
void IIC_SetSdaPin(void)
{
  GPIO_WriteBit(IIC_PORT,IIC_SDA_PIN,Bit_SET);
}

//�����ź�����͵�ƽ
void IIC_ClrSdaPin(void)
{
  GPIO_WriteBit(IIC_PORT,IIC_SDA_PIN,Bit_RESET);
}

//��ȡ�����ź��ϵ�����
bool IIC_GetSdaStus(void)
{
  return (bool)(GPIO_ReadInputDataBit(IIC_PORT,IIC_SDA_PIN));
}

bool IIC_CheckACK(void)
{
  return IIC_GetSdaStus();
}

//ʱ���ź����HLow
void IIC_SetScl(bool HLow)
{
  HLow?IIC_SetSclPin():IIC_ClrSclPin();
}

//�����ź����HLow
void IIC_SetSda(bool HLow)
{
  HLow?IIC_SetSdaPin():IIC_ClrSdaPin();
}

//---------------------------------------------------------
//�������ܣ�i2c��ʼ�źŲ���
//����˵����
//			void
//����ֵ˵����
//			void
//---------------------------------------------------------
void IIC_Start(void)
{
  SdaOutputMode();
  
  IIC_SetSda(HIGHT);
  IIC_SetScl(HIGHT);
  IIC_Delay(IIC_DELAYTIME);
  IIC_SetSda(LOW);
  IIC_Delay(IIC_DELAYTIME);
  IIC_SetScl(LOW);
}

//---------------------------------------------------------
//�������ܣ�i2cֹͣ�źŲ���
//����˵����
//			void
//����ֵ˵����
//			void
//---------------------------------------------------------
void IIC_Stop(void)
{
  SdaOutputMode();
  
  IIC_SetSda(LOW);
  IIC_SetScl(HIGHT);
  
  IIC_Delay(IIC_DELAYTIME);
  IIC_SetSda(HIGHT);
  IIC_Delay(IIC_DELAYTIME);
  //IIC_SetScl(LOW);
}

//---------------------------------------------------------
//������: i2c����һ���ֽ�
//����˵����
//			ucVal : �ֽ�����
//����ֵ˵����
//			0 - �ɹ�
//          1 - ʧ��
//---------------------------------------------------------
bool IIC_Send(unsigned char ucVal)
{
  bool Ack;
  u16 AckLoop;
  u8 i;
  
  SdaOutputMode();
  
  for(i=0; i<8; i++)
  {
    IIC_SetScl(LOW);
    IIC_Delay(IIC_DELAYTIME);
    IIC_SetSda((ucVal & 0x80)?TRUE:FALSE);
    IIC_Delay(IIC_DELAYTIME);
    IIC_SetScl(HIGHT);
    IIC_Delay(IIC_DELAYTIME);
    IIC_Delay(IIC_DELAYTIME);
    ucVal<<= 1;
  }
  IIC_SetScl(LOW);
  SdaInputMode();
  
  IIC_Delay(IIC_DELAYTIME);
  IIC_Delay(IIC_DELAYTIME);
  IIC_SetScl(HIGHT);
  IIC_Delay(IIC_DELAYTIME);
  IIC_Delay(IIC_DELAYTIME);
  Ack = IIC_NOACK;
  
  for(AckLoop=0; AckLoop<ACKCHECKTIME; AckLoop++) //260us
  {
    if(!IIC_GetSdaStus())
    {
      Ack = IIC_ACK;
      break;
    }
    IIC_Delay(IIC_DELAYTIME);
  }
  IIC_SetScl(LOW);
  return Ack;			//return success=0 / failure=1
}

//---------------------------------------------------------
//������: i2c��ȡһ���ֽ�
//����˵����
//			bACK - ��ȡһ���ֽ���ɺ�, ����SDA״ֵ̬
//����ֵ˵����
//			���յ�������
//---------------------------------------------------------
unsigned char IIC_Read(bool bACK)
{
  unsigned char Data;
  unsigned char i;
  
  SdaInputMode();
  
  Data = 0;
  for(i=0; i<8; i++)
  {
    Data <<= 1;
    IIC_SetScl(LOW);
    IIC_Delay(IIC_DELAYTIME);
    //IIC_SetSda(HIGHT);
    IIC_Delay(IIC_DELAYTIME);
    IIC_Delay(IIC_DELAYTIME);
    IIC_SetScl(HIGHT);
    IIC_Delay(IIC_DELAYTIME);
    IIC_Delay(IIC_DELAYTIME);
    if(IIC_GetSdaStus())
      Data |= 1;
    else
      Data &= 0xfe;
  }
  
  IIC_SetScl(LOW);
  SdaOutputMode();
  
  IIC_Delay(IIC_DELAYTIME);
  IIC_SetSda(bACK);
  IIC_Delay(IIC_DELAYTIME);
  IIC_SetScl(HIGHT);
  IIC_Delay(IIC_DELAYTIME);
  IIC_Delay(IIC_DELAYTIME);
  IIC_SetScl(LOW);
  return Data;
  
}

//---------------------------------------------------------
//������: ����1������
//����˵����
//			addr - I2C��ַ
//			buf	 - Ҫ��ȡ���ݻ�����ָ��
//			len	 - ����ȡ����
//����ֵ˵����
//			���յ�������
//---------------------------------------------------------
bool IIC_ReadWithAddr(unsigned char addr, unsigned char *buf, unsigned char len)
{
  unsigned char i;
  i = 10;
  while (i) {
    IIC_Start();
    if (IIC_Send(addr) == IIC_ACK) break;
    if (--i == 0) return IIC_NOACK;
  }
  for(i = 0; i < len -1 ; i++)
  {
    buf[i] = IIC_Read(IIC_ACK);
  }
  
  buf[i] = IIC_Read(IIC_NOACK);
  
  IIC_Stop();
  
  return IIC_ACK;
}

//---------------------------------------------------------
//������: д��1������
//����˵����
//			addr - I2C��ַ
//			buf	 - Ҫд�����ݻ�����ָ��
//			len	 - ��д�볤��
//����ֵ˵����
//			���յ�������
//---------------------------------------------------------
bool IIC_WriteWithAddr(unsigned char addr, unsigned char *buf, unsigned char len)
{
  unsigned char i;
  
  i = 10;
  while (i) {
    IIC_Start();
    if (IIC_Send(addr) == IIC_ACK) break;
    if (--i == 0) return IIC_NOACK;
  }
  
  for(i = 0; i < len; i++)
  {
    if(IIC_Send(buf[i]))
      return IIC_NOACK;
  }
  
  IIC_Delay(IIC_DELAYTIME);
  IIC_Delay(IIC_DELAYTIME);
  //IIC_Delay(20*IIC_DELAYTIME);
  //	DelayNms(40);
  
  IIC_Stop();
  
  return IIC_ACK;
  
}
