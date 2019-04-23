#ifndef __SMEC98SP_H
#define __SMEC98SP_H

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

#endif	   
