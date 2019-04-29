#ifndef __ESP8266_HANDLER_H__
#define __ESP8266_HANDLER_H__

#include "FreeRTOS.h"
#include "queue.h"

#define USER_PRINT_BASE( format, args... )  printf( format, ##args )
#define debug_log( format, args... ) USER_PRINT_BASE( "%s_%d:"format"\r\n",__FILE__,__LINE__, ##args )

#define ESP_ERROR_CNT			10

/* SmartConfigrationTime(MS) */
#define SMART_CONFIG_TIME		10000

/* 串口发送超时(MS) */
#define SEND_TIMEOUT_TIME		200

/* 串口发送间隔(MS) */
#define USART_SEND_INTERVAL		1000

#define NO_CONNECT        0
#define WIFI_CONNECT      1
#define SEVER_CONNECT_OK     2

#define WIFI_SSID       "tplink589"
#define WIFI_PASSWORD   "y18509213517"
//#define WIFI_SSID       "znkzxt"
//#define WIFI_PASSWORD   "zn123654xt"

/* ESP8266硬件状态类型 */
typedef enum
{
  ESP_HW_RESERVE_0 = 0,
  ESP_HW_RESERVE_1,
  ESP_HW_GET_IPADDR,
  ESP_HW_CONNECT_OK,
  ESP_HW_DISCONNECT,
  ESP_HW_LOST_WIFI,
  ESP_HW_BUSY_STUS,
  ESP_HW_NOT_RESP,
  ESP_HW_RECONFIG
}ESP_HW_STATUS_E;

typedef enum{
  STA,
  AP,
  STA_AP  
} ENUM_Net_ModeTypeDef;

/* ESP8266网络状态类型 */
typedef enum
{
  ESP_NETWORK_FAILED = 0,
  ESP_NETWORK_SUCCESS,
}ESP_NET_WORK_E;

/* ESP8266状态类型 */
typedef struct
{
  ESP_HW_STATUS_E esp_hw_status_e;	//硬件工作状态
  ESP_NET_WORK_E esp_net_work_e;		//MQTT服务器连接状态
}ESP_STATUS_T;

typedef struct
{
  uint8_t err_esp_disconnect;			//TCP掉线
  uint8_t err_esp_lostwifi;			//WIFI信号关闭
  uint8_t err_esp_notresp;			//esp8266串口无响应
  uint8_t err_esp_network;
}ESP_ERROR_T;

extern u8 wifi_flag,old_wifi_flag;

extern ESP_STATUS_T g_esp_status_t;		//定义esp8266模块工作状态全局变量

extern char gUsartReciveLineBuf[1000];
extern uint8_t mqttSubscribeData[1000];

typedef enum
{
  MQTT_IDLE = 0,
  MQTT_CONNECT,
  MQTT_SUBSCRB,
  MQTT_PUBLSH,
}MQTT_STATUS_E;
extern ESP_ERROR_T esp_error_t;
extern MQTT_STATUS_E Mqtt_status_step;	//MQTT工作状态机
extern TickType_t lastSendOutTime;

int MQTT_RB_Read(uint8_t *buf, uint16_t len);
void ReconfigWIFI(void);

/* 获取状态线程 */
void vGetEsp8266Status(void *ptr);

uint8_t Handle_Internet_Data(char *Dataptr);
int Esp8266_Tcp_Send(int socket, uint8_t *data, uint16_t len);
uint8_t Esp8266_GetTcpStatus(void);
void SendDataServer(uint8_t *data,int len);
uint8_t net_device_send_cmd(char *cmd, char *res);
void esp8266_cmd_handle(char *cmd);
ESP_HW_STATUS_E check_esp8266_status(void);
void getTcpConnect(void);
u8 ESP8266_JoinAP (char * pSSID, char * pPassWord);
u8 ESP8266_Net_Mode_Choose ( ENUM_Net_ModeTypeDef enumMode );
u8 ESP8266_Enable_MultipleId ( FunctionalState enumEnUnvarnishTx );
#endif

