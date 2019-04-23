//COPYRIGHT (c) 2018-2025	Sinormous Technology
//深圳市中巨伟业信息科技有限公司
//www.sinormous.com;
//Author:顾万水
//QQ:47583353

#include <string.h>
#include "stm32f10x.h"
#include "smec98sp.h"
#include "iic_smec98sp.h"
#include "des.h"			 //如调用到DES运算，需加载
#include "sha1.h"			 //如调用到SHA1运算，需加载




//===========================      加密芯片 SMEC98SP 接口  ===========================//
/*        加密芯片内部程序可自行编写及修改, 以下为根据加密芯片样例程序编写           */
/*
 	1.获取SMEC98SP的UID号
 	2.产生加密芯片的随机数
 	3.验证PIN
 	4.内部认证
 	5.外部认证
 	6.SHA1哈希算法认证
 	7.关键算法放在加密芯片内
 	8.构造算法
 	9.密文读数据
 	10.读数据
 	11.写数据
*/

#define IIC_ADDR	0x00			//对应SMEC98SP中地址

//---------------------------------------------------------
//函数名: 获取加密芯片SMEC98SP的UID
//参数说明：
//			pUID - UID存放指针
//返回值说明：
//			0 - 成功
//			1 - 失败
//---------------------------------------------------------
unsigned char SMEC_GetUid(unsigned char * pUID)
{
    unsigned char bBuf[128] = {0};
    unsigned char ret;
    unsigned char  len;

    bBuf[0] = 0x83;		        //读UID命令字，可更改，与加密芯片中程序保持一致
    bBuf[1] = 0x00;
    bBuf[2] = 0x00;
    bBuf[3] = len = 0x0c;		//需要获取的UID长度

    ret = IIC_WriteWithAddr(IIC_ADDR & 0xFE, bBuf, 4);
    if(ret)                     //出错
        return 1;

    //说明：若正确获取UID, 加密芯片返回数据90 00 + UID1 ~ UID12．　前2字节为状态码, 0x9000 表示执行正确, 其他表示错误
    ret = IIC_ReadWithAddr(IIC_ADDR | 0x1, bBuf, (len + 2));
    if(ret || bBuf[0] != 0x90 || bBuf[1] != 0x00)         //通信错误或状态码不为0x9000
        return 1;

    memcpy(pUID, bBuf + 2, len);
    return 0;		            //读取成功
}

//---------------------------------------------------------
//函数名: 获取加密芯片SMEC98SP的随机数
//参数说明：
//			pRandom - 随机数存放指针
//返回值说明：
//			0 - 成功
//			1 - 失败
//---------------------------------------------------------
unsigned char SMEC_GetRandom(unsigned char *pRandom)
{
    unsigned char bBuf[128] = {0};
    unsigned char  ret;
    unsigned char  len;

    bBuf[0] = 0x84;		        //可更改，与加密芯片中程序保持一致
    bBuf[1] = 0x00;
    bBuf[2] = 0x00;
    bBuf[3] = len = 0x08;	    //需要获取的随机数长度

    ret = IIC_WriteWithAddr(IIC_ADDR & 0xFE, bBuf, 4);
    if(ret)                     //出错
        return 1;

    //若正确获取随机数, 加密芯片返回数据90 00 + random1 ~ random8．前2字节为状态码, 0x9000 表示执行正确, 其他表示错误
    ret = IIC_ReadWithAddr(IIC_ADDR | 0x1, bBuf, (len+2));
    if(ret || bBuf[0] != 0x90 || bBuf[1] != 0x00)         //通信错误或状态码不为0x9000
        return 1;

    memcpy(pRandom, bBuf + 2, len);
    return 0;		            //成功
}

