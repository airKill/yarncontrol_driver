#ifndef __IIC_H
#define __IIC_H

#include <stdbool.h>

void SMEC_I2cInit(void);
bool IIC_ReadWithAddr(unsigned char addr, unsigned char *buf, unsigned char len);
bool IIC_WriteWithAddr(unsigned char addr, unsigned char *buf, unsigned char len);

#endif	   
