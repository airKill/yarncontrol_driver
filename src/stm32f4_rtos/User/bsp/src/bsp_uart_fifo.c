/*
*********************************************************************************************************
*
*	ģ������ : �����ж�+FIFO����ģ��
*	�ļ����� : bsp_uart_fifo.c
*	��    �� : V1.0
*	˵    �� : ���ô����ж�+FIFOģʽʵ�ֶ�����ڵ�ͬʱ����
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		V1.0    2013-02-01 armfly  ��ʽ����
*		V1.1    2013-06-09 armfly  FiFo�ṹ����TxCount��Ա�����������жϻ�������; ���� ��FiFo�ĺ���
*		V1.2	2014-09-29 armfly  ����RS485 MODBUS�ӿڡ����յ����ֽں�ֱ��ִ�лص�������
*		V1.3	2015-07-23 armfly  ���� UART_T �ṹ�Ķ�дָ�뼸����Ա������������ __IO ����,�����Ż���
*					�ᵼ�´��ڷ��ͺ���������
*		V1.4	2015-08-04 armfly  ���UART4����bug  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART1);
*		V1.5	2015-10-08 armfly  �����޸Ĳ����ʵĽӿں���
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"


/* ����ÿ�����ڽṹ����� */
#if UART1_FIFO_EN == 1
UART_T g_tUart1;
uint8_t g_TxBuf1[UART1_TX_BUF_SIZE];		/* ���ͻ����� */
uint8_t g_RxBuf1[UART1_RX_BUF_SIZE];		/* ���ջ����� */
#endif

#if UART2_FIFO_EN == 1
UART_T g_tUart2;
uint8_t g_TxBuf2[UART2_TX_BUF_SIZE];		/* ���ͻ����� */
uint8_t g_RxBuf2[UART2_RX_BUF_SIZE];		/* ���ջ����� */
#endif

#if UART3_FIFO_EN == 1
UART_T g_tUart3;
uint8_t g_TxBuf3[UART3_TX_BUF_SIZE];		/* ���ͻ����� */
uint8_t g_RxBuf3[UART3_RX_BUF_SIZE];		/* ���ջ����� */
#endif

#if UART4_FIFO_EN == 1
UART_T g_tUart4;
uint8_t g_TxBuf4[UART4_TX_BUF_SIZE];		/* ���ͻ����� */
uint8_t g_RxBuf4[UART4_RX_BUF_SIZE];		/* ���ջ����� */
#endif

#if UART5_FIFO_EN == 1
UART_T g_tUart5;
uint8_t g_TxBuf5[UART5_TX_BUF_SIZE];		/* ���ͻ����� */
uint8_t g_RxBuf5[UART5_RX_BUF_SIZE];		/* ���ջ����� */
#endif

#if UART6_FIFO_EN == 1
UART_T g_tUart6;
uint8_t g_TxBuf6[UART6_TX_BUF_SIZE];		/* ���ͻ����� */
uint8_t g_RxBuf6[UART6_RX_BUF_SIZE];		/* ���ջ����� */
#endif

void UartVarInit(void);

void InitHardUart(void);
void UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen);
uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte);
void UartIRQ(UART_T *_pUart);
void ConfigUartNVIC(void);

void RS485_InitTXE(void);

u8 UART5_RX_BUF[UART5_RX_BUF_SIZE];
u8 UART4_RX_BUF[UART4_RX_BUF_SIZE];
u8 UART2_RX_BUF[UART2_RX_BUF_SIZE];

u8 debug_cmd = 0;
/*
*********************************************************************************************************
*	�� �� ��: bsp_InitUart
*	����˵��: ��ʼ������Ӳ��������ȫ�ֱ�������ֵ.
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitUart(void)
{
//  UartVarInit();		/* �����ȳ�ʼ��ȫ�ֱ���,������Ӳ�� */
  InitHardUart();		/* ���ô��ڵ�Ӳ������(�����ʵ�) */
  RS485_InitTXE();	/* ����RS485оƬ�ķ���ʹ��Ӳ��������Ϊ������� */
  ConfigUartNVIC();	/* ���ô����ж� */
  UartDMAConfig();
  UART1ToPC('\0',1);
  UART2ToPC('\0',1);
  UART3ToPC('\0',1);
  UART4ToPC('\0',1);
  UART5ToPC("hello world",strlen("hello world"));
}

/*
*********************************************************************************************************
*	�� �� ��: ComToUart
*	����˵��: ��COM�˿ں�ת��ΪUARTָ��
*	��    ��: _ucPort: �˿ں�(COM1 - COM6)
*	�� �� ֵ: uartָ��
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
    /* �����κδ��� */
    return 0;
  }
}


