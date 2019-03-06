#ifndef __APPTASK_H
#define __APPTASK_H

#include "main.h"

#define soft_version "SB.0.1181212"

#define ZERO    200//零点，小于该值时，驱动板不工作

typedef enum
{
  PROCESS_STOP = 0,
  PROCESS_RUNNING,
  PROCESS_RESET,
  PROCESS_RESET_1,
  PROCESS_RESET_2,
  PROCESS_OVERCURRENTS,
  PROCESS_PAUSE
}DEVICE_PROCESS;

extern u8 Device_Process;
extern u8 old_Device_Process;

extern u8 start_stop;
extern u16 k3_short_cnt;
extern u8 k3_short_flag;
extern u8 key_reset;
extern u16 key_reset_time; 

void KeyProcess(void);
void AppTaskCreate (void);
void vTaskTaskSend485(void *pvParameters);
void vTaskRev485(void *pvParameters);
void vTaskSample(void *pvParameters);
void vTaskTaskKey(void *pvParameters);
void AppObjCreate(void);
//void UserTimerCallback(TimerHandle_t xTimer);
#endif