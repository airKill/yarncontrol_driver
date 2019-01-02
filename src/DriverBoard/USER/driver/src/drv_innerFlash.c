


#include "stm32f10x.h"
#include "stm32f10x_flash.h"                     //flash操作接口文件（在库文件中），必须要包含
#include "drv_innerFlash.h"


volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;      //Flash操作状态变量


/****************************************************************
*Name:		ReadFlashNBtye
*Function:	从内部Flash读取N字节数据
*Input:		ReadAddress：数据地址（偏移地址）ReadBuf：数据指针	ReadNum：读取字节数
*Output:	读取的字节数  
*Author:    ValerianFan
*Date:		2014/04/09
*E-Mail:	fanwenjingnihao@163.com
*Other:		
****************************************************************/
int ReadFlashNBtye(uint32_t ReadAddress, uint8_t *ReadBuf, int32_t ReadNum)
{
  int DataNum = 0;
  ReadAddress = (uint32_t)STARTADDR + ReadAddress; 
  while(DataNum < ReadNum) 
  {
    *(ReadBuf + DataNum) = *(__IO uint8_t*) ReadAddress++;
    DataNum++;
  }
  return DataNum;
}

/****************************************************************
*Name:		WriteFlashOneWord
*Function:	向内部Flash写入32位数据
*Input:		WriteAddress：数据地址（偏移地址）WriteData：写入数据
*Output:	NULL 
*Author:    ValerianFan
*Date:		2014/04/09
*E-Mail:	fanwenjingnihao@163.com
*Other:		
****************************************************************/

void WriteFlashOneWord(uint32_t WriteAddress,uint32_t WriteData)
{
  FLASH_UnlockBank1();
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); 
  FLASHStatus = FLASH_ErasePage(STARTADDR);
  
  if(FLASHStatus == FLASH_COMPLETE)
  {
    FLASHStatus = FLASH_ProgramWord(STARTADDR + WriteAddress, WriteData);    //flash.c 中API函数
    //FLASHStatus = FLASH_ProgramWord(StartAddress+4, 0x56780000);                      //需要写入更多数据时开启
    //FLASHStatus = FLASH_ProgramWord(StartAddress+8, 0x87650000);                      //需要写入更多数据时开启
  }
  FLASH_LockBank1();
} 



void WriteFlashData(uint32_t WriteAddress,u8 data[],u32 num)
{
  u32 i;
  u16 temp;
  FLASH_UnlockBank1();
  //  FLASH_Unlock();
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); 
  FLASHStatus = FLASH_ErasePage(STARTADDR);
  
  if(FLASHStatus == FLASH_COMPLETE)
  {
    for(i=0;i<num;i++)
    {
      temp = data[i];
      FLASHStatus = FLASH_ProgramHalfWord(STARTADDR+WriteAddress+i*2,temp);
    }
    
    //FLASHStatus = FLASH_ProgramWord(StartAddress+4, 0x56780000);                      //需要写入更多数据时开启
    //FLASHStatus = FLASH_ProgramWord(StartAddress+8, 0x87650000);                      //需要写入更多数据时开启
  }
  FLASH_LockBank1();
  //  FLASH_Lock();
} 

void ReadFlashData(uint32_t ReadAddress,u8 data[],u32 num)
{
  int DataNum = 0;
  ReadAddress = (uint32_t)STARTADDR + ReadAddress; 
  while(DataNum < num) 
  {
    *(data + DataNum) = *(__IO uint16_t*) ReadAddress;
    ReadAddress += 2;
    DataNum++;
  }
}
