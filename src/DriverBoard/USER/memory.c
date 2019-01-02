#include "main.h"

void Write_flash(uint32_t Address,uint16_t *buf,uint16_t len)
{  
  uint8_t i; 
  FLASH_Unlock();
  if((Address % 2048) == 0)
    FLASH_ErasePage(Address);
  for(i=0;i<len;i++)
  {     
    FLASH_ProgramHalfWord(Address+2*i,*buf++);
  }	 
  FLASH_Lock();
}
void Read_flash(uint16_t *Address,uint16_t *buf,uint8_t len)
{  
  memcpy(buf, Address, 2*len);
}
