/**
  ****************** (C) COPYRIGHT 2017 Nova  *******************
  * @file     drv_spi.c
  * @auther   wanghongbin
  * @version  V1.0.0
  * @date     2017.3.5
  * @brief    This file provides the functions of drv_spi.
  *
  ***************************************************************
  * @attention
  *
  * History<br>
  * version   date            auther         description<br>
  * v1.0.0    2017.3.5        wanghongbin    Initial Version<br>
  ***************************************************************
  */

#include "drv_spi.h"


  
uint8_t SPI1_TX_Buff[BUFFERSIZE];  
uint8_t SPI1_RX_Buff[BUFFERSIZE];  

/* Private macros -----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
typedef enum
{
  EN_SPI1_BUS = 0x00,
  EN_SPI2_BUS,
  EN_SPI4_BUS,
  EN_SPI_BUS_NUM,
} En_SPI_Bus_t;


/* Private define ------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u8  _gSPI_BspRegisterFlag[EN_SPI_BUS_NUM] = {0};   /* bsp's register flag for all the SPI bus */
u8  _gSPI_DMARegisterFlag[EN_SPI_BUS_NUM] = {0};   /* bsp's register flag for all the SPI bus */
u8  _gSPI_ModeRisterFlag[EN_SPI_BUS_NUM] = {0};   /* bsp's register flag for all the SPI bus */

u8  _gSPI_RecvBuf[SPI_BUF_SIZE] = {0};   /* DMA buffer */

/* Private functions ---------------------------------------------------------*/
static void SPI_BspInit(_SPI_Bus_t* spi);
static void SPI_DMA_Config(_SPI_Bus_t* spi);
static void SPI_ModeConfig(_SPI_Bus_t* spi);


/**
  * @brief  SPI hardware intialization.
  * @param  spi: description info struct of spi bus.
  * @return none.
  */
static void SPI_BspInit(_SPI_Bus_t* spi)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  switch(spi->bus_id)
  {
#if   USE_SPI1       
    case  SPI1_Bus :
    {
      if(!_gSPI_BspRegisterFlag[SPI1_Bus])
      {
        /* Enable the SPI clock */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);  
        
        /* Enable GPIO clocks */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
        
        /* SPI pins configuration: SPI SCK / SPI MOSI / SPI MISO */ 
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOA, &GPIO_InitStructure); 
        
        _gSPI_BspRegisterFlag[SPI1_Bus] = 0x01;
      }
      break;
    }
#endif  
#if   USE_SPI2   
    case  SPI2_Bus :
    {
      if(!_gSPI_BspRegisterFlag[SPI2_Bus])
      {
        /* to be implemented */
        _gSPI_BspRegisterFlag[SPI2_Bus] = 0x01;       
      }
      break;
    }
#endif 
#if   USE_SPI4    
    case  SPI4_Bus :
    {
      if(!_gSPI_BspRegisterFlag[SPI4_Bus])
      {
        /* Enable the SPI clock */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI4, ENABLE);  
        
        /* Enable GPIO clocks */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
        
        /* SPI pins configuration */ 
        GPIO_PinAFConfig(GPIOE, GPIO_PinSource2, GPIO_AF_SPI4);  /* SPI2_SCK */
        GPIO_PinAFConfig(GPIOE, GPIO_PinSource6, GPIO_AF_SPI4);  /* SPI2_MOSI */
        GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_SPI4);  /* SPI2_MISO */
        GPIO_PinAFConfig(GPIOE, GPIO_PinSource4, GPIO_AF_SPI4);  /* SPI2_NSS */
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
              
        /* SPI SCK pin configuration */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
        GPIO_Init(GPIOE, &GPIO_InitStructure);

        /* SPI MOSI pin configuration */
        GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
        GPIO_Init(GPIOE, &GPIO_InitStructure);

        /* SPI MISO pin configuration */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
        GPIO_Init(GPIOE, &GPIO_InitStructure);  
        
        /* SPI NSS pin configuration */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
        GPIO_Init(GPIOE, &GPIO_InitStructure);

        _gSPI_BspRegisterFlag[SPI4_Bus] = 0x01; 
      }
      break;
    }