//---------------------------------------------------------
//函数名: 验证加密芯片SMEC98SP的PIN码
//参数说明：
//			pbPin - PIN码
//          bPinLen - PIN长度
//返回值说明：
//			0 - 成功
//			1 - 失败
//---------------------------------------------------------
unsigned char SMEC_CheckPin(unsigned char *pbPin, unsigned char bPinLen)
{
    unsigned char bBuf[128] = {0};
    unsigned char  ret;

    bBuf[0] = 0x70;		        //可更改，与加密芯片中程序保持一致
    bBuf[1] = 0x00;
    bBuf[2] = 0x00;
    bBuf[3] = bPinLen;	    //需要获取的随机数长度
    memcpy(&bBuf[4], pbPin, bPinLen);

    ret = IIC_WriteWithAddr(IIC_ADDR & 0xFE, bBuf, bPinLen + 4);
    if(ret)                     //出错
        return 1;

    //若正确验证PIN 加密芯片返回 90 00
    ret = IIC_ReadWithAddr(IIC_ADDR | 0x1, bBuf, 2);
    if(ret || bBuf[0] != 0x90 || bBuf[1] != 0x00)         //通信错误或状态码不为0x9000
        return 1;

    return 0;		            //验证PIN成功
}

//---------------------------------------------------------
//函数名: SHA1认证
//参数说明：
//			pbSha1Key  	- SHA1认证密钥，实际为做SHA1运算的前置数据, 在主控芯片及SMEC98SP内部存在, 不用在I2C线路上传输
//			bSha1KeyLen - SHA1认证密钥长度
//			pbRandom    - SHA1运算的数据
//			bRandomLen  - 运算数据长度
//返回值说明：
//			0 - 成功
//			1 - 失败
//说明:主控芯片先对pbSha1Key做HASH(摘要)运算,然后再对pbRandom继续做HASH运算. 将其结果与SMEC98SP返回值作比较.
//      pbSha1Key分别放于主控芯片及SMEC98SP中,无需在线路上传输.这样如果主控芯片与加密芯片的pbSha1Key不一致, 则无法验证成功
//---------------------------------------------------------
unsigned char SMEC_Sha1Auth(unsigned char *pbSha1Key, unsigned char bSha1KeyLen, unsigned char *pbRandom, unsigned char bRandomLen)
{
    unsigned char bBuf[128] = {0};
    unsigned char ret;
    unsigned char bHashData1[20] = {0}, bHashData2[20] = {0};

    bBuf[0] = 0x71;		        //可更改，与加密芯片中程序保持一致
    bBuf[1] = 0x00;
    bBuf[2] = 0x00;
    bBuf[3] = bRandomLen;  		//8字节随机数长度
    memcpy(&bBuf[4], pbRandom, bRandomLen);

    ret = IIC_WriteWithAddr(IIC_ADDR & 0xFE, bBuf, bRandomLen + 4);
    if(ret)                     //出错
        return 1;

    //若执行正确，返回状态码90 00 + 20字节HASH结果
    ret = IIC_ReadWithAddr(IIC_ADDR | 0x1, bBuf, 20 + 2);
    if(ret || bBuf[0] != 0x90 || bBuf[1] != 0x00)         //通信错误或状态码不为0x9000
        return 1;
    memcpy(bHashData1, bBuf + 2, 20);

    SHA1Init( &Sha1_Context );
    SHA1Update(&Sha1_Context, pbSha1Key, bSha1KeyLen);		//先把pbSha1Key进行运算
    SHA1Update(&Sha1_Context, pbRandom, bRandomLen);
    SHA1Final(bHashData2, &Sha1_Context);					//算出结果

    if(memcmp(bHashData1, bHashData2, 20))					//如果主控芯片与加密芯片结果不一致,则认证失败
        return 1;
    return 0;
}

