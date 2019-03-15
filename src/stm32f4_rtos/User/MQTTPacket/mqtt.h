#ifndef __MQTT_H__
#define __MQTT_H__

#include "FreeRTOS.h"
#include "MQTTPacket.h"
#include "transport.h"

#define MQTT_SERVER_HOST            "ithaozi.imwork.net"		//实际代码请修改成自己的服务器域名
//#define MQTT_SERVER_HOST            "igmfrvb.mqtt.iot.bj.baidubce.com"		//实际代码请修改成自己的服务器域名
#define MQTT_SERVER_PORT            1883				//实际代码请修改成自己的服务器端口
#define MQTT_ALIVE_INTERVAL         60					//实际代码请修改成自己的心跳时间（秒）
//#define MQTT_USER                   "admin"				//实际代码请修改成自己的用户名
//#define MQTT_PASSWORD               "password"				//实际代码请修改成自己的密码
#define MQTT_USER                   "igmfrvb/device0"				//百度天工物接入用户名
#define MQTT_PASSWORD               "DjW1FIUXGlhDHhxs"				//百度天工物接入密码
#define MQTT_TOPIC_BASE             "topic/+"		//实际代码请修改成自己的订阅主题
#define MQTT_TOPIC_PUBLISH	     "topic22"	//实际代码请修改成自己的发布主题

#define MQTT_TOPIC_WEIMI      "PUBLISH/WEIMI"
#define MQTT_TOPIC_WEISHA     "PUBLISH/WEISHA"
#define MQTT_TOPIC_CHANNENG   "PUBLISH/CHANNENG"
#define MQTT_TOPIC_PEILIAO    "PUBLISH/PEILIAO"
#define MQTT_TOPIC_STOP       "PUBLISH/STOP"

#define TOPIC_WEIMI      1
#define TOPIC_WEISHA     2
#define TOPIC_CHANNENG   3
#define TOPIC_PEILIAO    4
#define TOPIC_STOP       5

static char MQTT_TOPIC_PUB[64] = {0};
typedef struct
{
  unsigned char dup;
  int qos;
  unsigned char retained; 
  unsigned short msgid;
  int payloadlen_in;
  unsigned char* payload_in;
  MQTTString receivedTopic;
} MQTT_Recv_t;

/*Mqtt状态结构体*/
typedef struct
{
  uint8_t connect;
  int socket;
}MqttSta_E;
extern MqttSta_E mqtt_status;

void MQTT_Init(void);
int MQTT_Connect(void);


#endif /*__MQTT_CONFIG_H__*/
