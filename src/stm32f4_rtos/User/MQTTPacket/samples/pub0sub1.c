/*******************************************************************************
* Copyright (c) 2014 IBM Corp.
*
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* and Eclipse Distribution License v1.0 which accompany this distribution.
*
* The Eclipse Public License is available at
*    http://www.eclipse.org/legal/epl-v10.html
* and the Eclipse Distribution License is available at
*   http://www.eclipse.org/org/documents/edl-v10.php.
*
* Contributors:
*    Ian Craggs - initial API and implementation and/or initial documentation
*    Sergio R. Caprile - clarifications and/or documentation extension
*******************************************************************************/

#include "includes.h"

/* This is in order to get an asynchronous signal to stop the sample,
as the code loops waiting for msgs on the subscribed topic.
Your actual code will depend on your hw and approach*/
//#include <signal.h>

MqttSta_E mqtt_status;
uint8_t g_transmit = 0;
MQTT_STATUS_E Mqtt_status_step = MQTT_IDLE;
int toStop = 0;
MQTTString pub_topicString = MQTTString_initializer;
char MQTT_TOPIC[64] = {0};
char CLIENT_ID[32]= {0};

/**
******************************************************************************
* @brief  Sign接收函数
* @param  iSignNo sign号
* @return None.
******************************************************************************/
void SignHandler(int iSignNo)
{
  printf("Capture sign no:%d ", iSignNo);
}
/**
******************************************************************************
* @brief  MQTT内部调用
* @param  sig 无
* @return None.
******************************************************************************/

void cfinish(int sig)
{
  //    signal(SIGINT, SignHandler);
  toStop = 1;
}

/**
******************************************************************************
* @brief  MQTT内部调用
* @return None.
******************************************************************************/
void stop_init(void)
{
  //    signal(SIGINT, cfinish);
  //    signal(SIGTERM, cfinish);
}

void ReadChipID(char* cpId, int len)
{
  uint32_t ChipId[3] = {0};
  
  ChipId[0] = *(uint32_t *)(0x1ffff7e8);
  ChipId[1] = *(uint32_t *)(0x1ffff7ec);
  ChipId[2] = *(uint32_t *)(0x1ffff7f0);
  
  snprintf(cpId, len, "%08x%08x%08x", ChipId[0], ChipId[1], ChipId[2]);
}

/**
******************************************************************************
* @brief  MQTT初始化
* @return None.
******************************************************************************/
void MQTT_Init(void)
{
  ReadChipID(CLIENT_ID, sizeof(CLIENT_ID));
  //    snprintf(MQTT_TOPIC, sizeof(MQTT_TOPIC), "%s%s", MQTT_TOPIC_BASE, CLIENT_ID);
  sprintf(MQTT_TOPIC,"%s", MQTT_TOPIC_BASE);
  printf("MQTT_TOPIC is: %s\r\n", MQTT_TOPIC);
  //    MQTT_RB_Init();
  
  //    snprintf(MQTT_TOPIC_PUB,sizeof(MQTT_TOPIC_PUB),"%s%s",MQTT_TOPIC_PUBLISH,CLIENT_ID);
//  sprintf(MQTT_TOPIC_PUB,"%s",MQTT_TOPIC_PUBLISH);
//  pub_topicString.cstring = MQTT_TOPIC_PUB;
//  printf("publish topic: %s\r\n",pub_topicString.cstring);
  stop_init();
}


