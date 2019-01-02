#include "bsp_lcd.h"
#include "bsp_systick.h"

/***********************************************************
*SPI-LCD 初始化
***********************************************************/
static void SpiLcdInit(void)
{
	SPI_InitTypeDef SpiLcd_Initstruct;
	GPIO_InitTypeDef GpioLcd_Initstruct;
	/*使能时钟*/
	LCD_SPI_APBxCLOCK_RUN(LCD_SPI_CLK,ENABLE);
	LCD_SPI_GPIO_APBxCLOCK_FUN(LCD_SPI_CS_CLK |LCD_SPI_SCK_CLK | \
								LCD_SPI_MISO_CLK | LCD_SPI_MOSI_CLK,ENABLE);
	/*配置LCD-LED*/
	GpioLcd_Initstruct.GPIO_Pin = LCD_TED_PIN;
	GpioLcd_Initstruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GpioLcd_Initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LCD_TED_GPIO,&GpioLcd_Initstruct);

	/*配置LCD-RC_DC*/
	GpioLcd_Initstruct.GPIO_Pin = LCD_RC_DC_PIN;
	GPIO_Init(LCD_RC_DC_GPIO,&GpioLcd_Initstruct);
	
	/*配置LCD-RESET*/
	GpioLcd_Initstruct.GPIO_Pin = LCD_RESET_PIN;
	GPIO_Init(LCD_RESET_GPIO,&GpioLcd_Initstruct);
									
	/*配置SPI--CS*/
	GpioLcd_Initstruct.GPIO_Pin = LCD_SPI_CS_PIN;
	GPIO_Init(LCD_SPI_CS_PORT,&GpioLcd_Initstruct);

	/*配置SPI--SCK*/
	GpioLcd_Initstruct.GPIO_Pin = LCD_SPI_SCK_PIN;
	GpioLcd_Initstruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GpioLcd_Initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LCD_SPI_SCK_PORT,&GpioLcd_Initstruct);

	/*配置SPI--MISO*/
	GpioLcd_Initstruct.GPIO_Pin = LCD_SPI_MISO_PIN;
	GPIO_Init(LCD_SPI_MISO_PORT,&GpioLcd_Initstruct);

	/*配置SPI--MOSI*/
	GpioLcd_Initstruct.GPIO_Pin = LCD_SPI_MOSI_PIN;
	GPIO_Init(LCD_SPI_MOSI_PORT,&GpioLcd_Initstruct);

	/*停止信号*/
	SPILCD_CS_LCD_SET;
	
	/*SPI 模式设置*/
	SpiLcd_Initstruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SpiLcd_Initstruct.SPI_Mode = SPI_Mode_Master;
	SpiLcd_Initstruct.SPI_DataSize = SPI_DataSize_8b;
	SpiLcd_Initstruct.SPI_CPOL = SPI_CPOL_High;
	SpiLcd_Initstruct.SPI_CPHA = SPI_CPHA_2Edge;
	SpiLcd_Initstruct.SPI_NSS = SPI_NSS_Soft;
	SpiLcd_Initstruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SpiLcd_Initstruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SpiLcd_Initstruct.SPI_CRCPolynomial = 7;
	SPI_Init(LCD_SPI,&SpiLcd_Initstruct);

	/*使能SPI*/
	SPI_Cmd(LCD_SPI, ENABLE);
}
/***********************************************************
*SPI发送一个字节数据
***********************************************************/
static uint8_t SpiLcd_SendByte(uint8_t byte)
{
	/*等待发送缓冲区为空，TXE事件*/
	while (SPI_I2S_GetFlagStatus(LCD_SPI,SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(LCD_SPI,byte);
	/*等待发送缓冲区为空，RXNE事件*/
	while (SPI_I2S_GetFlagStatus(LCD_SPI,SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(LCD_SPI);
}
/***********************************************************
*写Reg参数
***********************************************************/
static void LCDWriteReg(uint16_t regval)
{
	SPILCD_CS_LCD_RESET;
	SPILCD_CTR_RC_DC_RESET;
	SpiLcd_SendByte(regval & 0x00ff);
	SPILCD_CS_LCD_SET;
}
/***********************************************************
*写Lcd数据 --16位
***********************************************************/
static void LCDWriteData16(uint16_t data)
{
	SPILCD_CS_LCD_RESET;
	SPILCD_CTR_RC_DC_RESET;
	SpiLcd_SendByte(data>>8);
	SpiLcd_SendByte(data);
	SPILCD_CS_LCD_SET;
}
/***********************************************************
*写Lcd数据 --8位
***********************************************************/
static void LCDWriteData8(uint8_t data)
{
	SPILCD_CS_LCD_RESET;
	SPILCD_CTR_RC_DC_RESET;
	SpiLcd_SendByte(data);
	SPILCD_CS_LCD_SET;
}
/***********************************************************
*写LCD寄存器
***********************************************************/
static void LCD_WriteRegData(uint8_t LCD_Reg,uint16_t LCD_RegValue)
{
	LCDWriteReg(LCD_Reg);
	LCDWriteData16(LCD_RegValue);
}
/***********************************************************
*LCD初始化
***********************************************************/
void LcdInit(void)
{
	/*初始化SPI*/
	SpiLcdInit();
	/*RESET脚上升沿*/
	SPILCD_CTR_RESET_RESET;
	DelayMS(20);
	SPILCD_CTR_RESET_SET;
	DelayMS(20);

	/*功耗A设置*/
	LCDWriteReg(0xcb);
	LCDWriteData8(0x39);
	LCDWriteData8(0x2c);
	LCDWriteData8(0x00);
	LCDWriteData8(0x34);
	LCDWriteData8(0x02);

	/*功耗B设置*/
	LCDWriteReg(0xcf);
	LCDWriteData8(0x00);
	LCDWriteData8(0x81);
	LCDWriteData8(0x30);

	/*驱动时序控制A*/
	LCDWriteReg(0xe8);
	LCDWriteData8(0x85);
	LCDWriteData8(0x00);
	LCDWriteData8(0x78);

	/*驱动时序控制B*/
	LCDWriteReg(0xeA);
	LCDWriteData8(0x00);
	LCDWriteData8(0x00);

	/*电源时序控制*/
	LCDWriteReg(0xed);
	LCDWriteData8(0x64);
	LCDWriteData8(0x03);
	LCDWriteData8(0x12);
	LCDWriteData8(0x81);

	/*泵比控制*/
	LCDWriteReg(0xf7);
	LCDWriteData8(0x20);

	/*功耗控制1*/
	LCDWriteReg(0xc0);
	LCDWriteData8(0x23);

	/*功耗控制2*/
	LCDWriteReg(0xc1);
	LCDWriteData8(0x10);

	/* VCOM 控制 1*/
	LCDWriteReg(0xc5);
	LCDWriteData8(0x3e);
	LCDWriteData8(0x28);
	
	/* VCOM 控制 2*/
	LCDWriteReg(0xc7);
	LCDWriteData8(0x86);

	/*存贮器访问控制*/
	LCDWriteReg(0x36);
	LCDWriteData8(0xe8);

	/*像素格式设置*/
	LCDWriteReg(0x3A);
	LCDWriteData8(0x55);

	/*帧速率控制*/
	LCDWriteReg(0xB1);
	LCDWriteData8(0x00);
	LCDWriteData8(0x18);

	/*显示功能控制*/
	LCDWriteReg(0xB6);
	LCDWriteData8(0x08);
	LCDWriteData8(0x82);
	LCDWriteData8(0x27);

	/*使能3G*/
	LCDWriteReg(0xF2);
	LCDWriteData8(0x00);

	/*伽马设置*/
	LCDWriteReg(0x26);
	LCDWriteData8(0x01);	

	/*正极伽马设置*/
	LCDWriteReg(0xe0);
	LCDWriteData8(0x0f);
	LCDWriteData8(0x31);
	LCDWriteData8(0x2b);
	LCDWriteData8(0x0c);
	LCDWriteData8(0x0e);
	LCDWriteData8(0x08);
	LCDWriteData8(0x4e);
	LCDWriteData8(0xf1);
	LCDWriteData8(0x37);
	LCDWriteData8(0x07);
	LCDWriteData8(0x10);
	LCDWriteData8(0x03);
	LCDWriteData8(0x0e);
	LCDWriteData8(0x09);
	LCDWriteData8(0x00);

	/*负极伽马设置*/
	LCDWriteReg(0xe1);
	LCDWriteData8(0x00);
	LCDWriteData8(0x0e);
	LCDWriteData8(0x14);
	LCDWriteData8(0x03);
	LCDWriteData8(0x11);
	LCDWriteData8(0x07);
	LCDWriteData8(0x31);
	LCDWriteData8(0xc1);
	LCDWriteData8(0x48);
	LCDWriteData8(0x08);
	LCDWriteData8(0x0f);
	LCDWriteData8(0x0c);
	LCDWriteData8(0x31);
	LCDWriteData8(0x36);
	LCDWriteData8(0x0f);

	/*退出睡眠*/
	LCDWriteReg(0x11);
	DelayMS(120);
	/*开显示*/
	LCDWriteReg(0x29);
	/*存储器写*/
	LCDWriteReg(0x2c);
	
}
