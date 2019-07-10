#include "main.h"

u8 start_stop = 0,old_start_stop = 0;
u8 start_time_flag = 0;
u8 start_time = 0;
u8 rev_start_stop = 0;

u8 cut_down_flag = 0;
u8 cut_down_time = 0;

TaskHandle_t xHandleTaskKey = NULL;
TaskHandle_t xHandleTaskSend485 = NULL;
TaskHandle_t xHandleTaskRev485 = NULL;
TaskHandle_t xHandleTaskSample = NULL;

SemaphoreHandle_t  xSemaphore_rs485 = NULL;
TimerHandle_t xTimerUser = NULL;

u8 key_reset = 0;
u16 key_reset_time = 0;

u8 key2_reset = 0;
u16 key2_reset_time = 0;

u8 overCurrent_flag = 0;
u16 overCurrent_time = 0;

u8 overWeight_flag = 0;
u16 overWeight_time = 0;

u8 Device_Process = PROCESS_STOP;
u8 old_Device_Process = PROCESS_STOP;

u16 k3_short_cnt = 0;
u8 k3_short_flag = 0;
u8 link_err = 0;

void vTaskTaskKey(void *pvParameters)
{
  u8 ucKeyCode;
  while(1)
  {
    ucKeyCode = bsp_GetKey();
    if(ucKeyCode != KEY_NONE)
    {
      switch(ucKeyCode)
      {
        case KEY_DOWN_K1://����λ,����������,���ֹͣ
//          motor_dir = MOTOR_STOP;
//          motor_control(motor_dir); 
//          SetNull();//����������У׼
//          NULL_IDLE = Read_HX711_Count() / 100;
//          device_info.hx711_offset = (u32)NULLMASS;
//          __set_PRIMASK(1);
//          Write_flash(USER_FLASH_ADDR,(u16*)&device_info,sizeof(device_info) / 2);
//          __set_PRIMASK(0);
          LED1_ON();
//          bsp_StartHardTimer(1,50000,(void *)TIM_CallBack1);
          Device_Process = PROCESS_RESET_2;
          key_reset = 0;
//          if((device_info.onoff == 0) || (start_stop == 0))
//          {//��λ������λ�޿����źţ���ʼ��תһ����
//            Device_Process = PROCESS_RESET_2;
//            key_reset = 0;
//          }
//          else
//          {
//            Device_Process = PROCESS_RUNNING;
//          }
          printf("����λУ׼���\r\n");
          break;
        case KEY_LONG_K1:
          printf("����λ\r\n");
          break;    
        case KEY_DOWN_K2:	//����λ
          key2_reset = 0;
          Device_Process = PROCESS_RESET_1;
          printf("����λ��λ\r\n");
          break;  
        case KEY_LONG_K2:
          key2_reset = 0;
          Device_Process = PROCESS_RESET_1;
          printf("����λ��λ\r\n");
          break;  
        case KEY_DOWN_K3:
          if(k3_short_flag == 0)
          {
            k3_short_flag = 1;
            printf("k3 click.\r\n");
          }
          else if(k3_short_flag == 1)
          {
            if(k3_short_cnt < 200)
            {//���ΰ���ʱ��С��100ms����Ϊ��˫��
              k3_short_flag = 2;
              old_Device_Process = Device_Process;
              Device_Process = PROCESS_PAUSE;
              printf("k3 double click.\r\n");
            }
            else
            {
              k3_short_flag = 0;
              k3_short_cnt = 0;
              printf("cancel double click.\r\n");
            }
          }
          else if(k3_short_flag == 2)
          {
            k3_short_flag = 0;
            k3_short_cnt = 0;
            Device_Process = old_Device_Process;
            printf("continue...\r\n");
          }
          break;
        case KEY_LONG_K3://������λ��ť
          Device_Process = PROCESS_RESET;
          printf("����λ��λ\r\n");
          break;
          /* �����ļ�ֵ������ */
        default:                     
          break;
      }
      ucKeyCode = 0;
    }
    if(k3_short_flag == 1)
    {
      k3_short_cnt++;
      if(k3_short_cnt > 200)
      {
        k3_short_cnt = 0;
        k3_short_flag = 0;
        printf("cancel double click1.\r\n");
      }
    }
    vTaskDelay(10);
    IWDG_Feed();
  }
}

