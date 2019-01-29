#include "bsp.h"

void bsp_InitStepMotor(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_STEPMOTOR1_EN, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_STEPMOTOR2_EN, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_STEPMOTOR3_EN, ENABLE);
  
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource0,GPIO_AF_TIM3);//�������1 PWM�ӿ�
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_TIM1);//�������2 PWM�ӿ�
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_TIM1);//�������3 PWM�ӿ�
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* ��Ϊ����� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* ���������費ʹ�� */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO������ٶ� */
  
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
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		/* ��Ϊ����� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* ���������費ʹ�� */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO������ٶ� */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_STEPMOTOR1_CP;
  GPIO_Init(GPIO_PORT_STEPMOTOR1_CP, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_STEPMOTOR2_CP;
  GPIO_Init(GPIO_PORT_STEPMOTOR2_CP, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_STEPMOTOR3_CP;
  GPIO_Init(GPIO_PORT_STEPMOTOR3_CP, &GPIO_InitStructure);
  
  TIM3_PWM_SETPMOTOR();
  TIM1_PWM_SETPMOTOR();
  
  STEPMOTOR1_DIR_L();
  STEPMOTOR2_DIR_L();
  STEPMOTOR3_DIR_L();
}

void TIM3_PWM_SETPMOTOR(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);   //Open TIM3  Clock

  TIM_TimeBaseStructure.TIM_Prescaler = 4 - 1;          //��ʱ��ʱ��84MHZ/4=21
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM3 Count mode
  TIM_TimeBaseStructure.TIM_Period = 1000 - 1;         //Fout_clk=Fclk_cnt/(ARR+1)=21MHZ/1000=21KHZ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;   
  
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: TIM3_CH1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //select PWM1 mode
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //config oc1 as output 
  TIM_OCInitStructure.TIM_Pulse = 500;                            //config TIM3_CCR1 vaule
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //config oc1 high level avaliable
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);
  
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);         // turn on oc1 preload 
  TIM_ARRPreloadConfig(TIM3, ENABLE);
  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);
}

void TIM8_PWM_SETPMOTOR(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);   //Open TIM3  Clock

  TIM_TimeBaseStructure.TIM_Prescaler = 3;          //��ʱ��ʱ��42MHZ/(3+1)=15
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM3 Count mode
  TIM_TimeBaseStructure.TIM_Period = 999;         //Fout_clk=Fclk_cnt/(ARR+1)=15000000/1500=10KHZ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;   
  
  TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: TIM3_CH1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //select PWM1 mode
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //config oc1 as output 
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 500;                            //config TIM3_CCR1 vaule
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //config oc1 high level avaliable
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_Low;
  TIM_OC2Init(TIM8, &TIM_OCInitStructure);
  
  TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);         // turn on oc1 preload 
  TIM_ARRPreloadConfig(TIM8, ENABLE);
  /* TIM3 enable counter */
  TIM_Cmd(TIM8, ENABLE);
  TIM_CtrlPWMOutputs(TIM8, ENABLE);
}

void TIM1_PWM_SETPMOTOR(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);   //Open TIM3  Clock

  TIM_TimeBaseStructure.TIM_Prescaler = 21 - 1;          //��ʱ��ʱ��168MHZ/21=8MHZ
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
  /* PWM1 Mode configuration: TIM1_CH4 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //select PWM1 mode
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //config oc1 as output 
  TIM_OCInitStructure.TIM_Pulse = 200;                            //config TIM3_CCR1 vaule
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //config oc1 high level avaliable
  TIM_OC4Init(TIM1, &TIM_OCInitStructure);
  TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);         // turn on oc1 preload 
  
  TIM_ARRPreloadConfig(TIM1, ENABLE);
  /* TIM3 enable counter */
  TIM_Cmd(TIM1, ENABLE);
//  TIM_SetCompare4(TIM1,1000);
  TIM_SetAutoreload(TIM1,400);
  TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

void StepMotor_adjust_speed(u8 motor,u32 value)
{
  if(motor == 1)
  {
    TIM_SetCompare2(TIM8,value / 2);
    TIM_SetAutoreload(TIM8,value);
  }
  else if(motor == 2)
  {
    TIM_SetCompare2(TIM1,value / 2);
    TIM_SetAutoreload(TIM1,value);
  }
}

void StepMotor_stop(u8 motor)
{
  if(motor == 1)
  {
    TIM_Cmd(TIM3, DISABLE);
  }
  else if(motor == 2)
  {
    TIM_Cmd(TIM1, DISABLE);
    TIM_CtrlPWMOutputs(TIM1, DISABLE);//�߼���ʱ����Ҫ����
  }
}

void StepMotor_start(u8 motor)
{
  if(motor == 1)
  {
    TIM_Cmd(TIM3, ENABLE);
  }
  else if(motor == 2)
  {
    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);//�߼���ʱ����Ҫ����
  }
}
