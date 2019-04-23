#include "DSP2833x_Device.h"
//#define CPU_RATE   16.667L   // for a 60MHz CPU clock speed (SYSCLKOUT)
//#define DELAY_US(A)  DSP28x_usDelay(((((long double) A * 1000.0L) / (long double)CPU_RATE) - 9.0L) / 5.0L)
#include "DSP2833x_Examples.h"

#define IIC_NOACK         1
#define IIC_ACK           0

#define IIC_DELAYTIME     5			 //50
#define ACKCHECKTIME      2000


//===========================      ��SMEC98SPͨѶ�ĹܽŶ���      ===========================//
#define IIC_SCL_PIN			15			//��ͬIO�޸�����
#define IIC_SDA_PIN			14		//��ͬIO�޸�����

//===========================      I2CͨѶ���      ===========================//
//SMEC98SP IIC��������IO��ģ��IICЭ�飬����ʵ��ʹ�õ�IO��
//��������ʹ��GPIO14-SDA, GPIO15-SCL, ��ʹ�ò�ͬ��IO��,�����Ϊ��Ӧ��IO��
//---------------------------------------------------------
//�������ܣ�ģ��˿ڻ������ó�ʼ��
//����˵����
//			void
//����ֵ˵����
//			void
//---------------------------------------------------------
void SMEC_I2cInit(void)
{
  EALLOW;
  GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 0;//��ΪGPIO
  GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 0;//��ΪGPIO

  GpioCtrlRegs.GPADIR.bit.GPIO15=1;   // output
  GpioCtrlRegs.GPADIR.bit.GPIO14=1;

  GpioCtrlRegs.GPAPUD.bit.GPIO14 =1;
  GpioCtrlRegs.GPAPUD.bit.GPIO15 =1;

	#define SCL_H_SMEC98SP        		   GpioDataRegs.GPASET.bit.GPIO15 = 1
	#define SCL_L_SMEC98SP                 GpioDataRegs.GPACLEAR.bit.GPIO15 = 1

	#define SDA_H_SMEC98SP                 GpioDataRegs.GPASET.bit.GPIO14 = 1
	#define SDA_L_SMEC98SP                 GpioDataRegs.GPACLEAR.bit.GPIO14 = 1

	#define SDA_SMEC98SP_READ()            GpioDataRegs.GPADAT.bit.GPIO14

	#define SDA_SMEC98SP_RESET             (uint32_t)0
	EDIS;

}

//�����ź���Ϊ����ģʽ
void SdaInputMode(void)
{
	EALLOW;
	GpioCtrlRegs.GPADIR.bit.GPIO14=0;
	EDIS;
}

//�����ź���Ϊ���ģʽ
void SdaOutputMode(void)
{
	EALLOW;
	GpioCtrlRegs.GPADIR.bit.GPIO14=1;
	EDIS;
}

void IIC_Delay(unsigned int times)
{
//	unsigned short loop,loop1;
//	for(loop=0;loop<times;loop++)
//		for(loop1=times;loop1>0;loop1--);
	DELAY_US(times);
}

//ʱ���ź�����ߵ�ƽ
void IIC_SetSclPin(void)
{
	SCL_H_SMEC98SP;
}

//ʱ���ź�����͵�ƽ
void IIC_ClrSclPin(void)
{
	SCL_L_SMEC98SP;
}

//�����ź�����ߵ�ƽ
void IIC_SetSdaPin(void)
{
	SDA_H_SMEC98SP;
}

//�����ź�����͵�ƽ
void IIC_ClrSdaPin(void)
{
	SDA_L_SMEC98SP;
}

//��ȡ�����ź��ϵ�����
unsigned char IIC_GetSdaStus(void)
{
	unsigned char ret;
	EALLOW;
	ret = SDA_SMEC98SP_READ();
	EDIS;
	return ret;
}

unsigned char IIC_CheckACK(void)
{
	return IIC_GetSdaStus();
}

