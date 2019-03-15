#ifndef __PUB0SUB1_H
#define __PUB0SUB1_H

#include "MQTTPacket.h"

extern uint8_t g_transmit;
extern char MQTT_TOPIC[64];
extern char CLIENT_ID[32];
extern MQTTString pub_topicString;

int MQTT_Publish(unsigned char dup, int qos, unsigned char retained, unsigned short packetid,
                 MQTTString topicName, unsigned char* payload, int payloadlen);
int MQTT_Read(void);
int MQTT_Subscribe(char* topic_str);
void MqttHandle(void);
void MQTT_PingREQ(void);
void MQTT_Disconnect(void);
void ReadChipID(char* cpId, int len);
int MQTT_Package_Publish(char *topicName,u8 *buf,u16 len);
int keepalive(void);
#endif