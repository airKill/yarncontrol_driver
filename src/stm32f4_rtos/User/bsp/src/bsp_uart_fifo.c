/*
*********************************************************************************************************
*
*	模块名称 : 串口中断+FIFO驱动模块
*	文件名称 : bsp_uart_fifo.c
*	版    本 : V1.0
*	说    明 : 采用串口中断+FIFO模式实现多个串口的同时访问
*	修改记录 :
*		版本号  日期       作者    说明
*		V1.0    2013-02-01 armfly  正式发布
*		V1.1    2013-06-09 armfly  FiFo结构增加TxCount成员变量，方便判断缓冲区满; 增加 清FiFo的函数
*		V1.2	2014-09-29 armfly  增加RS485 MODBUS接口。接收到新字节后，直接执行回调函数。
*		V1.3	2015-07-23 armfly  增加 UART_T 结构的读写指针几个成员变量必须增加 __IO 修饰,否则优化后
*					会导致串口发送函数死机。
*		V1.4	2015-08-04 armfly  解决UART4配置bug  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART1);
*		V1.5	2015-10-08 armfly  增加修改波特率的接口函数
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"


/* 定义每个串口结构体变量 */
#if UART1_FIFO_EN == 1
UART_T g_tUart1;
uint8_t g_TxBuf1[UART1_TX_BUF_SIZE];		/* 发送缓冲区 */
//uint8_t g_RxBuf1[UART1_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

#if UART2_FIFO_EN == 1
UART_T g_tUart2;
uint8_t g_TxBuf2[UART2_TX_BUF_SIZE];		/* 发送缓冲区 */
uint8_t g_RxBuf2[UART2_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

#if UART3_FIFO_EN == 1
UART_T g_tUart3;
uint8_t g_RxBuf3[UART3_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

#if UART4_FIFO_EN == 1
UART_T g_tUart4;
uint8_t g_TxBuf4[UART4_TX_BUF_SIZE];		/* 发送缓冲区 */
uint8_t g_RxBuf4[UART4_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

#if UART5_FIFO_EN == 1
UART_T g_tUart5;
uint8_t g_TxBuf5[UART5_TX_BUF_SIZE];		/* 发送缓冲区 */
uint8_t g_RxBuf5[UART5_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

#if UART6_FIFO_EN == 1
UART_T g_tUart6;
uint8_t g_TxBuf6[UART6_TX_BUF_SIZE];		/* 发送缓冲区 */
uint8_t g_RxBuf6[UART6_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

void UartVarInit(void);

void InitHardUart(void);
void UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen);
uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte);
void UartIRQ(UART_T *_pUart);
void ConfigUartNVIC(void);

void RS485_InitTXE(void);

u8 UART3_RX_BUF[UART3_RX_BUF_SIZE];
u8 UART4_RX_BUF[UART4_RX_BUF_SIZE];
u8 UART2_RX_BUF[UART2_RX_BUF_SIZE];

u8 debug_cmd = 0;
/*
*********************************************************************************************************
*	函 数 名: bsp_InitUart
*	功能说明: 初始化串口硬件，并对全局变量赋初值.
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitUart(void)
{
//  UartVarInit();		/* 必须先初始化全局变量,再配置硬件 */
  InitHardUart();		/* 配置串口的硬件参数(波特率等) */
  RS485_InitTXE();	/* 配置RS485芯片的发送使能硬件，配置为推挽输出 */
  ConfigUartNVIC();	/* 配置串口中断 */
  UartDMAConfig();
  UART1ToPC('\0',1);
  UART2ToPC('\0',1);
  UART3ToPC('\0',1);
//  UART4ToPC('\0',1);
//  UART5ToPC('\0',1);
}

/*
*********************************************************************************************************
*	函 数 名: ComToUart
*	功能说明: 将COM端口号转换为UART指针
*	形    参: _ucPort: 端口号(COM1 - COM6)
*	返 回 值: uart指针
*********************************************************************************************************
*/
UART_T *ComToUart(COM_PORT_E _ucPort)
{
  if (_ucPort == COM1)
  {
#if UART1_FIFO_EN == 1
    return &g_tUart1;
#else
    return 0;
#endif
  }
  else if (_ucPort == COM2)
  {
#if UART2_FIFO_EN == 1
    return &g_tUart2;
#else
    return;
#endif
  }
  else if (_ucPort == COM3)
  {
#if UART3_FIFO_EN == 1
    return &g_tUart3;
#else
    return 0;
#endif
  }
  else if (_ucPort == COM4)
  {
#if UART4_FIFO_EN == 1
    return &g_tUart4;
#else
    return 0;
#endif
  }
  else if (_ucPort == COM5)
  {
#if UART5_FIFO_EN == 1
    return &g_tUart5;
#else
    return 0;
#endif
  }
  else if (_ucPort == COM6)
  {
#if UART6_FIFO_EN == 1
    return &g_tUart6;
#else
    return 0;
#endif
  }
  else
  {
    /* 不做任何处理 */
    return 0;
  }
}


/*
*********************************************************************************************************
*	函 数 名: ComToUart
*	功能说明: 将COM端口号转换为 USART_TypeDef* USARTx
*	形    参: _ucPort: 端口号(COM1 - COM6)
*	返 回 值: USART_TypeDef*,  USART1, USART2, USART3, UART4, UART5
*********************************************************************************************************
*/
USART_TypeDef *ComToUSARTx(COM_PORT_E _ucPort)
{
  if (_ucPort == COM1)
  {
#if UART1_FIFO_EN == 1
    return USART1;
#else
    return 0;
#endif
  }
  else if (_ucPort == COM2)
  {
#if UART2_FIFO_EN == 1
    return USART2;
#else
    return;
#endif
  }
  else if (_ucPort == COM3)
  {
#if UART3_FIFO_EN == 1
    return USART3;
#else
    return 0;
#endif
  }
  else if (_ucPort == COM4)
  {
#if UART4_FIFO_EN == 1
    return UART4;
#else
    return 0;
#endif
  }
  else if (_ucPort == COM5)
  {
#if UART5_FIFO_EN == 1
    return UART5;
#else
    return 0;
#endif
  }
  else
  {
    /* 不做任何处理 */
    return 0;
  }
}

/*
*********************************************************************************************************
*	函 数 名: comSendBuf
*	功能说明: 向串口发送一组数据。数据放到发送缓冲区后立即返回，由中断服务程序在后台完成发送
*	形    参: _ucPort: 端口号(COM1 - COM6)
*			  _ucaBuf: 待发送的数据缓冲区
*			  _usLen : 数据长度
*	返 回 值: 无
*********************************************************************************************************
*/
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen)
{
  UART_T *pUart;
  
  pUart = ComToUart(_ucPort);
  if (pUart == 0)
  {
    return;
  }
  
  if (pUart->SendBefor != 0)
  {
    pUart->SendBefor();		/* 如果是RS485通信，可以在这个函数中将RS485设置为发送模式 */
  }
  
  UartSend(pUart, _ucaBuf, _usLen);
}

/*
*********************************************************************************************************
*	函 数 名: comSendChar
*	功能说明: 向串口发送1个字节。数据放到发送缓冲区后立即返回，由中断服务程序在后台完成发送
*	形    参: _ucPort: 端口号(COM1 - COM6)
*			  _ucByte: 待发送的数据
*	返 回 值: 无
*********************************************************************************************************
*/
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte)
{
  comSendBuf(_ucPort, &_ucByte, 1);
}