//---------------------------------------------------------
//函数名: 内部认证
//参数说明：
//			pbKey - 内部认证的密钥
//          pbRandom - 内部认证所用随机数
//返回值说明：
//			0 - 成功
//			1 - 失败
//说明:主控芯片向SMEC98SP发送8字节随机数, SMEC98SP将随机数进行3DES加密后送给主控芯片, 由主控芯片判断回送数据的合法性
//---------------------------------------------------------
unsigned char SMEC_IntrAuth(unsigned char *pbKey, unsigned char *pbRandom)
{
    unsigned char bBuf[128] = {0};
    unsigned char ret;
    unsigned char bTmp[8] = {0};

    bBuf[0] = 0x88;		        //可更改，与加密芯片中程序保持一致
    bBuf[1] = 0x00;
    bBuf[2] = 0x00;
    bBuf[3] = 8;	    		//8字节随机数长度
    memcpy(&bBuf[4], pbRandom, 8);

    ret = IIC_WriteWithAddr(IIC_ADDR & 0xFE, bBuf, 8 + 4);
    if(ret)                     //出错
        return 1;

    //执行正确，返回90 00+随机数密文数据
    ret = IIC_ReadWithAddr(IIC_ADDR | 0x1, bBuf, 8+2);
    if(ret || bBuf[0] != 0x90 || bBuf[1] != 0x00)         //通信错误或状态码不为0x9000
        return 1;

    TripleDES(pbRandom, pbKey, bTmp);
    if(memcmp(bBuf + 2, bTmp, 8))//比对返回结果，看是否与预期的一致
        return 1;

    return 0;		            //内部认证正确
}

//---------------------------------------------------------
//函数名: 外部认证
//参数说明：
//			pbKey - 外部认证密钥
//返回值说明：
//			0 - 成功
//			1 - 失败
//说明:主控芯片先获取SMEC98SP的8字节随机数, 然后用bKey对随机数做3DES加密，再将密文送给SMEC98SP
//---------------------------------------------------------
unsigned char SMEC_ExtrAuth(unsigned char *pbKey)
{
    unsigned char bBuf[128] = {0};
    unsigned char ret;
    unsigned char bCryptData[8] = {0}, bRandom[8] = {0};

    if(SMEC_GetRandom(bRandom))//获取8字节随机数
        return 1;
    TripleDES(bRandom, pbKey, bCryptData);	//将随机数做3DES加密

    bBuf[0] = 0x82;		        //可更改，与加密芯片中程序保持一致
    bBuf[1] = 0x00;
    bBuf[2] = 0x00;
    bBuf[3] = 8;	    		//8字节随机数长度
    memcpy(&bBuf[4], bCryptData, 8);

    ret = IIC_WriteWithAddr(IIC_ADDR & 0xFE, bBuf, 8 + 4);
    if(ret)                     //出错
        return 1;

    //若执行正确，返回状态码90 00
    ret = IIC_ReadWithAddr(IIC_ADDR | 0x1, bBuf, 2);
    if(ret || bBuf[0] != 0x90 || bBuf[1] != 0x00)         //通信错误或状态码不为0x9000
        return 1;

    return 0;		            //外部认证正确
}

//---------------------------------------------------------
//函数名: 读Flash
//参数说明：
//			pbData - 读取数据存放地址
//			bLen  -  读取的数据长度
//返回值说明：
//			0 - 成功
//			1 - 失败
//说明:
//---------------------------------------------------------
unsigned char SMEC_ReadFlash(unsigned short wAddr, unsigned char *pbData, unsigned char bLen)
{
    unsigned char bBuf[128] = {0};
    unsigned char ret;

    bBuf[0] = 0xB0;		        //可更改，与加密芯片中程序保持一致
    bBuf[1] = (unsigned char)(wAddr>>8);
    bBuf[2] = (unsigned char) wAddr;
    bBuf[3] = bLen;	    		//需要读取数据长度

    ret = IIC_WriteWithAddr(IIC_ADDR & 0xFE, bBuf, 4);
    if(ret)                     //出错
        return 1;

    //若执行正确，返回状态码90 00 + 数据
    ret = IIC_ReadWithAddr(IIC_ADDR | 0x1, bBuf, bLen + 2);
    if(ret || bBuf[0] != 0x90 || bBuf[1] != 0x00)         //通信错误或状态码不为0x9000
        return 1;

    memcpy(pbData, bBuf + 2, bLen);
    return 0;		            //正确
}

