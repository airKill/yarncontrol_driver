#include <stc12c5a60s2.H>
#include "string.h"
#include <stdlib.h>
#include <SMEC98SP.h>

void StartUART( void );
void Starttimer0(void);
void delay_nms(unsigned int i);
void print_string(char *str);
void PrintHex(unsigned char *str,unsigned int len);


void SMEC_Test(void)
{
	/*各种密钥,不会在I2C线路上传输,可以使用同一组.应该将密钥分散存储,防止主控芯片被破解后,被攻击者在二进制码中找到密钥 */
	unsigned char InternalKey[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};//内部认证密钥,必须和SMEC98SP一致
	unsigned char ExternalKey[16] = {0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F};//外部认证密钥,必须和SMEC98SP一致
	unsigned char SHA1_Key[16] = {0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F};   //哈希算法认证密钥,必须和SMEC98SP一致
	unsigned char MKey[16] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F};   //主控密钥,用于产生过程密钥,必须和SMEC98SP一致

	unsigned char Pin[8] = {0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc};			//Pin认证密钥,必须和SMEC98SP一致
		
	unsigned char bSmec98spUid[12] = {0};		//存放SMEC98SP的UID
	unsigned short RandomSeek = 0;				//随机数种子
	unsigned char bRandom[8] = {0};				//存放随机数
	unsigned char bSessionKey[8] = {0};			//存放过程密钥,过程密钥为临时产生的密钥
	unsigned char bDataBuf[64] = {0};
	unsigned char ret, bLen;
	unsigned short i, j;

	/*获取SMEC98SP的UID*/
	ret = SMEC_GetUid(bSmec98spUid);
	if(ret)
	{
		print_string("SMEC_GetUid -> Error !\r\n");
		while(1);
	}
	print_string("SMEC_GetUid: ");
    PrintHex(bSmec98spUid, 12);
	print_string("\r\n");

	/*取加密芯片的随机数，作为MCU的RandomSeek因子,也可以用其他外部因素，如按键等待时间等*/
	if(SMEC_GetRandom(bDataBuf))//获取8字节随机数
	{
		print_string("SMEC_GetRandom -> Error !\r\n");
		while(1);
	}
	for(i = 0; i < 4; i += 2)
	{
		/*使RandomSeek与加密芯片随机数有关*/
		j = (bDataBuf[i] << 8) + bDataBuf[i + 1];
		RandomSeek ^= j;
	}
	for(i = 0; i < 6; i += 2)
	{
		/*使RandomSeek与SMEC98SP的UID相关,使得即使相同状况下,不同的加密芯片,RandomSeek值也不一样*/
		j = (bSmec98spUid[i] << 8) + bSmec98spUid[i + 1];
		RandomSeek ^= j;
	}
	srand(RandomSeek);
	print_string("RandomSeek:");
   	PrintHex((unsigned char *)&RandomSeek, 2);


	/*PIN码验证*/
	ret = SMEC_CheckPin(Pin, (unsigned char)sizeof(Pin));
	if(ret)
	{
		print_string("SMEC_CheckPin -> Error !\r\n");
		while(1);
	}
	print_string("SMEC_CheckPin OK !\r\n");

	/*内部认证, 主控芯片对SMEC98SP加密芯片合法性判断*/
	for(i = 0; i < 8; i ++)
	{
		bRandom[i] = (unsigned char) rand();
	}
	ret = SMEC_IntrAuth(InternalKey, bRandom);
	if(ret)
	{
		print_string("SMEC_IntrAuth -> Error !\r\n");
		while(1);
	}
	print_string("SMEC_IntrAuth OK !\r\n");

	/*外部认证, SMEC98SP加密芯片对主控芯片合法性判断*/
	ret = SMEC_ExtrAuth(ExternalKey);
	if(ret)
	{
		print_string("SMEC_ExtrAuth -> Error !\r\n");
		while(1);
	}
	print_string("SMEC_ExtrAuth OK !\r\n");

	/*SHA1摘要算法认证, 数据长度可自己设定*/
	for(i = 0; i < 16; i ++)
	{
		bDataBuf[i] = (unsigned char) rand();
	}
	ret = SMEC_Sha1Auth(SHA1_Key, (unsigned char)sizeof(SHA1_Key), bDataBuf, 16);
	if(ret)
	{
		print_string("SMEC_Sha1Auth -> Error !\r\n");
		while(1);
	}
	print_string("SMEC_Sha1Auth OK !\r\n");

	/*调用加密芯片内部计算圆周长算法*/
	bDataBuf[0] = 0x02;
	ret = SMEC_CircleAlg(bDataBuf, 1, bDataBuf, &bLen);
	if(ret)
	{
		print_string("SMEC_CircleAlg -> Error !\r\n");
		while(1);
	}
	print_string("SMEC_CircleAlg OK, C = ");
	PrintHex(bDataBuf, 1);

	/*产生过程密钥,用于后续的Flash数据加密读,及构造的"端口数据运算"*/
	for(i = 0; i < 8; i ++)
	{
		bRandom[i] = (unsigned char) rand();
	}
	ret = SMEC_GenSessionKey(MKey, bRandom, bSessionKey);
	if(ret)
	{
		print_string("SMEC_GenSessionKey -> Error !\r\n");
		while(1);
	}
	print_string("SMEC_GenSessionKey OK !\r\n");

	/*密文读取Flash数据*/
	ret = SMEC_CryptReadFlash(bSessionKey, 0x0000, bDataBuf, 16);
	if(ret)
	{
		print_string("SMEC_CryptReadFlash -> Error !\r\n");
		while(1);
	}
	print_string("SMEC_CryptReadFlash OK:\r\n");
    PrintHex(bDataBuf, 16);

	/*读取Flash数据*/
	ret = SMEC_ReadFlash(0x0000, bDataBuf, 16);
	if(ret)
	{
		print_string("SMEC_ReadFlash -> Error !\r\n");
		while(1);
	}
	print_string("SMEC_ReadFlash OK:\r\n");
    PrintHex(bDataBuf, 16);

	/*写Flash数据*/
	for(i = 0; i < 16; i ++)
	{
		bDataBuf[i] = (unsigned char) i;
	}
	ret = SMEC_WriteFlash(0x0000, bDataBuf, 16);
	if(ret)
	{
		print_string("SMEC_WriteFlash -> Error !\r\n");
		while(1);
	}
 	print_string("SMEC_WriteFlash OK!\r\n");

	/*构造"端口数据运算", 可以用实际的PA~PC端口数据*/
	bDataBuf[0] = 0x00;
	bDataBuf[1] = 0x00;
	ret = SMEC_GpioAlg(bSessionKey, bDataBuf,2, bDataBuf);
	if(ret)
	{
		print_string("SMEC_GpioAlg -> Error !\r\n");
		while(1);
	}
 	print_string("SMEC_GpioAlg OK:\r\n");
    PrintHex(bDataBuf, 2);

	/*调用加密芯片内部计算圆周长算法,并密文在线路上传输*/
	bDataBuf[0] = 0x02;
	ret = SMEC_CircleAlgCrypt(bSessionKey, bDataBuf, 1, bDataBuf, &bLen);
	if(ret)
	{
		print_string("SMEC_CircleAlgCrypt -> Error !\r\n");
		while(1);
	}
	print_string("SMEC_CircleAlgCrypt OK, C = ");
	PrintHex(bDataBuf, 1);
}