/*
*********************************************************************************************************
*	函 数 名: comGetChar
*	功能说明: 从接收缓冲区读取1字节，非阻塞。无论有无数据均立即返回。
*	形    参: _ucPort: 端口号(COM1 - COM5)
*			  _pByte: 接收到的数据存放在这个地址
*	返 回 值: 0 表示无数据, 1 表示读取到有效字节
*********************************************************************************************************
*/
uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte)
{
  UART_T *pUart;
  
  pUart = ComToUart(_ucPort);
  if (pUart == 0)
  {
    return 0;
  }
  
  return UartGetChar(pUart, _pByte);
}

/*
*********************************************************************************************************
*	函 数 名: comClearTxFifo
*	功能说明: 清零串口发送缓冲区
*	形    参: _ucPort: 端口号(COM1 - COM6)
*	返 回 值: 无
*********************************************************************************************************
*/
void comClearTxFifo(COM_PORT_E _ucPort)
{
  UART_T *pUart;
  
  pUart = ComToUart(_ucPort);
  if (pUart == 0)
  {
    return;
  }
  
  pUart->usTxWrite = 0;
  pUart->usTxRead = 0;
  pUart->usTxCount = 0;
}

/*
*********************************************************************************************************
*	函 数 名: comClearRxFifo
*	功能说明: 清零串口接收缓冲区
*	形    参: _ucPort: 端口号(COM1 - COM6)
*	返 回 值: 无
*********************************************************************************************************
*/
void comClearRxFifo(COM_PORT_E _ucPort)
{
  UART_T *pUart;
  
  pUart = ComToUart(_ucPort);
  if (pUart == 0)
  {
    return;
  }
  
  pUart->usRxWrite = 0;
  pUart->usRxRead = 0;
  pUart->usRxCount = 0;
}

/*
*********************************************************************************************************
*	函 数 名: comSetBaud
*	功能说明: 设置串口的波特率
*	形    参: _ucPort: 端口号(COM1 - COM5)
*			  _BaudRate: 波特率，0-4500000， 最大4.5Mbps
*	返 回 值: 无
*********************************************************************************************************
*/
void comSetBaud(COM_PORT_E _ucPort, uint32_t _BaudRate)
{
  USART_TypeDef* USARTx;
  
  USARTx = ComToUSARTx(_ucPort);
  if (USARTx == 0)
  {
    return;
  }
  
  USART_SetBaudRate(USARTx, _BaudRate);
}

