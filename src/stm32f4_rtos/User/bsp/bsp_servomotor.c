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

///* 开启一次DMA传输 */
//void DMA_PWM_Enable(DMA_Channel_TypeDef*DMA_CHx)
//{
//  DMA_Cmd(DMA_CHx, DISABLE );
//  TIM3->ARR = 2;	/* 由于最后一项是0，所以在最后的时刻ARR会被清零，导致下一次启动无效。*/
//  DMA_SetCurrDataCounter(DMA_CHx,DMA1_MEM_LEN);
//  DMA_Cmd(DMA_CHx, ENABLE);
//  TIM_Cmd(TIM3, ENABLE);  /* 使能TIM3 */
//  TIM3->EGR = 0x00000001;	/* 由于最后一次ARR值为0，这是为了停止定时器对io口的操作，但是不要忽略了一点：CNT并没有停止计数，而且是不会再停下来，如果没有手动操作的话，所以需要在每次dma使能时加上一句，将EGR里的UG位置1，清零计数器 */
//}	  
//
///*
//*进度反馈，返回剩下的数据量
//*/
//u16 DMA_send_feedback(DMA_Channel_TypeDef* DMA_CHx)
//{
//  return DMA_CHx->CNDTR;
//}

//void TIM3_PWM_Init(u16 arr,u16 psc)
//{  
//  GPIO_InitTypeDef GPIO_InitStructure;
//  TIM_OCInitTypeDef  TIM_OCInitStructure;
//  
//  
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	/* 使能定时器3时钟 */
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  /* 使能GPIO外设 */   
//  
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; /* TIM_CH1*/
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  /* 复用推挽输出 */
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);/* 初始化GPIO */
//  
//  /* 初始化TIM3 */
//  TIM_TimeBaseStructure.TIM_Period = arr; /* 设置在下一个更新事件装入活动的自动重装载寄存器周期的值 */
//  TIM_TimeBaseStructure.TIM_Prescaler =psc; /* 设置用来作为TIMx时钟频率除数的预分频值 */
//  TIM_TimeBaseStructure.TIM_ClockDivision = 0; /* 设置时钟分割:TDTS = Tck_tim */
//  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  /*TIM向上计数模式 */
//  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); /* 根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位 */
//  
//  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; /* 选择定时器模式:TIM脉冲宽度调制模式1 */
//  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; /* 比较输出使能 */
//  //TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  /* 使能TIM3在CCR1上的预装载寄存器*/
//  TIM_OCInitStructure.TIM_Pulse= 100;
//  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; /* 输出极性:TIM输出比较极性高 */
//  TIM_OC1Init(TIM3, &TIM_OCInitStructure);  /* 根据T指定的参数初始化外设TIM3 OC1 */
//  //TIM_DMACmd(TIM3, TIM_DMA_Update, ENABLE);	/* 如果是要调节占空比就把这行去掉注释，然后注释掉下面那行，再把DMA通道6改为DMA通道3 */
//  TIM_DMACmd(TIM3, TIM_DMA_CC1, ENABLE);
//  TIM_Cmd(TIM3, ENABLE);  /* 使能TIM3 */
//  
//}
//int main(void)
//{	
//  int i;
//  int feedback;
//  delay_init();	
//  uart_init(115200);
//  KEY_Init();
//  DMA_Config(DMA1_Channel6, (u32)&TIM3->ARR, (u32)send_buf, size);
//  TIM3_PWM_Init(599,7199);
//  for(i = 0; i < size; ++i)
//  {
//    if(i != size - 1)
//      send_buf[i] = 100 + 10 * i;
//    else
//      send_buf[i] = 0;
//  }
//  DMA_Enable(DMA1_Channel6);
//  while(1)
//  {
//    feedback = DMA_send_feedback(DMA1_Channel6);
//    if(feedback != 0)
//    {
//      printf("-> ");
//      printf("%d\r\n", DMA_send_feedback(DMA1_Channel6));
//    }
//    if(KEY_Scan(0) == 1)
//    {
//      DMA_Enable(DMA1_Channel6);
//    }
//  }
//}