//////////////////////////////////////////////////////////////////////
void main(void)
{
	P4SW = 0x30;
	AUXR1 = 0x00;
//	Beep();

//	ET0 = 0;
//	TR0	= 1;				//使用定时器0,作为随机数因子

	StartUART();
	EA = 1;	

	delay_nms(20);			//等待加密芯片上电复位完成

	SMEC_Test(); 			//加密芯片功能演示

	while(1);
}

//22.1184M  1T   
void delay_nms(unsigned int i)
{
	unsigned int  j;	
	
	for(;i>0;i--)
	{
	   	j = 1580;
		while(--j);	
			
	}
}



//************************************串口初始化*********************************************************
void StartUART( void )
{  							//波特率115200 TH1 = 0xFf; 57600 fe; 38400 fd;19200 fa; 9600 f4
     SCON = 0x50;
     TMOD = 0x21;
     TH1 = 0xff;
     TL1 = 0xff;
     PCON = 0x80;
     TR1 = 1;
     ES = 1;
}


void serial(void) interrupt 4 using 3 
{	
    if(RI)
    {
        RI = 0;
       
		return;
	}
	if(TI)
	{
		TI = 0;			
 	}
}


void R_S_Byte(unsigned char R_Byte)
{	
	 SBUF = R_Byte;  
     while( TI == 0 );				//查询法
  	 TI = 0;    
}


void print_string(char *str)
{
	
	unsigned int i;
	for(i=0; *str != 0; i++)
	{ 
		R_S_Byte(*str);        	
		str++;      
	}
}

void PrintHex(unsigned char *str,unsigned int len)
{
	unsigned int i;
	unsigned char a1,a2;
	for(i=0;i<len;i++)
	{ 
		a1=str[i]>>4;
		if(a1<10)a1+='0';
		else a1=a1-10+'A';		
		a2=str[i]&0x0f;		
		if(a2<10)a2+='0';
		else a2=a2-10+'A';		
		R_S_Byte(a1);
		R_S_Byte(a2);       
	}
	R_S_Byte('\r');
	R_S_Byte('\n');
	
}