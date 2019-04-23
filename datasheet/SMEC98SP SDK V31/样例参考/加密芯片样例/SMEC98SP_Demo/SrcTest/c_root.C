//-----------------------------------------------------------------------------
// COPYRIGHT (c) 2001-2008	Sinormous Technology
// File: C_ROOT.C
//-----------------------------------------------------------------------------
// ABSTRACT:
// Test application for SMEC98SP-I2C
//-----------------------------------------------------------------------------
// MODIFICATIONS:
// Author	Date		Description
// 陈文
//如有问题请联系
//QQ：26600347   （加好友注明加密芯片）
 #include <string.h>
 
#include <SMEC98SP_I2C.h>
#include <SMEC98SP_I2C_hard_api.h>
#include <SMEC98SP_API.h>

#include <global.h>
#include <variable.h>
#include <Sha1.h>

	unsigned char code gInternalKey[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};//内部认证密钥,必须和SMEC98SP一致
	unsigned char code gExternalKey[16] = {0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F};//外部认证密钥,必须和SMEC98SP一致
	unsigned char code gSHA1_Key[16] = {0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F};   //哈希算法认证密钥,必须和SMEC98SP一致
	unsigned char code gMKey[16] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F};   //主控密钥,用于产生过程密钥,必须和SMEC98SP一致
	unsigned char code gPin[8] = {0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc};			//Pin认证密钥,必须和SMEC98SP一致

	unsigned char gbSessionKey[8];												//存放过程密钥,过程密钥为临时产生的密钥
	unsigned char gbSessionFlag;													//用于记录是否产生了过程密钥


