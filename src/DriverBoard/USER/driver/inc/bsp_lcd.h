#ifndef _BSP_LCD_H__
#define _BSP_LCD_H__

#include "stm32f10x.h"
/*LCD接口定义*/
/*LCD-LED*/
#define LCD_TED_GPIO				GPIOB
#define LCD_TED_PIN					GPIO_Pin_9
/*LCD-RC/DC*/
#define LCD_RC_DC_GPIO		  GPIOB
#define LCD_RC_DC_PIN				GPIO_Pin_8
/*LCD-RESET*/
#define LCD_RESET_GPIO		  GPIOB
#define LCD_RESET_PIN				GPIO_Pin_7



#define SPILCD_CS_LCD_SET				      GPIO_SetBits(LCD_SPI_CS_PORT,LCD_SPI_CS_PIN)
#define SPILCD_CS_LCD_RESET				    GPIO_ResetBits(LCD_SPI_CS_PORT,LCD_SPI_CS_PIN)

#define SPILCD_CTR_LCD_SET				    GPIO_SetBits(LCD_TED_GPIO,LCD_TED_PIN)
#define SPILCD_CTR_LCD_RESET			    GPIO_ResetBits(LCD_TED_GPIO,LCD_TED_PIN)

#define SPILCD_CTR_RC_DC_SET			    GPIO_SetBits(LCD_RC_DC_GPIO,LCD_RC_DC_PIN)
#define SPILCD_CTR_RC_DC_RESET			  GPIO_ResetBits(LCD_RC_DC_GPIO,LCD_RC_DC_PIN)

#define SPILCD_CTR_RESET_SET			    GPIO_SetBits(LCD_RESET_GPIO,LCD_RESET_PIN)
#define SPILCD_CTR_RESET_RESET			  GPIO_ResetBits(LCD_RESET_GPIO,LCD_RESET_PIN)


void LcdInit(void);
static void LCD_WriteRegData(uint8_t LCD_Reg,uint16_t LCD_RegValue);
static void LCDWriteData8(uint8_t data);


/*LCD 结构体*/
typedef struct 
{
	uint16_t width;
	uint16_t height;
	uint16_t id;
	uint8_t CMD_Write_Garam;
	uint8_t CMD_SetX;
	uint8_t CMD_SetY;
}LCD_Dev;

typedef struct
{
	uint16_t LCD_REG;
	uint16_t LCD_RAM;
}LCD_Type;

extern LCD_Dev LCD_dev_stryct;
#define LCD_BASE_ADDR		((uint32_t)(0x60000000 | 0x0007FFFE))
#define LCD					(LCD_Type *)LCD_BASE_ADDR)
#endif