#endif    
    default :
      break;
  }
}

/**
  * @brief  SPI DMA configuration.
  * @param  spi: description info struct of spi bus.
  * @return none.
  */
static void SPI_DMA_Config(_SPI_Bus_t* spi)
{
//  u32 time_out = 0xffffffff;
  
  switch(spi->bus_id)
  {
#if   USE_SPI1 
    case  SPI1_Bus :
    {
#if  SPI1_DMA_ENABLE
      if(!_gSPI_DMARegisterFlag[SPI1_Bus])
      {
        RCC->AHBENR |= 1<<0 ;                     //DMA1时钟使能  
      
        /*------------------配置SPI1_RX_DMA通道Channel2---------------------*/  
      
        DMA1_Channel2->CCR &= ~( 1<<14 ) ;        //非存储器到存储器模式  
        DMA1_Channel2->CCR |=    2<<12   ;        //通道优先级高  
        DMA1_Channel2->CCR &= ~( 3<<10 ) ;        //存储器数据宽度8bit  
        DMA1_Channel2->CCR &= ~( 3<<8  ) ;        //外设数据宽度8bit  
        DMA1_Channel2->CCR |=    1<<7    ;        //存储器地址增量模式  
        DMA1_Channel2->CCR &= ~( 1<<6  ) ;        //不执行外设地址增量模式  
        DMA1_Channel2->CCR &= ~( 1<<5  ) ;        //执行循环操作  
        DMA1_Channel2->CCR &= ~( 1<<4  ) ;        //从外设读  
      
        DMA1_Channel2->CNDTR &= 0x0000   ;        //传输数量寄存器清零  
        DMA1_Channel2->CNDTR = BUFFERSIZE ;       //传输数量设置为buffersize个  
      
        DMA1_Channel2->CPAR = SPI1_DR_Addr ;      //设置外设地址，注意PSIZE  
        DMA1_Channel2->CMAR = (u32)SPI1_RX_Buff ; //设置DMA存储器地址，注意MSIZE  
      
        /*------------------配置SPI1_TX_DMA通道Channel3---------------------*/  
      
        DMA1_Channel3->CCR &= ~( 1<<14 ) ;        //非存储器到存储器模式  
        DMA1_Channel3->CCR |=    0<<12   ;        //通道优先级最低  
        DMA1_Channel3->CCR &= ~( 3<<10 ) ;        //存储器数据宽度8bit  
        DMA1_Channel3->CCR &= ~( 3<<8 )  ;        //外设数据宽度8bit  
        DMA1_Channel3->CCR |=    1<<7    ;        //存储器地址增量模式  
        DMA1_Channel3->CCR &= ~( 1<<6 )  ;        //不执行外设地址增量模式  
        DMA1_Channel3->CCR &= ~( 1<<5 ) ;         //不执行循环操作  
        DMA1_Channel3->CCR |=    1<<4    ;        //从存储器读  
      
        DMA1_Channel3->CNDTR &= 0x0000   ;        //传输数量寄存器清零  
        DMA1_Channel3->CNDTR = BUFFERSIZE ;       //传输数量设置为buffersize个  
          
        DMA1_Channel3->CPAR = SPI1_DR_Addr ;      //设置外设地址，注意PSIZE  
        DMA1_Channel3->CMAR = (uint32_t)SPI1_TX_Buff ; //设置DMA存储器地址，注意MSIZE
        
        _gSPI_DMARegisterFlag[SPI1_Bus] = 0x01;
      }
#endif  /* SPI1_DMA_ENABLE */
      break;
    }
#endif    
#if   USE_SPI2 
    case  SPI2_Bus :
    {
#if  SPI2_DMA_ENABLE
      if(!_gSPI_DMARegisterFlag[SPI2_Bus])
      {
        
        _gSPI_DMARegisterFlag[SPI2_Bus] = 0x01;
      }
#endif  /* SPI2_DMA_ENABLE */      
      break;
    }
#endif    
#if   USE_SPI4 
    case  SPI4_Bus :
    {
#if  SPI4_DMA_ENABLE
      if(!_gSPI_DMARegisterFlag[SPI4_Bus])
      {
        
        _gSPI_DMARegisterFlag[SPI4_Bus] = 0x01;
      }
#endif  /* SPI4_DMA_ENABLE */      
      break;
    }
#endif    
    default :
      break;
  }
}

