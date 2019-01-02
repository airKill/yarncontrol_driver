#include "main.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//IIC���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/9
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//��ʼ��IIC
void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//ʹ��GPIOBʱ��
  
	GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = IIC_SCL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IIC_SCL_PORT, &GPIO_InitStructure);
  
	GPIO_SetBits(IIC_SDA_PORT,IIC_SDA_PIN); 	//PB6,PB7 �����
  GPIO_SetBits(IIC_SCL_PORT,IIC_SCL_PIN); 	//PB6,PB7 �����
}

void SDA_OUT(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PORTB��ʱ�� 
	
  GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;				//PORTG.11 �������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure);
}

void SDA_IN(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PORTB��ʱ�� 
	
  GPIO_InitStructure.GPIO_Pin = IIC_SCL_PIN;				//PORTG.11 �������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(IIC_SCL_PORT, &GPIO_InitStructure);
}

void IIC_delayus(u32 us)
{
  u32 i;
  for(i=0;i<us;i++)
  {
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
  }
}

//����IIC��ʼ�ź�
void IIC_EEP_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA_H();	  	  
	IIC_SCL_H();
  IIC_delayus(4);
 	IIC_SDA_L();//START:when CLK is high,DATA change form high to low 
  IIC_delayus(4);
	IIC_SCL_L();//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_EEP_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL_L();
	IIC_SDA_L();//STOP:when CLK is high DATA change form low to high
  IIC_delayus(4);
	IIC_SCL_H(); 
	IIC_SDA_H();//����I2C���߽����ź�
  IIC_delayus(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_EEP_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	IIC_SDA_H();
  IIC_delayus(1);	   
	IIC_SCL_H();
  IIC_delayus(1);	 
	while(IIC_READ_SDA())
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_EEP_Stop();
			return 1;
		}
	}
	IIC_SCL_L();//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void IIC_Ack(void)
{
	IIC_SCL_L();
	SDA_OUT();
	IIC_SDA_L();
  IIC_delayus(2);
	IIC_SCL_H();
  IIC_delayus(2);
	IIC_SCL_L();
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	IIC_SCL_L();
	SDA_OUT();
	IIC_SDA_H();
  IIC_delayus(2);
	IIC_SCL_H();
  IIC_delayus(2);
	IIC_SCL_L();
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(u8 txd)
{                        
  u8 t;   
	SDA_OUT(); 	    
  IIC_SCL_L();//����ʱ�ӿ�ʼ���ݴ���
  for(t=0;t<8;t++)
  {              
    //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA_H();
		else
			IIC_SDA_L();
		txd<<=1; 	  
    IIC_delayus(2);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL_H();
    IIC_delayus(2); 
		IIC_SCL_L();	
    IIC_delayus(2);
  }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
  for(i=0;i<8;i++ )
	{
    IIC_SCL_L(); 
    IIC_delayus(2);
		IIC_SCL_H();
    receive<<=1;
    if(IIC_READ_SDA())
      receive++;   
    IIC_delayus(1); 
  }					 
  if (!ack)
    IIC_NAck();//����nACK
  else
    IIC_Ack(); //����ACK   
  return receive;
}



























