#include "includes.h"

u8 TIM3CH2_CAPTURE_STA=0;	
u32 TIM3CH2_CAPTURE_VAL = 0;
u16 ReadValue1,ReadValue2;

void Encoder_Cap_Init(u16 arr,u16 psc)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_ICInitTypeDef  TIM3_ICInitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//ê1?üTIM5ê±?ó
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		/* 设为输出口 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽模式 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 上下拉电阻不使能 */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO口最大速度 */
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ENCODER;
  GPIO_Init(GPIO_PORT_ENCODER, &GPIO_InitStructure);					 //PA0 ??à-
  
  //初始化定时器3
  TIM_TimeBaseStructure.TIM_Period = arr; //éè?¨??êy?÷×??ˉ??×°?μ
  TIM_TimeBaseStructure.TIM_Prescaler = psc; 	//?¤·??μ?÷
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
  TIM_Cmd(TIM3,ENABLE); 	//ê1?ü?¨ê±?÷5
}

//TIM3中断处理
void TIM3_IRQHandler(void)
{
  if((TIM3CH2_CAPTURE_STA & 0X80) == 0)//?1?′3é1|2???
  {
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
      if(TIM3CH2_CAPTURE_STA & 0X40)//ò??-2???μ???μ???á?
      {
        if((TIM3CH2_CAPTURE_STA & 0X3F) == 0X3f)//??μ???ì?3¤á?
        {
          TIM3CH2_CAPTURE_STA = 0X80;//±ê??3é1|2???á?ò?′?
          TIM3CH2_CAPTURE_VAL = 0;
        }
        else 
          TIM3CH2_CAPTURE_STA++;
      }
    }
    if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)//2???1·￠éú2???ê??t
    {
      if(TIM3CH2_CAPTURE_STA & 0X40)		//2?μú?t′???μ?é?éy??
      {
        ReadValue2 = TIM_GetCapture2(TIM3);
        if ((ReadValue2 > ReadValue1) && (TIM3CH2_CAPTURE_STA == 0x40))  //?Tò?3?
        {
          TIM3CH2_CAPTURE_VAL = (ReadValue2 - ReadValue1);
        }
        else  //óDò?3?
        {
          TIM3CH2_CAPTURE_VAL = ((0xFFFF - ReadValue1) + ReadValue2);
        }
        TIM3CH2_CAPTURE_STA |= 0X80;		//±ê??3é1|2???μ?ò?′?é?éy??
      }
      else  								//μúò?′?2???é?éy??
      {
        ReadValue1 = TIM_GetCapture2(TIM3);
        //TIM5CH1_CAPTURE_STA=0;			//????
        TIM3CH2_CAPTURE_VAL = 0;
        TIM3CH2_CAPTURE_STA = 0X40;		//±ê??2???μ?á?é?éy??
      }
    }
  }
  TIM_ClearITPendingBit(TIM3, TIM_IT_CC2 | TIM_IT_Update); //??3y?D??±ê????
}