/**
  * @brief  SPI mode configuration.
  * @param  spi: description info struct of spi bus.
  * @return none.
  */
static void SPI_ModeConfig(_SPI_Bus_t* spi)
{
  SPI_InitTypeDef  SPI_InitStructure;
  
  switch(spi->bus_id)
  {
#if   USE_SPI1 
    case  SPI1_Bus :
    {
      if(!_gSPI_ModeRisterFlag[SPI1_Bus])
      {
        SPI_Cmd(SPI1, DISABLE);

        /* SPI configuration */
        SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
        SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
        SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
        SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
        SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
        SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
        SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
        SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
        SPI_InitStructure.SPI_CRCPolynomial = 7;
        SPI_Init(SPI1, &SPI_InitStructure);
#if  SPI1_DMA_ENABLE
        SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);
#endif        
        /* Enable the sFLASH_SPI */
        SPI_Cmd(SPI1, ENABLE);
        
        _gSPI_ModeRisterFlag[SPI1_Bus] = 0x01;
      }
      break;
    }
#endif
#if   USE_SPI2    
    case  SPI2_Bus :
    {
      if(!_gSPI_ModeRisterFlag[SPI2_Bus])
      {
        SPI_Cmd(SPI2, DISABLE);

        /* SPI configuration */
        SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;    /* 全双工 */
        SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
        SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                     /* 8帧 */
        SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;                           /* 空闲时刻位高电平 */
        SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;                          /* 数据在第二个跳变沿被采集 */
        SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                             /* CS引脚为软件模式 */
        SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;    /* 8分帧 */
        SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                    /* 先传高字节 */
        SPI_InitStructure.SPI_CRCPolynomial = 7;
        SPI_Init(SPI2, &SPI_InitStructure);
#if  SPI2_DMA_ENABLE
        SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);
#endif        
        /* Enable the sFLASH_SPI */
        SPI_Cmd(SPI2, ENABLE);
        
        _gSPI_ModeRisterFlag[SPI2_Bus] = 0x01;
      }
      break;
    }
#endif
#if   USE_SPI4    
    case  SPI4_Bus :
    {
      if(!_gSPI_ModeRisterFlag[SPI4_Bus])
      {
        SPI_Cmd(SPI4, DISABLE);

        /* SPI configuration */
        SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
        SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
        SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
        SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
        SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
        SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
        SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
        SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
        SPI_InitStructure.SPI_CRCPolynomial = 7;
        SPI_Init(SPI4, &SPI_InitStructure);
#if  SPI4_DMA_ENABLE
        SPI_I2S_DMACmd(SPI4, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);
#endif        
        /* Enable the sFLASH_SPI */
        SPI_Cmd(SPI4, ENABLE);
        
        _gSPI_ModeRisterFlag[SPI4_Bus] = 0x01;
      }
      break;
    }
#endif   
    default :
      break;
  }
}

/**
  * @brief  SPI initialization.
  * @param  spi: description info struct of spi bus.
  * @return none.
  */
