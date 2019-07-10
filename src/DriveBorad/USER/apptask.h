#ifndef __APPTASK_H
#define __APPTASK_H

#include "main.h"

#define soft_version "SB.0.1181212"

#define ZERO    60//零点，小于该值时，驱动板不工作
#define MAX_WEIGHT    18000//最大重量限制15KG

typedef enum
{
  PROCESS_STOP = 0,
  PROCESS_RUNNING,
  PROCESS_RESET,
  PROCESS_RESET_1,
  PROCESS_RESET_2,
  PROCESS_OVERCURRENTS,
  PROCESS_OVERWEIGHT,
  PROCESS_PAUSE
}DEVICE_PROCESS;

extern u8 Device_Process;
extern u8 old_Device_Process;

extern u8 start_stop;
extern u8 start_time_flag;
extern u8 start_time;
extern u8 rev_start_stop;
extern u16 k3_short_cnt;
extern u8 k3_short_flag;
extern u8 key_reset;
extern u16 key_reset_time; 

extern u8 key2_reset;
extern u16 key2_reset_time;

extern u8 overCurrent_flag;
extern u16 overCurrent_time;

extern u8 overWeight_flag;
extern u16 overWeight_time;

extern u8 cut_down_flag;
extern u8 cut_down_time;

void KeyProcess(void);
void AppTaskCreate (void);
void vTaskTaskSend485(void *pvParameters);
void vTaskRev485(void *pvParameters);
void vTaskSample(void *pvParameters);
void vTaskTaskKey(void *pvParameters);
void AppObjCreate(void);
//void UserTimerCallback(TimerHandle_t xTimer);
#endif