//---------------------------------------------------------
//函数名: 密文读
//参数说明：
//			pbSeesionKey  - 解密文数据的密钥(8字节)
//			pbData		  - 密文解密后数据
//			bLen   		  - 读取的数据长度
//返回值说明：
//			0 - 成功
//			1 - 失败
//说明:
//---------------------------------------------------------
unsigned char SMEC_CryptReadFlash(unsigned char *pbSeesionKey, unsigned short wAddr, unsigned char *pbData, unsigned char bLen)
{
    unsigned char bBuf[128] = {0};
    unsigned char ret;
    unsigned char len, i;

    len = (bLen + 7) / 8 * 8;	//将bLen转化为8的整数

    bBuf[0] = 0xB1;		        //可更改，与加密芯片中程序保持一致
    bBuf[1] = (unsigned char)(wAddr>>8);
    bBuf[2] = (unsigned char) wAddr;
    bBuf[3] = len;	    		//需要读取的密文数据长度, 8的整数倍

    ret = IIC_WriteWithAddr(IIC_ADDR & 0xFE, bBuf, 4);
    if(ret)                     //出错
        return 1;

    //若执行正确，返回状态码90 00 + 密文数据
    ret = IIC_ReadWithAddr(IIC_ADDR | 0x1, bBuf, len + 2);
    if(ret || bBuf[0] != 0x90 || bBuf[1] != 0x00)         //通信错误或状态码不为0x9000
        return 1;

    for(i = 0; i < len; i += 8)
    {
        decrypt(&bBuf[2 + i], pbSeesionKey, &bBuf[2 + i]); 			//将读取到的密文数据解密
    }

    memcpy(pbData, bBuf + 2, bLen);
    return 0;		            //正确
}

//---------------------------------------------------------
//函数名: 写Flash
//参数说明：
//			pbData - 写入数据存放地址
//			bLen  -  写入的数据长度
//返回值说明：
//			0 - 成功
//			1 - 失败
//说明:
//---------------------------------------------------------
unsigned char SMEC_WriteFlash(unsigned short wAddr, unsigned char *pbData, unsigned char bLen)
{
    unsigned char bBuf[128] = {0};
    unsigned char ret;

    bBuf[0] = 0xD6;		        //可更改，与加密芯片中程序保持一致
    bBuf[1] = (unsigned char)(wAddr>>8);
    bBuf[2] = (unsigned char) wAddr;
    bBuf[3] = bLen;	    		//需要读取数据长度
    memcpy(&bBuf[4], pbData, bLen);

    ret = IIC_WriteWithAddr(IIC_ADDR & 0xFE, bBuf, bLen + 4);
    if(ret)                     //出错
        return 1;

    //若执行正确，返回状态码90 00
    ret = IIC_ReadWithAddr(IIC_ADDR | 0x1, bBuf, 2);
    if(ret || bBuf[0] != 0x90 || bBuf[1] != 0x00)         //通信错误或状态码不为0x9000
        return 1;
    return 0;		            //正确
}

//---------------------------------------------------------
//函数名: 调用加密芯片内部计算圆周长算法
//参数说明：
//			pbInput 	 	- 输入数据
//			bInputLen 		- 输入数据长度
//			pbOutput	  	- 加密芯片运算后,输出数据
//			pbOutputLen	  	- 输出数据长度
//返回值说明：
//			0 - 成功
//			1 - 失败
//说明: 将 MCU 中的一部分关键代码，放入加密芯片中运行，当需要用到SMEC98SP中的算法时，
//      由MCU 向SMEC98SP 发送指令，SMEC98SP 根据指令，在内部运行，返回结果给MCU。
//---------------------------------------------------------
unsigned char SMEC_CircleAlg(unsigned char *pbInput, unsigned char bInputLen, unsigned char *pbOutput, unsigned char *pbOutputLen)
{
    unsigned char bBuf[128] = {0};
    unsigned char ret;

    bBuf[0] = 0x72;		        //可更改，与加密芯片中程序保持一致
    bBuf[1] = 0x00;
    bBuf[2] = 0x00;
    bBuf[3] = bInputLen;
    memcpy(&bBuf[4], pbInput, bInputLen);

    ret = IIC_WriteWithAddr(IIC_ADDR & 0xFE, bBuf, 4 + bInputLen);
    if(ret)                     //出错
        return 1;

    //若执行正确，返回状态码90 00 + 计算结果, 我们样例中的算法为 计算圆周长, 只做了1个字节的输出
    ret = IIC_ReadWithAddr(IIC_ADDR | 0x1, bBuf, 1 + 2);
    if(ret || bBuf[0] != 0x90 || bBuf[1] != 0x00)         //通信错误或状态码不为0x9000
        return 1;

    *pbOutputLen = 1;	 		//样例中的算法为 计算圆周长, 只做了1个字节的输出
    memcpy(pbOutput, bBuf + 2, *pbOutputLen);
    return 0;		            //正确
}

