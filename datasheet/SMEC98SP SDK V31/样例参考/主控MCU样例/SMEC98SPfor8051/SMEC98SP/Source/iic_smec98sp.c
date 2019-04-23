//COPYRIGHT (c) 2008-2015	Sinormous Technology
//Author:����
//����bug������ϵ��koks98@tom.com
//QQ:26600347

#include <iic_smec98sp.h>
#include <stc12c5a60s2.H>
//����ģ��IIC��ʽͨѶ
sbit    IIC_SCL     =P1^0;			//����IIC SCL�ܽ�
sbit    IIC_SDA     =P1^1;			//����IIC SDA�ܽţ�

#define HIGH	1
#define LOW		0
#define IIC_NOACK 1
#define IIC_ACK 0
#define IIC_DELAYTIME 5			//IICͨѶ���ʴ�Լ100K
void IIC_Delayus(unsigned int n);
/******************************************************************************************
/*��ʱus����22.1184M  1T
/******************************************************************************************/
void IIC_Delayus(unsigned int n)
{
	unsigned char i;
	for(;n>0;n--)
	{
 		i = 1;
	}	
}
void    IIC_SetSDA(unsigned char level)
{
	if(level == HIGH)
	{
		IIC_SDA = 1;
	}else
	{
		IIC_SDA = 0;
	}
}
void    IIC_SetSCL(unsigned char level)
{
	if(level == HIGH)
	{
		IIC_SCL = 1;
	}else
	{
		IIC_SCL = 0;
	}
}
void SetSDAto_InputMode(void)
{
	//����оƬ�ֲ�����
	IIC_SDA = 1;			//��Ϊ����
}
void SetSDAto_OutputMode(void)
{
	//����оƬ�ֲ�����
	//����Ƭ���������ÿ�ֱ�����
}

void   IIC_Start(void)
{
	SetSDAto_OutputMode();			//SDA��Ϊ���ģʽ
	IIC_SetSDA(HIGH);
	IIC_SetSCL(HIGH);
	IIC_Delayus(IIC_DELAYTIME);			
	IIC_SetSDA(LOW);
	IIC_Delayus(IIC_DELAYTIME);
	IIC_SetSCL(LOW);
}

void   IIC_Stop(void)
{	
	SetSDAto_OutputMode();			//SDA��Ϊ���ģʽ
	IIC_SetSDA(LOW);
	IIC_SetSCL(HIGH);
	IIC_Delayus(IIC_DELAYTIME);
	IIC_SetSDA(HIGH);
	IIC_Delayus(IIC_DELAYTIME);
}
unsigned char IIC_GetSdaStus(void)
{
	unsigned char tmp;

	if(IIC_SDA == 1)	
		tmp=1;
	else 
		tmp=0;
	return tmp;
}

#define ACKCHECKTIME 600
unsigned char  IIC_Send(unsigned char IIC_data)
{
	unsigned char Ack;
	unsigned int AckLoop;
	unsigned char i;
	SetSDAto_OutputMode();			//SDA��Ϊ���ģʽ
	for(i=0;i<8;i++)
	{
		IIC_SetSCL(LOW);
		IIC_Delayus(IIC_DELAYTIME);
		IIC_SetSDA((IIC_data & 0x80)?1:0);
		IIC_Delayus(IIC_DELAYTIME);
		IIC_SetSCL(HIGH);
		IIC_Delayus(IIC_DELAYTIME);
		IIC_Delayus(IIC_DELAYTIME);
		IIC_data<<= 1;
	}
	IIC_SetSCL(LOW);
	IIC_Delayus(IIC_DELAYTIME);
	IIC_Delayus(IIC_DELAYTIME);
	IIC_SetSCL(HIGH);
	IIC_Delayus(IIC_DELAYTIME);
	IIC_Delayus(IIC_DELAYTIME);
	Ack = IIC_NOACK;
	SetSDAto_InputMode();			//SDA��Ϊ����ģʽ
	for(AckLoop=0;AckLoop<ACKCHECKTIME;AckLoop++) //260us
	{
		if(!IIC_GetSdaStus())
		{
			Ack = IIC_ACK;
			break;
		}
		IIC_Delayus(IIC_DELAYTIME);
	}
	IIC_SetSCL(LOW);
	return Ack;			//return success=0 / failure=1
}

unsigned char IIC_Read(unsigned char bACK)
{
	unsigned char Data;
	unsigned char i;
	Data = 0;
	SetSDAto_InputMode();			//SDA��Ϊ����ģʽ
	for(i=0;i<8;i++)
	{
		Data <<= 1;
		IIC_SetSCL(LOW);
		IIC_Delayus(IIC_DELAYTIME);
		IIC_Delayus(IIC_DELAYTIME);
		IIC_Delayus(IIC_DELAYTIME);
		IIC_SetSCL(HIGH);
		IIC_Delayus(IIC_DELAYTIME);
		IIC_Delayus(IIC_DELAYTIME);
		if(IIC_GetSdaStus())
			Data |= 1;
		else
			Data &= 0xfe;
	}
	IIC_SetSCL(LOW);
	SetSDAto_OutputMode();			//SDA��Ϊ���ģʽ	
	IIC_Delayus(IIC_DELAYTIME);   
  	IIC_SetSDA(bACK);
	IIC_Delayus(IIC_DELAYTIME);
	IIC_SetSCL(HIGH);
	IIC_Delayus(IIC_DELAYTIME);
	IIC_Delayus(IIC_DELAYTIME);
	IIC_SetSCL(LOW);
	return Data;
}


bit IIC_ReadWithAddr(unsigned char addr, unsigned char *buf, unsigned char len)
{
  	unsigned char i;
	i = 10;
    while (i) 
	{
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

bit IIC_WriteWithAddr(unsigned char addr, unsigned char *buf, unsigned char len)
{
  	unsigned char i;
	i = 10;
    while (i) 
	{
		IIC_Start();
        if (IIC_Send(addr) == IIC_ACK) break;
        if (--i == 0) return IIC_NOACK;
    }
	for(i = 0; i < len; i++)
	{
		if(IIC_Send(buf[i]))
			return IIC_NOACK;
	}
	IIC_Delayus(IIC_DELAYTIME);
	IIC_Delayus(IIC_DELAYTIME);
	IIC_Stop();
	return IIC_ACK;
}