/*
*********************************************************************************************************
*	�� �� ��: ComToUart
*	����˵��: ��COM�˿ں�ת��Ϊ USART_TypeDef* USARTx
*	��    ��: _ucPort: �˿ں�(COM1 - COM6)
*	�� �� ֵ: USART_TypeDef*,  USART1, USART2, USART3, UART4, UART5
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
    /* �����κδ��� */
    return 0;
  }
}

/*
*********************************************************************************************************
*	�� �� ��: comSendBuf
*	����˵��: �򴮿ڷ���һ�����ݡ����ݷŵ����ͻ��������������أ����жϷ�������ں�̨��ɷ���
*	��    ��: _ucPort: �˿ں�(COM1 - COM6)
*			  _ucaBuf: �����͵����ݻ�����
*			  _usLen : ���ݳ���
*	�� �� ֵ: ��
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
    pUart->SendBefor();		/* �����RS485ͨ�ţ���������������н�RS485����Ϊ����ģʽ */
  }
  
  UartSend(pUart, _ucaBuf, _usLen);
}

/*
*********************************************************************************************************
*	�� �� ��: comSendChar
*	����˵��: �򴮿ڷ���1���ֽڡ����ݷŵ����ͻ��������������أ����жϷ�������ں�̨��ɷ���
*	��    ��: _ucPort: �˿ں�(COM1 - COM6)
*			  _ucByte: �����͵�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte)
{
  comSendBuf(_ucPort, &_ucByte, 1);
}

/*
*********************************************************************************************************
*	�� �� ��: comGetChar
*	����˵��: �ӽ��ջ�������ȡ1�ֽڣ��������������������ݾ��������ء�
*	��    ��: _ucPort: �˿ں�(COM1 - COM5)
*			  _pByte: ���յ������ݴ���������ַ
*	�� �� ֵ: 0 ��ʾ������, 1 ��ʾ��ȡ����Ч�ֽ�
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
*	�� �� ��: comClearTxFifo
*	����˵��: ���㴮�ڷ��ͻ�����
*	��    ��: _ucPort: �˿ں�(COM1 - COM6)
*	�� �� ֵ: ��
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
*	�� �� ��: comClearRxFifo
*	����˵��: ���㴮�ڽ��ջ�����
*	��    ��: _ucPort: �˿ں�(COM1 - COM6)
*	�� �� ֵ: ��
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
*	�� �� ��: comSetBaud
*	����˵��: ���ô��ڵĲ�����
*	��    ��: _ucPort: �˿ں�(COM1 - COM5)
*			  _BaudRate: �����ʣ�0-4500000�� ���4.5Mbps
*	�� �� ֵ: ��
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
*	�� �� ��: USART_SetBaudRate
*	����˵��: �޸Ĳ����ʼĴ������������������á����ʹ�� USART_Init����, ����޸�Ӳ�����ز�����RX,TX����
*			  ���ݹ̼����� USART_Init���������������ò����ʵĲ��ֵ����������װΪһ������
*	��    ��: USARTx : USART1, USART2, USART3, UART4, UART5
*			  BaudRate : �����ʣ�ȡֵ 0 - 4500000
*	�� �� ֵ: ��
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

/* �����RS485ͨ�ţ��밴���¸�ʽ��д������ ���ǽ����� USART3��ΪRS485������ */

/*
*********************************************************************************************************
*	�� �� ��: RS485_InitTXE
*	����˵��: ����RS485����ʹ�ܿ��� TXE
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RS485_InitTXE(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_RS485_TXEN, ENABLE);	/* ��GPIOʱ�� */
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* ��Ϊ����� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ���� */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* ���������� */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO������ٶ� */
  GPIO_InitStructure.GPIO_Pin = PIN_RS485_TXEN;
  GPIO_Init(PORT_RS485_TXEN, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	�� �� ��: RS485_SetBaud
*	����˵��: �޸�485���ڵĲ����ʡ�
*	��    ��: _baud : ������.0-4500000
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RS485_SetBaud(uint32_t _baud)
{
  comSetBaud(COM3, _baud);
}

/*
*********************************************************************************************************
*	�� �� ��: RS485_SendBefor
*	����˵��: ��������ǰ��׼������������RS485ͨ�ţ�������RS485оƬΪ����״̬��
*			  ���޸� UartVarInit()�еĺ���ָ����ڱ������������� g_tUart2.SendBefor = RS485_SendBefor
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RS485_SendBefor(void)
{
  RS485_TX_EN();	/* �л�RS485�շ�оƬΪ����ģʽ */
}

/*
*********************************************************************************************************
*	�� �� ��: RS485_SendOver
*	����˵��: ����һ�����ݽ�������ƺ�������RS485ͨ�ţ�������RS485оƬΪ����״̬��
*			  ���޸� UartVarInit()�еĺ���ָ����ڱ������������� g_tUart2.SendOver = RS485_SendOver
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RS485_SendOver(void)
{
  RS485_RX_EN();	/* �л�RS485�շ�оƬΪ����ģʽ */
}