/*
*********************************************************************************************************
*	函 数 名: USART_SetBaudRate
*	功能说明: 修改波特率寄存器，不更改其他设置。如果使用 USART_Init函数, 则会修改硬件流控参数和RX,TX配置
*			  根据固件库中 USART_Init函数，将其中配置波特率的部分单独提出来封装为一个函数
*	形    参: USARTx : USART1, USART2, USART3, UART4, UART5
*			  BaudRate : 波特率，取值 0 - 4500000
*	返 回 值: 无
*********************************************************************************************************
*/
void USART_SetBaudRate(USART_TypeDef* USARTx, uint32_t BaudRate)
{
  uint32_t tmpreg = 0x00, apbclock = 0x00;
  uint32_t integerdivider = 0x00;
  uint32_t fractionaldivider = 0x00;
  RCC_ClocksTypeDef RCC_ClocksStatus;
  
  /* Check the parameters */
  assert_param(IS_USART_ALL_PERIPH(USARTx));
  assert_param(IS_USART_BAUDRATE(BaudRate));  
  
  /*---------------------------- USART BRR Configuration -----------------------*/
  /* Configure the USART Baud Rate */
  RCC_GetClocksFreq(&RCC_ClocksStatus);
  
  if ((USARTx == USART1) || (USARTx == USART6))
  {
    apbclock = RCC_ClocksStatus.PCLK2_Frequency;
  }
  else
  {
    apbclock = RCC_ClocksStatus.PCLK1_Frequency;
  }
  
  /* Determine the integer part */
  if ((USARTx->CR1 & USART_CR1_OVER8) != 0)
  {
    /* Integer part computing in case Oversampling mode is 8 Samples */
    integerdivider = ((25 * apbclock) / (2 * (BaudRate)));    
  }
  else /* if ((USARTx->CR1 & USART_CR1_OVER8) == 0) */
  {
    /* Integer part computing in case Oversampling mode is 16 Samples */
    integerdivider = ((25 * apbclock) / (4 * (BaudRate)));    
  }
  tmpreg = (integerdivider / 100) << 4;
  
  /* Determine the fractional part */
  fractionaldivider = integerdivider - (100 * (tmpreg >> 4));
  
  /* Implement the fractional part in the register */
  if ((USARTx->CR1 & USART_CR1_OVER8) != 0)
  {
    tmpreg |= ((((fractionaldivider * 8) + 50) / 100)) & ((uint8_t)0x07);
  }
  else /* if ((USARTx->CR1 & USART_CR1_OVER8) == 0) */
  {
    tmpreg |= ((((fractionaldivider * 16) + 50) / 100)) & ((uint8_t)0x0F);
  }
  
  /* Write to USART BRR register */
  USARTx->BRR = (uint16_t)tmpreg;
}

/* 如果是RS485通信，请按如下格式编写函数， 我们仅举了 USART3作为RS485的例子 */

/*
*********************************************************************************************************
*	函 数 名: RS485_InitTXE
*	功能说明: 配置RS485发送使能口线 TXE
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void RS485_InitTXE(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_RS485_TXEN, ENABLE);	/* 打开GPIO时钟 */
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* 设为输出口 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* 无上拉电阻 */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO口最大速度 */
  GPIO_InitStructure.GPIO_Pin = PIN_RS485_TXEN;
  GPIO_Init(PORT_RS485_TXEN, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	函 数 名: RS485_SetBaud
*	功能说明: 修改485串口的波特率。
*	形    参: _baud : 波特率.0-4500000
*	返 回 值: 无
*********************************************************************************************************
*/
void RS485_SetBaud(uint32_t _baud)
{
  comSetBaud(COM3, _baud);
}

/*
*********************************************************************************************************
*	函 数 名: RS485_SendBefor
*	功能说明: 发送数据前的准备工作。对于RS485通信，请设置RS485芯片为发送状态，
*			  并修改 UartVarInit()中的函数指针等于本函数名，比如 g_tUart2.SendBefor = RS485_SendBefor
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void RS485_SendBefor(void)
{
  RS485_TX_EN();	/* 切换RS485收发芯片为发送模式 */
}

/*
*********************************************************************************************************
*	函 数 名: RS485_SendOver
*	功能说明: 发送一串数据结束后的善后处理。对于RS485通信，请设置RS485芯片为接收状态，
*			  并修改 UartVarInit()中的函数指针等于本函数名，比如 g_tUart2.SendOver = RS485_SendOver
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void RS485_SendOver(void)
{
  RS485_RX_EN();	/* 切换RS485收发芯片为接收模式 */
}


/*
*********************************************************************************************************
*	函 数 名: RS485_SendBuf
*	功能说明: 通过RS485芯片发送一串数据。注意，本函数不等待发送完毕。
*	形    参: _ucaBuf : 数据缓冲区
*			  _usLen : 数据长度
*	返 回 值: 无
*********************************************************************************************************
*/
void RS485_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen)
{
  comSendBuf(COM3, _ucaBuf, _usLen);
}


/*
*********************************************************************************************************
*	函 数 名: RS485_SendStr
*	功能说明: 向485总线发送一个字符串，0结束。
*	形    参: _pBuf 字符串，0结束
*	返 回 值: 无
*********************************************************************************************************
*/
void RS485_SendStr(char *_pBuf)
{
  RS485_SendBuf((uint8_t *)_pBuf, strlen(_pBuf));
}

/*
*********************************************************************************************************
*	函 数 名: RS485_ReciveNew
*	功能说明: 接收到新的数据
*	形    参: _byte 接收到的新数据
*	返 回 值: 无
*********************************************************************************************************
*/
//extern void MODBUS_ReciveNew(uint8_t _byte);
void RS485_ReciveNew(uint8_t _byte)
{
  //	MODBUS_ReciveNew(_byte);
}

/*
*********************************************************************************************************
*	函 数 名: InitHardUart
*	功能说明: 配置串口的硬件参数（波特率，数据位，停止位，起始位，校验位，中断使能）适合于STM32-F4开发板
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void InitHardUart(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  
#if UART1_FIFO_EN == 1		/* 串口1 TX = PA9   RX = PA10 或 TX = PB6   RX = PB7*/
  
  /* 第1步： 配置GPIO */
