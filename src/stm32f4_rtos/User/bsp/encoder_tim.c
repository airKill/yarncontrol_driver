/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : stm32f10x_encoder.c 
* Author             : IMS Systems Lab  
* Date First Issued  : 21/11/07
* Description        : This file contains the software implementation for the
*                      encoder unit
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
/* Includes ------------------------------------------------------------------*/
#include "includes.h"

/* Private functions ---------------------------------------------------------*/
s16 ENC_Calc_Rot_Speed(void);

/* Private variables ---------------------------------------------------------*/
s16 hPrevious_angle, hSpeed_Buffer[SPEED_BUFFER_SIZE], hRot_Speed;
u8 bSpeed_Buffer_Index = 0;
u16 hEncoder_Timer_Overflow; 
u8 bIs_First_Measurement = 1;

/*******************************************************************************
* Function Name  : ENC_Init
* Description    : General Purpose Timer x set-up for encoder speed/position 
*                  sensors
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ENC_Init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_ICInitTypeDef TIM_ICInitStructure;
  
/* Encoder unit connected to TIM3, 4X mode */    
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* TIM3 clock source enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);	//ê1?üTIM5ê±?ó
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		/* 设为输出口 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;		/* 设为推挽模式 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* 上下拉电阻不使能 */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO口最大速度 */
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ENCODERA;
  GPIO_Init(GPIO_PORT_ENCODERA, &GPIO_InitStructure);					 //PA0 ??à-
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ENCODERB;
  GPIO_Init(GPIO_PORT_ENCODERB, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM8);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM8);
  
  /* Enable the TIM3 Update Interrupt */
//  NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 9;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);

  /* Timer configuration in Encoder mode */
  TIM_DeInit(ENCODER_TIMER);
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  
  TIM_TimeBaseStructure.TIM_Prescaler = 0;  // 42 prescaling 
//  TIM_TimeBaseStructure.TIM_Period = 3600;
  TIM_TimeBaseStructure.TIM_Period = (4 * ENCODER_PPR) - 1;  
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   
  TIM_TimeBaseInit(ENCODER_TIMER, &TIM_TimeBaseStructure);
 
  TIM_EncoderInterfaceConfig(ENCODER_TIMER, TIM_EncoderMode_TI12, 
                             TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
  TIM_ICStructInit(&TIM_ICInitStructure);
//  TIM_ICInitStructure.TIM_Channel = TIM_Channel_1 | TIM_Channel_2;
//  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
//  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
//  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = ICx_FILTER;
  TIM_ICInit(ENCODER_TIMER, &TIM_ICInitStructure);
  
 // Clear all pending interrupts
  TIM_ClearFlag(ENCODER_TIMER, TIM_FLAG_Update);
//  TIM_ITConfig(ENCODER_TIMER, TIM_IT_Update, ENABLE);
  //Reset counter
  TIM_SetCounter(TIM8,COUNTER_RESET);
  ENC_Clear_Speed_Buffer();
  
  TIM_Cmd(ENCODER_TIMER, ENABLE);  
}

/*******************************************************************************
* Function Name  : ENC_Get_Electrical_Angle
* Description    : Returns the absolute electrical Rotor angle 
* Input          : None
* Output         : None
* Return         : Rotor electrical angle: 0 -> 0 degrees, 
*                                          S16_MAX-> 180 degrees, 
*                                          S16_MIN-> -180 degrees                  
*******************************************************************************/
//s16 ENC_Get_Electrical_Angle(void)
//{
//  s32 temp;
//  
//  temp = (s32)(TIM_GetCounter(ENCODER_TIMER)) * (s32)(U32_MAX / ENCODER_PPR); 
//  return((s16)(temp/65536)); // s16 result
//}