/*
*********************************************************************************************************
*	�� �� ��: RS485_SendBuf
*	����˵��: ͨ��RS485оƬ����һ�����ݡ�ע�⣬���������ȴ�������ϡ�
*	��    ��: _ucaBuf : ���ݻ�����
*			  _usLen : ���ݳ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RS485_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen)
{
  comSendBuf(COM3, _ucaBuf, _usLen);
}


/*
*********************************************************************************************************
*	�� �� ��: RS485_SendStr
*	����˵��: ��485���߷���һ���ַ�����0������
*	��    ��: _pBuf �ַ�����0����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RS485_SendStr(char *_pBuf)
{
  RS485_SendBuf((uint8_t *)_pBuf, strlen(_pBuf));
}

/*
*********************************************************************************************************
*	�� �� ��: RS485_ReciveNew
*	����˵��: ���յ��µ�����
*	��    ��: _byte ���յ���������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
//extern void MODBUS_ReciveNew(uint8_t _byte);
void RS485_ReciveNew(uint8_t _byte)
{
  //	MODBUS_ReciveNew(_byte);
}

/*
*********************************************************************************************************
*	�� �� ��: UartVarInit
*	����˵��: ��ʼ��������صı���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void UartVarInit(void)
{
#if UART1_FIFO_EN == 1
  g_tUart1.uart = USART1;						/* STM32 �����豸 */
  g_tUart1.pTxBuf = g_TxBuf1;					/* ���ͻ�����ָ�� */
  g_tUart1.pRxBuf = g_RxBuf1;					/* ���ջ�����ָ�� */
  g_tUart1.usTxBufSize = UART1_TX_BUF_SIZE;	/* ���ͻ�������С */
  g_tUart1.usRxBufSize = UART1_RX_BUF_SIZE;	/* ���ջ�������С */
  g_tUart1.usTxWrite = 0;						/* ����FIFOд���� */
  g_tUart1.usTxRead = 0;						/* ����FIFO������ */
  g_tUart1.usRxWrite = 0;						/* ����FIFOд���� */
  g_tUart1.usRxRead = 0;						/* ����FIFO������ */
  g_tUart1.usRxCount = 0;						/* ���յ��������ݸ��� */
  g_tUart1.usTxCount = 0;						/* �����͵����ݸ��� */
  g_tUart1.SendBefor = 0;						/* ��������ǰ�Ļص����� */
  g_tUart1.SendOver = 0;						/* ������Ϻ�Ļص����� */
  g_tUart1.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