//ʱ���ź����HLow
void IIC_SetScl(unsigned char HLow)
{
	HLow?IIC_SetSclPin():IIC_ClrSclPin();
}

//�����ź����HLow
void IIC_SetSda(unsigned char HLow)
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

	IIC_SetSda(1);
	IIC_SetScl(1);
	IIC_Delay(IIC_DELAYTIME);
	IIC_SetSda(0);
	IIC_Delay(IIC_DELAYTIME);
	IIC_SetScl(0);
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
	
	IIC_SetSda(0);
	IIC_SetScl(1);

	IIC_Delay(IIC_DELAYTIME);
	IIC_SetSda(1);
	IIC_Delay(IIC_DELAYTIME);
	//IIC_SetScl(0);
}

//---------------------------------------------------------
//������: i2c����һ���ֽ�
//����˵����
//			ucVal : �ֽ�����
//����ֵ˵����
//			0 - �ɹ�
//          1 - ʧ��
//---------------------------------------------------------
unsigned char IIC_Send(unsigned char ucVal)
{
	unsigned char Ack;
	unsigned short AckLoop;
	unsigned char i;

	SdaOutputMode();

	for(i=0;i<8;i++)
	{
		IIC_SetScl(0);
		IIC_Delay(IIC_DELAYTIME);
		IIC_SetSda((ucVal & 0x80)?1:0);
		IIC_Delay(IIC_DELAYTIME);
		IIC_SetScl(1);
		IIC_Delay(IIC_DELAYTIME);
		IIC_Delay(IIC_DELAYTIME);
		ucVal<<= 1;
	}
	IIC_SetScl(0);
	SdaInputMode();

	IIC_Delay(IIC_DELAYTIME);
	IIC_Delay(IIC_DELAYTIME);
	IIC_SetScl(1);
	IIC_Delay(IIC_DELAYTIME);
	IIC_Delay(IIC_DELAYTIME);
	Ack = IIC_NOACK;

	for(AckLoop=0;AckLoop<ACKCHECKTIME;AckLoop++) //260us
	{
		if(!IIC_GetSdaStus())
		{
			Ack = IIC_ACK;
			break;
		}
		IIC_Delay(IIC_DELAYTIME);
	}
	IIC_SetScl(0);
	return Ack;			//return success=0 / failure=1
}

//---------------------------------------------------------
//������: i2c��ȡһ���ֽ�
//����˵����
//			bACK - ��ȡһ���ֽ���ɺ�, ����SDA״ֵ̬
//����ֵ˵����
//			���յ�������
//---------------------------------------------------------
unsigned char IIC_Read(unsigned char bACK)
{
	unsigned char Data;
	unsigned char i;

	SdaInputMode();

	Data = 0;
	for(i=0;i<8;i++)
	{
		Data <<= 1;
		IIC_SetScl(0);
		IIC_Delay(IIC_DELAYTIME);
		//IIC_SetSda(1);
		IIC_Delay(IIC_DELAYTIME);
		IIC_Delay(IIC_DELAYTIME);
		IIC_SetScl(1);
		IIC_Delay(IIC_DELAYTIME);
		IIC_Delay(IIC_DELAYTIME);
		if(IIC_GetSdaStus())
			Data |= 1;
		else
			Data &= 0xfe;
	}

	IIC_SetScl(0);
	SdaOutputMode();

	IIC_Delay(IIC_DELAYTIME);
    IIC_SetSda(bACK);
	IIC_Delay(IIC_DELAYTIME);
	IIC_SetScl(1);
	IIC_Delay(IIC_DELAYTIME);
	IIC_Delay(IIC_DELAYTIME);
	IIC_SetScl(0);
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
unsigned char IIC_ReadWithAddr(unsigned char addr, unsigned char *buf, unsigned char len)
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
unsigned char IIC_WriteWithAddr(unsigned char addr, unsigned char *buf, unsigned char len)
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