#if 1	/* TX = PA9   RX = PA10 */
  /* 打开 GPIO 时钟 */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
  /* 打开 UART 时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
  /* 将 PA9 映射为 USART1_TX */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
  
  /* 将 PA10 映射为 USART1_RX */
//  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
  
  /* 配置 USART Tx 为复用功能 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* 输出类型为推挽 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 内部上拉电阻使能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* 复用模式 */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* 配置 USART Rx 为复用功能 */
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
#else	/* TX = PB6   RX = PB7  */
  /* 打开 GPIO 时钟 */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  
  /* 打开 UART 时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
  /* 将 PB6 映射为 USART1_TX */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
  
  /* 将 PB7 映射为 USART1_RX */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
  
  /* 配置 USART Tx 为复用功能 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* 输出类型为推挽 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 内部上拉电阻使能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* 复用模式 */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  /* 配置 USART Rx 为复用功能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
  
  /* 第2步： 配置串口硬件参数 */
  USART_InitStructure.USART_BaudRate = UART1_BAUD;	/* 波特率 */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx;//UART1仅发送不接受
  USART_Init(USART1, &USART_InitStructure);
  
//  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	/* 使能接收中断 */
  /*
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  注意: 不要在此处打开发送中断
  发送中断使能在SendUart()函数打开
  */
  USART_Cmd(USART1, ENABLE);		/* 使能串口 */
  
  /* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
  如下语句解决第1个字节无法正确发送出去的问题 */
  USART_ClearFlag(USART1, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */
#endif
  
#if UART2_FIFO_EN == 1		/* 串口2 TX = PD5   RX = PD6 或  TX = PA2， RX = PA3  */
  /* 第1步： 配置GPIO */
#if 0	/* 串口2 TX = PD5   RX = PD6 */
  /* 打开 GPIO 时钟 */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  
  /* 打开 UART 时钟 */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  
  /* 将 PD5 映射为 USART2_TX */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
  
  /* 将 PD6 映射为 USART2_RX */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);
  
  /* 配置 USART Tx 为复用功能 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* 输出类型为推挽 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 内部上拉电阻使能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* 复用模式 */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* 配置 USART Rx 为复用功能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
#else	/* 串口2   TX = PA2， RX = PA3 */
  /* 打开 GPIO 时钟 */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
  /* 配置 PB2为推挽输出，用于切换 RS485芯片的收发状态 */
  {
    RCC_AHB1PeriphClockCmd(RCC_RS485_TXEN, ENABLE);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* 设为输出口 */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽模式 */
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* 上下拉电阻不使能 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO口最大速度 */
    
    GPIO_InitStructure.GPIO_Pin = PIN_RS485_TXEN;
    GPIO_Init(PORT_RS485_TXEN, &GPIO_InitStructure);
  }
  
  /* 打开 UART 时钟 */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  
  /* 将 PA2 映射为 USART2_TX. 在STM32-V5板中，PA2 管脚用于以太网 */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
  
  /* 将 PA3 映射为 USART2_RX */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
  
  /* 配置 USART Tx 为复用功能 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* 输出类型为推挽 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 内部上拉电阻使能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* 复用模式 */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* 配置 USART Rx 为复用功能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
  
  /* 第2步： 配置串口硬件参数 */
  USART_InitStructure.USART_BaudRate = UART2_BAUD;	/* 波特率 */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		/* 仅选择接收模式 */
  USART_Init(USART2, &USART_InitStructure);
  
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	/* 使能接收中断 */

  USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
  USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
  USART_Cmd(USART2, ENABLE);		/* 使能串口 */
  
  /* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
  如下语句解决第1个字节无法正确发送出去的问题 */
  USART_ClearFlag(USART2, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */
#endif
  
#if UART3_FIFO_EN == 1			/* 串口3 TX = PB10   RX = PB11 */
  
  /* 打开 GPIO 时钟 */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  
  /* 打开 UART 时钟 */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  
  /* 将 PB10 映射为 USART3_TX */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
  
  /* 将 PB11 映射为 USART3_RX */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
  
  /* 配置 USART Tx 为复用功能 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* 输出类型为推挽 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 内部上拉电阻使能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* 复用模式 */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  /* 配置 USART Rx 为复用功能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  /* 第2步： 配置串口硬件参数 */
  USART_InitStructure.USART_BaudRate = UART3_BAUD;	/* 波特率 */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART3, &USART_InitStructure);
  
  USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
  USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
  USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
  /*
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  注意: 不要在此处打开发送中断
  发送中断使能在SendUart()函数打开
  */
  USART_Cmd(USART3, ENABLE);		/* 使能串口 */
  
  /* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
  如下语句解决第1个字节无法正确发送出去的问题 */
  USART_ClearFlag(USART3, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */
#endif
  
#if UART4_FIFO_EN == 1			/* 串口4 TX = PC10   RX = PC11 */
  /* 第1步： 配置GPIO */
  
  /* 打开 GPIO 时钟 */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  
  /* 打开 UART 时钟 */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
  
  /* 将 PC10 映射为 UART4_TX */
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);
  
  /* 将 PC11 映射为 UART4_RX */
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);
  
  /* 配置 USART Tx 为复用功能 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* 输出类型为推挽 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 内部上拉电阻使能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* 复用模式 */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /* 配置 USART Rx 为复用功能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /* 第2步： 配置串口硬件参数 */
  USART_InitStructure.USART_BaudRate = UART4_BAUD;	/* 波特率 */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(UART4, &USART_InitStructure);
  
  USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);	/* 使能接收中断 */
  USART_DMACmd(UART4,USART_DMAReq_Rx,ENABLE);
  /*
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  注意: 不要在此处打开发送中断
  发送中断使能在SendUart()函数打开
  */
  USART_Cmd(UART4, ENABLE);		/* 使能串口 */
  
  /* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
  如下语句解决第1个字节无法正确发送出去的问题 */
  USART_ClearFlag(UART4, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */
#endif
  
#if UART5_FIFO_EN == 1			/* 串口5 TX = PC12   RX = PD2 */
  /* 第1步： 配置GPIO */
  
  /* 打开 GPIO 时钟 */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC |RCC_AHB1Periph_GPIOD, ENABLE);
  
  /* 打开 UART 时钟 */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
  
  /* 将 PC12 映射为 UART5_TX */
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);
  
  /* 将 PD2 映射为 UART5_RX */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);
  
  /* 配置 UART Tx 为复用功能 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* 输出类型为推挽 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 内部上拉电阻使能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* 复用模式 */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /* 配置 UART Rx 为复用功能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* 第2步： 配置串口硬件参数 */
  USART_InitStructure.USART_BaudRate = UART5_BAUD;	/* 波特率 */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(UART5, &USART_InitStructure);
  
  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
  USART_ITConfig(UART5, USART_IT_IDLE, ENABLE);	/* 使能接收中断 */
