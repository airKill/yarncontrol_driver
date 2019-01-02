/**
  ****************** (C) COPYRIGHT 2016 Nova  *******************
  * @file     drv_i2c.h
	* @auther   Liurongxue
  * @version  V1.0
  * @date     2/21/2017
  * @brief    This file contains all the functions prototypes for the I2C
  *
  ***************************************************************
  * @attention
  *
  * History<br>
  * version   date            auther         description<br>
  * v1.0.0    2017.2.21       Liurongxue     Initial Version<br>
  ***************************************************************
  */

#ifndef __DRV_I2C_H_
#define __DRV_I2C_H_

#include "stm32f10x.h"


#ifdef __cplusplus
 extern "C" {
#endif

/* Exported types ------------------------------------------------------------*/
/* I2C Bus List */
typedef enum{
  EN_I2C_BUS1 = 0x00,
  EN_I2C_BUS2,
  
  EN_I2C_BUS_NUM,
}En_I2C_Bus_t;

/* I2C Bus Definition */
typedef struct I2C_Bus_t{
    u32                 speed;  /* I2C bus delay time, unit:Hz */
    const En_I2C_Bus_t  index;  /* I2C bus index, used to assign which I2C bus */
    u32          ack_time_out;  /* assign waitting over time for ack */
}I2C_Bus_t;

/* I2C Device Info Definition */
typedef struct I2C_Dev_Info_t{
    u8                address;    /* device address */
    I2C_Bus_t         i2c_bus;    /* i2c bus No. */
    u8           repeat_times;    /* the opration(w/r) is error, repeat few times */
}I2C_Dev_Info_t;

/* Exported functions ------------------------------------------------------- */
void  I2C_Start(const I2C_Bus_t *i2c);
void  I2C_Stop(const I2C_Bus_t *i2c);
bool  I2C_WaitAck(const I2C_Bus_t *i2c);
void  I2C_SendNoAck(const I2C_Bus_t *i2c);
void  I2C_SendAck(const I2C_Bus_t *i2c);
void  I2C_WriteByte(const I2C_Bus_t *i2c, u8 byte);
u8    I2C_ReadByte(const I2C_Bus_t *i2c);
void  I2C_GPIO_Init(const I2C_Bus_t *i2c);
void  I2CC_GPIO_DeInit(const I2C_Bus_t *i2c);

#ifdef __cplusplus
}
#endif

#endif /* __DEV_I2C_H_ */
