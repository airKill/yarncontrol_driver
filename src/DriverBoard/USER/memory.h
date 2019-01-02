#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "stm32f10x.h"

void Write_flash(uint32_t Address,uint16_t *buf,uint16_t len);
void Read_flash(uint16_t *Address,uint16_t *buf,uint8_t len);

#endif
