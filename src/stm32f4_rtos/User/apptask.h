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
#define TASK_Rev485         4
#define TASK_ReadDisk         5

#define IWDG_BIT_LED            (1<<0)  
#define IWDG_BIT_LCD            (1<<1)
#define IWDG_BIT_MassStorage    (1<<2)
#define IWDG_BIT_MsgPro         (1<<3)
#define IWDG_BIT_Rev485         (1<<4)
#define IWDG_BIT_ReadDisk         (1<<5)
#define IWDG_BIT_ALL   (IWDG_BIT_LED | IWDG_BIT_LCD | IWDG_BIT_MassStorage | IWDG_BIT_MsgPro | IWDG_BIT_Rev485 | IWDG_BIT_ReadDisk)

#define REG_START       0
#define REG_ONOFF       1
#define REG_SET_WEIGHT      2
#define REG_GET_WEIGHT      3
#define REG_RESET       4

#define FUNC_WRITE   1
#define FUNC_READ   3

#define BROADCAST       0xff

typedef struct Msg
{
  u8 addr;
  u8 func;
  u8 reg;
  u16 value;
  u16 set_value;
  u8 onoff;
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

extern u8 usb_disk_flag;

extern u8 download_num;
extern SLAVE slave_info; /* ����һ���ṹ��������Ϣ���� */
extern u8 working_stage;
extern u8 isWork,old_isWork;
/*
**********************************************************************************************************
��������
**********************************************************************************************************
*/
extern TaskHandle_t xHandleTaskLED;
extern TaskHandle_t xHandleTaskLCD;
extern TaskHandle_t xHandleTaskMassStorage;
extern TaskHandle_t xHandleTaskReadDisk;
extern TaskHandle_t xHandleTaskMsgPro;
extern TaskHandle_t xHandleTaskRev485;
extern TaskHandle_t xHandleTaskManageCapacity;
extern TaskHandle_t xHandleTaskPluseCount;

extern SemaphoreHandle_t  xSemaphore_pluse;
extern SemaphoreHandle_t  xSemaphore_lcd;
extern SemaphoreHandle_t  xSemaphore_rs485;
extern SemaphoreHandle_t  xSemaphore_download;
extern SemaphoreHandle_t  xSemaphore_readDisk;
extern SemaphoreHandle_t  xSemaphore_debug;
extern QueueHandle_t xQueue_message;
extern EventGroupHandle_t idwgEventGroup;
extern TimerHandle_t xTimerUser;
/*
**********************************************************************************************************
��������
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
#endif
