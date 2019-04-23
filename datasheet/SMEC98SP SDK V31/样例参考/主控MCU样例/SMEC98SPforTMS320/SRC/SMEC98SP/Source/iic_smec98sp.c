#include "DSP2833x_Device.h"
//#define CPU_RATE   16.667L   // for a 60MHz CPU clock speed (SYSCLKOUT)
//#define DELAY_US(A)  DSP28x_usDelay(((((long double) A * 1000.0L) / (long double)CPU_RATE) - 9.0L) / 5.0L)
#include "DSP2833x_Examples.h"

#define IIC_NOACK         1
#define IIC_ACK           0

#define IIC_DELAYTIME     5			 //50
#define ACKCHECKTIME      2000


//===========================      与SMEC98SP通讯的管脚定义      ===========================//
#define IIC_SCL_PIN			15			//不同IO修改这里
#define IIC_SDA_PIN			14		//不同IO修改这里

//===========================      I2C通讯相关      ===========================//
//SMEC98SP IIC驱动程序，IO口模拟IIC协议，更换实际使用的IO脚
//本例程中使用GPIO14-SDA, GPIO15-SCL, 若使用不同的IO口,请更改为对应的IO口
//---------------------------------------------------------
//函数介绍：模拟端口基本配置初始化
//参数说明：
//			void
//返回值说明：
//			void
//---------------------------------------------------------
void SMEC_I2cInit(void)
{
  EALLOW;
  GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 0;//设为GPIO
  GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 0;//设为GPIO

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

//数据信号设为输入模式
void SdaInputMode(void)
{
	EALLOW;
	GpioCtrlRegs.GPADIR.bit.GPIO14=0;
	EDIS;
}

//数据信号设为输出模式
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

//时钟信号输出高电平
void IIC_SetSclPin(void)
{
	SCL_H_SMEC98SP;
}

//时钟信号输出低电平
void IIC_ClrSclPin(void)
{
	SCL_L_SMEC98SP;
}

//数据信号输出高电平
void IIC_SetSdaPin(void)
{
	SDA_H_SMEC98SP;
}

//数据信号输出低电平
void IIC_ClrSdaPin(void)
{
	SDA_L_SMEC98SP;
}

//获取数据信号上的输入
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

//时钟信号输出HLow
void IIC_SetScl(unsigned char HLow)
{
	HLow?IIC_SetSclPin():IIC_ClrSclPin();
}

//数据信号输出HLow
void IIC_SetSda(unsigned char HLow)
{
	HLow?IIC_SetSdaPin():IIC_ClrSdaPin();
}

//---------------------------------------------------------
//函数介绍：i2c起始信号产生
//参数说明：
//			void
//返回值说明：
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
//函数介绍：i2c停止信号产生
//参数说明：
//			void
//返回值说明：
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
//函数名: i2c发送一个字节
//参数说明：
//			ucVal : 字节数据
//返回值说明：
//			0 - 成功
//          1 - 失败
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
//函数名: i2c读取一个字节
//参数说明：
//			bACK - 读取一个字节完成后, 设置SDA状态值
//返回值说明：
//			接收到的数据
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
//函数名: 读出1串数据
//参数说明：
//			addr - I2C地址
//			buf	 - 要读取数据缓冲区指针
//			len	 - 待读取长度
//返回值说明：
//			接收到的数据
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
//函数名: 写入1串数据
//参数说明：
//			addr - I2C地址
//			buf	 - 要写入数据缓冲区指针
//			len	 - 待写入长度
//返回值说明：
//			接收到的数据
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
