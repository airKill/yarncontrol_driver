#include "stm32f10x.h"
#include "stdio.h"
#include "config.h"
#include "util.h"
#include <string.h>
#include <stdlib.h>
#include "stm32f10x_i2c.h"

#include "smec98sp.h"
#include "iic_smec98sp.h"


void RCC_Configuration(void);
void NVIC_Configuration(void);
void GPIO_Configuration(void);

//---------------------------------------------------------
//������: ��ȡSTM32��UID
//����˵����
//			pSTM32_UID - ���STM32��UID,12�ֽ�
//����ֵ˵����
//			void
//˵��:
//---------------------------------------------------------
void GetStm32Uid(unsigned char *pSTM32_UID)
{
    pSTM32_UID[0] =  *(unsigned char*)(0x1FFFF7E8);
    pSTM32_UID[1] =  *(unsigned char*)(0x1FFFF7E9);
    pSTM32_UID[2] =  *(unsigned char*)(0x1FFFF7Ea);
    pSTM32_UID[3] =  *(unsigned char*)(0x1FFFF7Eb);
    pSTM32_UID[4] =  *(unsigned char*)(0x1FFFF7Ec);
    pSTM32_UID[5] =  *(unsigned char*)(0x1FFFF7Ed);
    pSTM32_UID[6] =  *(unsigned char*)(0x1FFFF7Ee);
    pSTM32_UID[7] =  *(unsigned char*)(0x1FFFF7Ef);
    pSTM32_UID[8] =  *(unsigned char*)(0x1FFFF7f0);
    pSTM32_UID[9] =  *(unsigned char*)(0x1FFFF7f1);
    pSTM32_UID[10] =  *(unsigned char*)(0x1FFFF7f2);
    pSTM32_UID[11] =  *(unsigned char*)(0x1FFFF7f3);
}

#include "stm32f10x_adc.h"
//---------------------------------------------------------
//������: ��ʼ��ADC
//����˵����
//			void
//����ֵ˵����
//			void
//˵��: ����ADC�������Ų��������
//      ��PA1 ��Ϊģ��ͨ����������(һ��Ҫ�����ս�,�����ȡ�������,�������),
//---------------------------------------------------------
void  Adc_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1, ENABLE );      //ʹ��ADC1ͨ��ʱ��


    RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

    //PA1 ��Ϊģ��ͨ����������, һ��Ҫ�����ս�,�����ȡ�������,�������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;        //ģ����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;    //ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;    //ģ��ת�������ڵ�ͨ��ģʽ
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//DISABLE;    //ģ��ת�������ڵ���ת��ģʽ
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;    //ת��������������ⲿ��������
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;    //ADC�����Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = 1;    //˳����й���ת����ADCͨ������Ŀ
    ADC_Init(ADC1, &ADC_InitStructure);    //����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���

    ADC_Cmd(ADC1, ENABLE);    //ʹ��ָ����ADC1
    ADC_ResetCalibration(ADC1);    //ʹ�ܸ�λУ׼
    while(ADC_GetResetCalibrationStatus(ADC1));    //�ȴ���λУ׼����
    ADC_StartCalibration(ADC1);     //����ADУ׼
    while(ADC_GetCalibrationStatus(ADC1));     //�ȴ�У׼����
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);        //ʹ��ָ����ADC1�����ת����������
}

//---------------------------------------------------------
//������: ���ADCֵ,��Ϊ���������
//����˵����
//			void
//����ֵ˵����
//			ADC�������Ų����������
//˵��: �ɼ�4��ADC��ֵ��ÿ��ȡ�ɼ��ĵ���λ��ƴ��16λ��Ϊ����
//---------------------------------------------------------
unsigned short Get_Adc_RandomSeek(void)
{
    unsigned char Count;
    unsigned short ADC_RandomSeek = 0;

    //����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5 );    //ADC1,ADCͨ��,����ʱ��Ϊ239.5����
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);        //ʹ��ָ����ADC1�����ת����������
    for(Count = 0; Count < 4; Count++) {
        while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������
        ADC_RandomSeek <<= 4;
        ADC_RandomSeek += ADC_GetConversionValue(ADC1) & 0x000f;		/*�ɼ�4��ADC��ֵ��ÿ��ȡ�ɼ��ĵ���λ��ƴ��16λ��Ϊ����*/
    }
    ADC_SoftwareStartConvCmd(ADC1,DISABLE);
    return ADC_RandomSeek;
}


/*
  1.��ȡSMEC98SP��UID��, ��ȡSTM32��ID, ��ȡSTM32�����
  2.��֤PIN
  3.���ⲿ��֤
  4.SHA1=>ǰ������^�����
  5.���Ķ�
  6.������
  7.д����
  8.�����㷨(PA������->�����ͼ���оƬ, ���ķ���)

  ���ֱ������,�뽫print�ĵ�����Ϣȥ��
*/

