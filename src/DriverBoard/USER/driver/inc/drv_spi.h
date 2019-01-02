/**
  ****************** (C) COPYRIGHT 2017 Nova  *******************
  * @file     drv_spi.h
  * @auther   wanghongbin
  * @version  V1.0.0
  * @date     2017.3.5
  * @brief    This file provides the functions of spi.
  *
  ***************************************************************
  * @attention
  *
  * History<br>
  * version   date            auther         description<br>
  * v1.0.0    2017.3.5        wanghongbin    Initial Version<br>
  ***************************************************************
  */

#ifndef __DRV_SPI_H_
#define __DRV_SPI_H_

#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"

#ifdef __cplusplus
 extern "C" {
#endif


/* Exported macros --------------------------------------------------------*/
#define   USE_SPI1            1
#define   USE_SPI2            0
#define   USE_SPI4            0
   
#define   SPI1_DMA_ENABLE     1
#define   SPI2_DMA_ENABLE     1
#define   SPI4_DMA_ENABLE     1 

#define   SPI_BUF_SIZE        (u16)4096

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* fsmc instantiated device List */
typedef enum {
#if   USE_SPI1  
  SPI1_Bus = 0x00,
#endif
#if   USE_SPI2  
  SPI2_Bus,
#endif
#if   USE_SPI4    
  SPI4_Bus,
#endif  
  TOTAL_SPI_NUM,
}_SPI_BusType_t;


typedef struct {
  _SPI_BusType_t  bus_id;
} _SPI_Bus_t;

#define BUFFERSIZE 512
#define SPI1_DR_Addr ( (u32)0x4001300C )


/* Exported variables ------------------------------------------------------------*/
extern u8  _gSPI_RecvBuf[SPI_BUF_SIZE];

extern uint8_t SPI1_TX_Buff[BUFFERSIZE];  
extern uint8_t SPI1_RX_Buff[BUFFERSIZE];  

/* Exported functions ------------------------------------------------------- */
void  SPI_BusInit(_SPI_Bus_t* spi);
void  SPI_BusDeInit(_SPI_Bus_t* spi);
FlagStatus  SPI_RxStreamIsReady(_SPI_Bus_t* spi);
FlagStatus  SPI_TxStreamIsReady(_SPI_Bus_t* spi);
u8    SPI_WriteReadOneByte(_SPI_Bus_t* spi, u8 data);
void  SPI_DMA_ReadBuffer(_SPI_Bus_t* spi, u16 length, u8* pBuffer);
void  SPI_DMA_WriteBuffer(_SPI_Bus_t* spi, u16 length, const u8* pBuffer);

void SPI1_ReceiveSendByte(u16 num);

#ifdef __cplusplus
}
#endif

#endif /* __DRV_SPI_H_ */

