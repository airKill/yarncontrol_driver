#include "bsp_spi.h"

/***********************************************************
*SPI-LCD 初始化
***********************************************************/
void SPI_LCD_Init(void)
{
	SPI_InitTypeDef SpiLcd_Initstruct;
	GPIO_InitTypeDef GpioLcd_Initstruct;
	/*使能时钟*/
	LCD_SPI_APBxCLOCK_RUN(LCD_SPI_CLK,ENABLE);
	LCD_SPI_GPIO_APBxCLOCK_FUN(LCD_SPI_CS_CLK |LCD_SPI_SCK_CLK | \
								LCD_SPI_MISO_CLK | LCD_SPI_MOSI_CLK,ENABLE);

									
	/*配置SPI--CS*/
	GpioLcd_Initstruct.GPIO_Pin   = LCD_SPI_CS_PIN;
  GpioLcd_Initstruct.GPIO_Mode  = GPIO_Mode_Out_PP;
  GpioLcd_Initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LCD_SPI_CS_PORT,&GpioLcd_Initstruct);

	/*配置SPI--SCK*/
	GpioLcd_Initstruct.GPIO_Pin   = LCD_SPI_SCK_PIN;
	GpioLcd_Initstruct.GPIO_Mode  = GPIO_Mode_AF_PP;
	GpioLcd_Initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LCD_SPI_SCK_PORT,&GpioLcd_Initstruct);

	/*配置SPI--MOSI*/
	GpioLcd_Initstruct.GPIO_Pin   = LCD_SPI_MOSI_PIN;
	GPIO_Init(LCD_SPI_MOSI_PORT,&GpioLcd_Initstruct);

	/*配置SPI--MISO*/
	GpioLcd_Initstruct.GPIO_Pin   = LCD_SPI_MISO_PIN;
  GpioLcd_Initstruct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(LCD_SPI_MISO_PORT,&GpioLcd_Initstruct);



	/*停止信号*/
	LCD_SPI_CS_HIGH;
	
	/*SPI 模式设置*/
	SpiLcd_Initstruct.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
	SpiLcd_Initstruct.SPI_Mode              = SPI_Mode_Master;
	SpiLcd_Initstruct.SPI_DataSize          = SPI_DataSize_8b;
	SpiLcd_Initstruct.SPI_CPOL              = SPI_CPOL_High;
	SpiLcd_Initstruct.SPI_CPHA              = SPI_CPHA_2Edge;
	SpiLcd_Initstruct.SPI_NSS               = SPI_NSS_Soft;
	SpiLcd_Initstruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SpiLcd_Initstruct.SPI_FirstBit          = SPI_FirstBit_MSB;
	SpiLcd_Initstruct.SPI_CRCPolynomial     = 7;
	SPI_Init(LCD_SPI,&SpiLcd_Initstruct);

	/*使能SPI*/
	SPI_Cmd(LCD_SPI, ENABLE);
}


/***********************************************************
*SPI发送一个字节数据
***********************************************************/
uint8_t SPI_WriteByte(uint8_t byte)
{
	/*等待发送缓冲区为空，TXE事件*/
	while (SPI_I2S_GetFlagStatus(LCD_SPI,SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(LCD_SPI,byte);
	/*等待发送缓冲区为空，RXNE事件*/
	while (SPI_I2S_GetFlagStatus(LCD_SPI,SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(LCD_SPI);
}