void SMEC_Test(void)
{
    /*������Կ,������I2C��·�ϴ���,����ʹ��ͬһ��.Ӧ�ý���Կ��ɢ�洢,��ֹ����оƬ���ƽ��,���������ڶ����������ҵ���Կ */
    unsigned char InternalKey[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};//�ڲ���֤��Կ,�����SMEC98SPһ��
    unsigned char ExternalKey[16] = {0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F};//�ⲿ��֤��Կ,�����SMEC98SPһ��
    unsigned char SHA1_Key[16] = {0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F};   //��ϣ�㷨��֤��Կ,�����SMEC98SPһ��
    unsigned char MKey[16] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F};   //������Կ,���ڲ���������Կ,�����SMEC98SPһ��

    unsigned char Pin[8] = {0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc};			//Pin��֤��Կ,�����SMEC98SPһ��

    unsigned char bStm32Uid[12] = {0};			//���STM32��UID
    unsigned char bSmec98spUid[12] = {0};		//���SMEC98SP��UID
    unsigned short RandomSeek;					//���������
    unsigned char bRandom[8] = {0};				//��������
    unsigned char bSessionKey[8] = {0};			//��Ź�����Կ,������ԿΪ��ʱ��������Կ
    unsigned char bDataBuf[64] = {0};
    unsigned char ret, bLen;
    unsigned short i, j;

    /*����ADC�������Ų��������*/
    Adc_Init();									//�ɷ�����������
    RandomSeek = Get_Adc_RandomSeek();			//����ADC�������Ų��������

    /*��ȡSTM32��UID*/
    GetStm32Uid(bStm32Uid);
    printf("GetStm32Uid: ");
    PrintHex(bStm32Uid, 12);
    printf("\r\n");

    /*��ȡSMEC98SP��UID*/
    ret = SMEC_GetUid(bSmec98spUid);
    if(ret)
    {
        printf("SMEC_GetUid -> Error !\r\n");
        while(1);
    }
    printf("SMEC_GetUid: ");
    PrintHex(bSmec98spUid, 12);
    printf("\r\n");

    /*�������RandomSeek������һ���������(��STM32��UID, SMEC98SP��UID����, ʹ�ü�ʹ��ͬ�����,��ͬ��STM32,SMEC98SP���������Ҳ��ͬ)*/
    for(i = 0; i < 6; i += 2)
    {
        /*ʹRandomSeek��STM32��UID���*/
        j = (bStm32Uid[i] << 8) + bStm32Uid[i + 1];
        RandomSeek ^= j;

        /*ʹRandomSeek��SMEC98SP��UID���*/
        j = (bSmec98spUid[i] << 8) + bSmec98spUid[i + 1];
        RandomSeek ^= j;
    }
    srand(RandomSeek);
    printf("RandomSeek: %04x \r\n", RandomSeek);

    /*PIN����֤*/
    ret = SMEC_CheckPin(Pin, (unsigned char)sizeof(Pin));
    if(ret)
    {
        printf("SMEC_CheckPin -> Error !\r\n");
        while(1);
    }
    printf("SMEC_CheckPin OK !\r\n");

    /*�ڲ���֤, ����оƬ��SMEC98SP����оƬ�Ϸ����ж�*/
    for(i = 0; i < 8; i ++)
    {
        bRandom[i] = (unsigned char) rand();
    }
    ret = SMEC_IntrAuth(InternalKey, bRandom);
    if(ret)
    {
        printf("SMEC_IntrAuth -> Error !\r\n");
        while(1);
    }
    printf("SMEC_IntrAuth OK !\r\n");

    /*�ⲿ��֤, SMEC98SP����оƬ������оƬ�Ϸ����ж�*/
    ret = SMEC_ExtrAuth(ExternalKey);
    if(ret)
    {
        printf("SMEC_ExtrAuth -> Error !\r\n");
        while(1);
    }
    printf("SMEC_ExtrAuth OK !\r\n");

    /*SHA1ժҪ�㷨��֤, ���ݳ��ȿ��Լ��趨*/
    for(i = 0; i < 16; i ++)
    {
        bDataBuf[i] = (unsigned char) rand();
    }
    ret = SMEC_Sha1Auth(SHA1_Key, (unsigned char)sizeof(SHA1_Key), bDataBuf, 16);
    if(ret)
    {
        printf("SMEC_Sha1Auth -> Error !\r\n");
        while(1);
    }
    printf("SMEC_Sha1Auth OK !\r\n");

    /*���ü���оƬ�ڲ�����Բ�ܳ��㷨*/
    bDataBuf[0] = 0x02;
    ret = SMEC_CircleAlg(bDataBuf, 1, bDataBuf, &bLen);
    if(ret)
    {
        printf("SMEC_CircleAlg -> Error !\r\n");
        while(1);
    }
    printf("SMEC_CircleAlg OK, C = %02x !\r\n", bDataBuf[0]);

    /*����������Կ,���ں�����Flash���ݼ��ܶ�,�������"�˿���������"*/
    for(i = 0; i < 8; i ++)
    {
        bRandom[i] = (unsigned char) rand();
    }
    ret = SMEC_GenSessionKey(MKey, bRandom, bSessionKey);
    if(ret)
    {
        printf("SMEC_GenSessionKey -> Error !\r\n");
        while(1);
    }
    printf("SMEC_GenSessionKey OK !\r\n");

    /*���Ķ�ȡFlash����*/
    ret = SMEC_CryptReadFlash(bSessionKey, 0x0000, bDataBuf, 16);
    if(ret)
    {
        printf("SMEC_CryptReadFlash -> Error !\r\n");
        while(1);
    }
    printf("SMEC_CryptReadFlash OK:\r\n");
    PrintHex(bDataBuf, 16);

    /*��ȡFlash����*/
    ret = SMEC_ReadFlash(0x0000, bDataBuf, 16);
    if(ret)
    {
        printf("SMEC_ReadFlash -> Error !\r\n");
        while(1);
    }
    printf("SMEC_ReadFlash OK:\r\n");
    PrintHex(bDataBuf, 16);

    /*дFlash����*/
    for(i = 0; i < 16; i ++)
    {
        bDataBuf[i] = (unsigned char) i;
    }
    ret = SMEC_WriteFlash(0x0000, bDataBuf, 16);
    if(ret)
    {
        printf("SMEC_WriteFlash -> Error !\r\n");
        while(1);
    }
    printf("SMEC_WriteFlash OK!\r\n");

    /*����"�˿���������", ������ʵ�ʵ�PA~PC�˿�����*/
    bDataBuf[0] = 0x00;
    bDataBuf[1] = 0x00;
    ret = SMEC_GpioAlg(bSessionKey, bDataBuf,2, bDataBuf);
    if(ret)
    {
        printf("SMEC_GpioAlg -> Error !\r\n");
        while(1);
    }
    printf("SMEC_GpioAlg OK:\r\n");
    PrintHex(bDataBuf, 2);

    /*���ü���оƬ�ڲ�����Բ�ܳ��㷨,����������·�ϴ���*/
    bDataBuf[0] = 0x02;
    ret = SMEC_CircleAlgCrypt(bSessionKey, bDataBuf, 1, bDataBuf, &bLen);
    if(ret)
    {
        printf("SMEC_CircleAlgCrypt -> Error !\r\n");
        while(1);
    }
    printf("SMEC_CircleAlgCrypt OK, C = %02x !\r\n", bDataBuf[0]);
}