void  SPI_BusInit(_SPI_Bus_t* spi)
{
  switch(spi->bus_id)
  {
#if   USE_SPI1
    case  SPI1_Bus :
    {
      SPI_BspInit(spi);
      SPI_DMA_Config(spi);
      SPI_ModeConfig(spi);
      break;
    }
#endif
#if   USE_SPI2    
    case  SPI2_Bus :
    {
      SPI_BspInit(spi);
      SPI_DMA_Config(spi);
      SPI_ModeConfig(spi);
      break;
    }
#endif
#if   USE_SPI4    
    case  SPI4_Bus :
    {
      SPI_BspInit(spi);
      SPI_DMA_Config(spi);
      SPI_ModeConfig(spi);
      break;
    }
#endif    
    default :
      break;
  }
}

/**
  * @brief  inquire the state of Rx stream.
  * @param  spi: description info struct of spi bus.
  * @return none.
  */
FlagStatus  SPI_RxStreamIsReady(_SPI_Bus_t* spi)
{
  FlagStatus  stream_status = RESET;
  
  switch(spi->bus_id)
  {
#if   USE_SPI1
    case  SPI1_Bus :
    {
//      if(DMA2_Stream0->CR & 0x01)
//      {   
//        DMA2_Stream0->CR &= ~(u32)DMA_SxCR_EN;
//        stream_status = RESET;
//      }
//      else
//      {
//        stream_status = SET;
//      }
      stream_status = SET;
      break;
    }
#endif    
#if   USE_SPI2
    case  SPI2_Bus :
    {
//      if(DMA1_Stream3->CR & 0x01)
//      {   
//        DMA1_Stream3->CR &= ~(u32)DMA_SxCR_EN;
//        stream_status = RESET;
//      }
//      else
//      {
//        stream_status = SET;
//      }
      break;
    }
#endif
#if   USE_SPI4    
    case  SPI4_Bus :
    {
//      if(DMA2_Stream0->CR & 0x01)
//      {   
//        DMA2_Stream0->CR &= ~(u32)DMA_SxCR_EN;
//        stream_status = RESET;
//      }
//      else
//      {
//        stream_status = SET;
//      }
      break;
    }
#endif
    default :
      break;
  }
  
  return stream_status;
}

/**
  * @brief  inquire the state of Tx stream.
  * @param  spi: description info struct of spi bus.
  * @return none.
  */
FlagStatus  SPI_TxStreamIsReady(_SPI_Bus_t* spi)
{
  FlagStatus  stream_status = RESET;
  
  switch(spi->bus_id)
  {
#if   USE_SPI1    
    case  SPI1_Bus :
    {
//      if(DMA2_Stream3->CR & 0x01)
//      {   
//        DMA2_Stream3->CR &= ~(u32)DMA_SxCR_EN;
//        stream_status = RESET;
//      }
//      else
//      {
//        stream_status = SET;
//      }
      stream_status = SET;
      break;
    }
#endif
#if   USE_SPI2    
    case  SPI2_Bus :
    {
//      if(DMA1_Stream4->CR & 0x01)
//      {   
//        DMA1_Stream4->CR &= ~(u32)DMA_SxCR_EN;
//        stream_status = RESET;
//      }
//      else
//      {
//        stream_status = SET;
//      }
      break;
    }
#endif
#if   USE_SPI4    
    case  SPI4_Bus :
    {
//      if(DMA2_Stream1->CR & 0x01)
//      {   
//        DMA2_Stream1->CR &= ~(u32)DMA_SxCR_EN;
//        stream_status = RESET;
//      }
//      else
//      {
//        stream_status = SET;
//      }
      break;
    }
#endif    
    default :
      break;
  }
  
  return  stream_status;
}

/**
  * @brief  write or read one byte.
  * @param  spi: description info struct of spi bus.
  * @param  data: to be write.
  * @return one byte data to be read.
  */
