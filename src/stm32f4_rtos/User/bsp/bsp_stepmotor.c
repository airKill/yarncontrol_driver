#include "bsp.h"

void bsp_InitStepMotor(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_STEPMOTOR1_EN, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_STEPMOTOR2_EN, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_STEPMOTOR3_EN, ENABLE);
  
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource0,GPIO_AF_TIM3);//步进电机1 PWM接口
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_TIM1);//步进电机2 PWM接口
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM5);//步进电机2 PWM接口
  
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
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 上下拉电阻不使能 */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO口最大速度 */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_STEPMOTOR1_CP;
  GPIO_Init(GPIO_PORT_STEPMOTOR1_CP, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_STEPMOTOR2_CP;
  GPIO_Init(GPIO_PORT_STEPMOTOR2_CP, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_STEPMOTOR3_CP;
  GPIO_Init(GPIO_PORT_STEPMOTOR3_CP, &GPIO_InitStructure);
  
  TIM3_PWM_SETPMOTOR();
  TIM1_PWM_SETPMOTOR();
  TIM5_PWM_SETPMOTOR();
  
  STEPMOTOR1_DIR_L();
  STEPMOTOR2_DIR_L();
  STEPMOTOR3_DIR_L();
  
  STEPMOTOR1_EN_H();
  STEPMOTOR2_EN_H();
  STEPMOTOR3_EN_H();
}

void TIM3_PWM_SETPMOTOR(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);   //Open TIM3  Clock

  TIM_DeInit(TIM3);
  
  TIM_TimeBaseStructure.TIM_Prescaler = 21 - 1;          //定时器时钟84MHZ/4=21
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM3 Count mode
  TIM_TimeBaseStructure.TIM_Period = 400 - 1;         //Fout_clk=Fclk_cnt/(ARR+1)=21MHZ/1000=21KHZ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;   
  
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: TIM3_CH1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //select PWM1 mode
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //config oc1 as output 
  TIM_OCInitStructure.TIM_Pulse = 200;                            //config TIM3_CCR1 vaule
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //config oc1 high level avaliable
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);
  
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);         // turn on oc1 preload 
  TIM_ARRPreloadConfig(TIM3, ENABLE);
  /* TIM3 enable counter */
  TIM_Cmd(TIM3, DISABLE);
}

void STEP1_PWM_START(u32 per)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);   //Open TIM3  Clock

  TIM_DeInit(TIM3);
  
  TIM_TimeBaseStructure.TIM_Prescaler = 21 - 1;          //定时器时钟84MHZ/4=21
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM3 Count mode
  TIM_TimeBaseStructure.TIM_Period = per - 1;         //Fout_clk=Fclk_cnt/(ARR+1)=21MHZ/1000=21KHZ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;   
  
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: TIM3_CH1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //select PWM1 mode
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //config oc1 as output 
  TIM_OCInitStructure.TIM_Pulse = per / 2;                            //config TIM3_CCR1 vaule
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

  TIM_DeInit(TIM1);
  
  TIM_TimeBaseStructure.TIM_Prescaler = 42 - 1;          //定时器时钟168MHZ/21=8MHZ
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM3 Count mode
  TIM_TimeBaseStructure.TIM_Period = 400;         //8MHZ/400=20KHZ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;   
  
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: TIM1_CH2 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //select PWM1 mode
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //config oc1 as output 
  TIM_OCInitStructure.TIM_Pulse = 200;                            //config TIM3_CCR1 vaule
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //config oc1 high level avaliable
  TIM_OC2Init(TIM1, &TIM_OCInitStructure);
  TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);         // turn on oc1 preload 
  
  TIM_ARRPreloadConfig(TIM1, ENABLE);

//  TIM_SetAutoreload(TIM1,400);
  TIM_CtrlPWMOutputs(TIM1, ENABLE);
  TIM_Cmd(TIM1, DISABLE);
}

