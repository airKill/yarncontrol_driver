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
  
  RCC_AHB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//��1?��TIM5����?��
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		/* ��Ϊ����� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* ���������費ʹ�� */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO������ٶ� */
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ENCODER;
  GPIO_Init(GPIO_PORT_ENCODER, &GPIO_InitStructure);					 //PA0 ??��-
  
  //��ʼ����ʱ��3
  TIM_TimeBaseStructure.TIM_Period = arr; //����?��??��y?�¡�??��??����?��
  TIM_TimeBaseStructure.TIM_Prescaler = psc; 	//?�衤??��?��
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����??����?����???:TDTS = Tck_tim
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM?����???��y?�꨺?
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //?��?YTIM_TimeBaseInitStruct?D???����?2?��y3?��??��TIMx��?����???����y�̣�??
  
  //��ʼ����ʱ��3�������
  TIM3_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC1S=01 	????��?��??? IC1��3��?��?TI1��?
  TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//��?��y??2???
  TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //��3��?��?TI1��?
  TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //????��?��?��??��,2?��??��
  TIM3_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ????��?��???2��?�� 2???2��
  TIM_ICInit(TIM3, &TIM3_ICInitStructure);
  
  //?D??��?����3?��??��
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3?D??
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //?��??��??��??2??
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�䨮��??��??0??
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ�����̨���?��1?��
  NVIC_Init(&NVIC_InitStructure);  //?��?YNVIC_InitStruct?D???����?2?��y3?��??����a����NVIC??��??��
  
  TIM_ITConfig(TIM3,TIM_IT_Update|TIM_IT_CC2,ENABLE);//?��D��?��D??D?? ,?��D��CC1IE2????D??
  TIM_Cmd(TIM3,ENABLE); 	//��1?��?������?��5
}

//TIM3�жϴ���
void TIM3_IRQHandler(void)
{
  if((TIM3CH2_CAPTURE_STA & 0X80) == 0)//?1?��3��1|2???
  {
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
      if(TIM3CH2_CAPTURE_STA & 0X40)//��??-2???��???��???��?
      {
        if((TIM3CH2_CAPTURE_STA & 0X3F) == 0X3f)//??��???��?3�訢?
        {
          TIM3CH2_CAPTURE_STA = 0X80;//����??3��1|2???��?��?��?
          TIM3CH2_CAPTURE_VAL = 0;
        }
        else 
          TIM3CH2_CAPTURE_STA++;
      }
    }
    if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)//2???1���騦��2???��??t
    {
      if(TIM3CH2_CAPTURE_STA & 0X40)		//2?�̨�?t��???��?��?��y??
      {
        ReadValue2 = TIM_GetCapture2(TIM3);
        if ((ReadValue2 > ReadValue1) && (TIM3CH2_CAPTURE_STA == 0x40))  //?T��?3?
        {
          TIM3CH2_CAPTURE_VAL = (ReadValue2 - ReadValue1);
        }
        else  //��D��?3?
        {
          TIM3CH2_CAPTURE_VAL = ((0xFFFF - ReadValue1) + ReadValue2);
        }
        TIM3CH2_CAPTURE_STA |= 0X80;		//����??3��1|2???��?��?��?��?��y??
      }
      else  								//�̨���?��?2???��?��y??
      {
        ReadValue1 = TIM_GetCapture2(TIM3);
        //TIM5CH1_CAPTURE_STA=0;			//????
        TIM3CH2_CAPTURE_VAL = 0;
        TIM3CH2_CAPTURE_STA = 0X40;		//����??2???��?��?��?��y??
      }
    }
  }
  TIM_ClearITPendingBit(TIM3, TIM_IT_CC2 | TIM_IT_Update); //??3y?D??����????
}
