#include "arduino.h"

#define    IIC_SDA     52			//定义IIC SDA管脚，
#define    IIC_SCL     53			//定义IIC SCL管脚

#define IIC_NOACK 1
#define IIC_ACK 0
#define IIC_DELAYTIME 1			//IIC通讯速率大约50K

//===========================      I2C通讯相关      ===========================//
//---------------------------------------------------------
//函数介绍：模拟端口基本配置初始化
//参数说明：
//			void
//返回值说明：
//			void
//---------------------------------------------------------
void SMEC_I2cInit(void)
{
  pinMode(IIC_SDA,OUTPUT);
  pinMode(IIC_SCL,OUTPUT);
}
/******************************************************************************************
/*延时us函数22.1184M  1T
/******************************************************************************************/
void IIC_Delayus(unsigned int n)
{
  delayMicroseconds(n);	
}
void    IIC_SetSDA(unsigned char level)
{
  digitalWrite(IIC_SDA,level);
}
void    IIC_SetSCL(unsigned char level)
{
  digitalWrite(IIC_SCL,level);
}
void SetSDAto_InputMode(void)
{
  pinMode(IIC_SDA,INPUT);
}
void SetSDAto_OutputMode(void)
{
  pinMode(IIC_SDA,OUTPUT);
}

void   IIC_Start(void)
{
  SetSDAto_OutputMode();			//SDA设为输出模式
  IIC_SetSDA(HIGH);
  IIC_SetSCL(HIGH);
  IIC_Delayus(IIC_DELAYTIME);			
  IIC_SetSDA(LOW);
  IIC_Delayus(IIC_DELAYTIME);
  IIC_SetSCL(LOW);
}

void   IIC_Stop(void)
{	
  SetSDAto_OutputMode();			//SDA设为输出模式
  IIC_SetSDA(LOW);
  IIC_SetSCL(HIGH);
  IIC_Delayus(IIC_DELAYTIME);
  IIC_SetSDA(HIGH);
  IIC_Delayus(IIC_DELAYTIME);
}
unsigned char IIC_GetSdaStus(void)
{
  return digitalRead(IIC_SDA);
}

#define ACKCHECKTIME 600
unsigned char  IIC_Send(unsigned char IIC_data)
{
  unsigned char Ack;
  unsigned int AckLoop;
  unsigned char i;
  SetSDAto_OutputMode();			//SDA设为输出模式
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
  SetSDAto_InputMode();			//SDA设为输入模式
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
  SetSDAto_InputMode();			//SDA设为输入模式
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
  SetSDAto_OutputMode();			//SDA设为输出模式	
  IIC_Delayus(IIC_DELAYTIME);   
  IIC_SetSDA(bACK);
  IIC_Delayus(IIC_DELAYTIME);
  IIC_SetSCL(HIGH);
  IIC_Delayus(IIC_DELAYTIME);
  IIC_Delayus(IIC_DELAYTIME);
  IIC_SetSCL(LOW);
  return Data;
}


unsigned char IIC_ReadWithAddr(unsigned char addr, unsigned char *buf, unsigned char len)
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

unsigned char IIC_WriteWithAddr(unsigned char addr, unsigned char *buf, unsigned char len)
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

