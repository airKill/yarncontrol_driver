//COPYRIGHT (c) 2018-2025	Sinormous Technology
//深圳市中巨伟业信息科技有限公司
//www.sinormous.com;
//Author:顾万水
//QQ:47583353

#include <string.h>
#include "bsp.h"
#include <stdbool.h>



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

unsigned short Get_Adc_RandomSeek(void)
{
  unsigned char Count;
  unsigned short ADC_RandomSeek = 0;
  
  for(Count = 0; Count < 4; Count++) 
  {
    ADC_RegularChannelConfig(ADC1, SEMC_ADC_CHANNEL, 1, ADC_SampleTime_480Cycles );  //ADC1,ADC通道,480个周期,提高采样时间可以提高精确度
    ADC_SoftwareStartConv(ADC1);		//使能指定的ADC1的软件转换启动功能	
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//μè′y×a???áê?
    ADC_RandomSeek <<= 4;
    ADC_RandomSeek += ADC_GetConversionValue(ADC1) & 0x000f;		/*2é?ˉ4′?ADCμ??μ￡???′?è?2é?ˉμ?μú????￡??′3é16??×÷?a??×ó*/
  }
  return ADC_RandomSeek;
}

void semc_adc_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  ADC_InitTypeDef       ADC_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOA时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC1时钟
  
  //先初始化ADC1通道5 IO口
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SEMC_ADC;//PA4 通道4
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
  GPIO_Init(GPIO_PORT_SEMC_ADC, &GPIO_InitStructure);//初始化  
  
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1复位
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//复位结束	 
  
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//两个采样阶段之间的延迟5个时钟
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMA失能
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);//初始化
  
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12位模式
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;//非扫描模式	
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//关闭连续转换
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//禁止触发检测，使用软件触发
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐	
  ADC_InitStructure.ADC_NbrOfConversion = 1;//1个转换在规则序列中 也就是只转换规则序列1 
  ADC_Init(ADC1, &ADC_InitStructure);//ADC初始化
  
  ADC_Cmd(ADC1, ENABLE);//开启AD转换器	
}

void GetStm32Uid(unsigned char *pSTM32_UID)
{
  pSTM32_UID[0] =  *(unsigned char*)(0x1FFF7A10);
  pSTM32_UID[1] =  *(unsigned char*)(0x1FFF7A11);
  pSTM32_UID[2] =  *(unsigned char*)(0x1FFF7A12);
  pSTM32_UID[3] =  *(unsigned char*)(0x1FFF7A13);
  pSTM32_UID[4] =  *(unsigned char*)(0x1FFF7A14);
  pSTM32_UID[5] =  *(unsigned char*)(0x1FFF7A15);
  pSTM32_UID[6] =  *(unsigned char*)(0x1FFF7A16);
  pSTM32_UID[7] =  *(unsigned char*)(0x1FFF7A17);
  pSTM32_UID[8] =  *(unsigned char*)(0x1FFF7A18);
  pSTM32_UID[9] =  *(unsigned char*)(0x1FFF7A19);
  pSTM32_UID[10] = *(unsigned char*)(0x1FFF7A1a);
  pSTM32_UID[11] = *(unsigned char*)(0x1FFF7A1b);
}

