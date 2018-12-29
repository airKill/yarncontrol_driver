//**************************************************************************//
//	程序名称：直流电机测试程序
//	功能描述：直流电机正转2秒，反转2秒，自动加速正转，自动减速反转，依次循环
//	单片机：  AT89S52，外接12M晶振
//	硬件连接：P1.0----IN1
//			  P1.1----IN2
//            P1.2----ENA
//			  直流电机两端分别接OUT1和OUT2，
//			  电机驱动电压根据所接电机而定,驱动板芯片逻辑电压为+5V
//	维护记录：2012.11.2 双龙电子科技
//**************************************************************************//
#include<reg52.h>
sbit IN1=P1^0;
sbit IN2=P1^1;
sbit ENA=P1^2;
void delay(unsigned int z);
void delay_us(unsigned int aa);
/*******************主函数**************************/
void main()
{
	while(1)
	{
		unsigned int i,cycle=0,T=2048;
		IN1=1;      //正转
		IN2=0;
		for(i=0;i<200;i++)
		{
			delay(10);//PWM占空比为50%，修改延时调整PWM脉冲
			ENA=~ENA;
		}
		IN1=0;      //反转
		IN2=1;
		for(i=0;i<100;i++)
		{
			delay(20);//PWM占空比为50%，修改延时调整PWM脉冲
			ENA=~ENA;
		}
		IN1=1;     //自动加速正转
		IN2=0;
		while(cycle!=T)
		{	ENA=1;
			delay_us(cycle++);
			ENA=0;
			delay_us(T-cycle);
		
		}
		IN1=0;     //自动减速反转
		IN2=1;
		while(cycle!=T)
		{	ENA=1;
			delay_us(cycle++);
			ENA=0;
			delay_us(T-cycle);
		
		}
	}		
}
/******************z秒延时函数*************************/
void delay(unsigned int z)
{
	unsigned int x,y;
	for(x=z;x>0;x--)
		for(y=110;y>0;y--);
}
/****************微妙延时******************************/
void delay_us(unsigned int aa)
{
	while(aa--);	
}


