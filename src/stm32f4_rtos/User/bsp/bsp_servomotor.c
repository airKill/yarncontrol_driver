#include "bsp.h"

u16 DMA1_MEM_LEN = 0;

void bsp_InitServoMotor(void)
{
  bsp_io_ServoMotor();
  DIFF_G_H();
  DIFF_G0_L();
}

void bsp_io_ServoMotor(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_SERVOMOTOR, ENABLE);
  
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_TIM12);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_TIM4);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		/* 设为输出口 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽模式 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 上下拉电阻不使能 */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO口最大速度 */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR1;
  GPIO_Init(GPIO_PORT_SERVOMOTOR1, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR2;
  GPIO_Init(GPIO_PORT_SERVOMOTOR2, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* 设为输出口 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽模式 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* 上下拉电阻不使能 */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO口最大速度 */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR_G;
  GPIO_Init(GPIO_PORT_SERVOMOTOR_G, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR_G0;
  GPIO_Init(GPIO_PORT_SERVOMOTOR_G0, &GPIO_InitStructure);
}

/* 开启一次DMA传输 */
void DMA_PWM_Enable(void)
{
  DMA_Cmd(DMA1_Stream0, DISABLE);
  TIM_SetAutoreload(TIM4,2);/* 由于最后一项是0，所以在最后的时刻ARR会被清零，导致下一次启动无效。*/
  DMA_SetCurrDataCounter(DMA1_Stream0,DMA1_MEM_LEN);
  DMA_Cmd(DMA1_Stream0,ENABLE);
  TIM_Cmd(TIM4, ENABLE);  /* 使能TIM3 */
  TIM_GenerateEvent(TIM4,1);
}	  

//进度反馈，返回剩下的数据量
u16 DMA_send_feedback(void)
{
  return DMA_GetCurrDataCounter(DMA1_Stream0);
}

void TIM4_PWM_Init(u16 arr,u16 psc)
{
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	/* 使能定时器3时钟 */
  
  /* 初始化TIM4 */
  TIM_TimeBaseStructure.TIM_Period = arr; /* 设置在下一个更新事件装入活动的自动重装载寄存器周期的值 */
  TIM_TimeBaseStructure.TIM_Prescaler = psc; /* 设置用来作为TIMx时钟频率除数的预分频值 */
  TIM_TimeBaseStructure.TIM_ClockDivision = 0; /* 设置时钟分割:TDTS = Tck_tim */
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  /*TIM向上计数模式 */
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); /* 根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位 */
  
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; /* 选择定时器模式:TIM脉冲宽度调制模式1 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; /* 比较输出使能 */
  //TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  /* 使能TIM3在CCR1上的预装载寄存器*/
  TIM_OCInitStructure.TIM_Pulse = 100;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; /* 输出极性:TIM输出比较极性高 */
  TIM_OC1Init(TIM4, &TIM_OCInitStructure);  /* 根据T指定的参数初始化外设TIM3 OC1 */
  //TIM_DMACmd(TIM4, TIM_DMA_Update, ENABLE);	/* 如果是要调节占空比就把这行去掉注释，然后注释掉下面那行，再把DMA通道6改为DMA通道3 */
  TIM_DMACmd(TIM4, TIM_DMA_Update, ENABLE);
  TIM_Cmd(TIM4, ENABLE);  /* 使能TIM3 */
}

void TIM4_PWM_SETPMOTOR(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);   //Open TIM3  Clock

  TIM_TimeBaseStructure.TIM_Prescaler = 2;          //定时器时钟60MHZ/(3+1)=15
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM3 Count mode
  TIM_TimeBaseStructure.TIM_Period = 2000;         //Fout_clk=Fclk_cnt/(ARR+1)=15000000/1500=10KHZ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;   
  
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: TIM3_CH1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //select PWM1 mode
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //config oc1 as output 
  TIM_OCInitStructure.TIM_Pulse = 1000;                            //config TIM3_CCR1 vaule
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //config oc1 high level avaliable
  TIM_OC1Init(TIM4, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);         // turn on oc1 preload 
  
  TIM_ARRPreloadConfig(TIM4, ENABLE);
  /* TIM3 enable counter */
  TIM_Cmd(TIM4, ENABLE);
  TIM_SetCompare4(TIM4,1000);
  TIM_CtrlPWMOutputs(TIM4, ENABLE);
}

void DMA_PWM_Config(u32 cpar,u32 cmar,u16 cndtr)
{
  DMA_InitTypeDef DMA_InitStructure;  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);//DMA1 

  DMA_DeInit(DMA1_Stream0);
  while (DMA_GetCmdStatus(DMA1_Stream0) != DISABLE);//等待DMA可配置 
  /* 配置 DMA Stream */
  DMA1_MEM_LEN = cndtr;
  DMA_InitStructure.DMA_Channel = DMA_Channel_2;  //通道选择
  DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;//DMA外设地址
  DMA_InitStructure.DMA_Memory0BaseAddr = cmar;//DMA 存储器0地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;//外设到存储器模式
  DMA_InitStructure.DMA_BufferSize = cndtr;//数据传输量 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设数据长度:8位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//存储器数据长度:8位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式 
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;//中等优先级
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
  DMA_Init(DMA1_Stream0, &DMA_InitStructure);//初始化DMA Stream
  DMA_Cmd(DMA1_Stream0, ENABLE);  //开启DMA传输 	
} 

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


