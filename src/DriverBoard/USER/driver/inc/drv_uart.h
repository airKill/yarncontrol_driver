/**
  ****************** (C) COPYRIGHT 2017 Nova  *******************
  * @file     drv_uart.h
  * @auther   sunzhen
  * @version  V1.0.0
  * @date     2017.3.5
  * @brief    This file provides the functions of usart.
  *
  ***************************************************************
  * @attention
  *
  * History<br>
  * version   date            auther        description<br>
  * v1.0.0    2017.3.7        sunzhen       Initial Version<br>
  ***************************************************************
  */
  
#ifndef __DRV_UART_H_
#define __DRV_UART_H_

#include "stm32f10x_usart.h"
#include "string.h"


#ifdef __cplusplus
 extern "C" {
#endif



#define _UART1_INT_ENABLE
#define _UART1_TX_GPIOX                     GPIOA
#define _UART1_TX_GPIOPINSOURCEX            GPIO_PinSource9
#define _UART1_TX_GPIOPIN_X                 GPIO_Pin_9
#define _UART1_TX_RCC_AHB1Periph_GPIOx      RCC_APB2Periph_GPIOA
#define _UART1_RX_GPIOX                     GPIOA
#define _UART1_RX_GPIOPINSOURCEX            GPIO_PinSource10
#define _UART1_RX_GPIOPIN_X                 GPIO_Pin_10
#define _UART1_RX_RCC_AHB1Periph_GPIOx      RCC_APB2Periph_GPIOA


#define _UART2_INT_ENABLE
#define _UART2_TX_GPIOX                     GPIOA
#define _UART2_TX_GPIOPINSOURCEX            GPIO_PinSource2
#define _UART2_TX_GPIOPIN_X                 GPIO_Pin_2
#define _UART2_TX_RCC_AHB1Periph_GPIOx      RCC_APB2Periph_GPIOA
#define _UART2_RX_GPIOX                     GPIOA
#define _UART2_RX_GPIOPINSOURCEX            GPIO_PinSource3
#define _UART2_RX_GPIOPIN_X                 GPIO_Pin_3
#define _UART2_RX_RCC_AHB1Periph_GPIOx      RCC_APB2Periph_GPIOA

#define _UART3_INT_ENABLE
#define _UART3_TX_GPIOX                     GPIOB
#define _UART3_TX_GPIOPINSOURCEX            GPIO_PinSource10
#define _UART3_TX_GPIOPIN_X                 GPIO_Pin_10
#define _UART3_TX_RCC_AHB1Periph_GPIOx      RCC_APB2Periph_GPIOB
#define _UART3_RX_GPIOX                     GPIOB
#define _UART3_RX_GPIOPINSOURCEX            GPIO_PinSource11
#define _UART3_RX_GPIOPIN_X                 GPIO_Pin_11
#define _UART3_RX_RCC_AHB1Periph_GPIOx      RCC_APB2Periph_GPIOB


#define _UART5_INT_ENABLE
#define _UART5_TX_GPIOX                     GPIOC
#define _UART5_TX_GPIOPINSOURCEX            GPIO_PinSource12
#define _UART5_TX_GPIOPIN_X                 GPIO_Pin_12
#define _UART5_TX_RCC_AHB1Periph_GPIOx      RCC_APB2Periph_GPIOC
#define _UART5_RX_GPIOX                     GPIOD
#define _UART5_RX_GPIOPINSOURCEX            GPIO_PinSource2
#define _UART5_RX_GPIOPIN_X                 GPIO_Pin_2
#define _UART5_RX_RCC_AHB1Periph_GPIOx      RCC_APB2Periph_GPIOD


typedef enum
{
  UART1_PORT = 0x00,
  UART2_PORT,
  UART3_PORT,
  UART4_PORT,
  UART5_PORT,
}UARTx_PORT;


typedef struct _Uart_Port
{
  UARTx_PORT  uartx;
  u32         BaudRate;
}_Uart_Port_t;



void Uart_Port_Init(const _Uart_Port_t* uart);
void UartSend(const _Uart_Port_t* uart,u8 *buffer,u16 len);
void UartSendString(const _Uart_Port_t* uart,char *string);
void UartSendByte(const _Uart_Port_t* uart,u8 buffer);


extern const _Uart_Port_t _UART1_Drv;
extern const _Uart_Port_t _UART2_Drv;
extern const _Uart_Port_t _UART3_Drv;
extern const _Uart_Port_t _UART5_Drv;

#ifdef __cplusplus
}
#endif


#endif   /* __DRV_UART_H_ */


