/* @version V1.0
* @date    2017-12-21
* @brief   esp8266初始化
******************************************************************************
*/
#include "string.h"
#include "stdio.h"
#include "esp8266_init.h"

/**
******************************************************************************
* @brief  配置ESP8266 复位管脚
* @retval None.
****************************************************************************
**/
static void esp8266_gpio_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(ESP8266_EN_RCC, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* 设为输出口 */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽模式 */
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* 上下拉电阻不使能 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO口最大速度 */
    
    GPIO_InitStructure.GPIO_Pin = ESP8266_EN_PIN;
    GPIO_Init(ESP8266_EN_PORT, &GPIO_InitStructure);
}

/**
******************************************************************************
* @brief  复位ESP8266
* @retval None.
****************************************************************************
**/
static void esp8266_gpio_reset(void)
{
    uint32_t i;
    ESP8266_EN_INACTIVE;
    for(i = 0; i<100000; i++);

    ESP8266_EN_ACTIVE;
    for(i = 0; i<100000; i++);

}

void esp8266_init(void)
{
    esp8266_gpio_config();
    esp8266_gpio_reset();
}