//---------------------------------------------------------
//函数名: 调用加密芯片内部计算圆周长算法, 并用过程密钥加密后,在线路上传输
//参数说明：
//			pbSeesionKey  	- 过程密钥
//			pbInput 	 	- 输入数据
//			bInputLen 		- 输入数据长度
//			pbOutput	  	- 加密芯片运算后,输出数据
//			pbOutputLen	  	- 输出数据长度
//返回值说明：
//			0 - 成功
//			1 - 失败
//说明: 将 MCU 中的一部分关键代码，放入加密芯片中运行，当需要用到SMEC98SP中的算法时，
//      由MCU 向SMEC98SP 发送指令，SMEC98SP 根据指令，在内部运行，返回结果给MCU。
//---------------------------------------------------------
unsigned char SMEC_CircleAlgCrypt(unsigned char *pbSeesionKey, unsigned char *pbInput, unsigned char bInputLen, unsigned char *pbOutput, unsigned char *pbOutputLen)
{
    unsigned char bBuf[128] = {0}, bTmp[8] = {0};
    unsigned char ret;

    bBuf[0] = 0x73;		        //可更改，与加密芯片中程序保持一致
    bBuf[1] = 0x00;
    bBuf[2] = 0x00;
    bBuf[3] = 8;				//
    memcpy(bTmp, pbInput, bInputLen);
    encrypt(bTmp, pbSeesionKey, &bBuf[4]);				   //将端口数据加密,传给SMEC98SP,让其运算

    ret = IIC_WriteWithAddr(IIC_ADDR & 0xFE, bBuf, 8 + 4);
    if(ret)                     //出错
        return 1;

    //若执行正确，返回状态码90 00 + 计算结果的密文, 我们样例中的算法为 计算圆周长, 只做了1个字节的明文输出
    ret = IIC_ReadWithAddr(IIC_ADDR | 0x1, bBuf, 8 + 2);
    if(ret || bBuf[0] != 0x90 || bBuf[1] != 0x00)         //通信错误或状态码不为0x9000
        return 1;

    decrypt(&bBuf[2], pbSeesionKey, bTmp);					//将密文运算结果解密
    *pbOutputLen = bInputLen;	 							//样例中的算法为 计算圆周长, 只做了1个字节的输出.请根据具体算法修改, 这里假设等于输入的数据长度
    memcpy(pbOutput, bTmp, *pbOutputLen);
    return 0;		            //正确
}

