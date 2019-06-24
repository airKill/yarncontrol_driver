#include "main.h"

u8 motor_dir = MOTOR_STOP;
u8 old_motor_dir = MOTOR_STOP;

void motor_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* 使能(开启)KEY1引脚对应IO端口时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
  //  GPIO_InitStructure.GPIO_Pin = MOTOR_EN_GPIO_PIN;
  //  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  //  GPIO_Init(MOTOR_EN_GPIO_PORT, &GPIO_InitStructure);
  TIM3_PWM_Init(899,10);
  
  GPIO_InitStructure.GPIO_Pin = MOTOR_IN1_GPIO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(MOTOR_IN1_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = MOTOR_IN2_GPIO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(MOTOR_IN2_GPIO_PORT, &GPIO_InitStructure);
  motor_control(MOTOR_STOP);
}

void TIM3_PWM_Init(u16 arr,u16 psc)
{  
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  
  
  GPIO_InitStructure.GPIO_Pin = MOTOR_EN_GPIO_PIN; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(MOTOR_EN_GPIO_PORT, &GPIO_InitStructure);
  
  TIM_TimeBaseStructure.TIM_Period = arr; 
  TIM_TimeBaseStructure.TIM_Prescaler = psc; 
  TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 
  
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);  
  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  
  TIM_Cmd(TIM3, ENABLE); 
}

void motor_speed(u16 speed)
{
  TIM_SetCompare1(TIM3,speed);		
}

void motor_pwm_close(void)
{
  TIM_Cmd(TIM3, DISABLE); 
  //  TIM_ForcedOC1Config(TIM3,TIM_ForcedAction_Active);
}

void motor_control(u8 cmd)
{
  if(cmd == MOTOR_STOP)
  {
    TIM_Cmd(TIM3, DISABLE); 
    MOTOR_EN_H();
    MOTOR_IN1_H();
    MOTOR_IN2_H();
  }
  else if(cmd == MOTOR_FORWARD)
  {
    //    MOTOR_EN_H();
    TIM_Cmd(TIM3, ENABLE); 
    MOTOR_IN1_H();
    MOTOR_IN2_L();
  }
  else if(cmd == MOTOR_REVERSE)
  {
    //    MOTOR_EN_H();
    TIM_Cmd(TIM3, ENABLE); 
    MOTOR_IN1_L();
    MOTOR_IN2_H();
  }
  else if(cmd == MOTOR_BREAK)
  {
    MOTOR_EN_H();
    MOTOR_IN1_L();
    MOTOR_IN2_L();
  }
}

