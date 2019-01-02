#ifndef _BSP_SPI_H__
#define _BSP_SPI_H__
#include "stm32f10x.h"
#include <stdio.h>

/*SPI-LCD接口定义*/
#define LCD_SPI							          SPI3
#define LCD_SPI_APBxCLOCK_RUN			    RCC_APB1PeriphClockCmd
#define LCD_SPI_CLK						        RCC_APB1Periph_SPI3

/*CS(NSS)*/
#define LCD_SPI_GPIO_APBxCLOCK_FUN		RCC_APB2PeriphClockCmd
#define LCD_SPI_CS_CLK					      RCC_APB2Periph_GPIOB
#define LCD_SPI_CS_PORT					      GPIOB
#define LCD_SPI_CS_PIN					      GPIO_Pin_6

/*SCK*/
#define LCD_SPI_SCK_CLK					      RCC_APB2Periph_GPIOB
#define LCD_SPI_SCK_PORT				      GPIOB
#define LCD_SPI_SCK_PIN					      GPIO_Pin_3
      
/*MISO*/
#define LCD_SPI_MISO_CLK				      RCC_APB2Periph_GPIOB
#define LCD_SPI_MISO_PORT				      GPIOB
#define LCD_SPI_MISO_PIN				      GPIO_Pin_4

/*MOSI*/
#define LCD_SPI_MOSI_CLK				      RCC_APB2Periph_GPIOB
#define LCD_SPI_MOSI_PORT				      GPIOB
#define LCD_SPI_MOSI_PIN				      GPIO_Pin_5


#define LCD_SPI_CS_LOW				        GPIO_ResetBits(LCD_SPI_CS_PORT, GPIO_Pin_6)
#define LCD_SPI_CS_HIGH				        GPIO_SetBits(LCD_SPI_CS_PORT, GPIO_Pin_6)


void SPI_LCD_Init(void);
uint8_t SPI_WriteByte(uint8_t byte);



#endif
