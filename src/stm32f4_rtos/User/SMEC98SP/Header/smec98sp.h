#ifndef __SMEC98SP_H
#define __SMEC98SP_H

#define GPIO_PORT_SEMC_ADC  GPIOC
#define GPIO_PIN_SEMC_ADC	GPIO_Pin_0

#define SEMC_ADC_CHANNEL       ADC_Channel_10

unsigned char SMEC_GetUid(unsigned char * pUID);
unsigned char SMEC_GetRandom(unsigned char *pRandom);
unsigned char SMEC_CheckPin(unsigned char *pbPin, unsigned char bPinLen);
unsigned char SMEC_Sha1Auth(unsigned char *pbSha1Key, unsigned char bSha1KeyLen, unsigned char *pbRandom, unsigned char bRandomLen);
unsigned char SMEC_IntrAuth(unsigned char *pbKey, unsigned char *pbRandom);
unsigned char SMEC_ExtrAuth(unsigned char *pbKey);
unsigned char SMEC_ReadFlash(unsigned short wAddr, unsigned char *pbData, unsigned char bLen);
unsigned char SMEC_CryptReadFlash(unsigned char *pbSeesionKey, unsigned short wAddr, unsigned char *pbData, unsigned char bLen);
unsigned char SMEC_WriteFlash(unsigned short wAddr, unsigned char *pbData, unsigned char bLen);
unsigned char SMEC_CircleAlg(unsigned char *pbInput, unsigned char bInputLen, unsigned char *pbOutput, unsigned char *pbOutputLen);
unsigned char SMEC_CircleAlgCrypt(unsigned char *pbSeesionKey, unsigned char *pbInput, unsigned char bInputLen, unsigned char *pbOutput, unsigned char *pbOutputLen);
unsigned char SMEC_GenSessionKey(unsigned char *pbMKey, unsigned char *pbRandom, unsigned char *pbSeesionKey);
unsigned char SMEC_GpioAlg(unsigned char *pbSeesionKey, unsigned char *pbGpioInput, unsigned char bGpioDataLen, unsigned char *pbGpioOutput);
unsigned short Get_Adc_RandomSeek(void);
void semc_adc_Init(void);
void SMEC_Test(void);
void GetStm32Uid(unsigned char *pSTM32_UID);
#endif	   
