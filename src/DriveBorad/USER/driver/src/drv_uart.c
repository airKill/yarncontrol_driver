/**
****************** (C) COPYRIGHT 2017 Nova  *******************
* @file     drv_uart.c
* @auther   sunzhen
* @version  V1.0.0
* @date     2017.3.7
* @brief    This file provides the functions of drv_uart.
*
***************************************************************
* @attention
*
* History<br>
* version   date            auther        description<br>
* v1.0.0    2017.3.7        sunzhen       Initial Version<br>
***************************************************************
*/

#ifdef __cplusplus
extern "C" {
#endif
  
#include "drv_uart.h"
  
  
  const _Uart_Port_t _UART1_Drv =
  {
    UART1_PORT,
    115200
  };
  
  const _Uart_Port_t _UART2_Drv =
  {
    UART2_PORT,
    9600
  };
  
  const _Uart_Port_t _UART3_Drv =
  {
    UART3_PORT,
    9600
  };
  
  const _Uart_Port_t _UART5_Drv =
  {
    UART5_PORT,
    115200
  };
  
  /************************************************************************
  ** Name   : Uart_Port_Init
  ** brief  : Uart initialization.
  ** input  : _Uart_Port_t* uart: object of a uart.
  ** output : None
  ** Return : None
  ************************************************************************/
  void Uart_Port_Init(const _Uart_Port_t* uart)
  {
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    switch(uart->uartx)
    {
    case UART1_PORT:
      {
        USART_InitStructure.USART_BaudRate = uart->BaudRate;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Tx;
        
        //        RCC_APB2PeriphClockCmd(_UART1_TX_RCC_AHB1Periph_GPIOx, ENABLE);	/* Enable GPIO clock */
        RCC_APB2PeriphClockCmd(_UART1_RX_RCC_AHB1Periph_GPIOx, ENABLE);	/* Enable GPIO clock */
        
        /* Configure USART Tx as alternate function  */
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Pin = _UART1_TX_GPIOPIN_X;
        GPIO_Init(_UART1_TX_GPIOX, &GPIO_InitStructure);
        
        /* Configure USART Rx as alternate function  */
        //        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        //        GPIO_InitStructure.GPIO_Pin = _UART1_RX_GPIOPIN_X;
        //        GPIO_Init(_UART1_RX_GPIOX, &GPIO_InitStructure);
        
        
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); /* Enable UART clock */
        USART_Init(USART1, &USART_InitStructure);	/* USART configuration */ 
        USART_Cmd(USART1, ENABLE); /* Enable USART */
#ifdef _UART1_INT_ENABLE
        //        USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
        //      USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);
#endif
        break;
      }
    case UART2_PORT:
      {
        USART_InitStructure.USART_BaudRate = uart->BaudRate;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        
        RCC_APB2PeriphClockCmd(_UART2_TX_RCC_AHB1Periph_GPIOx, ENABLE);	/* Enable GPIO clock */
        RCC_APB2PeriphClockCmd(_UART2_RX_RCC_AHB1Periph_GPIOx, ENABLE);	/* Enable GPIO clock */
        
        /* Remap */
        //    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
        //   GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE);
        
        /* Configure USART Tx as alternate function  */
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Pin = _UART2_TX_GPIOPIN_X;
        GPIO_Init(_UART2_TX_GPIOX, &GPIO_InitStructure);
        
        /* Configure USART Rx as alternate function  */
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_InitStructure.GPIO_Pin = _UART2_RX_GPIOPIN_X;
        GPIO_Init(_UART2_RX_GPIOX, &GPIO_InitStructure);
        
        
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); /* Enable UART clock */
        USART_Init(USART2, &USART_InitStructure);	/* USART configuration */ 
        USART_Cmd(USART2, ENABLE); /* Enable USART */
#ifdef _UART2_INT_ENABLE
        USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
        USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);
#endif
        break;
      }
    case UART3_PORT:
      {
        USART_InitStructure.USART_BaudRate = uart->BaudRate;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        
        RCC_APB2PeriphClockCmd(_UART3_TX_RCC_AHB1Periph_GPIOx, ENABLE);	/* Enable GPIO clock */
        RCC_APB2PeriphClockCmd(_UART3_RX_RCC_AHB1Periph_GPIOx, ENABLE);	/* Enable GPIO clock */
        
        /* Remap */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
        //        GPIO_PinRemapConfig(GPIO_PartialRemap_USART3,ENABLE);
        
        /* Configure USART Tx as alternate function  */
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Pin = _UART3_TX_GPIOPIN_X;
        GPIO_Init(_UART3_TX_GPIOX, &GPIO_InitStructure);
        
        /* Configure USART Rx as alternate function  */
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_InitStructure.GPIO_Pin = _UART3_RX_GPIOPIN_X;
        GPIO_Init(_UART3_RX_GPIOX, &GPIO_InitStructure);
        
        
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); /* Enable UART clock */
        USART_Init(USART3, &USART_InitStructure);	/* USART configuration */ 
        USART_Cmd(USART3, ENABLE); /* Enable USART */