/****************************************************************************
*   功能：I2C处理函数
*   备注：
*         每一条I2C指令,由I2C Write 及 I2C Read成对组成. 
*         I2C Write 将命令头及数据传入SMEC98SP, 然后由SMEC98SP解析并执行命令, 然后将执行结果存放在I2C_Buf中,等待外部主控用I2C Read读取
*
*		  I2C Write格式: 4字节命令头 + 数据域组成
*		                 4字节命令头(I2C_Buf[0~4]): I2C_INS + I2C_P1 + I2C_P2 + I2C_P3 
*						 I2C_INS: 为命令类型, 如:PIN验证, 读写FLASH, 内外部认证等等
*						 I2C_P1,I2C_P2: 相关命令类型的参数
*						 I2C_P3: I2C Write数据域的长度 或者 期望I2C Read返回结果的数据长度
*
*		  I2C Read格式:  I2C_Buf[0~1]存放执行结果 + 需要返回的数据
*
*   有关I2C通讯的全局变量说明:
*         I2C_Buf: I2C Write/Read 的数据存放起始地址
*         I2C_count_bytes: 若I2C总线命令为write，接收到的数据长度。
*         I2C_send_bytes: 若I2C总线命令为I2C Read，加密芯片需要发送的数据长度。如果外部芯片I2C读超过此长度,则发送最后一个字节
*         I2C_State: 当前I2C总线状态, 0x01 - I2C Write操作, 0x02 - I2C Read操作
*****************************************************************************/
void i2c_CommandProc(void)
{
	dword 	addr;	
	byte 	i;
	byte	bLenP3;
	
	if(I2C_P3 > I2C_MEMSIZE)
	{
		SW=0x6700;
		return;
	}	

	I2C_send_bytes = 2;	//deafult only return 2bytes SW (I2C_Buf[0]+I2C_Buf[1])
	bLenP3 = I2C_P3;	//
	switch (I2C_INS)
	{
		//-----------------------------
		//Pin认证	 CMD = 70 00 00 08 0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc		
		//-----------------------------
		case 0x70:		
			if(I2C_Buf[3] != 0x08)
			{
				I2C_Buf[0] = 0x67;
				I2C_Buf[1] = 0x00;
				return;
			}
			if (memcmp(&I2C_Buf[4],gPin,sizeof(gPin)))
			{
				I2C_Buf[0] = 0x69;
				I2C_Buf[1] = 0x82;
			}
			else
			{
				I2C_Buf[0] = 0x90;			//认证正确
				I2C_Buf[1] = 0x00;
			}
			I2C_send_bytes=2;
			break;

		//-----------------------------
		//哈希算法认证	 CMD = 71 00 00 10 0x00,0x11,..............0xff	(0x00~0xff,16字节为输入的认证数据
		//-----------------------------
		case 0x71:
			SHA1Init( &Sha1_Context );
			SHA1Update(&Sha1_Context, &gSHA1_Key[0], sizeof(gSHA1_Key)); //先将SHA1_Key做一次Hash运算, SHA1_Key不会在I2C线路上传输
			SHA1Update(&Sha1_Context, &I2C_Buf[4], I2C_P3);
			SHA1Final( &I2C_Buf[2], &Sha1_Context);	//运算结果为20个字节，主控单片机通过相同算法得出同样的结果比较一致为通过
			I2C_Buf[0] = 0x90;
			I2C_Buf[1] = 0x00;
			I2C_send_bytes = 20 + 2;
			break;

		//-----------------------------
		//实现一个简单算法，计算圆周长，外部输入的是半径，计算出周长。C = 2*3.14*R	
		//CMD = 72 00 00 01 R
		//R只有一个字节，返回值也是1字节，注意不要超出范围。
		//-----------------------------
		case 0x72:
			I2C_Buf[0] = 0x90;
			I2C_Buf[1] = 0x00;
			I2C_Buf[2] = (2*314*I2C_Buf[4])/100;			//I2C_Buf[4]是R，半径
			I2C_send_bytes = 1 + 2;
			break;

		//-----------------------------
		//实现一个简单算法，计算圆周长，外部输入的是半径，计算出周长。C = 2*3.14*R	
		//CMD = 73 00 00 08 R密文
		//解密后R只有一个字节，返回值加密前也是1字节，注意不要超出范围。
		//利用过程密钥加密传进来后,运算后,再利用过程密钥将结果加密输出
		//-----------------------------
		case 0x73:
			if((gbSessionFlag == 0) || (I2C_P3 != 8))	//如果没有产生过程密钥 或者长度不为8
			{
				I2C_Buf[0] = 0x6A;
				I2C_Buf[1] = 0x86;
				return;
			}
			HW_DES(SIMPLE_DES_DE, gbSessionKey, &I2C_Buf[4], &I2C_Buf[4]);	  //将输入数据用过程密钥解密

			I2C_Buf[2] = (2*314*I2C_Buf[4])/100;			//I2C_Buf[4]是R，半径. 计算结果存放在I2C_Buf[2]
			memset(&I2C_Buf[3], 0x00, 7);					//不相关的数据清成0x00, 或设置成其他数据
			HW_DES(SIMPLE_DES_EN, gbSessionKey, &I2C_Buf[2], &I2C_Buf[2]);		//将计算结果I2C_Buf[2]用过程密钥加密, 
			I2C_Buf[0] = 0x90;
			I2C_Buf[1] = 0x00;
			I2C_send_bytes = bLenP3 + 2;
			break;

		//-----------------------------
		//外部认证	 CMD = 82 00 00 08
		//外部芯片对上次加密芯片产生并送出的随机数用外部认证密钥做3DES加密，并把密文
		//送入加密芯片，加密芯片对送入数据解密，比较解密的数据是否为上次产生的随机数，用于加密芯片验证外部芯片的合法性
		//-----------------------------
		case 0x82:
			if(I2C_P3 != 0x08 || I2C_P1 || I2C_P2)
			{
				I2C_Buf[0] = 0x6A;
				I2C_Buf[1] = 0x86;
				return;
			}

			HW_DES(TRIPLE_DES_DE, gExternalKey, &I2C_Buf[4], &I2C_Buf[2]);
			
			if (memcmp(&I2C_Buf[2],Random_Number,8))
			{
				I2C_Buf[0] = 0x69;
				I2C_Buf[1] = 0x82;
			}
			else
			{
				I2C_Buf[0] = 0x90;
				I2C_Buf[1] = 0x00;
			}

			break;

		//-----------------------------
		//取芯片唯一序列号	 CMD = 83 00 00 P3
		//-----------------------------
		case 0x83:
			if(I2C_P3 != 0x0C || I2C_P1 || I2C_P2)
			{
				I2C_Buf[0] = 0x6A;
				I2C_Buf[1] = 0x86;
				return;
			}
			GetChipID(&I2C_Buf[2]);
			I2C_Buf[0] = 0x90;
			I2C_Buf[1] = 0x00;
			I2C_send_bytes = bLenP3 + 2;
			break;

		//-----------------------------
		//取随机数	 CMD = 84 00 00 P3
		//-----------------------------
		case 0x84:
			if(I2C_P1 || I2C_P2)
			{
				I2C_Buf[0] = 0x6A;
				I2C_Buf[1] = 0x86;
				return;
			}
			Random_Number_Generator_Get(&I2C_Buf[2], bLenP3);
			memcpy(Random_Number, &I2C_Buf[2], 8);
			I2C_Buf[0] = 0x90;
			I2C_Buf[1] = 0x00;
			I2C_send_bytes = bLenP3 + 2;
			break;

		//-----------------------------
		//内部认证	 CMD = 88 00 00 08 D0~D7
		//对外部输入的数据用内部认证密钥做3DES加密，用于外部验证加密芯片的合法性
		//-----------------------------
		case 0x88:
			if(I2C_P3 != 0x08 || I2C_P1 || I2C_P2)
			{
				I2C_Buf[0] = 0x6A;
				I2C_Buf[1] = 0x86;
				return;
			}
			HW_DES(TRIPLE_DES_EN, gInternalKey, &I2C_Buf[4], &I2C_Buf[2]);
			
			I2C_Buf[0] = 0x90;
			I2C_Buf[1] = 0x00;
			I2C_send_bytes = 8 + 2;
			break;

		//-----------------------------
		//产生过程密钥, CMD = A0 00 00 08 D0~D7
		//过程密钥可用于密文读出Flash数据,及相关运算等
		//过程密钥的产生:  gMKey 对 (外部送入的随机数 ^ SMEC98SP随机数)做3DES加密运算
		//-----------------------------
		case 0xA0:
			if(I2C_P3 != 0x08 || I2C_P1 || I2C_P2)
			{
				I2C_Buf[0] = 0x6A;
				I2C_Buf[1] = 0x86;
				return;
			}
			Random_Number_Generator_Get(&Random_Number[0], 8);
			for(i = 0; i < 8; i++)
			{
			 	I2C_Buf[4 + i] ^= Random_Number[i];
			}
			HW_DES(TRIPLE_DES_EN, gMKey, &I2C_Buf[4], &gbSessionKey[0]);	//将过程密钥保存在gbSessionKey
			memcpy(&I2C_Buf[2], Random_Number, 8);		 					//将SMEC98SP产生的密钥送出给外部主控芯片
			gbSessionFlag = 1;												//置已经产生了过程密钥标志
			I2C_Buf[0] = 0x90;
			I2C_Buf[1] = 0x00;
			I2C_send_bytes = 8 + 2;
			break;

		//-----------------------------
		//端口数据运算, CMD = A2 00 00 08 D0~D7
		//将传入数据用过程密钥解密后,作运算(取反),然后再用过程密钥加密送出给外部主控芯片
		//-----------------------------
		case 0xA2:
			if((gbSessionFlag == 0) || (I2C_P3 != 0x08) || I2C_P1 || I2C_P2)
			{
				I2C_Buf[0] = 0x6A;
				I2C_Buf[1] = 0x86;
				return;
			}
			HW_DES(SIMPLE_DES_DE, gbSessionKey, &I2C_Buf[4], &I2C_Buf[2]);	//将输入数据解密
			for(i = 0; i < 8; i ++)
			{
			 	I2C_Buf[2 + i] ^= 0xFF;					//解密后数据取反, 后进行加密
			}
			HW_DES(SIMPLE_DES_EN, gbSessionKey, &I2C_Buf[2], &I2C_Buf[2]);	//加密送出
			I2C_Buf[0] = 0x90;
			I2C_Buf[1] = 0x00;
			I2C_send_bytes = 8 + 2;
			break;

		//-----------------------------
		//读取FLASH	 CMD = B0 P1 P2 P3
		//-----------------------------
		case 0xB0:
			addr = ((I2C_P1 << 8) + I2C_P2);
			if(FlashReadNBytes(bLenP3, &I2C_Buf[2], addr) != SUCCESSFULL)
			{
				I2C_Buf[0] = 0x65;
				I2C_Buf[1] = 0x81;
			}
			else
			{
				I2C_Buf[0] = 0x90;
				I2C_Buf[1] = 0x00;
				I2C_send_bytes = bLenP3 + 2;
			}
			break;

		//-----------------------------
		//密文读数据	CMD = B1 00 00 08 data0~data7
		//读取长度必须为8的整数倍,读取结果由过程密钥gbSessionKey做DES加密后送出
		//-----------------------------
		case 0xB1:
			if((gbSessionFlag == 0) || (I2C_P3 % 8))	//如果没有产生过程密钥 或者长度不为8的整数倍
			{
				I2C_Buf[0] = 0x6A;
				I2C_Buf[1] = 0x86;
				return;
			}

			addr = ((I2C_P1 << 8) + I2C_P2);
			if(FlashReadNBytes(bLenP3, &I2C_Buf[2], addr) != SUCCESSFULL)
			{
				I2C_Buf[0] = 0x65;
				I2C_Buf[1] = 0x81;
				return;
			}
			
			for(i = 0; i < bLenP3; i += 8)
			{
				HW_DES(SIMPLE_DES_EN, gbSessionKey, &I2C_Buf[i + 2], &I2C_Buf[i + 2]);
			}

			I2C_Buf[0] = 0x90;
			I2C_Buf[1] = 0x00;
			I2C_send_bytes = bLenP3 + 2;
		break;		
			
		//-----------------------------
		//擦FLASH	CMD = D0 P1 P2 00
		//-----------------------------
		case 0xD0:	
			addr = ((I2C_P1 << 8) + I2C_P2);
			if(FlashErasePage(addr) != SUCCESSFULL)
			{
				I2C_Buf[0] = 0x65;
				I2C_Buf[1] = 0x81;
			}
			else
			{
				I2C_Buf[0] = 0x90;
				I2C_Buf[1] = 0x00;
			}
			break;
		
		//-----------------------------
		//写FLASH	 CMD = D6 P1 P2 P3 data0~dataN
		//-----------------------------
		case 0xD6:	
			addr = ((I2C_P1 << 8) + I2C_P2);

			if(FlashWriteNBytes(addr, &I2C_Buf[4], I2C_P3) != SUCCESSFULL)
			{
				I2C_Buf[0] = 0x65;
				I2C_Buf[1] = 0x81;
			}
			else
			{
				I2C_Buf[0] = 0x90;
				I2C_Buf[1] = 0x00;
			}
			break;

		//-----------------------------
		// 取demo相关信息 CMD = CA 00 00 01
		//-----------------------------
		case 0xCA:	
			if (I2C_P3!=0x01 || I2C_P1 || I2C_P2)
			{
				I2C_Buf[0] = 0x6D;
				I2C_Buf[1] = 0x00;
				break;
			}
			I2C_Buf[0]=0x90;
			I2C_Buf[1]=0x00;		
			I2C_Buf[2]=DEMO_VERSION;
			I2C_send_bytes = 0x03;			
			break;

		//-----------------------------
		//重新进入Bootloader	 D8 00 00 08 11 11 11 11 11 11 11 11
		//-----------------------------
		case 0xD8:	
			if(I2C_P3 != 8)
			{
				I2C_Buf[0] = 0x67;
				I2C_Buf[1] = 0x00;
				return;
			}
			if(memcmp(&I2C_Buf[4], "\x11\x11\x11\x11\x11\x11\x11\x11", 8))
			{
				I2C_Buf[0] = 0x6A;
				I2C_Buf[1] = 0x80;
				break;
			}

			if(ReworkEntry() != SUCCESSFULL)
			{
				I2C_Buf[0] = 0x65;
				I2C_Buf[1] = 0x81;
			}
			else
			{
				I2C_Buf[0] = 0x90;
				I2C_Buf[1] = 0x00;
			}
			break;

		default:
			SW = 0x6E00;
			break;
	}

}

///////////////////////////////////////////////////////////////////////////////
// MAIN function
///////////////////////////////////////////////////////////////////////////////
void main(void) 
{
	//IC initialisation
	Set_Frequency(F_OSC_30M);	//Set device frequency at 30MHz
	
	gbSessionFlag = 0;
	
	I2CPAD = 0x00;
	I2CADR = 0x00;//可配置成自己需要的地址(7 bit)

	 while(1)
	 {
		 i2c_xfer();
		 if(I2C_State == 0x01)	//I2C write command
		 {
		  	i2c_CommandProc();
		 }
	 }	
}




