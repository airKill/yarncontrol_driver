//******************************************************************************************//
//	程序名称：28BYJ-48步进电机测试程序
//	功能描述：步进电机分别以单四拍、双四拍、八拍驱动方式驱动，正反转各360度
//	单片机：  AT89S52，FOSC=12MHz
//	硬件连接：P1.0----INA
//            P1.1----INB
//            P1.2----INC
//            P1.3----IND 
//            驱动板的驱动电压和芯片逻辑供电电压均为5V;	 
//            步进电机红线接+5V驱动电压，橙黄粉蓝分别接OUT1、OUT2、OUT3、OUT4，
//            注意外接电源要与单片机共地。
//	维护记录：2012.11.2 双龙电子科技
//*****************************************************************************************//
#include<reg52.h>
#define uint unsigned int
#define uchar unsigned char
uint i,j,k;
uint N=128;//主轴旋转度数设置，度数D=N*45/减速比，采用1:16减速比的步进电机，旋转度数即为D=128*45/16=360度

uchar code single_pos[4]={0x07,0x0b,0x0d,0x0e};//单四拍驱动方式正转表 D-C-B-A
uchar code single_rev[4]={0x0e,0x0d,0x0b,0x07};//单四拍驱动方式反转表 A-B-C-D
uchar code double_pos[4]={0x06,0x03,0x09,0x0c};//双四拍驱动方式正转表 AD-DC-CB-BA
uchar code double_rev[4]={0x0c,0x09,0x03,0x06};//双四拍驱动方式反转表 AB-BC-CD-DA
uchar code eight_pos[8]={0x06,0x07,0x03,0x0b,0x09,0x0d,0x0c,0x0e};//八拍驱动方式正转表 AD-D-DC-C-CB-B-BA-A
uchar code eight_rev[8]={0x0e,0x0c,0x0d,0x09,0x0b,0x03,0x07,0x06};//八拍驱动方式反转表 A-AB-B-BC-C-CD-D-DA

void delay(uint z);
void m_single_pos();
void m_single_rev();
void m_double_pos();
void m_double_rev();
void m_eight_pos();
void m_eight_rev();
void main()			
{	
	while(1)
	{
		m_single_pos();//单四拍驱动方式正转360度
		delay(200);
		m_single_rev();//单四拍驱动方式反转360度
		delay(200);
		m_double_pos();//双四拍驱动方式正转360度
		delay(200);
		m_double_rev();//双四拍驱动方式反转360度
		delay(200);
		m_eight_pos();//八拍驱动方式正转360度
		delay(200);
		m_eight_rev();//八拍驱动方式反转360度	
		delay(200);
	}		
}
/********************延时z毫秒**************************************/
void delay(uint z)
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=110;y>0;y--);
}
/*******************单四拍驱动正转(N*45/16)度***********************/
void m_single_pos()
{
	for(k=0;k<N;k++)
	{
		j=0;
		for(i=0;i<4;i++)//在单四拍工作方式下，一个脉冲转子转动角度为5.625*2=11.25度，四拍共45度
		{
			P1=single_pos[j];
			delay(5);//适当延时，保证转子转动时间，延时过短会丢拍
			j++;
		}
	}
}

/*******************单四拍驱动反转(N*45/16度)***********************/
void m_single_rev()
{
	for(k=0;k<N;k++)
	{
		j=0;
		for(i=0;i<4;i++)//在单四拍工作方式下，一个脉冲转子转动角度为5.625*2=11.25度，四拍共45度
		{
			P1=single_rev[j];
			delay(5);
			j++;
		}
	}
}

/*******************双四拍驱动正转(N*45/16)度***********************/
void m_double_pos()
{
	for(k=0;k<N;k++)
	{
		j=0;
		for(i=0;i<4;i++)//在双四拍工作方式下，一个脉冲转子转动角度为5.625*2=11.25度，四拍共45度
		{
			P1=double_pos[j];
			delay(8);
			j++;
		}
	}
}
/*******************双四拍驱动反转(N*45/16)度***********************/
void m_double_rev()
{
	for(k=0;k<N;k++)
	{
		j=0;
		for(i=0;i<4;i++)//在双四拍工作方式下，一个脉冲转子转动角度为5.625*2=11.25度，四拍共45度
		{
			P1=double_rev[j];
			delay(8);
			j++;
		}
	}
}
/*******************八拍驱动正转(N*45/16)度***********************/
void m_eight_pos()
{
	for(k=0;k<N;k++)
	{
		j=0;
		for(i=0;i<8;i++)//在八拍工作方式下，一个脉冲转子转动角度为5.625度，八拍共45度
		{
			P1=eight_pos[j];
			delay(2);
			j++;
		}
	}
}
/*******************八拍驱动反转(N*45/16)度***********************/
void m_eight_rev()
{
	for(k=0;k<N;k++)
	{
		j=0;
		for(i=0;i<8;i++)//在八拍工作方式下，一个脉冲转子转动角度为5.625度，八拍共45度
		{
			P1=eight_rev[j];
			delay(2);
			j++;
		}
	}
}

