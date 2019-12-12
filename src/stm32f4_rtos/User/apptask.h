#ifndef __APPTASK_H
#define __APPTASK_H

#include "includes.h"

#define soft_version "MB.0.1181212"

#define START   1
#define STOP    0

#define TASK_LED                0
#define TASK_LCD                1
#define TASK_MassStorage        2
#define TASK_MsgPro             3
#define TASK_Rev485             4
#define TASK_ReadDisk           5
#define TASK_RFID               6
#define TASK_MotorControl       7
#define TASK_ManageCapacity     8
#define TASK_Freq               9

#define IWDG_BIT_LED            (1<<0)  
#define IWDG_BIT_LCD            (1<<1)
#define IWDG_BIT_MassStorage    (1<<2)
#define IWDG_BIT_MsgPro         (1<<3)
#define IWDG_BIT_Rev485         (1<<4)
#define IWDG_BIT_ReadDisk         (1<<5)
#define IWDG_BIT_RFID         (1<<6)
#define IWDG_BIT_MotorControl         (1<<7)
#define IWDG_BIT_ManageCapacity         (1<<8)
#define IWDG_BIT_Freq         (1<<9)
#define IWDG_BIT_ALL   (IWDG_BIT_LED | IWDG_BIT_LCD | IWDG_BIT_MassStorage | IWDG_BIT_MsgPro | IWDG_BIT_Rev485  \
                      | IWDG_BIT_ReadDisk | IWDG_BIT_RFID | IWDG_BIT_MotorControl | IWDG_BIT_ManageCapacity     \
                        | IWDG_BIT_Freq)

#define REG_START       0
#define REG_ONOFF       1
#define REG_SET_WEIGHT      2
#define REG_GET_WEIGHT      3
#define REG_RESET       4
#define REG_CLEAR       5
#define REG_PRECISION       6

#define FUNC_WRITE   1
#define FUNC_READ   3

#define BROADCAST       0xff

#define DISK_IDLE       0
#define DISK_FILENAME    1
#define DISK_FILEDATA       2

typedef struct Msg
{
  u8 addr;
  u8 func;
  u8 reg;
  u16 value;
  u16 set_value;
  u16 hx711_offset;
  u8 onoff;
  u8 stop;
}SLAVE;

typedef enum
{
  WORK_STAGE_IDLE = 0,
  WORK_STAGE_SET_ONOFF,
  WORK_STAGE_GET_ALIVE,
  WORK_STAGE_SET_COMPARE,
  WORK_STAGE_START,
  WORK_STAGE_GET_VALUE,
  WORK_STAGE_STOP
}WORK_STAGE;

extern u16 device_speed_volate;

extern u8 usb_disk_flag;
extern u8 old_usb_disk_flag;

extern u8 download_num;
extern SLAVE slave_info; /* 定义一个结构体用于消息队列 */
extern u8 working_stage;
extern u8 isWork,old_isWork;
extern u8 card_record,old_card_record;
extern u8 page_num;
extern u8 readDisk;
extern u8 readFilenum;

extern u8 old_system_state;
extern u8 clear_text_flag;
/*
**********************************************************************************************************
变量声明
**********************************************************************************************************
*/
extern TaskHandle_t xHandleTaskLED;
extern TaskHandle_t xHandleTaskRFID;
extern TaskHandle_t xHandleTaskLCD;
extern TaskHandle_t xHandleTaskMassStorage;
extern TaskHandle_t xHandleTaskReadDisk;
extern TaskHandle_t xHandleTaskMsgPro;
extern TaskHandle_t xHandleTaskRev485;
extern TaskHandle_t xHandleTaskManageCapacity;
extern TaskHandle_t xHandleTaskPluseCount;
extern TaskHandle_t xHandleTaskMotorControl;
extern TaskHandle_t xHandleTaskFreq;
extern SemaphoreHandle_t  xSemaphore_encoder;
extern SemaphoreHandle_t  xSemaphore_pluse;
extern SemaphoreHandle_t  xSemaphore_lcd;
extern SemaphoreHandle_t  xSemaphore_rs485;
extern SemaphoreHandle_t  xSemaphore_download;
extern SemaphoreHandle_t  xSemaphore_readDisk;
extern SemaphoreHandle_t  xSemaphore_debug;
extern QueueHandle_t xQueue_message;
extern EventGroupHandle_t idwgEventGroup;
extern TimerHandle_t xTimerUser;

extern xQueueHandle xQueue_esp8266_Cmd;
extern xQueueHandle xQueue_MQTT_Recv;
extern xQueueHandle xQueue_MQTT_Transmit;
/*
**********************************************************************************************************
函数声明
**********************************************************************************************************
*/
void vTaskTaskLCD(void *pvParameters);
void vTaskMassStorage(void *pvParameters);
void vTaskTaskLED(void *pvParameters);
void vTaskMsgPro(void *pvParameters);
void vTaskRev485(void *pvParameters);
void AppTaskCreate (void);
void AppObjCreate (void);
void TIM_CallBack1(void);
void TIM_CallBack2(void);
void UserTimerCallback(TimerHandle_t xTimer);
void Task_iwdg_refresh(u8 task);
void vTaskManageCapacity(void *pvParameters);
void vMQTT_Tranmit_Task(void *ptr);
#endif
