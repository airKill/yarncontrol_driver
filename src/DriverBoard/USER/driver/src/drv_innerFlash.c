


#include "stm32f10x.h"
#include "stm32f10x_flash.h"                     //flash�����ӿ��ļ����ڿ��ļ��У�������Ҫ����
#include "drv_innerFlash.h"


volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;      //Flash����״̬����


/****************************************************************
*Name:		ReadFlashNBtye
*Function:	���ڲ�Flash��ȡN�ֽ�����
*Input:		ReadAddress�����ݵ�ַ��ƫ�Ƶ�ַ��ReadBuf������ָ��	ReadNum����ȡ�ֽ���
*Output:	��ȡ���ֽ���  
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
*Function:	���ڲ�Flashд��32λ����
*Input:		WriteAddress�����ݵ�ַ��ƫ�Ƶ�ַ��WriteData��д������
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
    FLASHStatus = FLASH_ProgramWord(STARTADDR + WriteAddress, WriteData);    //flash.c ��API����
    //FLASHStatus = FLASH_ProgramWord(StartAddress+4, 0x56780000);                      //��Ҫд���������ʱ����
    //FLASHStatus = FLASH_ProgramWord(StartAddress+8, 0x87650000);                      //��Ҫд���������ʱ����
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
    
    //FLASHStatus = FLASH_ProgramWord(StartAddress+4, 0x56780000);                      //��Ҫд���������ʱ����
    //FLASHStatus = FLASH_ProgramWord(StartAddress+8, 0x87650000);                      //��Ҫд���������ʱ����
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
