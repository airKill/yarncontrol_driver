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
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		/* ��Ϊ����� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* ���������費ʹ�� */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO������ٶ� */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR1;
  GPIO_Init(GPIO_PORT_SERVOMOTOR1, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR2;
  GPIO_Init(GPIO_PORT_SERVOMOTOR2, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* ��Ϊ����� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* ���������費ʹ�� */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO������ٶ� */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR_G;
  GPIO_Init(GPIO_PORT_SERVOMOTOR_G, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SERVOMOTOR_G0;
  GPIO_Init(GPIO_PORT_SERVOMOTOR_G0, &GPIO_InitStructure);
}

/* ����һ��DMA���� */
void DMA_PWM_Enable(void)
{
  DMA_Cmd(DMA1_Stream0, DISABLE);
  TIM_SetAutoreload(TIM4,2);/* �������һ����0������������ʱ��ARR�ᱻ���㣬������һ��������Ч��*/
  DMA_SetCurrDataCounter(DMA1_Stream0,DMA1_MEM_LEN);
  DMA_Cmd(DMA1_Stream0,ENABLE);
  TIM_Cmd(TIM4, ENABLE);  /* ʹ��TIM3 */
  TIM_GenerateEvent(TIM4,1);
}	  

//���ȷ���������ʣ�µ�������
u16 DMA_send_feedback(void)
{
  return DMA_GetCurrDataCounter(DMA1_Stream0);
}

void TIM4_PWM_Init(u16 arr,u16 psc)
{
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	/* ʹ�ܶ�ʱ��3ʱ�� */
  
  /* ��ʼ��TIM4 */
  TIM_TimeBaseStructure.TIM_Period = arr; /* ��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ */
  TIM_TimeBaseStructure.TIM_Prescaler = psc; /* ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ */
  TIM_TimeBaseStructure.TIM_ClockDivision = 0; /* ����ʱ�ӷָ�:TDTS = Tck_tim */
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  /*TIM���ϼ���ģʽ */
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); /* ����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ */
  
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; /* ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ1 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; /* �Ƚ����ʹ�� */
  //TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  /* ʹ��TIM3��CCR1�ϵ�Ԥװ�ؼĴ���*/
  TIM_OCInitStructure.TIM_Pulse = 100;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; /* �������:TIM����Ƚϼ��Ը� */
  TIM_OC1Init(TIM4, &TIM_OCInitStructure);  /* ����Tָ���Ĳ�����ʼ������TIM3 OC1 */
  //TIM_DMACmd(TIM4, TIM_DMA_Update, ENABLE);	/* �����Ҫ����ռ�ձȾͰ�����ȥ��ע�ͣ�Ȼ��ע�͵��������У��ٰ�DMAͨ��6��ΪDMAͨ��3 */
  TIM_DMACmd(TIM4, TIM_DMA_Update, ENABLE);
  TIM_Cmd(TIM4, ENABLE);  /* ʹ��TIM3 */
}

void TIM4_PWM_SETPMOTOR(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);   //Open TIM3  Clock

  TIM_TimeBaseStructure.TIM_Prescaler = 2;          //��ʱ��ʱ��60MHZ/(3+1)=15
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
  while (DMA_GetCmdStatus(DMA1_Stream0) != DISABLE);//�ȴ�DMA������ 
  /* ���� DMA Stream */
  DMA1_MEM_LEN = cndtr;
  DMA_InitStructure.DMA_Channel = DMA_Channel_2;  //ͨ��ѡ��
  DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;//DMA�����ַ
  DMA_InitStructure.DMA_Memory0BaseAddr = cmar;//DMA �洢��0��ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;//���赽�洢��ģʽ
  DMA_InitStructure.DMA_BufferSize = cndtr;//���ݴ����� 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//�������ݳ���:8λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//�洢�����ݳ���:8λ
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// ʹ����ͨģʽ 
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;//�е����ȼ�
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//�洢��ͻ�����δ���
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//����ͻ�����δ���
  DMA_Init(DMA1_Stream0, &DMA_InitStructure);//��ʼ��DMA Stream
  DMA_Cmd(DMA1_Stream0, ENABLE);  //����DMA���� 	
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


