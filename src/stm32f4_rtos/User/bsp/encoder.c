#include "includes.h"

u8 TIM3CH2_CAPTURE_STA=0;	
u32 TIM3CH2_CAPTURE_VAL = 0;
u16 ReadValue1,ReadValue2;

float Freq_value = 0;  //频率浮点值
u32 Freq[TempLen];        //频率值缓冲数组
u32 Freq_Sum=0;      //
u32 Overflow_ptr = 0;  //溢出计数值
u8 Freq_ptr1=0;      //滤波计数值1
u8 Freq_ptr2=0;      //溢出计数值2

u16 main_speed = 0;

void Encoder_Cap_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_ICInitTypeDef  TIM3_ICInitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//ê1?üTIM5ê±?ó
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM3);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		/* 设为输出口 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽模式 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 上下拉电阻不使能 */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO口最大速度 */
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ENCODER;
  GPIO_Init(GPIO_PORT_ENCODER, &GPIO_InitStructure);					 //PA0 ??à-
  
  //初始化定时器3
  TIM_TimeBaseStructure.TIM_Period = 0xffff; //éè?¨??êy?÷×??ˉ??×°?μ
  TIM_TimeBaseStructure.TIM_Prescaler = 840 - 1; 	//?¤·??μ?÷
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //éè??ê±?ó·???:TDTS = Tck_tim
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM?òé???êy?￡ê?
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //?ù?YTIM_TimeBaseInitStruct?D???¨μ?2?êy3?ê??ˉTIMxμ?ê±???ùêyμ￥??
  
  //初始化定时器3捕获参数
  TIM3_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC1S=01 	????ê?è??? IC1ó3é?μ?TI1é?
  TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//é?éy??2???
  TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ó3é?μ?TI1é?
  TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //????ê?è?·??μ,2?·??μ
  TIM3_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ????ê?è???2¨?÷ 2???2¨
  TIM_ICInit(TIM3, &TIM3_ICInitStructure);
  
  //?D??·?×é3?ê??ˉ
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3?D??
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //?è??ó??è??2??
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //′óó??è??0??
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQí¨μà±?ê1?ü
  NVIC_Init(&NVIC_InitStructure);  //?ù?YNVIC_InitStruct?D???¨μ?2?êy3?ê??ˉíaéèNVIC??′??÷
  
  TIM_ITConfig(TIM3,TIM_IT_Update|TIM_IT_CC2,ENABLE);//?êDí?üD??D?? ,?êDíCC1IE2????D??
//  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);//?êDí?üD??D?? ,?êDíCC1IE2????D??
  TIM_Cmd(TIM3,ENABLE); 	//ê1?ü?¨ê±?÷5
}

//频率采样滤波处理
void Freq_Sample(void)
{
  u32 Dtemp=0; 
  u8 i;
  //捕捉了两次高电平
  if(TIM3CH2_CAPTURE_STA & 0X80)
  {
    Freq_ptr1++;
    Freq_ptr2++;
    if(Freq_ptr1 >= TempLen)
      Freq_ptr1=0;    
    Dtemp = TIM3CH2_CAPTURE_STA & 0X3F;
    Dtemp *= 65536;//溢出时间总和
    if(Dtemp <= 65536)  //溢出时间总和<2
      Dtemp = TIM3CH2_CAPTURE_VAL;
    else 
      Dtemp = Dtemp - 65536 + TIM3CH2_CAPTURE_VAL;
    
    Freq[Freq_ptr1] = Dtemp;
    Dtemp = 0;
    
    if(Freq_ptr2 >= TempLen)
    {
      for(i=0;i<TempLen;i++)
        Dtemp += Freq[i];     
      Freq_value = 10000000.0 / Dtemp;
      main_speed = Freq_value / 600.0 * 60;//编码器频率*线数600*60=主轴转速/分钟
      Freq_ptr2 = TempLen;
    }
    TIM3CH2_CAPTURE_STA = 0;
    Overflow_ptr = 0;
  }
  else //?′2???μè′y??á?
  {
    Overflow_ptr++;
    if(Overflow_ptr > 720000)
    {
      Freq_value = Freq_value / 10;
      Overflow_ptr = 0;
    }
  }
}

//TIM3中断处理
void TIM3_IRQHandler(void)
{
  static u16 cnt = 0;
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//  if((TIM3CH2_CAPTURE_STA & 0X80) == 0)//?1?′3é1|2???
  {
//    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
//    {
//      if(TIM3CH2_CAPTURE_STA & 0X40)//ò??-2???μ???μ???á?
//      {
//        if((TIM3CH2_CAPTURE_STA & 0X3F) == 0X3f)//??μ???ì?3¤á?
//        {
//          TIM3CH2_CAPTURE_STA = 0X80;//±ê??3é1|2???á?ò?′?
//          TIM3CH2_CAPTURE_VAL = 0;
//        }
//        else 
//          TIM3CH2_CAPTURE_STA++;
//      }
//    }
    if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)//2???1·￠éú2???ê??t
    {
      if(TIM3CH2_CAPTURE_STA & 0X40)		//2?μú?t′???μ?é?éy??
      {
        ReadValue2 = TIM_GetCapture2(TIM3);
//        TIM_GetCounter(TIM3);
        if ((ReadValue2 > ReadValue1) && (TIM3CH2_CAPTURE_STA == 0x40))  //?Tò?3?
        {
          TIM3CH2_CAPTURE_VAL = (ReadValue2 - ReadValue1);
        }
        else  //óDò?3?
        {
          TIM3CH2_CAPTURE_VAL = ((0xFFFF - ReadValue1) + ReadValue2);
        }
//        TIM3CH2_CAPTURE_STA |= 0X80;		//±ê??3é1|2???μ?ò?′?é?éy??
        TIM3CH2_CAPTURE_STA = 0;
//        Freq_Sample();
//        bsp_LedOn(1);
      }
      else  								//μúò?′?2???é?éy??
      {
        ReadValue1 = TIM_GetCapture2(TIM3);
        //TIM5CH1_CAPTURE_STA=0;			//????
        TIM3CH2_CAPTURE_VAL = 0;
        TIM3CH2_CAPTURE_STA = 0X40;		//±ê??2???μ?á?é?éy??
//        bsp_LedOff(1);
      }
      cnt++;
      if(cnt >= ENCODER_LINE)
      {//编码器一圈
        cnt = 0;
        xSemaphoreGiveFromISR(xSemaphore_encoder, &xHigherPriorityTaskWoken);
        /* 如果xHigherPriorityTaskWoken = pdTRUE，那么退出中断后切到当前最高优先级任务执行 */
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
      }
    }
  }
  TIM_ClearITPendingBit(TIM3, TIM_IT_CC2 | TIM_IT_Update); //??3y?D??±ê????
}