u8   SPI_WriteReadOneByte(_SPI_Bus_t* spi, u8 data)
{
  u16 time_out = 0;
  u8  temp_data = 0;

  switch(spi->bus_id)
  {
#if   USE_SPI1    
    case  SPI1_Bus :
    {
      while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
      {
        time_out ++;
        if(time_out > 0xFFF)
        {
          return  0;
        }
      }
      SPI_I2S_SendData(SPI1, (u16)data);
      time_out = 0;
      while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
      {
        time_out ++;
        if(time_out > 0xFFF)
          return  0;
      }
      
      temp_data = (SPI_I2S_ReceiveData(SPI1)); 
      break;
    }
#endif
#if   USE_SPI2    
    case  SPI2_Bus :
    {
      while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)
      {
        time_out ++;
        if(time_out > 0xFFF)
        {
          return  0;
        }
      }
      SPI_I2S_SendData(SPI2, (u16)data);
      time_out = 0;
      while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
      {
        time_out ++;
        if(time_out > 0xFFF)
          return  0;
      }
      
      temp_data = (SPI_I2S_ReceiveData(SPI2)); 
      break;
    }
#endif
#if   USE_SPI4    
    case  SPI4_Bus :
    {
      while(SPI_I2S_GetFlagStatus(SPI4, SPI_I2S_FLAG_TXE) == RESET)
      {
        time_out ++;
        if(time_out > 0xFFF)
        {
          return  0;
        }
      }
      SPI_I2S_SendData(SPI4, (u16)data);
      time_out = 0;
      while(SPI_I2S_GetFlagStatus(SPI4, SPI_I2S_FLAG_RXNE) == RESET)
      {
        time_out ++;
        if(time_out > 0xFFF)
          return  0;
      }
      
      temp_data = (SPI_I2S_ReceiveData(SPI4)); 
      break;
    }
#endif    
    default :
      break;
  }
  
  return  temp_data; 
}

/**
  * @brief  DMA read in bytes.
  * @param  spi: description info struct of spi bus.
  * @param  length: bytes number.
  * @param  pBuffer: data to be transfer.
  * @return none.
  */