/*******************************************************************************
* Function Name  : ENC_Clear_Speed_Buffer
* Description    : Clear speed buffer used for average speed calculation  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ENC_Clear_Speed_Buffer(void)
{   
  u32 i;

  for (i=0;i<SPEED_BUFFER_SIZE;i++)
  {
    hSpeed_Buffer[i] = 0;
  }
  bIs_First_Measurement = 1;
}

/*******************************************************************************
* Function Name  : ENC_Calc_Rot_Speed
* Description    : Compute return latest speed measurement 
* Input          : None
* Output         : s16
* Return         : Return the speed in 0.1 Hz resolution.                    
*******************************************************************************/
s16 ENC_Calc_Rot_Speed(void)
{
  s32 wDelta_angle;
  u16 hEnc_Timer_Overflow_sample_one, hEnc_Timer_Overflow_sample_two;
  u16 hCurrent_angle_sample_one, hCurrent_angle_sample_two;
  signed long long temp;
  s16 haux;
  
  if(bIs_First_Measurement == 0)
  {
    // 1st reading of overflow counter    
    hEnc_Timer_Overflow_sample_one = hEncoder_Timer_Overflow; 
    // 1st reading of encoder timer counter
    hCurrent_angle_sample_one = ENCODER_TIMER->CNT;
    // 2nd reading of overflow counter
    hEnc_Timer_Overflow_sample_two = hEncoder_Timer_Overflow;  
    // 2nd reading of encoder timer counter
    hCurrent_angle_sample_two = ENCODER_TIMER->CNT;      

    // Reset hEncoder_Timer_Overflow and read the counter value for the next
    // measurement
    hEncoder_Timer_Overflow = 0;
    haux = ENCODER_TIMER->CNT;
    
    if(hEncoder_Timer_Overflow != 0)
    {
      haux = ENCODER_TIMER->CNT; 
      hEncoder_Timer_Overflow = 0;            
    }
     
    if(hEnc_Timer_Overflow_sample_one != hEnc_Timer_Overflow_sample_two)
    { //Compare sample 1 & 2 and check if an overflow has been generated right 
      //after the reading of encoder timer. If yes, copy sample 2 result in 
      //sample 1 for next process 
      hCurrent_angle_sample_one = hCurrent_angle_sample_two;
      hEnc_Timer_Overflow_sample_one = hEnc_Timer_Overflow_sample_two;
    }
    
    if((ENCODER_TIMER->CR1 & TIM_CounterMode_Down) == TIM_CounterMode_Down)
    {// encoder timer down-counting
      wDelta_angle = (s32)(hCurrent_angle_sample_one - hPrevious_angle - 
                    (hEnc_Timer_Overflow_sample_one) * (4 * ENCODER_PPR));
    }
    else  
    {//encoder timer up-counting
      wDelta_angle = (s32)(hCurrent_angle_sample_one - hPrevious_angle + 
                    (hEnc_Timer_Overflow_sample_one) * (4 * ENCODER_PPR));
    }
    
    // speed computation as delta angle * 1/(speed sempling time)
    temp = (signed long long)(wDelta_angle * 6000);
//    temp *= 10;  // 0.1 Hz resolution
    temp /= (4 * ENCODER_PPR);
        
  } //is first measurement, discard it
  else
  {
    bIs_First_Measurement = 0;
    temp = 0;
    hEncoder_Timer_Overflow = 0;
    haux = ENCODER_TIMER->CNT;       
    // Check if Encoder_Timer_Overflow is still zero. In case an overflow IT 
    // occured it resets overflow counter and wPWM_Counter_Angular_Velocity
    if (hEncoder_Timer_Overflow != 0) 
    {
      haux = ENCODER_TIMER->CNT; 
      hEncoder_Timer_Overflow = 0;            
    }
  }
  
  hPrevious_angle = haux;  
 
  return((s16) temp);
}

/*******************************************************************************
* Function Name  : ENC_Calc_Average_Speed
* Description    : Compute smoothed motor speed based on last SPEED_BUFFER_SIZE
                   informations and store it variable  
* Input          : None
* Output         : s16
* Return         : Return rotor speed in 0.1 Hz resolution. This routine 
                   will return the average mechanical speed of the motor.
*******************************************************************************/
u16 ENC_Calc_Average_Speed(void)
{   
  u16 wtemp,temp;

  u8 i,j;
  u16 sum = 0;
  wtemp = ENC_Calc_Rot_Speed();

  if(wtemp < 1600)
    hSpeed_Buffer[bSpeed_Buffer_Index++] = (s16)wtemp;
  if(bSpeed_Buffer_Index >= SPEED_BUFFER_SIZE)
    bSpeed_Buffer_Index = 0;
  
  //递推平均滤波（滑动平均滤波法）
  for(i=0;i<SPEED_BUFFER_SIZE;i++)
    sum = sum + hSpeed_Buffer[i];
  return (sum / SPEED_BUFFER_SIZE);
}

/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIMx Update interrupt request.
                   Encoder unit connected to TIM2
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void TIM8_UP_TIM13_IRQHandler(void)
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  TIM_ClearFlag(ENCODER_TIMER, TIM_FLAG_Update);
  /* Clear the interrupt pending flag */
//  if(TIM_GetITStatus(ENCODER_TIMER,TIM_IT_Update) == SET)
  {
    if(hEncoder_Timer_Overflow != 65535)
    {
      hEncoder_Timer_Overflow++;
    }
//    xSemaphoreGiveFromISR(xSemaphore_encoder, &xHigherPriorityTaskWoken);
//    /* 如果xHigherPriorityTaskWoken = pdTRUE，那么退出中断后切到当前最高优先级任务执行 */
//    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