#endif
}
//  
//#if UART2_FIFO_EN == 1
//  g_tUart2.uart = USART2;						/* STM32 �����豸 */
//  g_tUart2.pTxBuf = g_TxBuf2;					/* ���ͻ�����ָ�� */
//  g_tUart2.pRxBuf = g_RxBuf2;					/* ���ջ�����ָ�� */
//  g_tUart2.usTxBufSize = UART2_TX_BUF_SIZE;	/* ���ͻ�������С */
//  g_tUart2.usRxBufSize = UART2_RX_BUF_SIZE;	/* ���ջ�������С */
//  g_tUart2.usTxWrite = 0;						/* ����FIFOд���� */
//  g_tUart2.usTxRead = 0;						/* ����FIFO������ */
//  g_tUart2.usRxWrite = 0;						/* ����FIFOд���� */
//  g_tUart2.usRxRead = 0;						/* ����FIFO������ */
//  g_tUart2.usRxCount = 0;						/* ���յ��������ݸ��� */
//  g_tUart2.usTxCount = 0;						/* �����͵����ݸ��� */
//  g_tUart2.SendBefor = 0;						/* ��������ǰ�Ļص����� */
//  g_tUart2.SendOver = 0;						/* ������Ϻ�Ļص����� */
//  g_tUart2.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
//#endif
//  
//#if UART3_FIFO_EN == 1
//  g_tUart3.uart = USART3;						/* STM32 �����豸 */
//  g_tUart3.pTxBuf = g_TxBuf3;					/* ���ͻ�����ָ�� */
//  g_tUart3.pRxBuf = g_RxBuf3;					/* ���ջ�����ָ�� */
//  g_tUart3.usTxBufSize = UART3_TX_BUF_SIZE;	/* ���ͻ�������С */
//  g_tUart3.usRxBufSize = UART3_RX_BUF_SIZE;	/* ���ջ�������С */
//  g_tUart3.usTxWrite = 0;						/* ����FIFOд���� */
//  g_tUart3.usTxRead = 0;						/* ����FIFO������ */
//  g_tUart3.usRxWrite = 0;						/* ����FIFOд���� */
//  g_tUart3.usRxRead = 0;						/* ����FIFO������ */
//  g_tUart3.usRxCount = 0;						/* ���յ��������ݸ��� */
//  g_tUart3.usTxCount = 0;						/* �����͵����ݸ��� */
//  g_tUart3.SendBefor = RS485_SendBefor;		/* ��������ǰ�Ļص����� */
//  g_tUart3.SendOver = RS485_SendOver;			/* ������Ϻ�Ļص����� */
//  g_tUart3.ReciveNew = RS485_ReciveNew;		/* ���յ������ݺ�Ļص����� */
//#endif
//  
//#if UART4_FIFO_EN == 1
//  g_tUart4.uart = UART4;						/* STM32 �����豸 */
//  g_tUart4.pTxBuf = g_TxBuf4;					/* ���ͻ�����ָ�� */
//  g_tUart4.pRxBuf = g_RxBuf4;					/* ���ջ�����ָ�� */
//  g_tUart4.usTxBufSize = UART4_TX_BUF_SIZE;	/* ���ͻ�������С */
//  g_tUart4.usRxBufSize = UART4_RX_BUF_SIZE;	/* ���ջ�������С */
//  g_tUart4.usTxWrite = 0;						/* ����FIFOд���� */
//  g_tUart4.usTxRead = 0;						/* ����FIFO������ */
//  g_tUart4.usRxWrite = 0;						/* ����FIFOд���� */
//  g_tUart4.usRxRead = 0;						/* ����FIFO������ */
//  g_tUart4.usRxCount = 0;						/* ���յ��������ݸ��� */
//  g_tUart4.usTxCount = 0;						/* �����͵����ݸ��� */
//  g_tUart4.SendBefor = 0;						/* ��������ǰ�Ļص����� */
//  g_tUart4.SendOver = 0;						/* ������Ϻ�Ļص����� */
//  g_tUart4.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
//#endif
//  
//#if UART5_FIFO_EN == 1
//  g_tUart5.uart = UART5;						/* STM32 �����豸 */
//  g_tUart5.pTxBuf = g_TxBuf5;					/* ���ͻ�����ָ�� */
//  g_tUart5.pRxBuf = g_RxBuf5;					/* ���ջ�����ָ�� */
//  g_tUart5.usTxBufSize = UART5_TX_BUF_SIZE;	/* ���ͻ�������С */
//  g_tUart5.usRxBufSize = UART5_RX_BUF_SIZE;	/* ���ջ�������С */
//  g_tUart5.usTxWrite = 0;						/* ����FIFOд���� */
//  g_tUart5.usTxRead = 0;						/* ����FIFO������ */
//  g_tUart5.usRxWrite = 0;						/* ����FIFOд���� */
//  g_tUart5.usRxRead = 0;						/* ����FIFO������ */
//  g_tUart5.usRxCount = 0;						/* ���յ��������ݸ��� */
//  g_tUart5.usTxCount = 0;						/* �����͵����ݸ��� */
//  g_tUart5.SendBefor = 0;						/* ��������ǰ�Ļص����� */
//  g_tUart5.SendOver = 0;						/* ������Ϻ�Ļص����� */
//  g_tUart5.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
//#endif
//  
//  
//#if UART6_FIFO_EN == 1
//  g_tUart6.uart = USART6;						/* STM32 �����豸 */
//  g_tUart6.pTxBuf = g_TxBuf6;					/* ���ͻ�����ָ�� */
//  g_tUart6.pRxBuf = g_RxBuf6;					/* ���ջ�����ָ�� */
//  g_tUart6.usTxBufSize = UART6_TX_BUF_SIZE;	/* ���ͻ�������С */
//  g_tUart6.usRxBufSize = UART6_RX_BUF_SIZE;	/* ���ջ�������С */
//  g_tUart6.usTxWrite = 0;						/* ����FIFOд���� */
//  g_tUart6.usTxRead = 0;						/* ����FIFO������ */
//  g_tUart6.usRxWrite = 0;						/* ����FIFOд���� */
//  g_tUart6.usRxRead = 0;						/* ����FIFO������ */
//  g_tUart6.usRxCount = 0;						/* ���յ��������ݸ��� */
//  g_tUart6.usTxCount = 0;						/* �����͵����ݸ��� */
//  g_tUart6.SendBefor = 0;						/* ��������ǰ�Ļص����� */
//  g_tUart6.SendOver = 0;						/* ������Ϻ�Ļص����� */
//  g_tUart6.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
//#endif
//}

/*
*********************************************************************************************************
*	�� �� ��: InitHardUart
*	����˵��: ���ô��ڵ�Ӳ�������������ʣ�����λ��ֹͣλ����ʼλ��У��λ���ж�ʹ�ܣ��ʺ���STM32-F4������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void InitHardUart(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  
#if UART1_FIFO_EN == 1		/* ����1 TX = PA9   RX = PA10 �� TX = PB6   RX = PB7*/
  
  /* ��1���� ����GPIO */
