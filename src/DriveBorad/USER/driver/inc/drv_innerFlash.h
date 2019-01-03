

#ifndef __DRV_INNERFLASH_H
#define __DRV_INNERFLASH_H


#define  STARTADDR  0x08010000                   	 //STM32F103RB 其他型号基本适用，未测试

int ReadFlashNBtye(uint32_t ReadAddress, uint8_t *ReadBuf, int32_t ReadNum);
void WriteFlashOneWord(uint32_t WriteAddress,uint32_t WriteData);
void WriteFlashData(uint32_t WriteAddress,u8 data[],u32 num);
void ReadFlashData(uint32_t WriteAddress,u8 data[],u32 num);

#endif


