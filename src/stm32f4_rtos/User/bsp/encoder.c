#include "includes.h"

u8 TIM3CH2_CAPTURE_STA=0;	
u32 TIM3CH2_CAPTURE_VAL = 0;
u16 ReadValue1,ReadValue2;

float Freq_value = 0;  //Ƶ�ʸ���ֵ
u32 Freq[TempLen];        //Ƶ��ֵ��������
u8 Freq_ptr1 = 0;      //�˲�����ֵ1
u8 Freq_ptr2 = 0;      //�������ֵ2

u16 main_speed = 0;

void Encoder_Cap_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_ICInitTypeDef  TIM8_ICInitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);	//��1?��TIM5����?��
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM8);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		/* ��Ϊ����� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* ���������費ʹ�� */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO������ٶ� */
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ENCODER;
  GPIO_Init(GPIO_PORT_ENCODER, &GPIO_InitStructure);					 //PA0 ??��-
  
  //��ʼ����ʱ��3
  TIM_TimeBaseStructure.TIM_Period = 0xffff;
  TIM_TimeBaseStructure.TIM_Prescaler = 168 - 1;//��׽Ƶ��1MHZ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
  
  //��ʼ����ʱ��3�������
  TIM8_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC1S=01 	
  TIM8_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;	
  TIM8_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM8_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV4;
  TIM8_ICInitStructure.TIM_ICFilter = 0x10;//�����˲�
  TIM_ICInit(TIM8, &TIM8_ICInitStructure);
  
  //?D??��?����3?��??��
  NVIC_InitStructure.NVIC_IRQChannel = TIM8_CC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 9;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  TIM_ITConfig(TIM8,TIM_IT_CC2,ENABLE);

  TIM_Cmd(TIM8,ENABLE);
}

//Ƶ�ʲ����˲�����
u8 Freq_Sample(void)
{
  u8 valid = 0;
  u32 Dtemp=0; 
  u8 i;
  //��׽�����θߵ�ƽ
  if(TIM3CH2_CAPTURE_STA & 0X80)
  {
    Freq_ptr1++;
    Freq_ptr2++;
    if(Freq_ptr1 >= TempLen)
      Freq_ptr1=0;    
    Dtemp = TIM3CH2_CAPTURE_STA & 0X3F;
    Dtemp *= 65536;//���ʱ���ܺ�
    if(Dtemp <= 65536)  //���ʱ���ܺ�<2
      Dtemp = TIM3CH2_CAPTURE_VAL;
    else 
      Dtemp = Dtemp - 65536 + TIM3CH2_CAPTURE_VAL;
    
    Freq[Freq_ptr1] = Dtemp;
    Dtemp = 0;
    
    if(Freq_ptr2 >= TempLen)
    {
      for(i=0;i<TempLen;i++)
        Dtemp += Freq[i];     
      Freq_value = 10000000.0 / Dtemp * 4;
      Freq_ptr2 = TempLen;
      valid = 1;
    }
    TIM3CH2_CAPTURE_STA = 0;
  }
  return valid;
}

//void TIM8_UP_TIM13_IRQHandler(void)
//{
//  if (TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
//  {
//    if(TIM3CH2_CAPTURE_STA & 0X40)
//    {
//      if((TIM3CH2_CAPTURE_STA & 0X3F) == 0X3f)
//      {
//        TIM3CH2_CAPTURE_STA = 0X80;
//        TIM3CH2_CAPTURE_VAL = 0;
//      }
//      else 
//        TIM3CH2_CAPTURE_STA++;
//    }
//  }
//  printf("aaa\r\n");
//  TIM_ClearITPendingBit(TIM8,TIM_IT_Update); 
//}

