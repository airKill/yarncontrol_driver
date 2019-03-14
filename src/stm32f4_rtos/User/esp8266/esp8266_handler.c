#include "includes.h"

//#define USER_PRINT_BASE( format, args... )  printf( format, ##args )
//#define printf( format, args... ) USER_PRINT_BASE( "%s_%d:"format"\r\n",__FILE__,__LINE__, ##args )

/* 发送数据命令 */
#define SendDataCmd	("AT+CIPSEND=%d\r\n")

/* MQTT订阅数据缓存 */
uint8_t mqttSubscribeData[1000] = {0};

/* MCU ID */
//static char LONG_CLIENT_ID[32]= {0};
TickType_t lastSendOutTime = 0;

NET_DEVICE_INFO_T netDeviceInfo_t = {{0}, NULL};

/* ESP8266 连接状态初始化 */
ESP_STATUS_T g_esp_status_t = {ESP_HW_RESERVE_0,ESP_NETWORK_FAILED};

/* ESP8266 错误次数初始化 */
ESP_ERROR_T esp_error_t= {0,0,0,0};

/* 串口接收数据缓冲区 */
char gUsartReciveLineBuf[1000] = {0};

//==========================================================
//	函数名称：	net_device_send_cmd
//
//	函数功能：	向网络设备发送一条命令，并等待正确的响应
//
//	入口参数：	cmd：需要发送的命令
//				res：需要检索的响应
//
//	返回参数：	返回连接结果
//
//	说明：		0-成功		1-失败
//==========================================================
uint8_t net_device_send_cmd(char *cmd, char *res)
{
  uint8_t timeout = SEND_TIMEOUT_TIME;
  UART3ToPC(cmd,strlen(cmd));
  netDeviceInfo_t.cmd_hdl = res;
  if(res == NULL)
  {
    return 0;
  }
  while(netDeviceInfo_t.cmd_hdl != NULL && --timeout != 0)
  {
    vTaskDelay(10 / portTICK_RATE_MS);
  }
  if(timeout > 0)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

//==========================================================
//	函数名称：	ReconfigWIFI
//
//	函数功能：	重新配网
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		成功：立即重启		失败：超时重启
//==========================================================
void ReconfigWIFI(void)
{
  static uint16_t ConfigCnt = 0;
  printf("---------------long------------------\r\n");
  g_esp_status_t.esp_hw_status_e = ESP_HW_RECONFIG;
  g_esp_status_t.esp_net_work_e = ESP_NETWORK_FAILED ;
  
  //    LED0_OFF;
  if(net_device_send_cmd("AT+CWSMARTSTART=2\r\n","OK") == 0)
  {
    printf("Start smart config configure\r\n");
  }
  
  while(1)
  {
    if(strstr(netDeviceInfo_t.cmd_resp,"SMART SUCCESS") != NULL)//配网成功
    {
      /* 点亮三色灯 */
      
      printf("smart config ok\r\n");
      /* 5秒后重启，等待ESP8266给手机反馈配网结果*/
      vTaskDelay(5000 / portTICK_RATE_MS);
      NVIC_SystemReset();//重启
    }
    else
    {
      /* 10秒没成功 */
      if(ConfigCnt++ >= SMART_CONFIG_TIME)
      {
        /* 超时重启 */
        break;
      }
    }
    //        LED2_TOGGLE;
    vTaskDelay(50 / portTICK_RATE_MS);
  }
}

/**
******************************************************************************
**	函数名称：	ConnectTcp
**
**	函数功能：	连接TCP服务器
**
**	入口参数：	无
**
**	返回参数：	连接结果，成功1，失败0
**
**	说明：		命令处理成功则将指针置NULL
******************************************************************************
**/
uint8_t ConnectTcp(void)
{
  uint8_t tmp = 0;
  char buf[50];
  memset(buf,0,50);
  sprintf(buf,"AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",MQTT_SERVER_HOST,MQTT_SERVER_PORT);
  if(net_device_send_cmd(buf,"OK") == 0)
  {
    tmp = 1;
    printf("connect server tcp ok!");
  }
  else
    tmp = 0;
  return tmp;
}
/**
******************************************************************************
**	函数名称：	esp8266_cmd_handle
**
**	函数功能：	检查命令返回是否正确
**
**	入口参数：	cmd：需要发送的命令
**
**	返回参数：	无
**
**	说明：		命令处理成功则将指针置NULL
******************************************************************************
**/
void esp8266_cmd_handle(char *cmd)
{
  if(strstr(cmd,netDeviceInfo_t.cmd_hdl) != NULL)
  {
    netDeviceInfo_t.cmd_hdl = NULL;
  }
  memcpy(netDeviceInfo_t.cmd_resp,(const char*)cmd,sizeof(netDeviceInfo_t.cmd_resp));
  memset(gUsartReciveLineBuf,0,sizeof(gUsartReciveLineBuf));
}

/**
******************************************************************************
* @brief  检查ESP8266连接状态
* @return 状态值
******************************************************************************
**/
ESP_HW_STATUS_E check_esp8266_status(void)
{
  uint8_t tmp = 255;
  
  if(net_device_send_cmd("AT+CIPSTATUS\r\n", "OK") == 0)			//发送状态监测
  {
    if(strstr(netDeviceInfo_t.cmd_resp, "STATUS:2"))				//获得IP
    {
      tmp =  2;
    }
    else if(strstr(netDeviceInfo_t.cmd_resp, "STATUS:3"))			//建立连接
    {
      tmp = 3;
    }
    else if(strstr(netDeviceInfo_t.cmd_resp, "STATUS:4"))			//失去连接，断开tcp连接
    {
      tmp = 4;
    }
    else if(strstr(netDeviceInfo_t.cmd_resp, "STATUS:5"))			//物理掉线，关闭路由器电源，无WIFI信号
    {
      tmp = 5;
    }
    else if(strstr(netDeviceInfo_t.cmd_resp, "CLOSE"))
    {
      tmp = 4;
    }
    else
    {
      //nothing;
    }
  }
  else
  {
    if(strstr(netDeviceInfo_t.cmd_resp,"busy s...") != NULL)
      tmp = 6;
    else
      tmp = 7;
  }
  return (ESP_HW_STATUS_E)tmp;
}

int MQTT_RB_Read(uint8_t *buf, uint16_t len)
{
  u16 i;
  static uint8_t oldlen;
  //    printf("read data lenth = %d",len);
  while(1)
  {
    if(mqttSubscribeData[0] == 0)
    {
      oldlen = 0;
      
      /* 这里用于tcp掉线,MQTT重连服务器 */
      if(g_esp_status_t.esp_hw_status_e == ESP_HW_CONNECT_OK && g_esp_status_t.esp_net_work_e == ESP_NETWORK_FAILED
         && esp_error_t.err_esp_network > 5)
      {
        esp_error_t.err_esp_network = 0;
        return 0;
      }
      vTaskDelay(20);
    }
    else
    {
      break;
    }
    if(Mqtt_status_step == MQTT_PUBLSH)
    {
      keepalive();
    }
  }
//  printf("---------read len = %d",len);
  for(i = 0; i < len; i++)
  {
    *(buf + i) = mqttSubscribeData[i+oldlen];
//    printf("%02x",mqttSubscribeData[i+oldlen]);
  }
  oldlen += len;
  return i;
}


/**
******************************************************************************
* @brief  处理网络返回数据
* @param  Dataptr 缓冲区指针.
* @return 状态值,成功0，失败1
******************************************************************************
**/
uint8_t Handle_Internet_Data(char *Dataptr)
{
  uint8_t tmp = 255;
  char* ptrMao = NULL;
  /* 服务器返回成功 */
  ptrMao = strstr(Dataptr,":");
  if(ptrMao != NULL)
  {
    memset(mqttSubscribeData,0,sizeof(mqttSubscribeData));
    memcpy(mqttSubscribeData,(ptrMao+1),sizeof(mqttSubscribeData));
    memset(gUsartReciveLineBuf,0,sizeof(gUsartReciveLineBuf));
  }
  return tmp;
}

void SendDataServer(uint8_t *data,int len)
{
  char sendDataCmdBuf[20];
  sprintf(sendDataCmdBuf,(char *)SendDataCmd,len);

  if(g_esp_status_t.esp_net_work_e == ESP_NETWORK_SUCCESS)
  {
    if(net_device_send_cmd(sendDataCmdBuf,">") == 0)
    {
      printf("Send to esp8266 data len = %d",len);
      UART3ToPC(data,len);
    }
  }
}

void getTcpConnect(void)
{
  if(ConnectTcp() == 1)
  {
    g_esp_status_t.esp_hw_status_e = ESP_HW_CONNECT_OK;
    esp_error_t.err_esp_disconnect=0;
    esp_error_t.err_esp_lostwifi = 0;
    esp_error_t.err_esp_notresp = 0;
    printf("connect TCP server OK\r\n");
    Mqtt_status_step = MQTT_IDLE;
  }
  else
  {
    printf("connect TCP server Fail\r\n");
  }
}

/*---------------------------------------------------------------------------*/
int Esp8266_Tcp_Send(int socket, uint8_t *data, uint16_t len)
{
  uint8_t ret;
  char cmd[128] = {0};
  sprintf(cmd, "AT+CIPSEND=%d\r\n", len);
  //    printf("cmd :%s",cmd);
  if(net_device_send_cmd(cmd,">") == 0)
  {
    UART3ToPC(data,len);
    ret = 0;
  }
  else
    ret = 1;
  return ret;
}

uint8_t Esp8266_GetTcpStatus(void)
{
  uint8_t status = 0;
  if(	g_esp_status_t.esp_hw_status_e  == ESP_HW_CONNECT_OK )
    status = 1;
  return status;
}