void vTaskTaskSend485(void *pvParameters)
{
  while(1)
  {
    bsp_KeyScan();
    vTaskDelay(10);
    IWDG_Feed();
  }
}

void vTaskRev485(void *pvParameters)
{
  BaseType_t xResult;
  u8 res;
  m_frame_typedef rxframe;
  static u8 err = 0;
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(300); /* �������ȴ�ʱ��Ϊ300ms */
  while(1)
  {
    xResult = xSemaphoreTake(xSemaphore_rs485, (TickType_t)xMaxBlockTime);
    if(xResult == pdTRUE)
    {
      if(m_ctrl_dev.frameok)
      {
        res = mb_unpack_frame(&rxframe);
        if(res == MR_OK)
        {
          modbus_action(&rxframe,load_value);
          link_err = 0;//�յ�����������ȷ�����Ӵ����־����
        }
        else
        {
          err++;
          if(err >= 20)
          {
//            __disable_fault_irq();
            NVIC_SystemReset();
          }
        }
      }
    }
    else
    {
      GPIO_ToggleBits(LED1_PORT,LED1_PIN);
    }
    IWDG_Feed();
  }
}

void vTaskSample(void *pvParameters)
{
  u32 diff;
  InitMass();
//  start_stop = 1;
  while(1)
  {
    get_mass();
    //PA0��ѹ=����ֵ/4096*3.3
    //�����������˵�ѹV=PA0��ѹ/�Ŵ���11
    //�������=��������=������ѹV/��������0.01
    value_current = (float)Average_filter() / 4096 * 3.3 / 11 / 0.1;
    printf("%.3fkg,%.3fA\r\n",(float)load_value / 1000,value_current);
    if(value_current >= SAMPLE_CURRENT_MAX)
    {//��������
      if(Device_Process == PROCESS_RUNNING)
      {//������ת���У�����
        if(motor_dir == MOTOR_REVERSE)
        {
          motor_dir = MOTOR_FORWARD;
        }
        else if(motor_dir == MOTOR_FORWARD)
        {
          motor_dir = MOTOR_REVERSE;
        }
        u16 speed;
        speed = 850;
        motor_speed(speed);
        motor_control(motor_dir);
        Device_Process = PROCESS_OVERCURRENTS;
        overCurrent_flag = 1;
        overCurrent_time = 0;
      }
//      Device_Process = PROCESS_STOP;
//      motor_dir = MOTOR_STOP;
//      motor_control(motor_dir);
    }
    switch(Device_Process)
    {
      case PROCESS_STOP://ֹͣ
        if((device_info.onoff == 1) && (start_stop == 1))
        {
          Device_Process = PROCESS_RUNNING;
        }
        break;
      case PROCESS_RUNNING://����
        if((device_info.onoff == 1) && (start_stop == 1))
        {
          diff = abs(load_value - device_info.weight_value);
          if(load_value < MAX_WEIGHT)
          {//С�������������
            if((diff <= device_info.precision) || (load_value <= ZERO))
            {//��ǰ�������趨ֵ���0.2kg����ǰ����С�����ֵʱ��ֹͣ����
              motor_dir = MOTOR_STOP;
              motor_control(motor_dir);  
            }
            else
            {//��ֵ����0.5kgʱ��������ת
              u16 speed;
              if(diff > 1000)
              {
                speed = 850;
              }
              else if((diff > 800) && (diff <= 1000))
              {
                speed = 800;
              }
              else if((diff > 600) && (diff <= 800))
              {
                speed = 750;
              }
              else if((diff > 60) && (diff <= 600))
              {
                speed = 500;
              }
              if(load_value > device_info.weight_value)
              {
                motor_speed(speed);
                motor_dir = MOTOR_REVERSE;
                motor_control(motor_dir);
              }
              else if(load_value < device_info.weight_value)
              {
                motor_speed(speed);
                motor_dir = MOTOR_FORWARD;
                motor_control(motor_dir);
              }
            }
          }
          else
          {//��������������ƺ�ֹͣ
            motor_dir = MOTOR_STOP;
            motor_control(motor_dir);
//            motor_dir = MOTOR_REVERSE;
//            u16 speed;
//            speed = 850;
//            motor_speed(speed);
//            motor_control(motor_dir);
//            Device_Process = PROCESS_OVERWEIGHT;
//            overWeight_flag = 1;
//            overWeight_time = 0;
          }
        }
        else 
        {
          if(start_stop == 0)
          {//����������ź�
            if(old_start_stop != start_stop)
            {
  //            old_start_stop = start_stop;
              motor_dir = MOTOR_FORWARD;
              motor_control(motor_dir); 
              cut_down_flag = 1;
              cut_down_time = 0;
            }
          }
          else if(device_info.onoff == 0)
          {
            Device_Process = PROCESS_STOP;
            motor_dir = MOTOR_STOP;
            motor_control(motor_dir);          
          }
        }
        old_start_stop = start_stop;
        break;
      case PROCESS_RESET://��λ
        motor_speed(850);
        motor_dir = MOTOR_REVERSE;
        motor_control(motor_dir);
        break;
      case PROCESS_RESET_1:
        motor_speed(850);
        motor_dir = MOTOR_REVERSE;
        motor_control(motor_dir);
        if((device_info.onoff == 1) && (start_stop == 1))
        {
          diff = abs(load_value - device_info.weight_value);
          if(load_value < MAX_WEIGHT)
          {
            if((diff <= 300) || (load_value <= ZERO))
            {//��ǰ�������趨ֵ���0.2kg����ǰ����С�����ֵʱ��ֹͣ����
              motor_dir = MOTOR_STOP;
              motor_control(motor_dir);  
              key2_reset = 0;
              key2_reset_time = 0;
              Device_Process = PROCESS_RUNNING;
              break;
            }
          }
        }
        if(key2_reset == 0)
        {
          key2_reset = 1;
          key2_reset_time = 0;
        }
        else
        {
          if(key2_reset_time >= 30)
          {
            motor_dir = MOTOR_STOP;
            motor_control(motor_dir);
            key2_reset = 0;
            Device_Process = PROCESS_STOP;
          }
        }
        break; 
      case PROCESS_RESET_2:
//        if((device_info.onoff == 1) && (start_stop == 1))
//        {
//          Device_Process = PROCESS_RUNNING;
//          key_reset = 0;
//          key_reset_time = 0;
//        }
//        else
        {
          u16 speed;
          speed = 850;
          motor_speed(speed);
          motor_dir = MOTOR_FORWARD;
          motor_control(motor_dir);
          if(key_reset == 0)
          {
            key_reset = 1;
            key_reset_time = 0;
          }
          else
          {
            if(key_reset_time >= 45)
            {
              Device_Process = PROCESS_STOP;
              motor_dir = MOTOR_STOP;
              motor_control(motor_dir);
            }
          }
        }
        break;    
      case PROCESS_OVERCURRENTS:
        if(overCurrent_time >= 10)
        {
          overCurrent_time = 0;
          overCurrent_flag = 0;
          motor_dir = MOTOR_STOP;
          motor_control(motor_dir);
          Device_Process = PROCESS_RUNNING;
        }
        break; 
      case PROCESS_OVERWEIGHT:
        if(overWeight_time >= 20)
        {
          overWeight_time = 0;
          overWeight_flag = 0;
          motor_dir = MOTOR_STOP;
          motor_control(motor_dir);
          Device_Process = PROCESS_RUNNING;
        }
        break; 
      case PROCESS_PAUSE:
        motor_dir = MOTOR_STOP;
        motor_control(motor_dir);
        break;
      default:
        break;
    }
//    printf("Device_Process is %d\r\n",Device_Process);
    vTaskDelay(100);
    IWDG_Feed();
  }
}