//  USART_DMACmd(UART5,USART_DMAReq_Rx,ENABLE);
  /*
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  注意: 不要在此处打开发送中断
  发送中断使能在SendUart()函数打开
  */
  USART_Cmd(UART5, ENABLE);		/* 使能串口 */
  
  /* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
  如下语句解决第1个字节无法正确发送出去的问题 */
  USART_ClearFlag(UART5, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */
#endif
  
#if UART6_FIFO_EN == 1			/* PG14/USART6_TX , PC7/USART6_RX,PG8/USART6_RTS, PG15/USART6_CTS */
  /* 第1步： 配置GPIO */
  
  /* 打开 GPIO 时钟 */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC |RCC_AHB1Periph_GPIOG, ENABLE);
  
  /* 打开 UART 时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
  
  /* 将 PG14 映射为 USART6_TX */
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_USART6);
  
  /* 将 PC7 映射为 USART6_RX */
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);
  
  /* 将 PG8 映射为 USART6_RTS */
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource8, GPIO_AF_USART6);
  
  /* 将 PG15 映射为 USART6_CTS */
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource15, GPIO_AF_USART6);
  
  /* 配置 PG14/USART6_TX 为复用功能 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* 输出类型为推挽 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 内部上拉电阻使能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* 复用模式 */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  
  /* 配置 PC7/USART6_RX 为复用功能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /* 配置 PG8/USART6_RTS 为复用功能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  
  /* 配置 PG15/USART6_CTS 为复用功能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  
  /* 第2步： 配置串口硬件参数 */
  USART_InitStructure.USART_BaudRate = UART6_BAUD;	/* 波特率 */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  //USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;	/* 选择硬件流控 */
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	/* 不要硬件流控 */
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART6, &USART_InitStructure);
  
  USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);	/* 使能接收中断 */
  /*
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  注意: 不要在此处打开发送中断
  发送中断使能在SendUart()函数打开
  */
  USART_Cmd(USART6, ENABLE);		/* 使能串口 */
  
  /* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
  如下语句解决第1个字节无法正确发送出去的问题 */
  USART_ClearFlag(USART6, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */
#endif
}

/*
*********************************************************************************************************
*	函 数 名: ConfigUartNVIC
*	功能说明: 配置串口硬件中断.
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void ConfigUartNVIC(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Configure the NVIC Preemption Priority Bits */
  /*	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  --- 在 bsp.c 中 bsp_Init() 中配置中断优先级组 */
  
//#if UART1_FIFO_EN == 1
//  /* 使能串口1中断 */
//  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//#endif
  
#if UART2_FIFO_EN == 1
  /* 使能串口2中断 */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif
  
#if UART3_FIFO_EN == 1
  /* 使能串口3中断t */
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif
  
#if UART4_FIFO_EN == 1
  /* 使能串口4中断t */
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif
  
#if UART5_FIFO_EN == 1
  /* 使能串口5中断t */
  NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif
  
#if UART6_FIFO_EN == 1
  /* 使能串口6中断t */
  NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif
}