void  SPI_DMA_ReadBuffer(_SPI_Bus_t* spi, u16 length, u8* pBuffer)
{
//  u16 dummy = 0x0000;
  u32 time_out = 0xfffffff;
  
  switch(spi->bus_id)
  {
#if   USE_SPI1    
    case  SPI1_Bus :
    {
//      DMA2_Stream0->NDTR = length;
//      DMA2_Stream0->M0AR = (u32)pBuffer;

//      DMA2_Stream3->NDTR = length;
//      DMA2_Stream3->M0AR = (u32)dummy;
//      
//      if(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
//      {
//        dummy = SPI1->DR;
//      }
//      
//      DMA2_Stream0->CR |= (u32)DMA_SxCR_EN;
//      DMA2_Stream3->CR |= (u32)DMA_SxCR_EN;
//        
//      while(!(DMA2->LISR & 0x08000000) && time_out){
//        time_out--;
//      }
//      while((!(DMA2->LISR & 0x00000020)) && time_out){
//        time_out--; 
//      }
//          
//      DMA2->LIFCR = 0x08000020;
//                  
//      DMA2_Stream0->CR &= ~(u32)DMA_SxCR_EN;
//      DMA2_Stream3->CR &= ~(u32)DMA_SxCR_EN;

      DMA1_Channel3->CPAR = SPI1_DR_Addr ;      //设置外设地址，注意PSIZE
      DMA1_Channel3->CMAR = (uint32_t)SPI1_TX_Buff ; //设置DMA存储器地址，注意MSIZE

      DMA1_Channel2->CPAR = SPI1_DR_Addr ;      //设置外设地址，注意PSIZE
      DMA1_Channel2->CMAR = (u32)pBuffer ; //设置DMA存储器地址，注意MSIZE

      SPI1_ReceiveSendByte(length);

      break;
    }
#endif
#if   USE_SPI2    
    case  SPI2_Bus :
    {
//      DMA1_Stream3->NDTR = length;
//      DMA1_Stream3->M0AR = (u32)pBuffer;

//      DMA1_Stream4->NDTR = length;
//      DMA1_Stream4->M0AR = (u32)dummy;
//      
//      if(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
//      {
//        dummy = SPI1->DR;
//        }
//      
//      DMA1_Stream3->CR |= (u32)DMA_SxCR_EN;
//      DMA1_Stream4->CR |= (u32)DMA_SxCR_EN;
//        
//      while(!(DMA1->HISR & 0x00000020) && time_out){  /* Tx transfer complete */
//        time_out--;
//      }
//      while((!(DMA1->LISR & 0x08000000)) && time_out){
//        time_out--; 
//      }
//          
//      DMA1->LIFCR = 0x08000000;
//      DMA1->HIFCR = 0x00000020;
//                  
//      DMA1_Stream3->CR &= ~(u32)DMA_SxCR_EN;
//      DMA1_Stream4->CR &= ~(u32)DMA_SxCR_EN;
      break;
    }
#endif
#if   USE_SPI4    
    case  SPI4_Bus :
    {
//      DMA2_Stream0->NDTR = length;
//      DMA2_Stream0->M0AR = (u32)pBuffer;

//      DMA2_Stream1->NDTR = length;
//      DMA2_Stream1->M0AR = (u32)dummy;
//      
//      if(SPI_I2S_GetFlagStatus(SPI4, SPI_I2S_FLAG_RXNE) == RESET)
//      {
//        dummy = SPI1->DR;
//        }
//      
//      DMA2_Stream0->CR |= (u32)DMA_SxCR_EN;
//      DMA2_Stream1->CR |= (u32)DMA_SxCR_EN;
//        
//      while(!(DMA2->LISR & 0x00000800) && time_out){
//        time_out--;
//      }
//      while((!(DMA2->LISR & 0x00000020)) && time_out){
//        time_out--; 
//      }
//          
//      DMA2->LIFCR = 0x00000820;
//                  
//      DMA2_Stream0->CR &= ~(u32)DMA_SxCR_EN;
//      DMA2_Stream1->CR &= ~(u32)DMA_SxCR_EN;
      break;
    }
#endif    
    default :
      break;
  }  
  
}

/**
  * @brief  DMA write in bytes.
  * @param  spi: description info struct of spi bus.
  * @param  length: bytes number.
  * @param  pBuffer: data to be transfer.
  * @return none.
  */