/**
******************************************************************************
* @brief  MQTT连接服务器
* @return < 0,连接错误，其他 返回socket
******************************************************************************/
int MQTT_Connect(void)
{
  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  unsigned char buf[200];
  int buflen = sizeof(buf);
  int len = 0;
  char *host = MQTT_SERVER_HOST;
  int port = MQTT_SERVER_PORT;
  
  printf("Sending to hostname:%s,port:%d\r\n", host, port);
  
  
  data.clientID.cstring = CLIENT_ID;
  data.keepAliveInterval = MQTT_ALIVE_INTERVAL;
  data.cleansession = 1;
//  data.username.cstring = MQTT_USER;
//  data.password.cstring = MQTT_PASSWORD;
  
  len = MQTTSerialize_connect(buf, buflen, &data);
  
//  for(uint8_t i=0;i<len;i++)
//    printf("%02x ",buf[i]);
//  printf("\r\n");
  transport_sendPacketBuffer(mqtt_status.socket, buf, len);
  
  /* wait for connack */
  if (MQTTPacket_read(buf, buflen, transport_getdata) == CONNACK)
  {
    unsigned char sessionPresent, connack_rc;
    
    if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 || connack_rc != 0)
    {
      printf("Unable to connect, return code %d\r\n", connack_rc);
      goto exit;
    }
  }
  else
    goto exit;
  
  printf("Connected to MQTT server\r\n");
  TimerCountdown(&last_ping,MQTT_ALIVE_INTERVAL);
  return mqtt_status.socket;
  
exit:
  transport_close(mqtt_status.socket);
  return -1;
}

/**
******************************************************************************
* @brief  订阅主题
* @param  topic_str 主题名
* @return the length of the serialized data.  <= 0 indicates error
******************************************************************************/
int MQTT_Subscribe(char* topic_str)
{
  MQTTString topicString = MQTTString_initializer;
  
  int len = 0;
  unsigned char buf[200];
  int buflen = sizeof(buf);
  int msgid = 1;
  int req_qos = 0;
  int rc;
  
  topicString.cstring = topic_str;
  //    printf("MQTT_SubscribeTopic:%s",topicString.cstring);
  len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos);
  
  transport_sendPacketBuffer(mqtt_status.socket, buf, len);
  //    vTaskDelay(2000 / portTICK_RATE_MS);
  if ((rc = MQTTPacket_read(buf, buflen, transport_getdata)) == SUBACK) 	/* wait for suback */
  {
    unsigned short submsgid;
    int subcount;
    int granted_qos;
    printf("MQTT_Subscribe Success!\r\n");
    rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
    if (granted_qos != 0)
    {
      printf("granted qos != 0, %d\r\n", granted_qos);
      goto exit;
    }
  }
  else
  {
    printf("MQTT_Subscribe Error %d\r\n", rc);
    goto exit;
  }
exit:
  return rc;
}


/**
******************************************************************************
* @brief 读取主题，阻塞方式
* @return the length of the serialized data.  <= 0 indicates error
******************************************************************************/
int MQTT_Read(void)
{
  int rc;
  static unsigned char buf[1000] = {0};
  int buflen = sizeof(buf);
  while (!toStop)
  {
    /* transport_getdata() has a built-in 1 second timeout, your mileage will vary */
    rc = MQTTPacket_read(buf, buflen, transport_getdata);
    //        printf("read rc = %d",rc);
    if (rc == PUBLISH)
    {
      printf("----------PUBLISH----------\r\n");
      MQTT_Recv_t mqtt_recv_t;
      MQTTDeserialize_publish(&mqtt_recv_t.dup, &mqtt_recv_t.qos, &mqtt_recv_t.retained, &mqtt_recv_t.msgid, &mqtt_recv_t.receivedTopic,
                              &mqtt_recv_t.payload_in, &mqtt_recv_t.payloadlen_in, buf, buflen);
      xQueueSend(xQueue_MQTT_Recv,  &mqtt_recv_t,  5);
      TimerCountdown(&last_ping,MQTT_ALIVE_INTERVAL);
      //            printf("\r\nmessage arrived %d, %s", mqtt_recv_t.payloadlen_in, mqtt_recv_t.payload_in);
    }
    else if(rc == PINGRESP)
    {
      printf("Recv MQTT PINGRESP\r\n");
      TimerCountdown(&last_ping,MQTT_ALIVE_INTERVAL);
    }
    else if(rc == DISCONNECT)
    {
      printf("MQTT Disconnect\r\n");
      goto exit;
    }
    else if(rc == -1)
    {
      printf("MQTT Disconnect2\r\n");
      goto exit;
    }
    else
    {
      printf("MQTT %d\r\n", rc);
    }
    memset(mqttSubscribeData,0,sizeof(mqttSubscribeData));
    
    vTaskDelay(20);
  }
exit:
  return rc;
}