#if 1	/* TX = PA9   RX = PA10 */
  /* �� GPIO ʱ�� */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
  /* �� UART ʱ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
  /* �� PA9 ӳ��Ϊ USART1_TX */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
  
  /* �� PA10 ӳ��Ϊ USART1_RX */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
  
  /* ���� USART Tx Ϊ���ù��� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* ���� USART Rx Ϊ���ù��� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
#else	/* TX = PB6   RX = PB7  */
  /* �� GPIO ʱ�� */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  
  /* �� UART ʱ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
  /* �� PB6 ӳ��Ϊ USART1_TX */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
  
  /* �� PB7 ӳ��Ϊ USART1_RX */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
  
  /* ���� USART Tx Ϊ���ù��� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  /* ���� USART Rx Ϊ���ù��� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
  
  /* ��2���� ���ô���Ӳ������ */
  USART_InitStructure.USART_BaudRate = UART1_BAUD;	/* ������ */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);
  
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
  /*
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  ע��: ��Ҫ�ڴ˴��򿪷����ж�
  �����ж�ʹ����SendUart()������
  */
  USART_Cmd(USART1, ENABLE);		/* ʹ�ܴ��� */
  
  /* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
  �����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
  USART_ClearFlag(USART1, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */
#endif
  
#if UART2_FIFO_EN == 1		/* ����2 TX = PD5   RX = PD6 ��  TX = PA2�� RX = PA3  */
  /* ��1���� ����GPIO */
#if 0	/* ����2 TX = PD5   RX = PD6 */
  /* �� GPIO ʱ�� */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  
  /* �� UART ʱ�� */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  
  /* �� PD5 ӳ��Ϊ USART2_TX */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
  
  /* �� PD6 ӳ��Ϊ USART2_RX */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);
  
  /* ���� USART Tx Ϊ���ù��� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* ���� USART Rx Ϊ���ù��� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
#else	/* ����2   TX = PA2�� RX = PA3 */
  /* �� GPIO ʱ�� */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
  /* ���� PB2Ϊ��������������л� RS485оƬ���շ�״̬ */
  {
    RCC_AHB1PeriphClockCmd(RCC_RS485_TXEN, ENABLE);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* ��Ϊ����� */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* ���������費ʹ�� */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO������ٶ� */
    
    GPIO_InitStructure.GPIO_Pin = PIN_RS485_TXEN;
    GPIO_Init(PORT_RS485_TXEN, &GPIO_InitStructure);
  }
  
  /* �� UART ʱ�� */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  
  /* �� PA2 ӳ��Ϊ USART2_TX. ��STM32-V5���У�PA2 �ܽ�������̫�� */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
  
  /* �� PA3 ӳ��Ϊ USART2_RX */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
  
  /* ���� USART Tx Ϊ���ù��� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* ���� USART Rx Ϊ���ù��� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
  
  /* ��2���� ���ô���Ӳ������ */
  USART_InitStructure.USART_BaudRate = UART2_BAUD;	/* ������ */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		/* ��ѡ�����ģʽ */
  USART_Init(USART2, &USART_InitStructure);
  
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */

  USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
  USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
  USART_Cmd(USART2, ENABLE);		/* ʹ�ܴ��� */
  
  /* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
  �����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
  USART_ClearFlag(USART2, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */
#endif
  
#if UART3_FIFO_EN == 1			/* ����3 TX = PB10   RX = PB11 */
  
  /* �� GPIO ʱ�� */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  
  /* �� UART ʱ�� */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  
  /* �� PB10 ӳ��Ϊ USART3_TX */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
  
  /* �� PB11 ӳ��Ϊ USART3_RX */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
  
  /* ���� USART Tx Ϊ���ù��� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  /* ���� USART Rx Ϊ���ù��� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  /* ��2���� ���ô���Ӳ������ */
  USART_InitStructure.USART_BaudRate = UART3_BAUD;	/* ������ */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART3, &USART_InitStructure);
  
//  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
  /*
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  ע��: ��Ҫ�ڴ˴��򿪷����ж�
  �����ж�ʹ����SendUart()������
  */
  USART_Cmd(USART3, ENABLE);		/* ʹ�ܴ��� */
  
  /* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
  �����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
  USART_ClearFlag(USART3, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */
#endif
  
#if UART4_FIFO_EN == 1			/* ����4 TX = PC10   RX = PC11 */
  /* ��1���� ����GPIO */
  
  /* �� GPIO ʱ�� */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  
  /* �� UART ʱ�� */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
  
  /* �� PC10 ӳ��Ϊ UART4_TX */
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);
  
  /* �� PC11 ӳ��Ϊ UART4_RX */
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);
  
  /* ���� USART Tx Ϊ���ù��� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /* ���� USART Rx Ϊ���ù��� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /* ��2���� ���ô���Ӳ������ */
  USART_InitStructure.USART_BaudRate = UART4_BAUD;	/* ������ */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(UART4, &USART_InitStructure);
  
  USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);	/* ʹ�ܽ����ж� */
  USART_DMACmd(UART4,USART_DMAReq_Rx,ENABLE);
  /*
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  ע��: ��Ҫ�ڴ˴��򿪷����ж�
  �����ж�ʹ����SendUart()������
  */
  USART_Cmd(UART4, ENABLE);		/* ʹ�ܴ��� */
  
  /* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
  �����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
  USART_ClearFlag(UART4, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */
