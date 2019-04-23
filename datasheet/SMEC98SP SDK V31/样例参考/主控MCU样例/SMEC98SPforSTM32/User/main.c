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
//函数名: 获取STM32的UID
//参数说明：
//			pSTM32_UID - 存放STM32的UID,12字节
//返回值说明：
//			void
//说明:
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
//函数名: 初始化ADC
//参数说明：
//			void
//返回值说明：
//			void
//说明: 利用ADC悬空引脚产生随机数
//      将PA1 作为模拟通道输入引脚(一定要用悬空脚,否则获取的随机数,不够随机),
//---------------------------------------------------------
void  Adc_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1, ENABLE );      //使能ADC1通道时钟


    RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

    //PA1 作为模拟通道输入引脚, 一定要用悬空脚,否则获取的随机数,不够随机
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;        //模拟输入引脚
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;    //ADC工作模式:ADC1和ADC2工作在独立模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;    //模数转换工作在单通道模式
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//DISABLE;    //模数转换工作在单次转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;    //转换由软件而不是外部触发启动
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;    //ADC数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1;    //顺序进行规则转换的ADC通道的数目
    ADC_Init(ADC1, &ADC_InitStructure);    //根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器

    ADC_Cmd(ADC1, ENABLE);    //使能指定的ADC1
    ADC_ResetCalibration(ADC1);    //使能复位校准
    while(ADC_GetResetCalibrationStatus(ADC1));    //等待复位校准结束
    ADC_StartCalibration(ADC1);     //开启AD校准
    while(ADC_GetCalibrationStatus(ADC1));     //等待校准结束
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);        //使能指定的ADC1的软件转换启动功能
}

//---------------------------------------------------------
//函数名: 获得ADC值,作为随机数种子
//参数说明：
//			void
//返回值说明：
//			ADC悬空引脚产生的随机数
//说明: 采集4次ADC的值，每次取采集的第四位，拼成16位作为种子
//---------------------------------------------------------
unsigned short Get_Adc_RandomSeek(void)
{
    unsigned char Count;
    unsigned short ADC_RandomSeek = 0;

    //设置指定ADC的规则组通道，一个序列，采样时间
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5 );    //ADC1,ADC通道,采样时间为239.5周期
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);        //使能指定的ADC1的软件转换启动功能
    for(Count = 0; Count < 4; Count++) {
        while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束
        ADC_RandomSeek <<= 4;
        ADC_RandomSeek += ADC_GetConversionValue(ADC1) & 0x000f;		/*采集4次ADC的值，每次取采集的第四位，拼成16位作为种子*/
    }
    ADC_SoftwareStartConvCmd(ADC1,DISABLE);
    return ADC_RandomSeek;
}


/*
  1.获取SMEC98SP的UID号, 获取STM32的ID, 获取STM32随机数
  2.验证PIN
  3.内外部认证
  4.SHA1=>前置数据^随机数
  5.密文读
  6.读数据
  7.写数据
  8.构造算法(PA口数据->密文送加密芯片, 密文返回)

  如果直接引用,请将print的调试信息去除
*/

