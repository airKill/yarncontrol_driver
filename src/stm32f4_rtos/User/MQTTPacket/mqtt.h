#ifndef __MQTT_H__
#define __MQTT_H__

#include "FreeRTOS.h"
#include "MQTTPacket.h"
#include "transport.h"

#define MQTT_SERVER_HOST            "ithaozi.imwork.net"		//ʵ�ʴ������޸ĳ��Լ��ķ���������
//#define MQTT_SERVER_HOST            "igmfrvb.mqtt.iot.bj.baidubce.com"		//ʵ�ʴ������޸ĳ��Լ��ķ���������
#define MQTT_SERVER_PORT            1883				//ʵ�ʴ������޸ĳ��Լ��ķ������˿�
#define MQTT_ALIVE_INTERVAL         60					//ʵ�ʴ������޸ĳ��Լ�������ʱ�䣨�룩
//#define MQTT_USER                   "admin"				//ʵ�ʴ������޸ĳ��Լ����û���
//#define MQTT_PASSWORD               "password"				//ʵ�ʴ������޸ĳ��Լ�������
#define MQTT_USER                   "igmfrvb/device0"				//�ٶ��칤������û���
#define MQTT_PASSWORD               "DjW1FIUXGlhDHhxs"				//�ٶ��칤���������
#define MQTT_TOPIC_BASE             "topic/+"		//ʵ�ʴ������޸ĳ��Լ��Ķ�������
#define MQTT_TOPIC_PUBLISH	     "topic22"	//ʵ�ʴ������޸ĳ��Լ��ķ�������

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

extern xQueueHandle xQueue_MQTT_Recv;

/*Mqtt״̬�ṹ��*/
typedef struct
{
  uint8_t connect;
  int socket;
}MqttSta_E;
extern MqttSta_E mqtt_status;

void MQTT_Init(void);
int MQTT_Connect(void);


#endif /*__MQTT_CONFIG_H__*/
