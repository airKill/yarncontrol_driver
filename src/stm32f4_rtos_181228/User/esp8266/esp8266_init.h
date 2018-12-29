#ifndef __ESP8266_INIT_H__
#define __ESP8266_INIT_H__

#include "stm32f4xx.h"

#define ESP8266_EN_PORT           	 GPIOD
#define ESP8266_EN_PIN             GPIO_Pin_6	//金老师开发板
//#define ESP8266_EN_PIN             	GPIO_Pin_7		//实际pcb
#define ESP8266_EN_RCC            	RCC_AHB1Periph_GPIOD

#define ESP8266_EN_ACTIVE          GPIO_SetBits(ESP8266_EN_PORT, ESP8266_EN_PIN)
#define ESP8266_EN_INACTIVE        GPIO_ResetBits(ESP8266_EN_PORT, ESP8266_EN_PIN)

#define ESP8266_USART				USART2

typedef struct
{
  char cmd_resp[100];					//命令回复指针。比如获取命令返回的数据，可以提取此指针内的数据
  char *cmd_hdl;					//命令处理。在发送命令后，会在返回内容里搜索用户指定的返回数据
} NET_DEVICE_INFO_T;


extern NET_DEVICE_INFO_T netDeviceInfo_t;


void esp8266_init(void);



#endif