void UartDMAConfig(void)
{  
  DMA_InitTypeDef DMA_InitStructure;  
  /* DMA clock enable */  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);//DMA2 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);//DMA1 
  
  DMA_DeInit(DMA1_Stream2);
  while (DMA_GetCmdStatus(DMA1_Stream2) != DISABLE);//等待DMA可配置 
  /* 配置 DMA Stream */
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //通道选择
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&UART4->DR;//DMA外设地址
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)UART4_RX_BUF;//DMA 存储器0地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;//外设到存储器模式
  DMA_InitStructure.DMA_BufferSize = UART4_RX_BUF_SIZE;//数据传输量 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度:8位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度:8位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式 
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中等优先级
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
  DMA_Init(DMA1_Stream2, &DMA_InitStructure);//初始化DMA Stream
  DMA_Cmd(DMA1_Stream2, ENABLE);  //开启DMA传输 
  
  DMA_DeInit(DMA1_Stream5);
  while (DMA_GetCmdStatus(DMA1_Stream5) != DISABLE);//等待DMA可配置 
  /* 配置 DMA Stream */
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //通道选择
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART2->DR;//DMA外设地址
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)UART2_RX_BUF;//DMA 存储器0地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;//外设到存储器模式
  DMA_InitStructure.DMA_BufferSize = UART2_RX_BUF_SIZE;//数据传输量 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度:8位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度:8位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式 
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中等优先级
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
  DMA_Init(DMA1_Stream5, &DMA_InitStructure);//初始化DMA Stream
  DMA_Cmd(DMA1_Stream5, ENABLE);  //开启DMA传输 
  
  DMA_DeInit(DMA1_Stream1);
  while (DMA_GetCmdStatus(DMA1_Stream1) != DISABLE);//等待DMA可配置 
  /* 配置 DMA Stream */
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //通道选择
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART3->DR;//DMA外设地址
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)UART3_RX_BUF;//DMA 存储器0地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;//外设到存储器模式
  DMA_InitStructure.DMA_BufferSize = UART3_RX_BUF_SIZE;//数据传输量 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度:8位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度:8位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式 
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中等优先级
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
  DMA_Init(DMA1_Stream1, &DMA_InitStructure);//初始化DMA Stream
  DMA_Cmd(DMA1_Stream1, ENABLE);  //开启DMA传输 
}  
/*
*********************************************************************************************************
*	函 数 名: UartSend
*	功能说明: 填写数据到UART发送缓冲区,并启动发送中断。中断处理函数发送完毕后，自动关闭发送中断
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen)
{
  uint16_t i;
  
  for (i = 0; i < _usLen; i++)
  {
    /* 如果发送缓冲区已经满了，则等待缓冲区空 */
#if 0
    /*
    在调试GPRS例程时，下面的代码出现死机，while 死循环
    原因： 发送第1个字节时 _pUart->usTxWrite = 1；_pUart->usTxRead = 0;
    将导致while(1) 无法退出
    */
    while (1)
    {
      uint16_t usRead;
      
      DISABLE_INT();
      usRead = _pUart->usTxRead;
      ENABLE_INT();
      
      if (++usRead >= _pUart->usTxBufSize)
      {
        usRead = 0;
      }
      
      if (usRead != _pUart->usTxWrite)
      {
        break;
      }
    }
#else
    /* 当 _pUart->usTxBufSize == 1 时, 下面的函数会死掉(待完善) */
    while (1)
    {
      __IO uint16_t usCount;
      
      DISABLE_INT();
      usCount = _pUart->usTxCount;
      ENABLE_INT();
      
      if (usCount < _pUart->usTxBufSize)
      {
        break;
      }
    }
#endif
    
    /* 将新数据填入发送缓冲区 */
    _pUart->pTxBuf[_pUart->usTxWrite] = _ucaBuf[i];
    
    DISABLE_INT();
    if (++_pUart->usTxWrite >= _pUart->usTxBufSize)
    {
      _pUart->usTxWrite = 0;
    }
    _pUart->usTxCount++;
    ENABLE_INT();
  }
  
  USART_ITConfig(_pUart->uart, USART_IT_TXE, ENABLE);
}

/*
*********************************************************************************************************
*	函 数 名: UartGetChar
*	功能说明: 从串口接收缓冲区读取1字节数据 （用于主程序调用）
*	形    参: _pUart : 串口设备
*			  _pByte : 存放读取数据的指针
*	返 回 值: 0 表示无数据  1表示读取到数据
*********************************************************************************************************
*/
static uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte)
{
  uint16_t usCount;
  
  /* usRxWrite 变量在中断函数中被改写，主程序读取该变量时，必须进行临界区保护 */
  DISABLE_INT();
  usCount = _pUart->usRxCount;
  ENABLE_INT();
  
  /* 如果读和写索引相同，则返回0 */
  //if (_pUart->usRxRead == usRxWrite)
  if (usCount == 0)	/* 已经没有数据 */
  {
    return 0;
  }
  else
  {
    *_pByte = _pUart->pRxBuf[_pUart->usRxRead];		/* 从串口接收FIFO取1个数据 */
    
    /* 改写FIFO读索引 */
    DISABLE_INT();
    if (++_pUart->usRxRead >= _pUart->usRxBufSize)
    {
      _pUart->usRxRead = 0;
    }
    _pUart->usRxCount--;
    ENABLE_INT();
    return 1;
  }
}