#endif
  
#if UART5_FIFO_EN == 1			/* ����5 TX = PC12   RX = PD2 */
  /* ��1���� ����GPIO */
  
  /* �� GPIO ʱ�� */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC |RCC_AHB1Periph_GPIOD, ENABLE);
  
  /* �� UART ʱ�� */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
  
  /* �� PC12 ӳ��Ϊ UART5_TX */
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);
  
  /* �� PD2 ӳ��Ϊ UART5_RX */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);
  
  /* ���� UART Tx Ϊ���ù��� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /* ���� UART Rx Ϊ���ù��� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* ��2���� ���ô���Ӳ������ */
  USART_InitStructure.USART_BaudRate = UART5_BAUD;	/* ������ */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(UART5, &USART_InitStructure);
  
//  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
  USART_ITConfig(UART5, USART_IT_IDLE, ENABLE);	/* ʹ�ܽ����ж� */
  USART_DMACmd(UART5,USART_DMAReq_Rx,ENABLE);
  /*
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  ע��: ��Ҫ�ڴ˴��򿪷����ж�
  �����ж�ʹ����SendUart()������
  */
  USART_Cmd(UART5, ENABLE);		/* ʹ�ܴ��� */
  
  /* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
  �����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
//  USART_ClearFlag(UART5, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */
#endif
  
#if UART6_FIFO_EN == 1			/* PG14/USART6_TX , PC7/USART6_RX,PG8/USART6_RTS, PG15/USART6_CTS */
  /* ��1���� ����GPIO */
  
  /* �� GPIO ʱ�� */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC |RCC_AHB1Periph_GPIOG, ENABLE);
  
  /* �� UART ʱ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
  
  /* �� PG14 ӳ��Ϊ USART6_TX */
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_USART6);
  
  /* �� PC7 ӳ��Ϊ USART6_RX */
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);
  
  /* �� PG8 ӳ��Ϊ USART6_RTS */
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource8, GPIO_AF_USART6);
  
  /* �� PG15 ӳ��Ϊ USART6_CTS */
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource15, GPIO_AF_USART6);
  
  /* ���� PG14/USART6_TX Ϊ���ù��� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  
  /* ���� PC7/USART6_RX Ϊ���ù��� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /* ���� PG8/USART6_RTS Ϊ���ù��� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  
  /* ���� PG15/USART6_CTS Ϊ���ù��� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  
  /* ��2���� ���ô���Ӳ������ */
  USART_InitStructure.USART_BaudRate = UART6_BAUD;	/* ������ */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  //USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;	/* ѡ��Ӳ������ */
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	/* ��ҪӲ������ */
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART6, &USART_InitStructure);
  
  USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
  /*
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  ע��: ��Ҫ�ڴ˴��򿪷����ж�
  �����ж�ʹ����SendUart()������
  */
  USART_Cmd(USART6, ENABLE);		/* ʹ�ܴ��� */
  
  /* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
  �����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
  USART_ClearFlag(USART6, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */
#endif
}

/*
*********************************************************************************************************
*	�� �� ��: ConfigUartNVIC
*	����˵��: ���ô���Ӳ���ж�.
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void ConfigUartNVIC(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Configure the NVIC Preemption Priority Bits */
  /*	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  --- �� bsp.c �� bsp_Init() �������ж����ȼ��� */
  
#if UART1_FIFO_EN == 1
  /* ʹ�ܴ���1�ж� */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif
  
#if UART2_FIFO_EN == 1
  /* ʹ�ܴ���2�ж� */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif
  
#if UART3_FIFO_EN == 1
  /* ʹ�ܴ���3�ж�t */
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif
  
#if UART4_FIFO_EN == 1
  /* ʹ�ܴ���4�ж�t */
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif
  
#if UART5_FIFO_EN == 1
  /* ʹ�ܴ���5�ж�t */
  NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif
  
