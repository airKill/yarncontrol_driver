#ifndef __IIC_H
#define __IIC_H

bit IIC_ReadWithAddr(unsigned char addr, unsigned char *buf, unsigned char len);
bit IIC_WriteWithAddr(unsigned char addr, unsigned char *buf, unsigned char len);

#endif