void SMEC_Test(void)
{
  /*?÷???ü??,2??á?úI2C???·é?′?ê?,?éò?ê1ó?í?ò?×é.ó|?????ü??·?é￠′?′￠,·à?1?÷??D???±????aoó,±?1￥?÷???ú?t???????D?òμ??ü?? */
  unsigned char InternalKey[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};//?ú2?è??¤?ü??,±?D?oíSMEC98SPò???
  unsigned char ExternalKey[16] = {0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F};//ía2?è??¤?ü??,±?D?oíSMEC98SPò???
  unsigned char SHA1_Key[16] = {0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F};   //1t?￡??·¨è??¤?ü??,±?D?oíSMEC98SPò???
  unsigned char MKey[16] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F};   //?÷???ü??,ó?óú2úéú1y3ì?ü??,±?D?oíSMEC98SPò???
  
  unsigned char Pin[8] = {0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc};			//Pinè??¤?ü??,±?D?oíSMEC98SPò???
  
  unsigned char bStm32Uid[12] = {0};			//′?·?STM32μ?UID
  unsigned char bSmec98spUid[12] = {0};		//′?·?SMEC98SPμ?UID
  unsigned short RandomSeek;					//???úêy??×ó
  unsigned char bRandom[8] = {0};				//′?·????úêy
  unsigned char bSessionKey[8] = {0};			//′?·?1y3ì?ü??,1y3ì?ü???aáùê±2úéúμ??ü??
  unsigned char bDataBuf[64] = {0};
  unsigned char ret, bLen;
  unsigned short i, j;
  
  SMEC_I2cInit();
  /*à?ó?ADCDü??òy??2úéú???úêy*/
  semc_adc_Init();									//?é·??ú?÷3ìDò?D
  RandomSeek = Get_Adc_RandomSeek();			//à?ó?ADCDü??òy??2úéú???úêy
  
  /*??è?STM32μ?UID*/
  GetStm32Uid(bStm32Uid);
  printf("GetStm32Uid: ");
//  PrintHex(bStm32Uid, 12);
  printf("\r\n");
  
  /*??è?SMEC98SPμ?UID*/
  ret = SMEC_GetUid(bSmec98spUid);
  if(ret)
  {
    printf("SMEC_GetUid -> Error !\r\n");
    while(1);
  }
  printf("SMEC_GetUid: ");
//  PrintHex(bSmec98spUid, 12);
  printf("\r\n");
  
  /*?????úêyRandomSeek￡??ù×?ò?′????ú′|àí(ó?STM32μ?UID, SMEC98SPμ?UID×÷°ó?¨, ê1μ??′ê1?àí??é????,2?í?μ?STM32,SMEC98SP???úêy??×óò22?í?)*/
  for(i = 0; i < 6; i += 2)
  {
    /*ê1RandomSeekó?STM32μ?UID?à1?*/
    j = (bStm32Uid[i] << 8) + bStm32Uid[i + 1];
    RandomSeek ^= j;
    
    /*ê1RandomSeekó?SMEC98SPμ?UID?à1?*/
    j = (bSmec98spUid[i] << 8) + bSmec98spUid[i + 1];
    RandomSeek ^= j;
  }
  srand(RandomSeek);
  printf("RandomSeek: %04x \r\n", RandomSeek);
  
  /*PIN???é?¤*/
  ret = SMEC_CheckPin(Pin, (unsigned char)sizeof(Pin));
  if(ret)
  {
    printf("SMEC_CheckPin -> Error !\r\n");
    while(1);
  }
  printf("SMEC_CheckPin OK !\r\n");
  
  /*?ú2?è??¤, ?÷??D?????SMEC98SP?ó?üD???o?·¨D??D??*/
  for(i = 0; i < 8; i ++)
  {
    bRandom[i] = (unsigned char) rand();
  }
  ret = SMEC_IntrAuth(InternalKey, bRandom);
  if(ret)
  {
    printf("SMEC_IntrAuth -> Error !\r\n");
    while(1);
  }
  printf("SMEC_IntrAuth OK !\r\n");
  