/*
*********************************************************************************************************
*	函 数 名: UartIRQ
*	功能说明: 供中断服务程序调用，通用串口中断处理函数
*	形    参: _pUart : 串口设备
*	返 回 值: 无
*********************************************************************************************************
*/
static void UartIRQ(UART_T *_pUart)
{
  /* 处理接收中断  */
  if (USART_GetITStatus(_pUart->uart, USART_IT_RXNE) != RESET)
  {
    /* 从串口接收数据寄存器读取数据存放到接收FIFO */
    uint8_t ch;
    
    ch = USART_ReceiveData(_pUart->uart);
    _pUart->pRxBuf[_pUart->usRxWrite] = ch;
    if (++_pUart->usRxWrite >= _pUart->usRxBufSize)
    {
      _pUart->usRxWrite = 0;
    }
    if (_pUart->usRxCount < _pUart->usRxBufSize)
    {
      _pUart->usRxCount++;
    }
    
    /* 回调函数,通知应用程序收到新数据,一般是发送1个消息或者设置一个标记 */
    //if (_pUart->usRxWrite == _pUart->usRxRead)
    //if (_pUart->usRxCount == 1)
    {
      if (_pUart->ReciveNew)
      {
        _pUart->ReciveNew(ch);
      }
    }
  }
  
  /* 处理发送缓冲区空中断 */
  if (USART_GetITStatus(_pUart->uart, USART_IT_TXE) != RESET)
  {
    //if (_pUart->usTxRead == _pUart->usTxWrite)
    if (_pUart->usTxCount == 0)
    {
      /* 发送缓冲区的数据已取完时， 禁止发送缓冲区空中断 （注意：此时最后1个数据还未真正发送完毕）*/
      USART_ITConfig(_pUart->uart, USART_IT_TXE, DISABLE);
      
      /* 使能数据发送完毕中断 */
      USART_ITConfig(_pUart->uart, USART_IT_TC, ENABLE);
    }
    else
    {
      /* 从发送FIFO取1个字节写入串口发送数据寄存器 */
      USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
      if (++_pUart->usTxRead >= _pUart->usTxBufSize)
      {
        _pUart->usTxRead = 0;
      }
      _pUart->usTxCount--;
    }
    
  }
  /* 数据bit位全部发送完毕的中断 */
  else if (USART_GetITStatus(_pUart->uart, USART_IT_TC) != RESET)
  {
    //if (_pUart->usTxRead == _pUart->usTxWrite)
    if (_pUart->usTxCount == 0)
    {
      /* 如果发送FIFO的数据全部发送完毕，禁止数据发送完毕中断 */
      USART_ITConfig(_pUart->uart, USART_IT_TC, DISABLE);
      
      /* 回调函数, 一般用来处理RS485通信，将RS485芯片设置为接收模式，避免抢占总线 */
      if (_pUart->SendOver)
      {
        _pUart->SendOver();
      }
    }
    else
    {
      /* 正常情况下，不会进入此分支 */
      
      /* 如果发送FIFO的数据还未完毕，则从发送FIFO取1个数据写入发送数据寄存器 */
      USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
      if (++_pUart->usTxRead >= _pUart->usTxBufSize)
      {
        _pUart->usTxRead = 0;
      }
      _pUart->usTxCount--;
    }
  }
}

/*
*********************************************************************************************************
*	函 数 名: USART1_IRQHandler  USART2_IRQHandler USART3_IRQHandler UART4_IRQHandler UART5_IRQHandler
*	功能说明: USART中断服务程序
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
#if UART1_FIFO_EN == 1
void USART1_IRQHandler(void)
{
  u8 temp;
  if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)
  {
    temp = USART_ReceiveData(USART1);
    debug_cmd = temp;
  }
}
#endif

#if UART2_FIFO_EN == 1
void USART2_IRQHandler(void)
{
  u16 data;
  u16 UART_ReceiveSize = 0;
//  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  if(USART_GetITStatus(USART2,USART_IT_IDLE) != RESET)
  {
    DMA_Cmd(DMA1_Stream5, DISABLE);
    data = USART2->SR;
    data = USART2->DR;
    data = data;
    UART_ReceiveSize = UART2_RX_BUF_SIZE - DMA_GetCurrDataCounter(DMA1_Stream5);
//    memcpy(rs485_rev_buf,UART2_RX_BUF,UART_ReceiveSize);
//    rs485_rev_len = UART_ReceiveSize;
//    UART1ToPC(UART2_RX_BUF,UART_ReceiveSize);
    memcpy(m_ctrl_dev.rxbuf,UART2_RX_BUF,UART_ReceiveSize);
    m_ctrl_dev.rxlen = UART_ReceiveSize;
    DMA_ClearFlag(DMA1_Stream5,DMA_FLAG_TCIF5 | DMA_FLAG_FEIF5 | DMA_FLAG_DMEIF5 | DMA_FLAG_TEIF5 | DMA_FLAG_HTIF5);//??DMA1_Steam3??????
    DMA_SetCurrDataCounter(DMA1_Stream5, UART2_RX_BUF_SIZE);
    DMA_Cmd(DMA1_Stream5, ENABLE);
    m_ctrl_dev.frameok = 1;
//    /* 发送同步信号 */
//    xSemaphoreGiveFromISR(xSemaphore_rs485, &xHigherPriorityTaskWoken);
//    /* 如果xHigherPriorityTaskWoken = pdTRUE，那么退出中断后切到当前最高优先级任务执行 */
//    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}
#endif

