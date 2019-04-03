/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : stm32f10x_encoder.h
* Author             : IMS Systems Lab 
* Date First Issued  : 21/11/07
* Description        : This file contains the software implementation for the
*                      encoder position and speed reading.
********************************************************************************
* History:
* 21/11/07 v1.0
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ENCODER_TIM_H
#define __ENCODER_TIM_H

/* Private typedef -----------------------------------------------------------*/
typedef enum {DISPLAY_TIMCNT = 0,DISPLAY_THETA,DISPLAY_W} DisplayType;

/* Includes ------------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SPEED_SAMPLING_TIME  9     // (9+1)*500usec = 10msec

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ENCODER_TIMER   TIM8  // Encoder unit connected to TIM3
#define ENCODER_PPR           (u16)(600)   // number of pulses per revolution
#define SPEED_BUFFER_SIZE 12

#define COUNTER_RESET   (u16)0
#define ICx_FILTER      (u8) 6 // 6<-> 670nsec

#define TIMx_PRE_EMPTION_PRIORITY 1
#define TIMx_SUB_PRIORITY 0

#define SPEED_SAMPLING_FREQ (u16)(2000 / (SPEED_SAMPLING_TIME + 1))

#define GPIO_PORT_ENCODERA      GPIOC
#define GPIO_PIN_ENCODERA	 GPIO_Pin_7

#define GPIO_PORT_ENCODERB      GPIOC
#define GPIO_PIN_ENCODERB	 GPIO_Pin_6

/* Exported functions ------------------------------------------------------- */
void ENC_Init(void);
s16 ENC_Get_Electrical_Angle(void);
void ENC_Clear_Speed_Buffer(void);
u16 ENC_Calc_Average_Speed(void);
void LCD_Display(DisplayType DisplayStatus);
s16 ENC_Get_Speed(void);
void TIM3_IRQHandler(void);;

#endif  /*__STM32F10x_ENCODER_H*/
/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