//---------------------------------------------------------
//函数名: 产生过程密钥
//参数说明：
//			pbMKey 	 		- 控制密钥,用于产生过程密钥, 16字节
//			pbRandom 		- 主控芯片的随机数, 8字节
//			pbSeesionKey  	- 过程密钥
//返回值说明：
//			0 - 成功
//			1 - 失败
//说明: 成功发生此命令, 加密芯片回送8字节随机数, 并将过程密钥保存在RAM中
//      过程密钥的产生:  pbMKey 对 (pbRandom ^ SMEC98SP随机数)做3DES加密运算, 所得结果为 pbSeesionKey(8字节)
//---------------------------------------------------------
unsigned char SMEC_GenSessionKey(unsigned char *pbMKey, unsigned char *pbRandom, unsigned char *pbSeesionKey)
{
    unsigned char bBuf[128] = {0}, bTmp[8] = {0};
    unsigned char ret;
    unsigned char i;

    bBuf[0] = 0xA0;		        //可更改，与加密芯片中程序保持一致
    bBuf[1] = 0x00;
    bBuf[2] = 0x00;
    bBuf[3] = 8;
    memcpy(&bBuf[4], pbRandom, 8);

    ret = IIC_WriteWithAddr(IIC_ADDR & 0xFE, bBuf, 8 + 4);
    if(ret)                     //出错
        return 1;

    //若执行正确，返回状态码90 00 + 8字节随机数
    ret = IIC_ReadWithAddr(IIC_ADDR | 0x1, bBuf, 8 + 2);
    if(ret || bBuf[0] != 0x90 || bBuf[1] != 0x00)         //通信错误或状态码不为0x9000
        return 1;

    for(i = 0; i < 8; i ++)
    {
        bTmp[i] = pbRandom[i] ^ bBuf[2 + i]; 				//将SMEC98SP的随机数与主控芯片随机数做异或运算
    }

    TripleDES(bTmp, pbMKey, pbSeesionKey);					//计算过程密钥
    return 0;		            //正确
}

//---------------------------------------------------------
//函数名: 端口数据运算
//参数说明：
//			pbSeesionKey  	- 过程密钥
//			pbGpioInput  	- 需要运算的IO口数据输入
//			bPortDataLen	- IO口数据长度
//			pbGpioOutput	- IO口"运算"后的结果
//返回值说明：
//			0 - 成功
//			1 - 失败
//说明: 构造算法(PA口数据->密文送加密芯片, 密文返回)
//  针对很多控制类需求,没有"关键算法"可以存放在加密芯片中,我们构造了一个算法:
//  PA端口数据2字节, 用过程密钥加密后,送给SMEC98SP,由SMEC98SP解密后取反,再由过程密钥加密回送给主控芯片.这样就构造出一个算法
//  如PA = 0x0000, 用过程密钥加密送给SMEC98SP, SMEC98SP解密后得到0x0000, 取反后为0xFFFF, 再用过程密钥加密给主控芯片. 主控芯片解密后得到0xFFFF
//  这样, 判断IO口数据方式,只要跟之前相反就可以. 比如说PA0 高电平才做的动作, 调用了这个函数后,则判断PA0为低电平去做
//  由于每次上电,过程密钥是临时产生的, 并且是变化的, 这样即使PA口数据相同, 在线路上通讯的数据也是不同的, 而主控芯片程序又是基于"运算结果"而工作的,
//  从而增加了破解难度, 可以防止"真值点"攻击
//---------------------------------------------------------
unsigned char SMEC_GpioAlg(unsigned char *pbSeesionKey, unsigned char *pbGpioInput, unsigned char bGpioDataLen, unsigned char *pbGpioOutput)
{
    unsigned char bBuf[128] = {0}, bTmp[8] = {0};
    unsigned char ret;

    bBuf[0] = 0xA2;		        //可更改，与加密芯片中程序保持一致
    bBuf[1] = 0x00;
    bBuf[2] = 0x00;
    bBuf[3] = 8;
    memcpy(bTmp, pbGpioInput, bGpioDataLen);
    encrypt(bTmp, pbSeesionKey, &bBuf[4]);				   //将端口数据加密,传给SMEC98SP,让其"运算"

    ret = IIC_WriteWithAddr(IIC_ADDR & 0xFE, bBuf, 8 + 4);
    if(ret)                     //出错
        return 1;

    //若执行正确，返回状态码90 00 + 被"运算"过的端口密文数据
    ret = IIC_ReadWithAddr(IIC_ADDR | 0x1, bBuf, 8 + 2);
    if(ret || bBuf[0] != 0x90 || bBuf[1] != 0x00)         //通信错误或状态码不为0x9000
        return 1;
    decrypt(&bBuf[2], pbSeesionKey, bTmp);					//将"运算"结果解密
    memcpy(pbGpioOutput, bTmp, bGpioDataLen);

    return 0;		            //正确
}