#if UART3_FIFO_EN == 1
void USART3_IRQHandler(void)
{
  u16 data;
  u16 UART_ReceiveSize = 0;
  
  if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
  {
    DMA_Cmd(DMA1_Stream1,DISABLE);
    DMA_ClearFlag(DMA1_Stream1,DMA_FLAG_TCIF1 | DMA_FLAG_FEIF1 | DMA_FLAG_DMEIF1 | DMA_FLAG_TEIF1 | DMA_FLAG_HTIF1);
    data = USART3->SR;
    data = USART3->DR; 												                //读SR后读DR 清USART_IT_IDLE标志
    data = data;
    if(UART3_RX_BUF[0] != 0 && strlen((char *)UART3_RX_BUF) > 2)
    {
      UART_ReceiveSize = UART3_RX_BUF_SIZE - DMA_GetCurrDataCounter(DMA1_Stream1);
      xQueueSendFromISR(xQueue_esp8266_Cmd, (void *)UART3_RX_BUF,0);
      memset(UART3_RX_BUF, 0x00, UART3_RX_BUF_SIZE);
    }
    if(UART_ReceiveSize == UART3_RX_BUF_SIZE)
    {
      USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    }
    else
    {
      DMA_SetCurrDataCounter(DMA1_Stream1, UART3_RX_BUF_SIZE);
      DMA_Cmd(DMA1_Stream1, ENABLE);
    }
  }
  if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
  {
    USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
    
    DMA_SetCurrDataCounter(DMA1_Stream1, UART3_RX_BUF_SIZE);
    DMA_Cmd(DMA1_Stream1, ENABLE);
  }
}

#endif

#if UART4_FIFO_EN == 1
void UART4_IRQHandler(void)
{
  u16 data;
  u16 UART_ReceiveSize = 0;
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  if(USART_GetITStatus(UART4,USART_IT_IDLE) != RESET)
  {
    DMA_Cmd(DMA1_Stream2, DISABLE);
    data = UART4->SR;
    data = UART4->DR;
    data = data;
    UART_ReceiveSize = UART4_RX_BUF_SIZE - DMA_GetCurrDataCounter(DMA1_Stream2);
    memcpy(lcd_rev_buf,UART4_RX_BUF,UART_ReceiveSize);
    lcd_rev_len = UART_ReceiveSize;
    DMA_ClearFlag(DMA1_Stream2,DMA_FLAG_TCIF2 | DMA_FLAG_FEIF2 | DMA_FLAG_DMEIF2 | DMA_FLAG_TEIF2 | DMA_FLAG_HTIF2);//??DMA1_Steam3??????
    DMA_SetCurrDataCounter(DMA1_Stream2, UART4_RX_BUF_SIZE);
    DMA_Cmd(DMA1_Stream2, ENABLE);
//    UART1ToPC(lcd_rev_buf,lcd_rev_len);
    /* 发送同步信号 */
    xSemaphoreGiveFromISR(xSemaphore_lcd, &xHigherPriorityTaskWoken);
    /* 如果xHigherPriorityTaskWoken = pdTRUE，那么退出中断后切到当前最高优先级任务执行 */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}
#endif

#if UART5_FIFO_EN == 1
void UART5_IRQHandler(void)
{
  u16 data;
  u8 tmp;
  if(USART_GetITStatus(UART5,USART_IT_RXNE) != RESET)
  {
    USART_ClearITPendingBit(UART5,USART_IT_RXNE);
    tmp = USART_ReceiveData(UART5); 
    rfid_rev_buf[rfid_rev_cnt] = tmp;
    rfid_rev_cnt++;
  }
  if(USART_GetITStatus(UART5,USART_IT_IDLE) != RESET)
  {
    data = UART5->SR;
    data = UART5->DR;
    rfid_rev_flag = 1;
    if(rfid_rev_cnt >= 20)
    {
      rfid_rev_cnt = 0;
      rfid_rev_flag = 0;
    }
  }
}
#endif

#if UART6_FIFO_EN == 1
void USART6_IRQHandler(void)
{
  UartIRQ(&g_tUart6);
}
#endif



void UART1ToPC(u8 *buf,u8 len)
{
  unsigned char i;
  for(i = 0; i < len; i++) 
  {
//    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, buf[i]);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
  }
}

void UART2ToPC(u8 *buf,u8 len)
{
  unsigned char i;
  for(i = 0; i < len; i++) 
  {
//    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    USART_SendData(USART2, buf[i]);
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
  }
}

void RS485_sendbuf(u8 *buf,u8 len)
{
  unsigned char i;
  RS485_TX_EN();
  for(i = 0; i < len; i++) 
  {
//    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    USART_SendData(USART2, buf[i]);
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
  }
  RS485_RX_EN();
  
//  UART1ToPC(buf,len);
}

void UART3ToPC(u8 *buf,u8 len)
{
  unsigned char i;
  for(i = 0; i < len; i++) 
  {
//    while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    USART_SendData(USART3, buf[i]);
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
  }
}

void UART4ToPC(u8 *buf,u8 len)
{
  unsigned char i;
  __set_PRIMASK(1);  
  for(i = 0; i < len; i++) 
  {
//    while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
    USART_SendData(UART4, buf[i]);
    while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);
  }
  __set_PRIMASK(0);  
}

void UART5ToPC(u8 *buf,u8 len)
{
  unsigned char i;
  __set_PRIMASK(1);
  for(i = 0; i < len; i++) 
  {
//    while(USART_GetFlagStatus(UART5, USART_FLAG_TXE) == RESET);
    USART_SendData(UART5, buf[i]);
    while (USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);
  }
  __set_PRIMASK(0);
}

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
  USART_SendData(USART1, (uint8_t) ch);
  
  /* Loop until the end of transmission */
//  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
//  {}
  return ch;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