void  SPI_DMA_WriteBuffer(_SPI_Bus_t* spi, u16 length, const u8* pBuffer)
{
  u16 dummy = 0x0000;
  u32 time_out = 0xfffffff;
  
  switch(spi->bus_id)
  {
#if   USE_SPI1
    case  SPI1_Bus :
    {
//      DMA2_Stream0->NDTR = length;
//      DMA2_Stream0->M0AR = (u32)dummy;

//      DMA2_Stream3->NDTR = length;
//      DMA2_Stream3->M0AR = (u32)pBuffer;
//      
//      if(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
//      {
//        dummy = SPI1->DR;
//        }
//      
//      DMA2_Stream0->CR |= (u32)DMA_SxCR_EN;
//      DMA2_Stream3->CR |= (u32)DMA_SxCR_EN;
//        
//      while(!(DMA2->LISR & 0x08000000) && time_out){
//        time_out--;
//      }
//      while((!(DMA2->LISR & 0x00000020)) && time_out){
//        time_out--; 
//      }
//          
//      DMA2->LIFCR = 0x08000020;
//                  
//      DMA2_Stream0->CR &= ~(u32)DMA_SxCR_EN;
//      DMA2_Stream3->CR &= ~(u32)DMA_SxCR_EN;

      DMA1_Channel3->CPAR = SPI1_DR_Addr ;      //设置外设地址，注意PSIZE
      DMA1_Channel3->CMAR = (uint32_t)pBuffer ; //设置DMA存储器地址，注意MSIZE

      DMA1_Channel2->CPAR = SPI1_DR_Addr ;      //设置外设地址，注意PSIZE
      DMA1_Channel2->CMAR = (u32)SPI1_RX_Buff ; //设置DMA存储器地址，注意MSIZE

      SPI1_ReceiveSendByte(length);

      break;
    }
#endif
#if   USE_SPI2    
    case  SPI2_Bus :
    {
//      DMA1_Stream3->NDTR = length;
//      DMA1_Stream3->M0AR = (u32)dummy;

//      DMA1_Stream4->NDTR = length;
//      DMA1_Stream4->M0AR = (u32)pBuffer;
//      
//      if(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
//      {
//        dummy = SPI1->DR;
//        }
//      
//      DMA1_Stream3->CR |= (u32)DMA_SxCR_EN;
//      DMA1_Stream4->CR |= (u32)DMA_SxCR_EN;
//        
//      while(!(DMA1->HISR & 0x00000020) && time_out){
//        time_out--;
//      }
//      while((!(DMA1->LISR & 0x08000000)) && time_out){
//        time_out--; 
//      }
//          
//      DMA1->LIFCR = 0x08000000;
//      DMA1->HIFCR = 0x00000020;
//                  
//      DMA1_Stream3->CR &= ~(u32)DMA_SxCR_EN;
//      DMA1_Stream4->CR &= ~(u32)DMA_SxCR_EN;
      break;
    }
#endif
#if   USE_SPI4    
    case  SPI4_Bus :
    {
//      DMA2_Stream0->NDTR = length;
//      DMA2_Stream0->M0AR = (u32)dummy;

//      DMA2_Stream1->NDTR = length;
//      DMA2_Stream1->M0AR = (u32)pBuffer;
//      
//      if(SPI_I2S_GetFlagStatus(SPI4, SPI_I2S_FLAG_RXNE) == RESET)
//      {
//        dummy = SPI1->DR;
//        }
//      
//      DMA2_Stream0->CR |= (u32)DMA_SxCR_EN;
//      DMA2_Stream1->CR |= (u32)DMA_SxCR_EN;
//        
//      while(!(DMA2->LISR & 0x00000800) && time_out){
//        time_out--;
//      }
//      while((!(DMA2->LISR & 0x00000020)) && time_out){
//        time_out--; 
//      }
//          
//      DMA2->LIFCR = 0x00000820;
//                  
//      DMA2_Stream0->CR &= ~(u32)DMA_SxCR_EN;
//      DMA2_Stream1->CR &= ~(u32)DMA_SxCR_EN;
      break;
    }
#endif    
    default :
      break;
  }    
}


void SPI1_ReceiveSendByte(u16 num)
{  
    DMA1_Channel2->CNDTR = 0x0000   ;           //传输数量寄存器清零  
    DMA1_Channel2->CNDTR = num ;         //传输数量设置为buffersize个  
  
    DMA1_Channel3->CNDTR = 0x0000   ;           //传输数量寄存器清零  
    DMA1_Channel3->CNDTR = num ;         //传输数量设置为buffersize个  
  
    DMA1->IFCR = 0xF0 ;                         //清除通道2的标志位  
    DMA1->IFCR = 0xF00 ;                        //清除通道3的标志位  
  
    SPI1->DR ;                                   //接送前读一次SPI1->DR，保证接收缓冲区为空  
  
    while( ( SPI1->SR & 0x02 ) == 0 );  
      
    DMA1_Channel3->CCR |= 1 << 0 ;              //开启DMA通道3  
    DMA1_Channel2->CCR |= 1 << 0 ;              //开启DMA通道2   
  
    while( ( DMA1->ISR & 0x20 ) == 0 );  
  
    DMA1_Channel3->CCR &= ~( 1 << 0 ) ;         //关闭DMA通道3  
    DMA1_Channel2->CCR &= ~( 1 << 0 ) ;         //关闭DMA通道2  
  
}