#if UART6_FIFO_EN == 1
  /* ʹ�ܴ���6�ж�t */
  NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
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
  while (DMA_GetCmdStatus(DMA1_Stream2) != DISABLE);//�ȴ�DMA������ 
  /* ���� DMA Stream */
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //ͨ��ѡ��
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&UART4->DR;//DMA�����ַ
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)UART4_RX_BUF;//DMA �洢��0��ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;//���赽�洢��ģʽ
  DMA_InitStructure.DMA_BufferSize = UART4_RX_BUF_SIZE;//���ݴ����� 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//�������ݳ���:8λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//�洢�����ݳ���:8λ
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// ʹ����ͨģʽ 
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//�е����ȼ�
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//�洢��ͻ�����δ���
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//����ͻ�����δ���
  DMA_Init(DMA1_Stream2, &DMA_InitStructure);//��ʼ��DMA Stream
  DMA_Cmd(DMA1_Stream2, ENABLE);  //����DMA���� 
  
  DMA_DeInit(DMA1_Stream5);
  while (DMA_GetCmdStatus(DMA1_Stream5) != DISABLE);//�ȴ�DMA������ 
  /* ���� DMA Stream */
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //ͨ��ѡ��
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART2->DR;//DMA�����ַ
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)UART2_RX_BUF;//DMA �洢��0��ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;//���赽�洢��ģʽ
  DMA_InitStructure.DMA_BufferSize = UART2_RX_BUF_SIZE;//���ݴ����� 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//�������ݳ���:8λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//�洢�����ݳ���:8λ
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// ʹ����ͨģʽ 
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//�е����ȼ�
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//�洢��ͻ�����δ���
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//����ͻ�����δ���
  DMA_Init(DMA1_Stream5, &DMA_InitStructure);//��ʼ��DMA Stream
  DMA_Cmd(DMA1_Stream5, ENABLE);  //����DMA���� 
  
  DMA_DeInit(DMA1_Stream0);
  while (DMA_GetCmdStatus(DMA1_Stream0) != DISABLE);//�ȴ�DMA������ 
  /* ���� DMA Stream */
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //ͨ��ѡ��
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&UART5->DR;//DMA�����ַ
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)UART5_RX_BUF;//DMA �洢��0��ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;//���赽�洢��ģʽ
  DMA_InitStructure.DMA_BufferSize = UART5_RX_BUF_SIZE;//���ݴ����� 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//�������ݳ���:8λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//�洢�����ݳ���:8λ
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// ʹ����ͨģʽ 
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//�е����ȼ�
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//�洢��ͻ�����δ���
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//����ͻ�����δ���
  DMA_Init(DMA1_Stream0, &DMA_InitStructure);//��ʼ��DMA Stream
  DMA_Cmd(DMA1_Stream0, ENABLE);  //����DMA���� 
}  
/*
*********************************************************************************************************
*	�� �� ��: UartSend
*	����˵��: ��д���ݵ�UART���ͻ�����,�����������жϡ��жϴ�����������Ϻ��Զ��رշ����ж�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen)
{
  uint16_t i;
  
  for (i = 0; i < _usLen; i++)
  {
    /* ������ͻ������Ѿ����ˣ���ȴ��������� */
