#include "bsp.h"

void bsp_InitServoMotor(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_SERVOMOTOR, ENABLE);
  
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_TIM12);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_TIM4);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		/* ��Ϊ����� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* ���������費ʹ�� */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO������ٶ� */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR1;
  GPIO_Init(GPIO_PORT_SERVOMOTOR1, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR2;
  GPIO_Init(GPIO_PORT_SERVOMOTOR2, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR3;
  GPIO_Init(GPIO_PORT_SERVOMOTOR3, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR4;
  GPIO_Init(GPIO_PORT_SERVOMOTOR4, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* ��Ϊ����� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* ���������費ʹ�� */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO������ٶ� */
  
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

  TIM_TimeBaseStructure.TIM_Prescaler = 3;          //��ʱ��ʱ��60MHZ/(3+1)=15
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

  TIM_TimeBaseStructure.TIM_Prescaler = 3;          //��ʱ��ʱ��60MHZ/(3+1)=15
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

///* ����һ��DMA���� */
//void DMA_PWM_Enable(DMA_Channel_TypeDef*DMA_CHx)
//{
//  DMA_Cmd(DMA_CHx, DISABLE );
//  TIM3->ARR = 2;	/* �������һ����0������������ʱ��ARR�ᱻ���㣬������һ��������Ч��*/
//  DMA_SetCurrDataCounter(DMA_CHx,DMA1_MEM_LEN);
//  DMA_Cmd(DMA_CHx, ENABLE);
//  TIM_Cmd(TIM3, ENABLE);  /* ʹ��TIM3 */
//  TIM3->EGR = 0x00000001;	/* �������һ��ARRֵΪ0������Ϊ��ֹͣ��ʱ����io�ڵĲ��������ǲ�Ҫ������һ�㣺CNT��û��ֹͣ�����������ǲ�����ͣ���������û���ֶ������Ļ���������Ҫ��ÿ��dmaʹ��ʱ����һ�䣬��EGR���UGλ��1����������� */
//}	  
//
///*
//*���ȷ���������ʣ�µ�������
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
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	/* ʹ�ܶ�ʱ��3ʱ�� */
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  /* ʹ��GPIO���� */   
//  
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; /* TIM_CH1*/
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  /* ����������� */
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);/* ��ʼ��GPIO */
//  
//  /* ��ʼ��TIM3 */
//  TIM_TimeBaseStructure.TIM_Period = arr; /* ��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ */
//  TIM_TimeBaseStructure.TIM_Prescaler =psc; /* ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ */
//  TIM_TimeBaseStructure.TIM_ClockDivision = 0; /* ����ʱ�ӷָ�:TDTS = Tck_tim */
//  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  /*TIM���ϼ���ģʽ */
//  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); /* ����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ */
//  
//  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; /* ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ1 */
//  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; /* �Ƚ����ʹ�� */
//  //TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  /* ʹ��TIM3��CCR1�ϵ�Ԥװ�ؼĴ���*/
//  TIM_OCInitStructure.TIM_Pulse= 100;
//  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; /* �������:TIM����Ƚϼ��Ը� */
//  TIM_OC1Init(TIM3, &TIM_OCInitStructure);  /* ����Tָ���Ĳ�����ʼ������TIM3 OC1 */
//  //TIM_DMACmd(TIM3, TIM_DMA_Update, ENABLE);	/* �����Ҫ����ռ�ձȾͰ�����ȥ��ע�ͣ�Ȼ��ע�͵��������У��ٰ�DMAͨ��6��ΪDMAͨ��3 */
//  TIM_DMACmd(TIM3, TIM_DMA_CC1, ENABLE);
//  TIM_Cmd(TIM3, ENABLE);  /* ʹ��TIM3 */
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


