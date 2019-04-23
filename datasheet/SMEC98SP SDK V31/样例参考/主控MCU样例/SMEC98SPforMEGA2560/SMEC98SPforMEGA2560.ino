#include "smec98sp.h"
#include "iic_smec98sp.h"
void PrintHex(unsigned char *str,unsigned int len)
{
  unsigned int i;
  for(i=0;i<len;i++)
  { 
    Serial.print(*str++,HEX);
  }
  Serial.print("\r\n");  

}

/*
  1.获取SMEC98SP的UID号, 获取STM32的ID, 获取STM32随机数
 2.验证PIN
 3.内外部认证
 4.SHA1=>前置数据^随机数
 5.密文读
 6.读数据
 7.写数据
 8.构造算法(PA口数据->密文送加密芯片, 密文返回)
 
 如果直接引用,请将print的调试信息去除
 */
void SMEC_Test(void)
{
  /*各种密钥,不会在I2C线路上传输,可以使用同一组.应该将密钥分散存储,防止主控芯片被破解后,被攻击者在二进制码中找到密钥 */
  unsigned char InternalKey[16] = {
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F                              };//内部认证密钥,必须和SMEC98SP一致
  unsigned char ExternalKey[16] = {
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F                              };//外部认证密钥,必须和SMEC98SP一致
  unsigned char SHA1_Key[16] = {
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F                              };   //哈希算法认证密钥,必须和SMEC98SP一致
  unsigned char MKey[16] = {
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F                              };   //主控密钥,用于产生过程密钥,必须和SMEC98SP一致

  unsigned char Pin[8] = {
    0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc                              };			//Pin认证密钥,必须和SMEC98SP一致

  unsigned char bSmec98spUid[12] = {
    0                              };		//存放SMEC98SP的UID
  unsigned short RandomSeek = 0;				//随机数种子
  unsigned char bRandom[8] = {
    0                              };				//存放随机数
  unsigned char bSessionKey[8] = {
    0                              };			//存放过程密钥,过程密钥为临时产生的密钥
  unsigned char bDataBuf[64] = {
    0                              };
  unsigned char ret, bLen;
  unsigned short i, j;

  /*获取SMEC98SP的UID*/
  ret = SMEC_GetUid(bSmec98spUid);
  if(ret)
  {
    Serial.print("SMEC_GetUid -> Error !\r\n");
    while(1);
  }
  Serial.print("SMEC_GetUid: ");
  PrintHex(bSmec98spUid,12);

  /*取加密芯片的随机数，作为MCU的RandomSeek因子,也可以用其他外部因素，如按键等待时间等*/
  if(SMEC_GetRandom(bDataBuf))//获取8字节随机数
  {
    Serial.print("SMEC_GetRandom -> Error !\r\n");
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
  Serial.print("RandomSeek:");
  PrintHex((unsigned char *)&RandomSeek, 2);


  /*PIN码验证*/
  ret = SMEC_CheckPin(Pin, (unsigned char)sizeof(Pin));
  if(ret)
  {
    Serial.print("SMEC_CheckPin -> Error !\r\n");
    while(1);
  }
  Serial.print("SMEC_CheckPin OK !\r\n");

  /*内部认证, 主控芯片对SMEC98SP加密芯片合法性判断*/
  for(i = 0; i < 8; i ++)
  {
    bRandom[i] = (unsigned char) rand();
  }
  ret = SMEC_IntrAuth(InternalKey, bRandom);
  if(ret)
  {
    Serial.print("SMEC_IntrAuth -> Error !\r\n");
    while(1);
  }
  Serial.print("SMEC_IntrAuth OK !\r\n");

  /*外部认证, SMEC98SP加密芯片对主控芯片合法性判断*/
  ret = SMEC_ExtrAuth(ExternalKey);
  if(ret)
  {
    Serial.print("SMEC_ExtrAuth -> Error !\r\n");
    while(1);
  }
  Serial.print("SMEC_ExtrAuth OK !\r\n");

  /*SHA1摘要算法认证, 数据长度可自己设定*/
  for(i = 0; i < 16; i ++)
  {
    bDataBuf[i] = (unsigned char) rand();
  }
  ret = SMEC_Sha1Auth(SHA1_Key, (unsigned char)sizeof(SHA1_Key), bDataBuf, 16);
  if(ret)
  {
    Serial.print("SMEC_Sha1Auth -> Error !\r\n");
    while(1);
  }
  Serial.print("SMEC_Sha1Auth OK !\r\n");

  /*调用加密芯片内部计算圆周长算法*/
  bDataBuf[0] = 0x02;
  ret = SMEC_CircleAlg(bDataBuf, 1, bDataBuf, &bLen);
  if(ret)
  {
    Serial.print("SMEC_CircleAlg -> Error !\r\n");
    while(1);
  }
  Serial.print("SMEC_CircleAlg OK, C = ");
  PrintHex(bDataBuf, 1);

  /*产生过程密钥,用于后续的Flash数据加密读,及构造的"端口数据运算"*/
  for(i = 0; i < 8; i ++)
  {
    bRandom[i] = (unsigned char) rand();
  }
  ret = SMEC_GenSessionKey(MKey, bRandom, bSessionKey);
  if(ret)
  {
    Serial.print("SMEC_GenSessionKey -> Error !\r\n");
    while(1);
  }
  Serial.print("SMEC_GenSessionKey OK !\r\n");

  /*密文读取Flash数据*/
  ret = SMEC_CryptReadFlash(bSessionKey, 0x0000, bDataBuf, 16);
  if(ret)
  {
    Serial.print("SMEC_CryptReadFlash -> Error !\r\n");
    while(1);
  }
  Serial.print("SMEC_CryptReadFlash OK:\r\n");
  PrintHex(bDataBuf, 16);

  /*读取Flash数据*/
  ret = SMEC_ReadFlash(0x0000, bDataBuf, 16);
  if(ret)
  {
    Serial.print("SMEC_ReadFlash -> Error !\r\n");
    while(1);
  }
  Serial.print("SMEC_ReadFlash OK:\r\n");
  PrintHex(bDataBuf, 16);

  /*写Flash数据*/
  for(i = 0; i < 16; i ++)
  {
    bDataBuf[i] = (unsigned char) i;
  }
  ret = SMEC_WriteFlash(0x0000, bDataBuf, 16);
  if(ret)
  {
    Serial.print("SMEC_WriteFlash -> Error !\r\n");
    while(1);
  }
  Serial.print("SMEC_WriteFlash OK!\r\n");

  /*构造"端口数据运算", 可以用实际的PA~PC端口数据*/
  bDataBuf[0] = 0x00;
  bDataBuf[1] = 0x00;
  ret = SMEC_GpioAlg(bSessionKey, bDataBuf,2, bDataBuf);
  if(ret)
  {
    Serial.print("SMEC_GpioAlg -> Error !\r\n");
    while(1);
  }
  Serial.print("SMEC_GpioAlg OK:\r\n");
  PrintHex(bDataBuf, 2);  

  /*调用加密芯片内部计算圆周长算法,并密文在线路上传输*/
  bDataBuf[0] = 0x02;
  ret = SMEC_CircleAlgCrypt(bSessionKey, bDataBuf, 1, bDataBuf, &bLen);
  if(ret)
  {
    Serial.print("SMEC_CircleAlgCrypt -> Error !\r\n");
    while(1);
  }
  Serial.print("SMEC_CircleAlgCrypt OK, C = ");
  PrintHex(bDataBuf, 1);
}



void setup() {
  Serial.begin(115200);
  SMEC_I2cInit();
}

void loop() {
  SMEC_Test();
  delay(200);
}


