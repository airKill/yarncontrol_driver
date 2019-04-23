#ifndef __IIC_H
#define __IIC_H
void SMEC_I2cInit(void);
unsigned char IIC_ReadWithAddr(unsigned char addr, unsigned char *buf, unsigned char len);
unsigned char IIC_WriteWithAddr(unsigned char addr, unsigned char *buf, unsigned char len);

#endif