void AppTaskCreate (void)
{
  xTaskCreate( vTaskTaskKey,            /* ������  */
              "vTaskTaskKey",           /* ������    */
              128,                     /* stack��С����λword��Ҳ����4�ֽ� */
              NULL,                    /* �������  */
              1,                       /* �������ȼ�*/
              &xHandleTaskKey);  /* ������  */
  xTaskCreate( vTaskTaskSend485,   /* ������  */
              "vTaskTaskSend485",     /* ������    */
              128,               /* stack��С����λword��Ҳ����4�ֽ� */
              NULL,              /* �������  */
              2,                 /* �������ȼ�*/
              &xHandleTaskSend485 );  /* ������  */
  
  xTaskCreate( vTaskSample,            /* ������  */
              "vTaskSample",           /* ������    */
              256,                     /* stack��С����λword��Ҳ����4�ֽ� */
              NULL,                    /* �������  */
              3,                       /* �������ȼ�*/
              &xHandleTaskSample);  /* ������  */
  xTaskCreate( vTaskRev485,           /* ������  */
              "vTaskRev485",         /* ������    */
              512,                /* stack��С����λword��Ҳ����4�ֽ� */
              NULL,               /* �������  */
              4,                  /* �������ȼ�*/
              &xHandleTaskRev485 ); /* ������  */
}

