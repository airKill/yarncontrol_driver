#include "bsp.h"

void bsp_InitServoMotor(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_SERVOMOTOR, ENABLE);
  
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_TIM12);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_TIM4);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		/* 设为输出口 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽模式 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* 上下拉电阻不使能 */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO口最大速度 */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR1;
  GPIO_Init(GPIO_PORT_SERVOMOTOR1, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR2;
  GPIO_Init(GPIO_PORT_SERVOMOTOR2, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR3;
  GPIO_Init(GPIO_PORT_SERVOMOTOR3, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR4;
  GPIO_Init(GPIO_PORT_SERVOMOTOR4, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* 设为输出口 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽模式 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* 上下拉电阻不使能 */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO口最大速度 */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR_G;
  GPIO_Init(GPIO_PORT_SERVOMOTOR_G, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR_G0;
  GPIO_Init(GPIO_PORT_SERVOMOTOR_G0, &GPIO_InitStructure);
}

void TIM12_PWM_SERVOMOTOR(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);   //Open TIM3  Clock

  TIM_TimeBaseStructure.TIM_Prescaler = 3;          //定时器时钟60MHZ/(3+1)=15
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM3 Count mode
  TIM_TimeBaseStructure.TIM_Period = 999;         //Fout_clk=Fclk_cnt/(ARR+1)=15000000/1500=10KHZ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV2;   
  
  TIM_TimeBaseInit(TIM12, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: TIM3_CH1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //select PWM1 mode
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //config oc1 as output 
  TIM_OCInitStructure.TIM_Pulse = 0;                            //config TIM3_CCR1 vaule
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //config oc1 high level avaliable
  TIM_OC1Init(TIM12, &TIM_OCInitStructure);
  
  TIM_OC1PreloadConfig(TIM12, TIM_OCPreload_Enable);         // turn on oc1 preload 
  TIM_ARRPreloadConfig(TIM12, ENABLE);
  /* TIM3 enable counter */
  TIM_Cmd(TIM12, ENABLE);
}

void TIM4_PWM_SERVOMOTOR(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);   //Open TIM3  Clock

  TIM_TimeBaseStructure.TIM_Prescaler = 3;          //定时器时钟60MHZ/(3+1)=15
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM3 Count mode
  TIM_TimeBaseStructure.TIM_Period = 999;         //Fout_clk=Fclk_cnt/(ARR+1)=15000000/1500=10KHZ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV2;   
  
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: TIM3_CH1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //select PWM1 mode
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //config oc1 as output 
  TIM_OCInitStructure.TIM_Pulse = 0;                            //config TIM3_CCR1 vaule
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //config oc1 high level avaliable
  
  TIM_OC1Init(TIM4, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);         // turn on oc1 preload 
  
  TIM_OC4Init(TIM4, &TIM_OCInitStructure);
  TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
  
  TIM_OC2Init(TIM4, &TIM_OCInitStructure);
  TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
  
  TIM_ARRPreloadConfig(TIM4, ENABLE);
  /* TIM3 enable counter */
  TIM_Cmd(TIM4, ENABLE);
}