void SMEC_Test(void)
{
    /*各种密钥,不会在I2C线路上传输,可以使用同一组.应该将密钥分散存储,防止主控芯片被破解后,被攻击者在二进制码中找到密钥 */
    unsigned char InternalKey[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};//内部认证密钥,必须和SMEC98SP一致
    unsigned char ExternalKey[16] = {0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F};//外部认证密钥,必须和SMEC98SP一致
    unsigned char SHA1_Key[16] = {0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F};   //哈希算法认证密钥,必须和SMEC98SP一致
    unsigned char MKey[16] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F};   //主控密钥,用于产生过程密钥,必须和SMEC98SP一致

    unsigned char Pin[8] = {0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc};			//Pin认证密钥,必须和SMEC98SP一致

    unsigned char bStm32Uid[12] = {0};			//存放STM32的UID
    unsigned char bSmec98spUid[12] = {0};		//存放SMEC98SP的UID
    unsigned short RandomSeek;					//随机数种子
    unsigned char bRandom[8] = {0};				//存放随机数
    unsigned char bSessionKey[8] = {0};			//存放过程密钥,过程密钥为临时产生的密钥
    unsigned char bDataBuf[64] = {0};
    unsigned char ret, bLen;
    unsigned short i, j;

    /*利用ADC悬空引脚产生随机数*/
    Adc_Init();									//可放在主程序中
    RandomSeek = Get_Adc_RandomSeek();			//利用ADC悬空引脚产生随机数

    /*获取STM32的UID*/
    GetStm32Uid(bStm32Uid);
    printf("GetStm32Uid: ");
    PrintHex(bStm32Uid, 12);
    printf("\r\n");

    /*获取SMEC98SP的UID*/
    ret = SMEC_GetUid(bSmec98spUid);
    if(ret)
    {
        printf("SMEC_GetUid -> Error !\r\n");
        while(1);
    }
    printf("SMEC_GetUid: ");
    PrintHex(bSmec98spUid, 12);
    printf("\r\n");

    /*将随机数RandomSeek，再做一次随机处理(与STM32的UID, SMEC98SP的UID作绑定, 使得即使相同情况下,不同的STM32,SMEC98SP随机数种子也不同)*/
    for(i = 0; i < 6; i += 2)
    {
        /*使RandomSeek与STM32的UID相关*/
        j = (bStm32Uid[i] << 8) + bStm32Uid[i + 1];
        RandomSeek ^= j;

        /*使RandomSeek与SMEC98SP的UID相关*/
        j = (bSmec98spUid[i] << 8) + bSmec98spUid[i + 1];
        RandomSeek ^= j;
    }
    srand(RandomSeek);
    printf("RandomSeek: %04x \r\n", RandomSeek);

    /*PIN码验证*/
    ret = SMEC_CheckPin(Pin, (unsigned char)sizeof(Pin));
    if(ret)
    {
        printf("SMEC_CheckPin -> Error !\r\n");
        while(1);
    }
    printf("SMEC_CheckPin OK !\r\n");

    /*内部认证, 主控芯片对SMEC98SP加密芯片合法性判断*/
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

    /*外部认证, SMEC98SP加密芯片对主控芯片合法性判断*/
    ret = SMEC_ExtrAuth(ExternalKey);
    if(ret)
    {
        printf("SMEC_ExtrAuth -> Error !\r\n");
        while(1);
    }
    printf("SMEC_ExtrAuth OK !\r\n");

    /*SHA1摘要算法认证, 数据长度可自己设定*/
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

    /*调用加密芯片内部计算圆周长算法*/
    bDataBuf[0] = 0x02;
    ret = SMEC_CircleAlg(bDataBuf, 1, bDataBuf, &bLen);
    if(ret)
    {
        printf("SMEC_CircleAlg -> Error !\r\n");
        while(1);
    }
    printf("SMEC_CircleAlg OK, C = %02x !\r\n", bDataBuf[0]);

    /*产生过程密钥,用于后续的Flash数据加密读,及构造的"端口数据运算"*/
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

    /*密文读取Flash数据*/
    ret = SMEC_CryptReadFlash(bSessionKey, 0x0000, bDataBuf, 16);
    if(ret)
    {
        printf("SMEC_CryptReadFlash -> Error !\r\n");
        while(1);
    }
    printf("SMEC_CryptReadFlash OK:\r\n");
    PrintHex(bDataBuf, 16);

    /*读取Flash数据*/
    ret = SMEC_ReadFlash(0x0000, bDataBuf, 16);
    if(ret)
    {
        printf("SMEC_ReadFlash -> Error !\r\n");
        while(1);
    }
    printf("SMEC_ReadFlash OK:\r\n");
    PrintHex(bDataBuf, 16);

    /*写Flash数据*/
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

    /*构造"端口数据运算", 可以用实际的PA~PC端口数据*/
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

    /*调用加密芯片内部计算圆周长算法,并密文在线路上传输*/
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
    SMEC_I2cInit();				//初始化加密芯片IIC  IO
    Delay_ms(10);			//等待保证加密芯片已经运行

    printf("Z\r\n");


    SMEC_Test(); 			//加密芯片功能演示

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





