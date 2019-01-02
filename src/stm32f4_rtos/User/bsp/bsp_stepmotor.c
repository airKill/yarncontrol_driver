#include "bsp.h"

void bsp_InitStepMotor(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_STEPMOTOR1_EN, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_STEPMOTOR2_EN, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_STEPMOTOR3_EN, ENABLE);
  
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource0,GPIO_AF_TIM3);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_TIM1);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_TIM1);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* 设为输出口 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽模式 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 上下拉电阻不使能 */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO口最大速度 */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_STEPMOTOR1_EN;
  GPIO_Init(GPIO_PORT_STEPMOTOR1_EN, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_STEPMOTOR1_DIR;
  GPIO_Init(GPIO_PORT_STEPMOTOR1_DIR, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_STEPMOTOR2_EN;
  GPIO_Init(GPIO_PORT_STEPMOTOR2_EN, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_STEPMOTOR2_DIR;
  GPIO_Init(GPIO_PORT_STEPMOTOR2_DIR, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_STEPMOTOR3_EN;
  GPIO_Init(GPIO_PORT_STEPMOTOR3_EN, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_STEPMOTOR3_DIR;
  GPIO_Init(GPIO_PORT_STEPMOTOR3_DIR, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		/* 设为输出口 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽模式 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* 上下拉电阻不使能 */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO口最大速度 */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_STEPMOTOR1_CP;
  GPIO_Init(GPIO_PORT_STEPMOTOR1_CP, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_STEPMOTOR2_CP;
  GPIO_Init(GPIO_PORT_STEPMOTOR2_CP, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_STEPMOTOR3_CP;
  GPIO_Init(GPIO_PORT_STEPMOTOR3_CP, &GPIO_InitStructure);
  
  TIM3_PWM_SETPMOTOR();
  TIM1_PWM_SETPMOTOR();
  
  STEPMOTOR3_EN_L();
  STEPMOTOR3_DIR_L();
}

void TIM3_PWM_SETPMOTOR(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);   //Open TIM3  Clock

  TIM_TimeBaseStructure.TIM_Prescaler = 3;          //定时器时钟60MHZ/(3+1)=15
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM3 Count mode
  TIM_TimeBaseStructure.TIM_Period = 999;         //Fout_clk=Fclk_cnt/(ARR+1)=15000000/1500=10KHZ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV2;   
  
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: TIM3_CH1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //select PWM1 mode
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //config oc1 as output 
  TIM_OCInitStructure.TIM_Pulse = 0;                            //config TIM3_CCR1 vaule
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //config oc1 high level avaliable
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);
  
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);         // turn on oc1 preload 
  TIM_ARRPreloadConfig(TIM3, ENABLE);
  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);
}

void TIM1_PWM_SETPMOTOR(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);   //Open TIM3  Clock

  TIM_TimeBaseStructure.TIM_Prescaler = 2;          //定时器时钟60MHZ/(3+1)=15
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM3 Count mode
  TIM_TimeBaseStructure.TIM_Period = 1000;         //Fout_clk=Fclk_cnt/(ARR+1)=15000000/1500=10KHZ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV2;   
  
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: TIM3_CH1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //select PWM1 mode
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //config oc1 as output 
  TIM_OCInitStructure.TIM_Pulse = 0;                            //config TIM3_CCR1 vaule
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //config oc1 high level avaliable
  TIM_OC2Init(TIM1, &TIM_OCInitStructure);
  TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);         // turn on oc1 preload 
  
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //select PWM1 mode
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //config oc1 as output 
  TIM_OCInitStructure.TIM_Pulse = 0;                            //config TIM3_CCR1 vaule
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //config oc1 high level avaliable
  TIM_OC4Init(TIM1, &TIM_OCInitStructure);
  TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);         // turn on oc1 preload 
  
  TIM_ARRPreloadConfig(TIM1, ENABLE);
  /* TIM3 enable counter */
  TIM_Cmd(TIM1, ENABLE);
  TIM_SetCompare4(TIM1,50);
  TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

