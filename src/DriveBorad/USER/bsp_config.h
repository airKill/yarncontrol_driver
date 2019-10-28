/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_CONFIG_H
#define __BSP_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>

#define PUTCHAR_PROTOTYPE int   fputc(int ch, FILE *f)

#define UART1_MAX_RECV_LEN      50
#define UART2_MAX_RECV_LEN      50
#define UART3_MAX_RECV_LEN      50

#define LED1_PORT       GPIOA
#define LED1_PIN        GPIO_Pin_8

#define SWITCH_LED_PORT       GPIOB
#define SWITCH_LED_PIN        GPIO_Pin_11
//
#define LED1_ON()       GPIO_SetBits(LED1_PORT,LED1_PIN) 
#define LED1_OFF()       GPIO_ResetBits(LED1_PORT,LED1_PIN) 

#define SWITCH_LED_ON()       GPIO_SetBits(SWITCH_LED_PORT,SWITCH_LED_PIN) 
#define SWITCH_LED_OFF()       GPIO_ResetBits(SWITCH_LED_PORT,SWITCH_LED_PIN) 
//
//#define LED2_PORT       GPIOG
//#define LED2_PIN        GPIO_Pin_6
//
//#define LED2_ON()       GPIO_SetBits(LED2_PORT,LED2_PIN) 
//#define LED2_OFF()       GPIO_ResetBits(LED2_PORT,LED2_PIN) 
//
//#define LED3_PORT       GPIOG
//#define LED3_PIN        GPIO_Pin_7
//
//#define LED3_ON()       GPIO_SetBits(LED3_PORT,LED3_PIN) 
//#define LED3_OFF()       GPIO_ResetBits(LED3_PORT,LED3_PIN) 
#define RS485_EN_PORT       GPIOA
#define RS485_EN_PIN        GPIO_Pin_1

//#define RS485_EN_PORT       GPIOB
//#define RS485_EN_PIN        GPIO_Pin_2

#define RS485_TX()     GPIO_SetBits(RS485_EN_PORT,RS485_EN_PIN)
#define RS485_RX()     GPIO_ResetBits(RS485_EN_PORT,RS485_EN_PIN)


extern u8 UART1_RX_BUF[UART1_MAX_RECV_LEN];
extern u16 Uart1_rx_count;

extern u8 UART2_RX_BUF[UART2_MAX_RECV_LEN];
extern u16 Uart2_rx_count;

extern u8 UART3_RX_BUF[UART3_MAX_RECV_LEN];
extern u16 Uart3_rx_count;

void Switch_LED_init(void);
void DMA_Config(void);
void watchdog(void);
void IWDG_Feed(void);
void RS485_Init(void);
void Printf_Init(void);
void bsp_init(void);
void LED_init(void);
void TIM2_Configuration(void);
void InterruptConfig(void);
void UART1_TO_PC(u8 *buf,u16 len);
void GPIO_ToggleBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void RS485_TO_PC(u8 *buf,u16 len);
#endif 