/////////////////////////////////////--------------///////////////////////////////////////

/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
    RCC_Configuration(); // Configure the system clocks
    NVIC_Configuration(); // NVIC Configuration
    GPIO_Configuration();
    USART1_Init();
    SMEC_I2cInit();				//��ʼ������оƬIIC  IO
    Delay_ms(10);			//�ȴ���֤����оƬ�Ѿ�����

    printf("Z\r\n");


    SMEC_Test(); 			//����оƬ������ʾ

    while(1);
}

void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    AFIO_TypeDef AFIO_InitStructure;
    // Configure the USART1_Tx as Alternate function Push-Pull
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin =  USART1_TX;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure the USART1_Rx as input floating
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = USART1_RX;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}


/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{
    ErrorStatus HSEStartUpStatus;

    /* RCC system reset(for debug purpose) */
    RCC_DeInit();

    /* Enable HSE */
    RCC_HSEConfig(RCC_HSE_ON);

    /* Wait till HSE is ready */
    HSEStartUpStatus = RCC_WaitForHSEStartUp();

    if(HSEStartUpStatus == SUCCESS)
    {
        /* Enable Prefetch Buffer */
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

        /* Flash 2 wait state */
        FLASH_SetLatency(FLASH_Latency_2);

        /* HCLK = SYSCLK */
        RCC_HCLKConfig(RCC_SYSCLK_Div1);

        /* PCLK2 = HCLK */
        RCC_PCLK2Config(RCC_HCLK_Div1);

        /* PCLK1 = HCLK/2 */
        RCC_PCLK1Config(RCC_HCLK_Div2);

        /* PLLCLK = 8MHz * 9 = 72 MHz */
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

        /* Enable PLL */
        RCC_PLLCmd(ENABLE);

        /* Wait till PLL is ready */
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }

        /* Select PLL as system clock source */
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

        /* Wait till PLL is used as system clock source */
        while(RCC_GetSYSCLKSource() != 0x08)
        {
        }
    }

    /* TIM2 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC
                           |RCC_APB2Periph_AFIO  | RCC_APB2Periph_USART1, ENABLE);
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

#ifdef  VECT_TAB_RAM
    /* Set the Vector Table base location at 0x20000000 */
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);

#else  /* VECT_TAB_FLASH  */
    /* Configure one bit for preemption priority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    /* Enable the USART1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

//   /* Enable the USART2 Interrupt */
//   NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
//   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//   NVIC_Init(&NVIC_InitStructure);

    /* Enable the USART3 Interrupt */


    /* Enable the TIM2 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable the TIM2 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Set the Vector Table base location at 0x08000000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);

    /* Set the Vector Table base location at 0x08002000 -> USE AIP*/
    // NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x2000);
    // NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);
#endif
}