#if 0
    /*
    �ڵ���GPRS����ʱ������Ĵ������������while ��ѭ��
    ԭ�� ���͵�1���ֽ�ʱ _pUart->usTxWrite = 1��_pUart->usTxRead = 0;
    ������while(1) �޷��˳�
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
    /* �� _pUart->usTxBufSize == 1 ʱ, ����ĺ���������(������) */
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
    
    /* �����������뷢�ͻ����� */
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
*	�� �� ��: UartGetChar
*	����˵��: �Ӵ��ڽ��ջ�������ȡ1�ֽ����� ��������������ã�
*	��    ��: _pUart : �����豸
*			  _pByte : ��Ŷ�ȡ���ݵ�ָ��
*	�� �� ֵ: 0 ��ʾ������  1��ʾ��ȡ������
*********************************************************************************************************
*/
static uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte)
{
  uint16_t usCount;
  
  /* usRxWrite �������жϺ����б���д���������ȡ�ñ���ʱ����������ٽ������� */
  DISABLE_INT();
  usCount = _pUart->usRxCount;
  ENABLE_INT();
  
  /* �������д������ͬ���򷵻�0 */
  //if (_pUart->usRxRead == usRxWrite)
  if (usCount == 0)	/* �Ѿ�û������ */
  {
    return 0;
  }
  else
  {
    *_pByte = _pUart->pRxBuf[_pUart->usRxRead];		/* �Ӵ��ڽ���FIFOȡ1������ */
    
    /* ��дFIFO������ */
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
*	�� �� ��: UartIRQ
*	����˵��: ���жϷ��������ã�ͨ�ô����жϴ�����
*	��    ��: _pUart : �����豸
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void UartIRQ(UART_T *_pUart)
{
  /* ��������ж�  */
  if (USART_GetITStatus(_pUart->uart, USART_IT_RXNE) != RESET)
  {
    /* �Ӵ��ڽ������ݼĴ�����ȡ���ݴ�ŵ�����FIFO */
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
    
    /* �ص�����,֪ͨӦ�ó����յ�������,һ���Ƿ���1����Ϣ��������һ����� */
    //if (_pUart->usRxWrite == _pUart->usRxRead)
    //if (_pUart->usRxCount == 1)
    {
      if (_pUart->ReciveNew)
      {
        _pUart->ReciveNew(ch);
      }
    }
  }
  
  /* �����ͻ��������ж� */
  if (USART_GetITStatus(_pUart->uart, USART_IT_TXE) != RESET)
  {
    //if (_pUart->usTxRead == _pUart->usTxWrite)
    if (_pUart->usTxCount == 0)
    {
      /* ���ͻ�������������ȡ��ʱ�� ��ֹ���ͻ��������ж� ��ע�⣺��ʱ���1�����ݻ�δ����������ϣ�*/
      USART_ITConfig(_pUart->uart, USART_IT_TXE, DISABLE);
      
      /* ʹ�����ݷ�������ж� */
      USART_ITConfig(_pUart->uart, USART_IT_TC, ENABLE);
    }
    else
    {
      /* �ӷ���FIFOȡ1���ֽ�д�봮�ڷ������ݼĴ��� */
      USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
      if (++_pUart->usTxRead >= _pUart->usTxBufSize)
      {
        _pUart->usTxRead = 0;
      }
      _pUart->usTxCount--;
    }
    
  }
  /* ����bitλȫ��������ϵ��ж� */
  else if (USART_GetITStatus(_pUart->uart, USART_IT_TC) != RESET)
  {
    //if (_pUart->usTxRead == _pUart->usTxWrite)
    if (_pUart->usTxCount == 0)
    {
      /* �������FIFO������ȫ��������ϣ���ֹ���ݷ�������ж� */
      USART_ITConfig(_pUart->uart, USART_IT_TC, DISABLE);
      
      /* �ص�����, һ����������RS485ͨ�ţ���RS485оƬ����Ϊ����ģʽ��������ռ���� */
      if (_pUart->SendOver)
      {
        _pUart->SendOver();
      }
    }
    else
    {
      /* ��������£��������˷�֧ */
      
      /* �������FIFO�����ݻ�δ��ϣ���ӷ���FIFOȡ1������д�뷢�����ݼĴ��� */
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
*	�� �� ��: USART1_IRQHandler  USART2_IRQHandler USART3_IRQHandler UART4_IRQHandler UART5_IRQHandler
*	����˵��: USART�жϷ������
*	��    ��: ��
*	�� �� ֵ: ��
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
//    /* ����ͬ���ź� */
//    xSemaphoreGiveFromISR(xSemaphore_rs485, &xHigherPriorityTaskWoken);
//    /* ���xHigherPriorityTaskWoken = pdTRUE����ô�˳��жϺ��е���ǰ������ȼ�����ִ�� */
//    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}
#endif

#if UART3_FIFO_EN == 1
void USART3_IRQHandler(void)
{
  UartIRQ(&g_tUart3);
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
    /* ����ͬ���ź� */
    xSemaphoreGiveFromISR(xSemaphore_lcd, &xHigherPriorityTaskWoken);
    /* ���xHigherPriorityTaskWoken = pdTRUE����ô�˳��жϺ��е���ǰ������ȼ�����ִ�� */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}
#endif

#if UART5_FIFO_EN == 1
void UART5_IRQHandler(void)
{
  u16 data;
  u16 UART_ReceiveSize = 0;
  if(USART_GetITStatus(UART5,USART_IT_IDLE) != RESET)
  {
    DMA_Cmd(DMA1_Stream0, DISABLE);
    data = UART5->SR;
    data = UART5->DR;
    data = data;
    UART_ReceiveSize = UART5_RX_BUF_SIZE - DMA_GetCurrDataCounter(DMA1_Stream0);
    rfid_rev_cnt = UART_ReceiveSize;
    memcpy(rfid_rev_buf,UART5_RX_BUF,rfid_rev_cnt);
//    UART1ToPC(rfid_rev_buf,rfid_rev_cnt);
    DMA_ClearFlag(DMA1_Stream0,DMA_FLAG_TCIF0 | DMA_FLAG_FEIF0 | DMA_FLAG_DMEIF0 | DMA_FLAG_TEIF0 | DMA_FLAG_HTIF0);//??DMA1_Steam3??????
    DMA_SetCurrDataCounter(DMA1_Stream0, UART5_RX_BUF_SIZE);
    DMA_Cmd(DMA1_Stream0, ENABLE);
    rfid_rev_flag = 1;
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
  for(i = 0; i < len; i++) 
  {
//    while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
    USART_SendData(UART4, buf[i]);
    while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);
  }
}

void UART5ToPC(u8 *buf,u8 len)
{
  unsigned char i;
  for(i = 0; i < len; i++) 
  {
//    while(USART_GetFlagStatus(UART5, USART_FLAG_TXE) == RESET);
    USART_SendData(UART5, buf[i]);
    while (USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);
  }
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

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
