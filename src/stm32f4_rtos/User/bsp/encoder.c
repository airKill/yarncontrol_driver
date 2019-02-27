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
//  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM8);
  
//  TIM_EncoderInterfaceConfig(TIM8,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);//������ģʽ��ͨ��12��������
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		/* ��Ϊ����� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* ���������費ʹ�� */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO������ٶ� */
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ENCODERA;
  GPIO_Init(GPIO_PORT_ENCODERA, &GPIO_InitStructure);					 //PA0 ??��-
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ENCODERB;
  GPIO_Init(GPIO_PORT_ENCODERB, &GPIO_InitStructure);					 //PA0 ??��-
  
  //��ʼ����ʱ��3
  TIM_TimeBaseStructure.TIM_Period = 0xffff;
  TIM_TimeBaseStructure.TIM_Prescaler = 168 - 1;//��׽Ƶ��1MHZ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
  
  //��ʼ����ʱ��3�������
  TIM8_ICInitStructure.TIM_Channel = TIM_Channel_6; //CC1S=01 	
  TIM8_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;	
  TIM8_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM8_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV4;
  TIM8_ICInitStructure.TIM_ICFilter = 0x10;//�����˲�
  TIM_ICInit(TIM8, &TIM8_ICInitStructure);
  
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

u16 recursive_average_filter(u16 filter_object)
{
  u16 sum = 0;         
  float temp = 0;	
  static u16 filter_buf[filter_num + 1]; 
  filter_buf[filter_num] = filter_object;
  for(char i = 0; i < filter_num; i++)
  {
    filter_buf[i] = filter_buf[i + 1]; // �����������ƣ���λ�ӵ�
    sum += filter_buf[i];
  }
  temp = sum / filter_num;
  return temp;
}