int MQTT_Package_Publish(char *topicName,u8 *buf,u16 len)
{
  int rc = 0;
  if(Mqtt_status_step == MQTT_PUBLSH)
  {
    pub_topicString.cstring = topicName;
    pub_topicString.cstring[strlen(topicName)] = '\0';
    rc = MQTT_Publish(0, 0, 0, 0, pub_topicString, buf, len);
  }
  return rc;
}

int keepalive(void)
{
  int rc = 0;
  unsigned char buf[200];
  int buflen = sizeof(buf);
  int len;
  if(MQTT_GetConnected())
  {
//    if (TimerIsExpired(&last_sent) || TimerIsExpired(&last_received))
    if(TimerIsExpired(&last_ping))
    {
      printf("PING REQ\r\n");
      int len = MQTTSerialize_pingreq(buf, buflen);
      rc = transport_sendPacketBuffer(mqtt_status.socket, buf, len);
      TimerCountdown(&last_ping,MQTT_ALIVE_INTERVAL);
    }
  }
  return rc;
}


/**
******************************************************************************
* @brief 发布主题
* @param dup integer - the MQTT dup flag
* @param qos integer - the MQTT QoS value
* @param retained integer - the MQTT retained flag
* @param packetid integer - the MQTT packet identifier
* @param topicName MQTTString - the MQTT topic in the publish
* @param payload byte buffer - the MQTT publish payload
* @param payloadlen integer - the length of the MQTT payload
* @return the length of the serialized data.  <= 0 indicates error
******************************************************************************/
int MQTT_Publish(unsigned char dup, int qos, unsigned char retained, unsigned short packetid,
                 MQTTString topicName, unsigned char* payload, int payloadlen)
{
  int len, rc;
  unsigned char buf[256];
  int buflen = 256;
  //    printf("MQTT_Publish");
  if(MQTT_GetConnected())
  {
    len = MQTTSerialize_publish(buf, buflen, dup, qos, retained, packetid, topicName, payload, payloadlen);
    //len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, payloadlen);
    rc  = transport_sendPacketBuffer(mqtt_status.socket, buf, len);
    return rc;
  }
  else
  {
    printf("MQTT Disconneted\r\n");
    return -1;
  }
}

/**
******************************************************************************
* @brief  MQTT发送心跳，采用定时触发
* @param  xTimer
* @retval None.
******************************************************************************/
void MQTT_PingREQ(void)
{
  unsigned char buf[200];
  int buflen = sizeof(buf);
  int len;
  
  if(MQTT_GetConnected())
  {
    printf("PING REQ\r\n");
    len = MQTTSerialize_pingreq(buf, buflen);
    transport_sendPacketBuffer(mqtt_status.socket, buf, len);
  }
}

/**
******************************************************************************
* @brief  MQTT发送心跳，采用定时触发
* @param  xTimer
* @retval None.
******************************************************************************/
void MQTT_Disconnect(void)
{
  unsigned char buf[200];
  int buflen = sizeof(buf);
  int len;
  
  if(MQTT_GetConnected())
  {
    printf("MQTT_Disconnect\r\n");
    
    len = MQTTSerialize_disconnect(buf, buflen);
    transport_sendPacketBuffer(mqtt_status.socket, buf, len);
  }
}

/**
******************************************************************************
* @brief  获取连接状态
* @retval 连接状态
******************************************************************************/
uint8_t MQTT_GetConnected(void)
{
  uint8_t i;
  
  if((g_esp_status_t.esp_hw_status_e == ESP_HW_CONNECT_OK) 
     && (g_esp_status_t.esp_net_work_e == ESP_NETWORK_SUCCESS))
  {
    i = 1;
  }
  else
  {
    i = 0;
  }
  return i;
}

void MqttHandle(void)
{
  MQTT_Init();
  TimerInit(&last_ping);
}