void UserTimerCallback(TimerHandle_t xTimer)
{
  if(key_reset == 1)
  {
    if(Device_Process != PROCESS_PAUSE)
      key_reset_time++;
  }
  else
    key_reset_time = 0;
  
  if(key2_reset == 1)
  {
    if(Device_Process != PROCESS_PAUSE)
      key2_reset_time++;
  }
  else
    key2_reset_time = 0;
  
  link_err++;
  if(link_err >= 10)
  {//10s��δ�յ������������
    link_err = 0;
    NVIC_SystemReset();
  }
  if(overCurrent_flag == 1)
  {
    overCurrent_time++;
    if(overCurrent_time >= 100)
    {//�������
      overCurrent_time = 0;
      overCurrent_flag = 0;
      motor_dir = MOTOR_STOP;
      motor_control(motor_dir); 
      Device_Process = PROCESS_STOP;
    }
  }
  if(overWeight_flag == 1)
  {
    overWeight_time++;
    if(overWeight_time >= 100)
    {//�������
      overWeight_time = 0;
      overWeight_flag = 0;
      motor_dir = MOTOR_STOP;
      motor_control(motor_dir); 
      Device_Process = PROCESS_STOP;
    }
  }
  if(start_time_flag == 1)
  {
    if(start_time < 5)
      start_time++;
    else if(start_time >= 5)
      start_stop = 1;
  }
  if(cut_down_flag == 1)
  {
    cut_down_time++;
    if(cut_down_time >= 3)
    {
      cut_down_flag = 0;
      cut_down_time = 0;
      motor_dir = MOTOR_STOP;
      motor_control(motor_dir); 
    }
  }
}

/*
*********************************************************************************************************
*	�� �� ��: AppObjCreate
*	����˵��: ��������ͨ�Ż���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void AppObjCreate(void)
{
  /* ������ֵ�ź������״δ����ź�������ֵ��0 */
  xSemaphore_rs485 = xSemaphoreCreateBinary();
  
  if(xSemaphore_rs485 == NULL)
  {
    printf("xSemaphore_rs485 failure\r\n");
    /* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
  }
  
  xTimerUser = xTimerCreate("Timer",          /* ��ʱ������ */
                             1000,    /* ��ʱ������,��λʱ�ӽ��� */
                             pdTRUE,          /* ������ */
                             (void *)0,      /* ��ʱ��ID */
                             UserTimerCallback); /* ��ʱ���ص����� */
    
  if(xTimerUser != NULL)
  {
    
    if(xTimerStart(xTimerUser, 1000) != pdPASS)
    {
      /* ��ʱ����û�н��뼤��״̬ */
    }
  }
}