//TIM3�жϴ���
void TIM8_CC_IRQHandler(void)
{
  static u16 cnt = 0;
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  if((TIM3CH2_CAPTURE_STA & 0X80) == 0)//?1?��3��1|2???
  {
    if (TIM_GetITStatus(TIM8, TIM_IT_CC2) != RESET)//2???1���騦��2???��??t
    {
      if(TIM3CH2_CAPTURE_STA & 0X40)		//2?�̨�?t��???��?��?��y??
      {
        ReadValue2 = TIM_GetCapture2(TIM8);
        if ((ReadValue2 > ReadValue1) && (TIM3CH2_CAPTURE_STA == 0x40))  //?T��?3?
        {
          TIM3CH2_CAPTURE_VAL = (ReadValue2 - ReadValue1);
        }
        else  
        {
          TIM3CH2_CAPTURE_VAL = ((0xFFFF - ReadValue1) + ReadValue2);
        }
        TIM3CH2_CAPTURE_STA |= 0X80;		
        xSemaphoreGiveFromISR(xSemaphore_freq, &xHigherPriorityTaskWoken);
        /* ���xHigherPriorityTaskWoken = pdTRUE����ô�˳��жϺ��е���ǰ������ȼ�����ִ�� */
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
      }
      else  								//�̨���?��?2???��?��y??
      {
        ReadValue1 = TIM_GetCapture2(TIM8);
        TIM3CH2_CAPTURE_VAL = 0;
        TIM3CH2_CAPTURE_STA = 0X40;		//����??2???��?��?��?��y??
      }
      cnt = cnt + 4;
      if(cnt >= ENCODER_LINE)
      {//������һȦ
        cnt = 0;
        xSemaphoreGiveFromISR(xSemaphore_encoder, &xHigherPriorityTaskWoken);
        /* ���xHigherPriorityTaskWoken = pdTRUE����ô�˳��жϺ��е���ǰ������ȼ�����ִ�� */
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
      }
    }
  }
  TIM_ClearITPendingBit(TIM8, TIM_IT_CC2); 
//  TIM_ClearITPendingBit(TIM8, TIM_IT_CC2); 
}

//TIM3�жϴ���
void TIM3_IRQHandler(void)
{
  static u16 cnt = 0;
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//  if((TIM3CH2_CAPTURE_STA & 0X80) == 0)//?1?��3��1|2???
  {
//    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
//    {
//      if(TIM3CH2_CAPTURE_STA & 0X40)//��??-2???��???��???��?
//      {
//        if((TIM3CH2_CAPTURE_STA & 0X3F) == 0X3f)//??��???��?3�訢?
//        {
//          TIM3CH2_CAPTURE_STA = 0X80;//����??3��1|2???��?��?��?
//          TIM3CH2_CAPTURE_VAL = 0;
//        }
//        else 
//          TIM3CH2_CAPTURE_STA++;
//      }
//    }
    if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)//2???1���騦��2???��??t
    {
      if(TIM3CH2_CAPTURE_STA & 0X40)		//2?�̨�?t��???��?��?��y??
      {
        ReadValue2 = TIM_GetCapture2(TIM3);
//        TIM_GetCounter(TIM3);
        if ((ReadValue2 > ReadValue1) && (TIM3CH2_CAPTURE_STA == 0x40))  //?T��?3?
        {
          TIM3CH2_CAPTURE_VAL = (ReadValue2 - ReadValue1);
        }
        else  //��D��?3?
        {
          TIM3CH2_CAPTURE_VAL = ((0xFFFF - ReadValue1) + ReadValue2);
        }
//        TIM3CH2_CAPTURE_STA |= 0X80;		//����??3��1|2???��?��?��?��?��y??
        TIM3CH2_CAPTURE_STA = 0;
//        Freq_Sample();
//        bsp_LedOn(1);
      }
      else  								//�̨���?��?2???��?��y??
      {
        ReadValue1 = TIM_GetCapture2(TIM3);
        //TIM5CH1_CAPTURE_STA=0;			//????
        TIM3CH2_CAPTURE_VAL = 0;
        TIM3CH2_CAPTURE_STA = 0X40;		//����??2???��?��?��?��y??
//        bsp_LedOff(1);
      }
      cnt++;
      if(cnt >= ENCODER_LINE)
      {//������һȦ
        cnt = 0;
        xSemaphoreGiveFromISR(xSemaphore_encoder, &xHigherPriorityTaskWoken);
        /* ���xHigherPriorityTaskWoken = pdTRUE����ô�˳��жϺ��е���ǰ������ȼ�����ִ�� */
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
      }
    }
  }
  TIM_ClearITPendingBit(TIM3, TIM_IT_CC2 | TIM_IT_Update); //??3y?D??����????
}