#ifdef _UART3_INT_ENABLE
        USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
        USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);
#endif
        break;
      }
    case UART5_PORT:
      {
        USART_InitStructure.USART_BaudRate = uart->BaudRate;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        
        RCC_APB2PeriphClockCmd(_UART5_TX_RCC_AHB1Periph_GPIOx, ENABLE);	/* Enable GPIO clock */
        RCC_APB2PeriphClockCmd(_UART5_RX_RCC_AHB1Periph_GPIOx, ENABLE);	/* Enable GPIO clock */
        
        
        /* Configure USART Tx as alternate function  */
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Pin = _UART5_TX_GPIOPIN_X;
        GPIO_Init(_UART5_TX_GPIOX, &GPIO_InitStructure);
        
        /* Configure USART Rx as alternate function  */
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_InitStructure.GPIO_Pin = _UART5_RX_GPIOPIN_X;
        GPIO_Init(_UART5_RX_GPIOX, &GPIO_InitStructure);
        
        
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE); /* Enable UART clock */
        USART_Init(UART5, &USART_InitStructure);	/* USART configuration */ 
        USART_Cmd(UART5, ENABLE); /* Enable USART */
#ifdef _UART5_INT_ENABLE
        USART_ITConfig(UART5,USART_IT_RXNE,ENABLE);
#endif
        break;
      }
      
    default:
      break;
    }
  }
  
  /************************************************************************
  ** Name   : UartSend
  ** brief  : send data via uart.
  ** input  : _Uart_Port_t* uart: object of a uart.
  u8 *buffer:data buffer.
  u16 len:data length.
  ** output : None
  ** Return : None
  ************************************************************************/
  void UartSend(const _Uart_Port_t* uart,u8 *buffer,u16 len)
  {
    unsigned int Count = 0;
    USART_TypeDef* USARTx;
    
    if(uart->uartx == UART1_PORT)
    {
      USARTx = USART1;
    }
    else if(uart->uartx == UART2_PORT)
    {
      USARTx = USART2;
    }
    else if(uart->uartx == UART3_PORT)
    {
      USARTx = USART3;
    }
    else if(uart->uartx == UART4_PORT)
    {
      USARTx = UART4;
    }
    else if(uart->uartx == UART5_PORT)
    {
      USARTx = UART5;
    }
    
    for(Count=0;Count<len;Count++)
    {
      while( USART_GetFlagStatus(USARTx,USART_FLAG_TXE)!= SET)
        ;
      USART_SendData(USARTx,*(buffer+Count));
    }
    //    while( USART_GetFlagStatus(USARTx,USART_FLAG_TC)!= SET)
    ;
  }
  
  /************************************************************************
  ** Name   : UartSendString
  ** brief  : send string via uart.
  ** input  : _Uart_Port_t* uart: object of a uart.
  char *string:string to be send.
  ** output : None
  ** Return : None
  ************************************************************************/
  void UartSendString(const _Uart_Port_t* uart,char *string)
  {
    int len;
    len = strlen(string);
    
    UartSend(uart,(u8*)string,len);
  }
  
  
  /************************************************************************
  ** Name   : UartSendByte
  ** brief  : send byte via uart.
  ** input  : _Uart_Port_t* uart: object of a uart.
  u8 buffer:byte to be send.
  ** output : None
  ** Return : None
  ************************************************************************/
  void UartSendByte(const _Uart_Port_t* uart,u8 buffer)
  {
    UartSend(uart,&buffer,1);
  }
  
  
  
#ifdef __cplusplus
}
#endif