void STEP2_PWM_START(u32 per)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);   //Open TIM3  Clock

  TIM_DeInit(TIM1);
  
  TIM_TimeBaseStructure.TIM_Prescaler = 42 - 1;          //定时器时钟168MHZ/21=8MHZ
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM3 Count mode
  TIM_TimeBaseStructure.TIM_Period = per;         //8MHZ/400=20KHZ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;   
  
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: TIM1_CH2 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //select PWM1 mode
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //config oc1 as output 
  TIM_OCInitStructure.TIM_Pulse = per / 2;                            //config TIM3_CCR1 vaule
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //config oc1 high level avaliable
  TIM_OC2Init(TIM1, &TIM_OCInitStructure);
  TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);         // turn on oc1 preload 
  
  TIM_ARRPreloadConfig(TIM1, ENABLE);

//  TIM_SetAutoreload(TIM1,400);
  TIM_CtrlPWMOutputs(TIM1, ENABLE);
  TIM_Cmd(TIM1, ENABLE);
}


void TIM5_PWM_SETPMOTOR(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);   //Open TIM3  Clock

  TIM_DeInit(TIM5);
  
  TIM_TimeBaseStructure.TIM_Prescaler = 21 - 1;          //定时器时钟84MHZ/4=21
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM3 Count mode
  TIM_TimeBaseStructure.TIM_Period = 400 - 1;         //Fout_clk=Fclk_cnt/(ARR+1)=21MHZ/1000=21KHZ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;   
  
  TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: TIM5_CH2 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //select PWM1 mode
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //config oc1 as output 
  TIM_OCInitStructure.TIM_Pulse = 200;                            //config TIM3_CCR1 vaule
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //config oc1 high level avaliable
  TIM_OC2Init(TIM5, &TIM_OCInitStructure);
  
  TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);         // turn on oc1 preload 
  TIM_ARRPreloadConfig(TIM5, ENABLE);
  /* TIM3 enable counter */
  TIM_Cmd(TIM5, DISABLE);
}

void STEP3_PWM_START(u32 per)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);   //Open TIM3  Clock

  TIM_DeInit(TIM5);
  
  TIM_TimeBaseStructure.TIM_Prescaler = 21 - 1;          //定时器时钟84MHZ/4=21
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM3 Count mode
  TIM_TimeBaseStructure.TIM_Period = per - 1;         //Fout_clk=Fclk_cnt/(ARR+1)=21MHZ/1000=21KHZ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;   
  
  TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: TIM5_CH2 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //select PWM1 mode
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //config oc1 as output 
  TIM_OCInitStructure.TIM_Pulse = per / 2;                            //config TIM3_CCR1 vaule
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //config oc1 high level avaliable
  TIM_OC2Init(TIM5, &TIM_OCInitStructure);
  
  TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);         // turn on oc1 preload 
  TIM_ARRPreloadConfig(TIM5, ENABLE);
  /* TIM3 enable counter */
  TIM_Cmd(TIM5, ENABLE);
}

void StepMotor_adjust_speed(u8 motor,u32 value)
{
  if(motor == STEPMOTOR1)
  {
    TIM_SetCompare3(TIM3,value / 2);
    TIM_SetAutoreload(TIM3,value);
  }
  else if(motor == STEPMOTOR2)
  {
    TIM_SetCompare2(TIM1,value / 2);
    TIM_SetAutoreload(TIM1,value);
  }
  else if(motor == STEPMOTOR3)
  {
    TIM_SetCompare2(TIM5,value / 2);
    TIM_SetAutoreload(TIM5,value);
  }
}

void StepMotor_stop(u8 motor)
{
  if(motor == STEPMOTOR1)
  {
    STEPMOTOR1_EN_H();
    TIM_Cmd(TIM3, DISABLE);
  }
  else if(motor == STEPMOTOR2)
  {
    STEPMOTOR2_EN_H();
    TIM_Cmd(TIM1, DISABLE);
    TIM_CtrlPWMOutputs(TIM1, DISABLE);//高级定时器需要增加
  }
  else if(motor == STEPMOTOR3)
  {
    STEPMOTOR3_EN_H();
    TIM_Cmd(TIM5, DISABLE);
  }
}

void StepMotor_start(u8 motor,u32 per)
{
  if(motor == STEPMOTOR1)
  {
    STEP1_PWM_START(per);
    STEPMOTOR1_EN_L();
  }
  else if(motor == STEPMOTOR2)
  {
    STEP2_PWM_START(per);
    STEPMOTOR2_EN_L();
  }
  else if(motor == STEPMOTOR3)
  {
    STEP3_PWM_START(per);
    STEPMOTOR3_EN_L();
  }
}
