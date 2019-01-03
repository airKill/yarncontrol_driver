/**
******************************************************************************
* @file    Project/STM32F0xx_StdPeriph_Templates/stm32f0xx_it.c 
* @author  MCD Application Team
* @version V1.5.0
* @date    05-December-2014
* @brief   Main Interrupt Service Routines.
*          This file provides template for all exceptions handler and 
*          peripherals interrupt service routine.
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
*
* Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at:
*
*        http://www.st.com/software_license_agreement_liberty_v2
*
* Unless required by applicable law or agreed to in writing, software 
* distributed under the License is distributed on an "AS IS" BASIS, 
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "main.h"

extern SemaphoreHandle_t  xSemaphore_rs485;
/** @addtogroup Template_Project
* @{
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
* @brief  This function handles NMI exception.
* @param  None
* @retval None
*/
void NMI_Handler(void)
{
}

/**
* @brief  This function handles Hard Fault exception.
* @param  None
* @retval None
*/
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
    printf("HardFault_Handler\r\n");
  }
}

/**
* @brief  This function handles SVCall exception.
* @param  None
* @retval None
*/
//void SVC_Handler(void)
//{
//}

/**
* @brief  This function handles PendSVC exception.
* @param  None
* @retval None
*/
//void PendSV_Handler(void)
//{
//}

/**
* @brief  This function handles SysTick Handler.
* @param  None
* @retval None
*/
//void SysTick_Handler(void)
//{
//}

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                            */
/******************************************************************************/

/**
* @brief  This function handles PPP interrupt request.
* @param  None
* @retval None
*/
/*void PPP_IRQHandler(void)
{
}*/

/**
* @}
*/ 

void USART1_IRQHandler(void)
{
  u16 data;
  u8 tmp;
  if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)
  {
    USART_ClearITPendingBit(USART1,USART_IT_RXNE);
    tmp = USART_ReceiveData(USART1); 
    UART1_RX_BUF[Uart1_rx_count] = tmp;
    Uart1_rx_count++;
  }
  if(USART_GetITStatus(USART1,USART_IT_IDLE) != RESET)
  {    
    data = USART1->SR;
    data = USART1->DR;
    data = data;
    uart1_debug();
    Uart1_rx_count = 0;
  }
}  

void USART2_IRQHandler(void)
{
  u16 data;
  u8 tmp;
  u16 UART_ReceiveSize = 0;
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  if(USART_GetITStatus(USART2,USART_IT_IDLE) != RESET)
  {    
    data = USART2->SR;
    data = USART2->DR;
    data = data;

    UART_ReceiveSize = UART2_MAX_RECV_LEN - DMA_GetCurrDataCounter(DMA1_Channel6);
    DMA_Cmd(DMA1_Channel6, DISABLE);
    DMA_ClearFlag(DMA1_FLAG_GL6 | DMA1_FLAG_TC6 | DMA1_FLAG_HT6 | DMA1_FLAG_TE6);//清除DMA1_Steam3传输完成标志
    DMA_SetCurrDataCounter(DMA1_Channel6,UART2_MAX_RECV_LEN);

    if(UART_ReceiveSize == 7)
    {
      if((UART2_RX_BUF[0] == device_addr) || (UART2_RX_BUF[0] == BROADCAST))
      {
        xSemaphoreGiveFromISR(xSemaphore_rs485, &xHigherPriorityTaskWoken);
        /* 如果xHigherPriorityTaskWoken = pdTRUE，那么退出中断后切到当前最高优先级任务执行 */
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
        m_ctrl_dev.frameok = 1;
        memcpy(m_ctrl_dev.rxbuf,UART2_RX_BUF,UART_ReceiveSize);
        m_ctrl_dev.rxlen = UART_ReceiveSize;
      }
    }
    DMA_Cmd(DMA1_Channel6, ENABLE);
  }
}

//void USART3_IRQHandler(void)
//{
//  u16 data;
//  u8 tmp;
//  u16 UART_ReceiveSize = 0;
//  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//  if(USART_GetITStatus(USART3,USART_IT_IDLE) != RESET)
//  {    
//    data = USART3->SR;
//    data = USART3->DR;
//    data = data;
//
//    UART_ReceiveSize = UART3_MAX_RECV_LEN - DMA_GetCurrDataCounter(DMA1_Channel3);
//    DMA_Cmd(DMA1_Channel3, DISABLE);
//    DMA_ClearFlag(DMA1_FLAG_GL3 | DMA1_FLAG_TC3 | DMA1_FLAG_HT3 | DMA1_FLAG_TE3);//清除DMA1_Steam3传输完成标志
//    DMA_SetCurrDataCounter(DMA1_Channel3,UART3_MAX_RECV_LEN);
//    if(UART_ReceiveSize == 7)
//    {
//      if(UART3_RX_BUF[0] == device_addr)
//      {
//        xSemaphoreGiveFromISR(xSemaphore_rs485, &xHigherPriorityTaskWoken);
//        /* 如果xHigherPriorityTaskWoken = pdTRUE，那么退出中断后切到当前最高优先级任务执行 */
//        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
//        m_ctrl_dev.frameok = 1;
//        memcpy(m_ctrl_dev.rxbuf,UART3_RX_BUF,UART_ReceiveSize);
//        m_ctrl_dev.rxlen = UART_ReceiveSize;
//      }
//    }
//    DMA_Cmd(DMA1_Channel3, ENABLE);
//    
//  }
//}

void TIM3_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM3, TIM_IT_CC1))
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
    TIM_ITConfig(TIM3, TIM_IT_CC1, DISABLE);	
    
    s_TIM_CallBack1();
  }
  
  if (TIM_GetITStatus(TIM3, TIM_IT_CC2))
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
    TIM_ITConfig(TIM3, TIM_IT_CC2, DISABLE);	
    
    s_TIM_CallBack2();
  }
  
  if (TIM_GetITStatus(TIM3, TIM_IT_CC3))
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
    TIM_ITConfig(TIM3, TIM_IT_CC3, DISABLE);	
    
    s_TIM_CallBack3();
  }
  
  if (TIM_GetITStatus(TIM3, TIM_IT_CC4))
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC4);
    TIM_ITConfig(TIM3, TIM_IT_CC4, DISABLE);	
    
    s_TIM_CallBack4();
  }
}