//  /*ía2?è??¤, SMEC98SP?ó?üD??????÷??D???o?·¨D??D??*/
//  ret = SMEC_ExtrAuth(ExternalKey);
//  if(ret)
//  {
//    printf("SMEC_ExtrAuth -> Error !\r\n");
//    while(1);
//  }
//  printf("SMEC_ExtrAuth OK !\r\n");
//  
//  /*SHA1?aòa??·¨è??¤, êy?Y3¤?è?é×??oéè?¨*/
//  for(i = 0; i < 16; i ++)
//  {
//    bDataBuf[i] = (unsigned char) rand();
//  }
//  ret = SMEC_Sha1Auth(SHA1_Key, (unsigned char)sizeof(SHA1_Key), bDataBuf, 16);
//  if(ret)
//  {
//    printf("SMEC_Sha1Auth -> Error !\r\n");
//    while(1);
//  }
//  printf("SMEC_Sha1Auth OK !\r\n");
//  
//  /*μ÷ó??ó?üD????ú2??????2?ü3¤??·¨*/
//  bDataBuf[0] = 0x02;
//  ret = SMEC_CircleAlg(bDataBuf, 1, bDataBuf, &bLen);
//  if(ret)
//  {
//    printf("SMEC_CircleAlg -> Error !\r\n");
//    while(1);
//  }
//  printf("SMEC_CircleAlg OK, C = %02x !\r\n", bDataBuf[0]);
//  
//  /*2úéú1y3ì?ü??,ó?óúoóD?μ?Flashêy?Y?ó?ü?á,?°11?ìμ?"???úêy?Y????"*/
//  for(i = 0; i < 8; i ++)
//  {
//    bRandom[i] = (unsigned char) rand();
//  }
//  ret = SMEC_GenSessionKey(MKey, bRandom, bSessionKey);
//  if(ret)
//  {
//    printf("SMEC_GenSessionKey -> Error !\r\n");
//    while(1);
//  }
//  printf("SMEC_GenSessionKey OK !\r\n");
//  
//  /*?ü???áè?Flashêy?Y*/
//  ret = SMEC_CryptReadFlash(bSessionKey, 0x0000, bDataBuf, 16);
//  if(ret)
//  {
//    printf("SMEC_CryptReadFlash -> Error !\r\n");
//    while(1);
//  }
//  printf("SMEC_CryptReadFlash OK:\r\n");
////  PrintHex(bDataBuf, 16);
//  
//  /*?áè?Flashêy?Y*/
//  ret = SMEC_ReadFlash(0x0000, bDataBuf, 16);
//  if(ret)
//  {
//    printf("SMEC_ReadFlash -> Error !\r\n");
//    while(1);
//  }
//  printf("SMEC_ReadFlash OK:\r\n");
////  PrintHex(bDataBuf, 16);
//  
//  /*D′Flashêy?Y*/
//  for(i = 0; i < 16; i ++)
//  {
//    bDataBuf[i] = (unsigned char) i;
//  }
//  ret = SMEC_WriteFlash(0x0000, bDataBuf, 16);
//  if(ret)
//  {
//    printf("SMEC_WriteFlash -> Error !\r\n");
//    while(1);
//  }
//  printf("SMEC_WriteFlash OK!\r\n");
//  
//  /*11?ì"???úêy?Y????", ?éò?ó?êμ?êμ?PA~PC???úêy?Y*/
//  bDataBuf[0] = 0x00;
//  bDataBuf[1] = 0x00;
//  ret = SMEC_GpioAlg(bSessionKey, bDataBuf,2, bDataBuf);
//  if(ret)
//  {
//    printf("SMEC_GpioAlg -> Error !\r\n");
//    while(1);
//  }
//  printf("SMEC_GpioAlg OK:\r\n");
////  PrintHex(bDataBuf, 2);
//  
//  /*μ÷ó??ó?üD????ú2??????2?ü3¤??·¨,2￠?ü???ú???·é?′?ê?*/
//  bDataBuf[0] = 0x02;
//  ret = SMEC_CircleAlgCrypt(bSessionKey, bDataBuf, 1, bDataBuf, &bLen);
//  if(ret)
//  {
//    printf("SMEC_CircleAlgCrypt -> Error !\r\n");
//    while(1);
//  }
//  printf("SMEC_CircleAlgCrypt OK, C = %02x !\r\n", bDataBuf[0]);
}

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


