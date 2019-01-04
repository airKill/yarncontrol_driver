#include "includes.h"

TaskHandle_t xHandleTaskLCD = NULL;
TaskHandle_t xHandleTaskLED = NULL;
TaskHandle_t xHandleTaskRFID = NULL;
TaskHandle_t xHandleTaskMassStorage = NULL;
TaskHandle_t xHandleTaskReadDisk = NULL;
TaskHandle_t xHandleTaskMsgPro = NULL;
TaskHandle_t xHandleTaskRev485 = NULL;
TaskHandle_t xHandleTaskManageCapacity = NULL;

SemaphoreHandle_t  xSemaphore_lcd = NULL;
SemaphoreHandle_t  xSemaphore_rs485 = NULL;
SemaphoreHandle_t  xSemaphore_pluse = NULL;
SemaphoreHandle_t  xSemaphore_download = NULL;
SemaphoreHandle_t  xSemaphore_readDisk = NULL;
SemaphoreHandle_t  xSemaphore_debug = NULL;
EventGroupHandle_t idwgEventGroup = NULL;
TimerHandle_t xTimerUser = NULL;

SLAVE slave_info;
SLAVE readSlave;
QueueHandle_t xQueue_message = NULL;
u8 download_num = 0;
u8 working_stage = WORK_STAGE_IDLE;
u8 isWork = 0,old_isWork = 0;
u8 usb_disk_flag = 0;

u32 pluse_count = 0;
u8 grade = 0;

extern void DemoFatFS(void);
extern const char * FR_Table[];
extern u8 modbus_send_frame(m_frame_typedef * fx,SLAVE info);
/*
*********************************************************************************************************
*	�� �� ��: vTaskTaskLCD
*	����˵��: �ӿ���Ϣ����
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 1  (��ֵԽС���ȼ�Խ�ͣ������uCOS�෴)
*********************************************************************************************************
*/
void vTaskTaskLCD(void *pvParameters)
{
  u16 var_addr,value;
  SLAVE *ptMsg;
  /* ��ʼ���ṹ��ָ�� */
  ptMsg = &slave_info;
  /* ��ʼ������ */
  ptMsg->addr = 0;
  ptMsg->reg = 0;
  ptMsg->value = 0;
  
  BaseType_t xResult;
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(300); /* �������ȴ�ʱ��Ϊ300ms */
//  LCD_POWER_ON();
  vTaskDelay(500);
  
  while(1)
  {
    xResult = xSemaphoreTake(xSemaphore_lcd, (TickType_t)xMaxBlockTime);
    if(xResult == pdTRUE)
    {
      /* ���յ�ͬ���ź� */
      if((lcd_rev_buf[0] == 0xA5) && (lcd_rev_buf[1] == 0x5A) && (lcd_rev_buf[2] == (lcd_rev_len - 3)))
      {//ָ���ʽ��ȷ
        if(lcd_rev_buf[3] == 0x81)
        {//���Ĵ�������
          var_addr = lcd_rev_buf[4];
          if(var_addr == 0x20)
          {//����ʱ����Ϣ
            rtc_time.year = hex_to_decimal(lcd_rev_buf[6]);
            rtc_time.month = hex_to_decimal(lcd_rev_buf[7]);
            rtc_time.day = hex_to_decimal(lcd_rev_buf[8]);
            rtc_time.week = hex_to_decimal(lcd_rev_buf[9]);
            rtc_time.hour = hex_to_decimal(lcd_rev_buf[10]);
            rtc_time.minute = hex_to_decimal(lcd_rev_buf[11]);
            rtc_time.second = hex_to_decimal(lcd_rev_buf[12]);
          }
        }
        else if(lcd_rev_buf[3] == 0x83)
        {//�������洢������
          var_addr = (lcd_rev_buf[4] << 8) + lcd_rev_buf[5];
          if(var_addr == MAIN_PAGE_KEY_JINGSHA)
          {//��ɴ����
            vTaskDelay(100);
            Init_JINGSHA_GUI();
          }
          else if(var_addr == MAIN_PAGE_KEY_WEIMI)
          {//γ��
            
          }
          else if(var_addr == MAIN_PAGE_KEY_CHANNENG)
          {//����
            
          }
          else if(var_addr == MAIN_PAGE_KEY_SYS_CONFIG)
          {//ϵͳ����
            
          }
          else if(var_addr == PAGE_FILE_KEY)
          {//��ʷ����ҳ��
            u8 i;
            value = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
            Sdwe_refresh_allname(device_info.page_count_all);
            for(i=0;i<10;i++)
            {
              Sdwe_writeIcon(i + PAGE_HISTORY_ICON_FILE1,file_select[i]);
            }
          }
//          else if(var_addr == PAGE_START_STOP)
//          {
//            value = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
//            isWork = value;
//          }
          else if(var_addr == PAGE1_KEY_SET_WEIGHT)
          {//��һҳ�޸��趨
            memset(input_password_buf,0,10);
            Sdwe_clearString(PAGE1_SECRET_TEXT_DIS);
            Sdwe_clearString(PAGE1_SECRET_TEXT_WARN);
          }
          else if(var_addr == PAGE2_KEY_SET_WEIGHT)
          {//�ڶ�ҳ�޸��趨
            memset(input_password_buf,0,10);
            Sdwe_clearString(PAGE2_SECRET_TEXT_DIS);
            Sdwe_clearString(PAGE2_SECRET_TEXT_WARN);
          }
          else if(var_addr == PAGE3_KEY_SET_WEIGHT)
          {//����ҳ�޸��趨
            memset(input_password_buf,0,10);
            Sdwe_clearString(PAGE3_SECRET_TEXT_DIS);
            Sdwe_clearString(PAGE3_SECRET_TEXT_WARN);
          }
          else if(var_addr == PAGE1_KEY_LEFT)
          {//��һҳ����
          
          }
          else if(var_addr == PAGE2_KEY_LEFT)
          {//�ڶ�ҳ����
            vTaskDelay(100);
            Init_JINGSHA_GUI();
          }
          else if(var_addr == PAGE3_KEY_LEFT)
          {//����ҳ����
            vTaskDelay(100);
            Init_JINGSHA_GUI();
          }
          else if(var_addr == PAGE1_KEY_SAVE)
          {//��һҳ����
            memset(input_password_buf,0,10);
            Sdwe_clearString(PAGE1_FILE_TEXT_DIS);
            Sdwe_clearString(PAGE1_FILE_TEXT_WARN);
            Sdwe_readRTC();
          }
          else if(var_addr == PAGE2_KEY_SAVE)
          {//�ڶ�ҳ����
            memset(input_password_buf,0,10);
            Sdwe_clearString(PAGE2_FILE_TEXT_DIS);
            Sdwe_clearString(PAGE2_FILE_TEXT_WARN);
            Sdwe_readRTC();
          }
          else if(var_addr == PAGE3_KEY_SAVE)
          {//����ҳ����
            memset(input_password_buf,0,10);
            Sdwe_clearString(PAGE3_FILE_TEXT_DIS);
            Sdwe_clearString(PAGE3_FILE_TEXT_WARN);
            Sdwe_readRTC();
          }
          else if(var_addr == PAGE1_KEY_RIGHT)
          {//��һҳ����
            vTaskDelay(100);
            Init_JINGSHA_GUI();
          }
          else if(var_addr == PAGE2_KEY_RIGHT)
          {//�ڶ�ҳ����
            vTaskDelay(100);
            Init_JINGSHA_GUI();
          }
          else if(var_addr == PAGE3_KEY_RIGHT)
          {//����ҳ����
            vTaskDelay(100);
            Init_JINGSHA_GUI();
          }
          else if(var_addr == PAGE1_SECRET_KEY_CANCEL)
          {//��һҳȡ����������
          
          }
          else if(var_addr == PAGE2_SECRET_KEY_CANCEL)
          {//�ڶ�ҳȡ����������
          
          }
          else if(var_addr == PAGE3_SECRET_KEY_CANCEL)
          {//����ҳȡ����������
          
          }
          else if(var_addr == PAGE1_SECRET_KEY_ENTER)
          {//��һҳȷ����������
          
          }
          else if(var_addr == PAGE2_SECRET_KEY_ENTER)
          {//�ڶ�ҳȷ����������
            
          }
          else if(var_addr == PAGE3_SECRET_KEY_ENTER)
          {//����ҳȷ����������

          }
          else if((var_addr == PAGE1_FILE_KEY_ENTER) || (var_addr == PAGE2_FILE_KEY_ENTER) 
                  || (var_addr == PAGE3_FILE_KEY_ENTER))
          {
            u8 i;
            FILE_INFO file_write;
            if(input_password_len > 0)
            {
              u8 repeat = 0,j;
              for(i=0;i<device_info.page_count_all;i++)
              {
                u8 name_buf[11];
                __set_PRIMASK(1);
                W25QXX_Read(name_buf,(u32)W25QXX_ADDR_FILE + FILE_SIZE * i,11);
                __set_PRIMASK(0);
                if(input_password_len == name_buf[0])
                {//�ļ���������ͬ
                  u8 tt = 0;
                  for(j=0;j<input_password_len;j++)
                  {
                    if(name_buf[j + 1] != input_password_buf[j])
                    {
                      tt = 1;
                      break;
                    }
                  }
                  if(tt == 0)
                  {//ÿ���ַ���ͬ
                    repeat = 1;
                    break;
                  } 
                }
              }
              if(repeat == 0)
              {//���ظ��ļ�
                if(device_info.page_count_all >= 10)
                {//ȥ����һ���ļ��������ļ���������
                  u8 file_read[1024];
                  __set_PRIMASK(1);
                  W25QXX_Read(file_read,(u32)W25QXX_ADDR_FILE + FILE_SIZE,FILE_SIZE * 9);//����2-10�ļ�������
                  W25QXX_Write(file_read,(u32)W25QXX_ADDR_FILE,FILE_SIZE * 9);//��д��1-9�ļ���ַ
                  __set_PRIMASK(0);
                  file_write.filename_len = input_password_len;                //�ļ�������               
                  memcpy(file_write.filename,input_password_buf,input_password_len);//�ļ���
                  file_write.year = rtc_time.year;          //����ʱ��      
                  file_write.month = rtc_time.month;
                  file_write.day = rtc_time.day;
                  file_write.hour = rtc_time.hour;
                  file_write.minute = rtc_time.minute;
                  file_write.second = rtc_time.second;
                  for(i=0;i<30;i++)
                  {
                    file_write.weight_value[i] = SlavePara.value_set[i];
                  }
                  __set_PRIMASK(1);
                  W25QXX_Write((u8 *)&file_write,(u32)(W25QXX_ADDR_FILE + FILE_SIZE * 9),sizeof(file_write));//д��ǰҳ�����ݵ��ļ�10��ַ
                  __set_PRIMASK(0);
                  if(var_addr == PAGE1_FILE_KEY_ENTER)//���µ�һҳ���水ť
                    Sdwe_disPicture(PAGE_1);//��ת��ҳ��1
                  else if(var_addr == PAGE2_FILE_KEY_ENTER)//���µڶ�ҳ���水ť
                    Sdwe_disPicture(PAGE_2);//��ת��ҳ��2
                  else if(var_addr == PAGE3_FILE_KEY_ENTER)//���µ���ҳ���水ť
                    Sdwe_disPicture(PAGE_3);//��ת��ҳ��3
                }
                else
                {
                  /******************�ļ������ʽ********************
                  �ļ�������                1byte   
                  �ļ��������10Byte��      10byte
                  ����ʱ�䣨YY MM DD HH MM SS�� 6byte
                  �������������趨ֵ        30byte
                  **************************************************/
                  file_write.filename_len = input_password_len;                //�ļ�������               
                  memcpy(file_write.filename,input_password_buf,input_password_len);//�ļ���
                  file_write.year = rtc_time.year;          //����ʱ��      
                  file_write.month = rtc_time.month;
                  file_write.day = rtc_time.day;
                  file_write.hour = rtc_time.hour;
                  file_write.minute = rtc_time.minute;
                  file_write.second = rtc_time.second;
                  for(i=0;i<30;i++)
                  {
                    file_write.weight_value[i] = SlavePara.value_set[i];
                  }
                  __set_PRIMASK(1);
                  W25QXX_Write((u8 *)&file_write,(u32)(W25QXX_ADDR_FILE + FILE_SIZE * device_info.page_count_all),sizeof(file_write));
                  device_info.page_count_all++;
                  W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
                  __set_PRIMASK(0);
                  if(var_addr == PAGE1_FILE_KEY_ENTER)
                    Sdwe_disPicture(PAGE_1);
                  else if(var_addr == PAGE2_FILE_KEY_ENTER)
                    Sdwe_disPicture(PAGE_2);
                  else if(var_addr == PAGE3_FILE_KEY_ENTER)
                    Sdwe_disPicture(PAGE_3);
                }
              }
              else
              {//���ظ��ļ���
                if(var_addr == PAGE1_FILE_KEY_ENTER)
                  Sdwe_disString(PAGE1_FILE_TEXT_WARN,"�ļ����ظ�",strlen("�ļ����ظ�"));
                else if(var_addr == PAGE2_FILE_KEY_ENTER)
                  Sdwe_disString(PAGE2_FILE_TEXT_WARN,"�ļ����ظ�",strlen("�ļ����ظ�"));
                else if(var_addr == PAGE3_FILE_KEY_ENTER)
                  Sdwe_disString(PAGE3_FILE_TEXT_WARN,"�ļ����ظ�",strlen("�ļ����ظ�"));
              }
            }
            else
            {
              if(var_addr == PAGE1_FILE_KEY_ENTER)
                Sdwe_disString(PAGE1_FILE_TEXT_WARN,"�ļ�����",strlen("�ļ�����"));
              else if(var_addr == PAGE2_FILE_KEY_ENTER)
                Sdwe_disString(PAGE2_FILE_TEXT_WARN,"�ļ�����",strlen("�ļ�����"));
              else if(var_addr == PAGE3_FILE_KEY_ENTER)
                Sdwe_disString(PAGE3_FILE_TEXT_WARN,"�ļ�����",strlen("�ļ�����"));
            }
          }
          else if(var_addr == PAGE1_FILE_KEY_CANCEL)
          {
            
          }
          else if(var_addr == PAGE2_FILE_KEY_CANCEL)
          {
            
          }
          else if(var_addr == PAGE3_FILE_KEY_CANCEL)
          {
            
          }
          else if(var_addr == PAGE1_ICON1_10_ONOFF)
          {
            u8 i;
            value = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
            for(i=0;i<10;i++)
            {
              if(value == 0)
              {
                Sdwe_writeIcon(PAGE1_SLAVE_ONOFF1 + i,VGUS_ON);
                SlavePara.onoff[i] = 1;
                device_info.onoff[i] = 1;
                ptMsg->value = 1;
              }
              else
              {
                Sdwe_writeIcon(PAGE1_SLAVE_ONOFF1 + i,VGUS_OFF);
                SlavePara.onoff[i] = 0;
                device_info.onoff[i] = 0;
                ptMsg->value = 0;
              }
            }
            __set_PRIMASK(1);
            W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
            __set_PRIMASK(0);
            ptMsg->addr = 0xAA;
            ptMsg->func = FUNC_WRITE;
            ptMsg->reg = REG_ONOFF;
//            ptMsg->value = value;
            if(xQueueSend(xQueue_message,                  /* ��Ϣ���о�� */
                          (void *) &ptMsg,           /* ���ͽṹ��ָ�����ptMsg�ĵ�ַ */
                          (TickType_t)10) == pdPASS )
            {
              /* ����ʧ�ܣ���ʹ�ȴ���10��ʱ�ӽ��� */
              
            }
          }
          else if(var_addr == PAGE2_ICON11_20_ONOFF)
          {
            u8 i;
            value = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
            for(i=0;i<10;i++)
            {
              if(value == 0)
              {
                Sdwe_writeIcon(PAGE1_SLAVE_ONOFF1 + 10 + i,VGUS_ON);
                SlavePara.onoff[i + 10] = 1;
                device_info.onoff[i + 10] = 1;
                ptMsg->value = 1;
              }
              else
              {
                Sdwe_writeIcon(PAGE1_SLAVE_ONOFF1 + 10 + i,VGUS_OFF);
                SlavePara.onoff[i + 10] = 0;
                device_info.onoff[i + 10] = 0;
                ptMsg->value = 0;
              }
            }
            __set_PRIMASK(1);
            W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
            __set_PRIMASK(0);
            ptMsg->addr = 0xBB;
            ptMsg->func = FUNC_WRITE;
            ptMsg->reg = REG_ONOFF;
//            ptMsg->value = value;
            if(xQueueSend(xQueue_message,                  /* ��Ϣ���о�� */
                          (void *) &ptMsg,           /* ���ͽṹ��ָ�����ptMsg�ĵ�ַ */
                          (TickType_t)10) == pdPASS )
            {
              /* ����ʧ�ܣ���ʹ�ȴ���10��ʱ�ӽ��� */
              
            }
          }
          else if(var_addr == PAGE3_ICON21_30_ONOFF)
          {
            u8 i;
            value = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
            for(i=0;i<10;i++)
            {
              if(value == 0)
              {
                Sdwe_writeIcon(PAGE1_SLAVE_ONOFF1 + 20 + i,VGUS_ON);
                SlavePara.onoff[i + 20] = 1;
                device_info.onoff[i + 20] = 1;
                ptMsg->value = 1;
              }
              else
              {
                Sdwe_writeIcon(PAGE1_SLAVE_ONOFF1 + 20 + i,VGUS_OFF);
                SlavePara.onoff[i + 20] = 0;
                device_info.onoff[i + 20] = 0;
                ptMsg->value = 0;
              }
            }
            __set_PRIMASK(1);
            W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
            __set_PRIMASK(0);
            ptMsg->addr = 0xCC;
            ptMsg->func = FUNC_WRITE;
            ptMsg->reg = REG_ONOFF;
//            ptMsg->value = value;
            if(xQueueSend(xQueue_message,                  /* ��Ϣ���о�� */
                          (void *) &ptMsg,           /* ���ͽṹ��ָ�����ptMsg�ĵ�ַ */
                          (TickType_t)10) == pdPASS )
            {
              /* ����ʧ�ܣ���ʹ�ȴ���10��ʱ�ӽ��� */
              
            }
          }
          else if((var_addr >= PAGE1_SET_VALUE1) && (var_addr <= (PAGE1_SET_VALUE1 + 30)))
          {//�趨ֵ�޸�
            value = ((lcd_rev_buf[7] << 8) + lcd_rev_buf[8]) * 1000 / 100;//��������Ϊ��λ��С������λkg��ת��Ϊg
            SlavePara.value_set[var_addr - 0x0320] = value;
            File_info.weight_value[var_addr - 0x0320] = value;
            __set_PRIMASK(1);
            W25QXX_Write((u8 *)&File_info,(u32)(W25QXX_ADDR_FILE + FILE_SIZE * device_info.page_count_select),sizeof(File_info));
            __set_PRIMASK(0);
            ptMsg->addr = var_addr - PAGE1_SET_VALUE1;
            ptMsg->func = FUNC_WRITE;
            ptMsg->reg = REG_SET_WEIGHT;
            ptMsg->value = value;
            if(xQueueSend(xQueue_message,                  /* ��Ϣ���о�� */
                          (void *) &ptMsg,           /* ���ͽṹ��ָ�����ptMsg�ĵ�ַ */
                          (TickType_t)10) == pdPASS )
            {
              /* ����ʧ�ܣ���ʹ�ȴ���10��ʱ�ӽ��� */
              printf("%d# value %.2f ���з��ͳɹ�\r\n",var_addr - 0x0320 + 1,(float)value / 1000);
            }
          }
          else if(var_addr == PAGE1_SECRET_TEXT_IMPORT)
          {//�����¼����
            u8 llen;
            u8 buf[20],i;
            llen = lcd_rev_buf[6] * 2;//���ڷ���Ϊ�ֳ�
            memset(input_password_buf,0,10);
            memcpy(input_password_buf,lcd_rev_buf + 7,llen);
            input_password_len = get_valid_length(input_password_buf,llen);
            input_password_buf[input_password_len] = '\0';
            for(i=0;i<input_password_len;i++)
              buf[i] = '*';
            Sdwe_disString(PAGE1_SECRET_TEXT_DIS,buf,input_password_len);//������ʾΪ*
            
            if(strcmp(input_password_buf,"111111") == 0)
            {//������ȷ
              Sdwe_disPicture(PAGE_1_SET_VALUE);
              vTaskDelay(10);
              Init_JINGSHA_GUI();
            }
            else
            {//�������
              Sdwe_disString(PAGE1_SECRET_TEXT_WARN,"�������",strlen("�������"));
            }
          }
          else if(var_addr == PAGE2_SECRET_TEXT_IMPORT)
          {//�����¼����
            u8 llen;
            u8 buf[20],i;
            llen = lcd_rev_buf[6] * 2;//���ڷ���Ϊ�ֳ�
            memset(input_password_buf,0,10);
            memcpy(input_password_buf,lcd_rev_buf + 7,llen);
            input_password_len = get_valid_length(input_password_buf,llen);
            input_password_buf[input_password_len] = '\0';
            for(i=0;i<input_password_len;i++)
              buf[i] = '*';
            Sdwe_disString(PAGE2_SECRET_TEXT_DIS,buf,input_password_len);//������ʾΪ*
            
            if(strcmp(input_password_buf,"111111") == 0)
            {//������ȷ
              Sdwe_disPicture(PAGE_2_SET_VALUE);
              vTaskDelay(10);
              Init_JINGSHA_GUI();
            }
            else
            {//�������
              Sdwe_disString(PAGE2_SECRET_TEXT_WARN,"�������",strlen("�������"));
            }
          }
          else if(var_addr == PAGE3_SECRET_TEXT_IMPORT)
          {//�����¼����
            u8 llen;
            u8 buf[20],i;
            llen = lcd_rev_buf[6] * 2;//���ڷ���Ϊ�ֳ�
            memset(input_password_buf,0,10);
            memcpy(input_password_buf,lcd_rev_buf + 7,llen);
            input_password_len = get_valid_length(input_password_buf,llen);
            input_password_buf[input_password_len] = '\0';
            for(i=0;i<input_password_len;i++)
              buf[i] = '*';
            Sdwe_disString(PAGE3_SECRET_TEXT_DIS,buf,input_password_len);//������ʾΪ*
            
            if(strcmp(input_password_buf,"111111") == 0)
            {//������ȷ
              Sdwe_disPicture(PAGE_3_SET_VALUE);
              vTaskDelay(10);
              Init_JINGSHA_GUI();
            }
            else
            {//�������
              Sdwe_disString(PAGE3_SECRET_TEXT_WARN,"�������",strlen("�������"));
            }
          }
          else if((var_addr == PAGE1_FILE_TEXT_IMPORT) || (var_addr == PAGE2_FILE_TEXT_IMPORT) || (var_addr == PAGE3_FILE_TEXT_IMPORT))
          {//�ļ���¼�룬ȥ��ȷ�ϰ�ť����OK��Ĭ��ȷ��
            u8 llen;
            llen = lcd_rev_buf[6] * 2;
            memcpy(input_password_buf,lcd_rev_buf + 7,llen);
            input_password_len = get_valid_length(input_password_buf,llen);
            input_password_buf[input_password_len] = '\0';
            
            u8 i;
            FILE_INFO file_write;
            memset(file_write.filename,0,10);
            if(input_password_len > 0)
            {
              u8 repeat = 0;
              for(i=0;i<device_info.page_count_all;i++)
              {
                u8 name_buf[10];
                __set_PRIMASK(1);
                W25QXX_Read(name_buf,(u32)W25QXX_ADDR_FILE + FILE_SIZE * i + 1,10);
                __set_PRIMASK(0);
                if(strcmp(input_password_buf,name_buf) == 0)
                {
                  repeat = 1;
                  break;
                }
              }
              if(repeat == 0)
              {//���ظ��ļ�
                if(device_info.page_count_all >= 10)
                {//ȥ����һ���ļ��������ļ���������
                  u8 file_read[1024];
                  __set_PRIMASK(1);
                  W25QXX_Read(file_read,(u32)W25QXX_ADDR_FILE + FILE_SIZE,FILE_SIZE * 9);//����2-10�ļ�������
                  W25QXX_Write(file_read,(u32)W25QXX_ADDR_FILE,FILE_SIZE * 9);//��д��1-9�ļ���ַ
                  __set_PRIMASK(0);
                  file_write.filename_len = input_password_len;                //�ļ�������               
                  memcpy(file_write.filename,input_password_buf,input_password_len);//�ļ���
                  file_write.year = rtc_time.year;          //����ʱ��      
                  file_write.month = rtc_time.month;
                  file_write.day = rtc_time.day;
                  file_write.hour = rtc_time.hour;
                  file_write.minute = rtc_time.minute;
                  file_write.second = rtc_time.second;
                  for(i=0;i<30;i++)
                  {
                    file_write.weight_value[i] = SlavePara.value_set[i];
                  }
                  __set_PRIMASK(1);
                  W25QXX_Write((u8 *)&file_write,(u32)(W25QXX_ADDR_FILE + FILE_SIZE * 9),sizeof(file_write));//д��ǰҳ�����ݵ��ļ�10��ַ
                  __set_PRIMASK(0);
                  if(var_addr == PAGE1_FILE_TEXT_IMPORT)//���µ�һҳ���水ť
                    Sdwe_disPicture(PAGE_1);//��ת��ҳ��1
                  else if(var_addr == PAGE2_FILE_TEXT_IMPORT)//���µڶ�ҳ���水ť
                    Sdwe_disPicture(PAGE_2);//��ת��ҳ��2
                  else if(var_addr == PAGE3_FILE_TEXT_IMPORT)//���µ���ҳ���水ť
                    Sdwe_disPicture(PAGE_3);//��ת��ҳ��3
                }
                else
                {
                  /******************�ļ������ʽ********************
                  �ļ�������                1byte   
                  �ļ��������10Byte��      10byte
                  ����ʱ�䣨YY MM DD HH MM SS�� 6byte
                  �������������趨ֵ        30byte
                  **************************************************/
                  file_write.filename_len = input_password_len;                //�ļ�������               
                  memcpy(file_write.filename,input_password_buf,input_password_len);//�ļ���
                  file_write.year = rtc_time.year;          //����ʱ��      
                  file_write.month = rtc_time.month;
                  file_write.day = rtc_time.day;
                  file_write.hour = rtc_time.hour;
                  file_write.minute = rtc_time.minute;
                  file_write.second = rtc_time.second;
                  for(i=0;i<30;i++)
                  {
                    file_write.weight_value[i] = SlavePara.value_set[i];
                  }
                  __set_PRIMASK(1);
                  W25QXX_Write((u8 *)&file_write,(u32)(W25QXX_ADDR_FILE + FILE_SIZE * device_info.page_count_all),sizeof(file_write));
                  device_info.page_count_all++;
                  W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
                  __set_PRIMASK(0);
                  if(var_addr == PAGE1_FILE_TEXT_IMPORT)
                    Sdwe_disPicture(PAGE_1);
                  else if(var_addr == PAGE2_FILE_TEXT_IMPORT)
                    Sdwe_disPicture(PAGE_2);
                  else if(var_addr == PAGE3_FILE_TEXT_IMPORT)
                    Sdwe_disPicture(PAGE_3);
                }
              }
              else
              {//���ظ��ļ���
                if(var_addr == PAGE1_FILE_TEXT_IMPORT)
                  Sdwe_disString(PAGE1_FILE_TEXT_WARN,"�ļ����ظ�",strlen("�ļ����ظ�"));
                else if(var_addr == PAGE2_FILE_TEXT_IMPORT)
                  Sdwe_disString(PAGE2_FILE_TEXT_WARN,"�ļ����ظ�",strlen("�ļ����ظ�"));
                else if(var_addr == PAGE3_FILE_TEXT_IMPORT)
                  Sdwe_disString(PAGE3_FILE_TEXT_WARN,"�ļ����ظ�",strlen("�ļ����ظ�"));
              }
            }
            else
            {
              if(var_addr == PAGE1_FILE_TEXT_IMPORT)
                Sdwe_disString(PAGE1_FILE_TEXT_WARN,"�ļ�����",strlen("�ļ�����"));
              else if(var_addr == PAGE2_FILE_TEXT_IMPORT)
                Sdwe_disString(PAGE2_FILE_TEXT_WARN,"�ļ�����",strlen("�ļ�����"));
              else if(var_addr == PAGE3_FILE_TEXT_IMPORT)
                Sdwe_disString(PAGE3_FILE_TEXT_WARN,"�ļ�����",strlen("�ļ�����"));
            }
          }
          else if((var_addr == PAGE_HISTORY_KEY_SELECT) || (var_addr == PAGE_HISTORY_KEY_DOWNLOAD))
          {//����/����
            u8 i;
            u16 ison = 0,num = 0,cnt = 0;//�ļ�ѡ���־��ĳ���ļ�ѡ�񣬶�Ӧ��λ��1
            for(i=0;i<10;i++)
            {
              if(file_select[i] == 1)
              {
                ison = ison | (1<<i);
                cnt++;
                num = i;
              }
            }
            if(ison == 0)
            {//δѡ���ļ�
              Sdwe_disString(PAGE_HISTORY_TEXT_FILE_WARN,"δѡ���ļ�",strlen("δѡ���ļ�"));
              bsp_StartHardTimer(1 ,500000, (void *)TIM_CallBack1);
            }
            else
            {
              if(device_info.page_count_all == 0)
              {//û�б�����ļ�
                Sdwe_disString(PAGE_HISTORY_TEXT_FILE_WARN,"���ļ�",strlen("���ļ�"));
                bsp_StartHardTimer(1 ,500000, (void *)TIM_CallBack1);
              }
              else
              {
                if(cnt > 1)
                {//ѡ���ļ�����1��
                  Sdwe_disString(PAGE_HISTORY_TEXT_FILE_WARN,"ֻ��ѡһ��",strlen("ֻ��ѡһ��"));
                  bsp_StartHardTimer(1 ,500000, (void *)TIM_CallBack1);
                }
                else
                {
                  if(var_addr == PAGE_HISTORY_KEY_SELECT)
                  {//�����ļ�
                    FILE_INFO file_read;
                    u8 i;
                    __set_PRIMASK(1);
                    W25QXX_Read((u8 *)&file_read,(u32)(W25QXX_ADDR_FILE + FILE_SIZE * num),sizeof(file_read));//����num������
                    __set_PRIMASK(0);
                    device_info.page_count_select = num;
                    SlavePara.filename_len = file_read.filename_len;
                    for(i=0;i<file_read.filename_len;i++)
                    {
                      SlavePara.filename[i] = file_read.filename[i];
                    }
                    for(i=0;i<30;i++)
                    {
                      SlavePara.value_set[i] = file_read.weight_value[i];
                    }
                    __set_PRIMASK(1);
                    W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
                    __set_PRIMASK(0);
                    Init_JINGSHA_GUI();
                    Sdwe_disString(PAGE_HISTORY_TEXT_FILE_WARN,"���óɹ�",strlen("���óɹ�"));
                    bsp_StartHardTimer(1 ,500000, (void *)TIM_CallBack1);
                    ptMsg->addr = 0xff;
                    ptMsg->func = FUNC_WRITE;
                    ptMsg->reg = REG_SET_WEIGHT;
                    ptMsg->value = 0;
                    if(xQueueSend(xQueue_message,                  /* ��Ϣ���о�� */
                                  (void *) &ptMsg,           /* ���ͽṹ��ָ�����ptMsg�ĵ�ַ */
                                  (TickType_t)10) == pdPASS )
                    {
                      /* ����ʧ�ܣ���ʹ�ȴ���10��ʱ�ӽ��� */
                      printf("%d# value %.2f ���з��ͳɹ�\r\n",var_addr - 0x0320 + 1,(float)value / 1000);
                    }
                  }
                  else
                  {
                    download_num = num;
                    xSemaphoreGive(xSemaphore_download);
                  }
                }
              }
            }
          }
          else if((var_addr >= PAGE1_SLAVE_ONOFF1) && (var_addr < (PAGE1_SLAVE_ONOFF1 + 30)))
          {//�ӻ���Ч����
            value = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
            if(value == 0)
            {
              Sdwe_writeIcon(PAGE1_SLAVE_STATE1 + var_addr - PAGE1_SLAVE_ONOFF1,VGUS_OFF);
            }
            SlavePara.onoff[var_addr - PAGE1_SLAVE_ONOFF1] = value;
            device_info.onoff[var_addr - PAGE1_SLAVE_ONOFF1] = value;
            __set_PRIMASK(1);
            W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
            __set_PRIMASK(0);
            /* ����Ϣ���з����ݣ������Ϣ�������ˣ��ȴ�10��ʱ�ӽ��� */
            ptMsg->addr = var_addr - PAGE1_SLAVE_ONOFF1;
            ptMsg->func = FUNC_WRITE;
            ptMsg->reg = REG_ONOFF;
            ptMsg->value = value;
            if(xQueueSend(xQueue_message,                  /* ��Ϣ���о�� */
                          (void *) &ptMsg,           /* ���ͽṹ��ָ�����ptMsg�ĵ�ַ */
                          (TickType_t)10) == pdPASS )
            {
              /* ����ʧ�ܣ���ʹ�ȴ���10��ʱ�ӽ��� */
              printf("%d# sw %d ���з��ͳɹ�\r\n",var_addr - PAGE1_SLAVE_ONOFF1 + 1,value);
            }
          }
          else if((var_addr >= PAGE_HISTORY_ICON_FILE1) && (var_addr <= (PAGE_HISTORY_ICON_FILE1 + 9)))
          {//�ļ�ѡ��
            value = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
            file_select[var_addr - PAGE_HISTORY_ICON_FILE1] = value;
            device_info.file_select[var_addr - PAGE_HISTORY_ICON_FILE1] = file_select[var_addr - PAGE_HISTORY_ICON_FILE1];
            __set_PRIMASK(1);
            W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
            __set_PRIMASK(0);
            if(value == 0)
            {
              printf("file %d off\r\n",var_addr - PAGE_HISTORY_ICON_FILE1 + 1);
            }
            else
            {
              printf("file %d on\r\n",var_addr - PAGE_HISTORY_ICON_FILE1 + 1);
            }
          }
          else if(var_addr == PAGE_HISTORY_KEY_READ)
          {
            xSemaphoreGive(xSemaphore_readDisk);
          }
          else if((var_addr >= PAGE_U_ICON_SELECT1) && (var_addr <= PAGE_U_ICON_SELECT1 + 9))
          {//ѡ��U���ļ�
            value = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
            Disk_File.fileselect[var_addr - PAGE_U_ICON_SELECT1] = value;
            printf("Disk file %d,%d\r\n",var_addr - PAGE_HISTORY_ICON_FILE1 + 1,value);
          }
          else if(var_addr == PAGE_U_KEY_READ)
          {//��ȡU���ļ�
            u8 i;
            u16 num = 0,cnt = 0;//�ļ�ѡ���־��ĳ���ļ�ѡ�񣬶�Ӧ��λ��1
            for(i=0;i<10;i++)
            {
              if(Disk_File.fileselect[i] == 1)
              {
                cnt++;
                num = i;
              }
            }
            if(cnt == 0)
            {//δѡ���ļ�
              Sdwe_disString(PAGE_U_TEXT_READ_STATE,"δѡ���ļ�",strlen("δѡ���ļ�"));
              bsp_StartHardTimer(1 ,500000, (void *)TIM_CallBack1);
            }
            else
            {
              if(Disk_File.filenum == 0)
              {//û�б�����ļ�
                Sdwe_disString(PAGE_U_TEXT_READ_STATE,"���ļ�",strlen("���ļ�"));
                bsp_StartHardTimer(1 ,500000, (void *)TIM_CallBack1);
              }
              else
              {
                
              }
            }
          }
          else if(var_addr == PAGE1_KEY_RESET)
          {
            ptMsg->addr = BROADCAST;
            ptMsg->func = FUNC_WRITE;
            ptMsg->reg = REG_RESET;
            ptMsg->value = 0;
            if(xQueueSend(xQueue_message,                  /* ��Ϣ���о�� */
                          (void *) &ptMsg,           /* ���ͽṹ��ָ�����ptMsg�ĵ�ַ */
                          (TickType_t)10) == pdPASS )
            {
              /* ����ʧ�ܣ���ʹ�ȴ���10��ʱ�ӽ��� */
              printf("��λ���з��ͳɹ�\r\n");
            }
          }
        }
      }
    }
    Task_iwdg_refresh(TASK_LCD);
  }
}

/*
*********************************************************************************************************
*	�� �� ��: vTaskMassStorage
*	����˵��: ʹ�ú���xQueueReceive�Ӷ�ʱ���жϷ��͵���Ϣ��������	
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 2  
*********************************************************************************************************
*/
static void vTaskMassStorage(void *pvParameters)
{
  FILE_INFO read_info;
  FRESULT result;
  FATFS fs;
  FIL file;
  DIR DirInf;
  uint32_t bw;
  BaseType_t xResult;
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(5); /* �������ȴ�ʱ��Ϊ5ms */
  usbh_OpenMassStorage();
  while(1)
  {
    USBH_Process(&USB_OTG_Core, &USB_Host);
    xResult = xSemaphoreTake(xSemaphore_download, (TickType_t)xMaxBlockTime);
    if(xResult == pdTRUE)
    {
      if(usb_disk_flag == 1)
      {//
        /* �����ļ�ϵͳ */
        result = f_mount(&fs, "2:", 0);			/* Mount a logical drive */
        if (result != FR_OK)
        {
          printf("�����ļ�ϵͳʧ�� (%s)\r\n", FR_Table[result]);
        }
        /* �򿪸��ļ��� */
        result = f_opendir(&DirInf, "2:"); /* ���������������ӵ�ǰĿ¼��ʼ */
        if (result != FR_OK)
        {
          printf("�򿪸�Ŀ¼ʧ��  (%s)\r\n", FR_Table[result]);
          return;
        }
        __set_PRIMASK(1);
        W25QXX_Read((u8 *)&read_info,(u32)(W25QXX_ADDR_FILE + FILE_SIZE * download_num),sizeof(read_info));//����num������
        __set_PRIMASK(0);
        u8 name_1[10],name_2[20];
        memset(name_1,0,10);
        memset(name_2,0,20);
        memcpy(name_1,read_info.filename,read_info.filename_len);
        sprintf(name_2,"2:/%d�Ż�-%s.CSV",device_info.master_id,name_1);
  //      result = f_open(&file,"2:/22.CSV",FA_CREATE_ALWAYS | FA_WRITE);
        result = f_open(&file,name_2,FA_CREATE_ALWAYS | FA_WRITE);
        if(result != FR_OK)
        {
          printf("���ļ�ʧ��\r\n");
          return;
        }
        char buf[500] = "λ��,�趨����\n",buf1[20];
        u16 len,i;
        for(i=0;i<30;i++)
        {
          sprintf(buf1,"%d#,%.2f\n",i + 1,(float)read_info.weight_value[i] / 1000.0);
          strcat(buf,buf1);
        }
        sprintf(buf,"%sʱ������,%02d/%02d/%02d,%02d:%02d:%02d",buf,read_info.year,
                read_info.month,read_info.day,read_info.hour,read_info.minute,read_info.second);
        len = strlen(buf);
        result = f_write(&file,buf,len,&bw);
        if(result == FR_OK)
        {
          Sdwe_disString(PAGE_HISTORY_TEXT_FILE_WARN,"д��ɹ�",strlen("д��ɹ�"));
          bsp_StartHardTimer(1 ,500000, (void *)TIM_CallBack1);
          printf("%s.CSV�ļ�д��ɹ�\r\n",name_1);
        }
        else
        {
          Sdwe_disString(PAGE_HISTORY_TEXT_FILE_WARN,"д��ʧ��",strlen("д��ʧ��"));
          bsp_StartHardTimer(1 ,500000, (void *)TIM_CallBack1);
          printf("%s.CSV�ļ�д��ʧ��\r\n",name_1);
        }
        /* �ر��ļ�*/
        f_close(&file);
        
        /* ж���ļ�ϵͳ */
        f_mount(NULL, "2:", 0);
      }
      else
      {
        Sdwe_disString(PAGE_HISTORY_TEXT_FILE_WARN,"��U��",strlen("��U��"));
        bsp_StartHardTimer(1 ,500000, (void *)TIM_CallBack1);
        printf("U��δ����\r\n");
      }
    }
    Task_iwdg_refresh(TASK_MassStorage);
  }
}

static void vTaskTaskLED(void *pvParameters)
{
  while(1)
  {
    bsp_LedToggle(1);
    bsp_LedToggle(2);
    vTaskDelay(500);
    Task_iwdg_refresh(TASK_LED);
  }
}

static void vTaskTaskRFID(void *pvParameters)
{
  u16 isCard;
  u8 card_id[4];
<<<<<<< HEAD
  while(1)
  {
    rc522_cmd_request(REQUEST_TYPE_ALL);
    vTaskDelay(100);
    rc522_cmd_anticoll(COLLISION_GRADE_1);
    vTaskDelay(100);
//    if(rfid_rev_flag)
//    {
//      rfid_rev_flag = 0;
//      printf("rev rc522\r\n");
//      isCard = rc522_find(rfid_rev_buf,rfid_rev_cnt);
//      if(isCard == Mifare1_S50)
//      {
//        printf("s50 ");
//        rc522_cmd_anticoll(COLLISION_GRADE_1);
//        vTaskDelay(100);
//        if(rfid_rev_flag)
//        {
//          rfid_rev_flag = 0;
////          rc522_card_id(rfid_rev_buf,rfid_rev_cnt,card_id);
////          printf("%x %x %x %x\r\n",card_id[0],card_id[1],card_id[2],card_id[3]);
//        }
//      }
//    }
//    vTaskDelay(1000);
=======
  u8 cnt = 0;
  u8 timeout = 0;
  while(1)
  {
    switch(cnt)
    {
      case 0:
        rc522_cmd_request(REQUEST_TYPE_ALL);
        cnt++;
        timeout = 10;
        break;
      case 1:
        if(rfid_rev_flag)
        {
          rfid_rev_flag = 0;
          printf("rev rc522\r\n");
          isCard = rc522_find(rfid_rev_buf,rfid_rev_cnt);
          if(isCard == Mifare1_S50)
          {
            printf("s50 ");
            rc522_cmd_anticoll(COLLISION_GRADE_1);
            cnt++;
            timeout = 10;
          }
          else
            cnt = 0;
        }
        else
        {
          timeout--;
          if(timeout == 0)
          {//1s��ģ���޻ظ��˳�
            cnt = 0;
          }
        }
        break;
      case 2:
        if(rfid_rev_flag)
        {
          rfid_rev_flag = 0;
          u8 err;
          err = rc522_card_id(rfid_rev_buf,rfid_rev_cnt,card_id);
          if(err == 0)
          {//������ȷ
            printf("%x %x %x %x\r\n",card_id[0],card_id[1],card_id[2],card_id[3]);
          }
          cnt = 0;
        }
        else
        {
          timeout--;
          if(timeout == 0)
          {
            cnt = 0;
          }
        }
        break;
      default:
        break;
    }
    vTaskDelay(100);
>>>>>>> e91465d28df06ad15ef63bed982c7be4489bd12e
  }
}

/*
*********************************************************************************************************
*	�� �� ��: vTaskMsgPro
*	����˵��: ʹ�ú���xQueueReceive�Ӷ�ʱ���жϷ��͵���Ϣ��������
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 3  
*********************************************************************************************************
*/
static void vTaskMsgPro(void *pvParameters)
{
  SLAVE *ptMsg1;
  SLAVE info;
  BaseType_t xResult;
  m_frame_typedef txframe;
  u8 res;
  u8 index = 0;
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(10); /* �������ȴ�ʱ��Ϊ200ms */
  while(1)
  {
    xResult = xQueueReceive(xQueue_message,                   /* ��Ϣ���о�� */
                            (void *)&ptMsg1,  		   /* �����ȡ���ǽṹ��ĵ�ַ */
                            (TickType_t)xMaxBlockTime);/* ��������ʱ�� */
    if(xResult == pdPASS)
    {
      if(ptMsg1->addr == 0xAA)
      {
        u8 i;
        for(i=0;i<10;i++)
        {
          info.addr = i + 1;
          info.func = ptMsg1->func;
          info.reg = ptMsg1->reg;
          info.value = ptMsg1->value;
          res = modbus_send_frame(&txframe,info);
          if(res == MR_OK)
          {//��Ӧλ����ʾ��ȷͼ��
            Sdwe_writeIcon(PAGE1_SLAVE_STATE1 + i,VGUS_OFF);
          }
          else
          {//��Ӧλ����ʾ����ͼ��
            Sdwe_writeIcon(PAGE1_SLAVE_STATE1 + i,VGUS_ON);
          }
        }
      }
      else if(ptMsg1->addr == 0xBB)
      {
        u8 i;
        for(i=10;i<20;i++)
        {
          info.addr = i + 1;
          info.func = ptMsg1->func;
          info.reg = ptMsg1->reg;
          info.value = ptMsg1->value;
          res = modbus_send_frame(&txframe,info);
          if(res == MR_OK)
          {//��Ӧλ����ʾ��ȷͼ��
            Sdwe_writeIcon(PAGE1_SLAVE_STATE1 + i,VGUS_OFF);
          }
          else
          {//��Ӧλ����ʾ����ͼ��
            Sdwe_writeIcon(PAGE1_SLAVE_STATE1 + i,VGUS_ON);
          }
        }
      }
      else if(ptMsg1->addr == 0xCC)
      {
        u8 i;
        for(i=20;i<30;i++)
        {
          info.addr = i + 1;
          info.func = ptMsg1->func;
          info.reg = ptMsg1->reg;
          info.value = ptMsg1->value;
          res = modbus_send_frame(&txframe,info);
          if(res == MR_OK)
          {//��Ӧλ����ʾ��ȷͼ��
            Sdwe_writeIcon(PAGE1_SLAVE_STATE1 + i,VGUS_OFF);
          }
          else
          {//��Ӧλ����ʾ����ͼ��
            Sdwe_writeIcon(PAGE1_SLAVE_STATE1 + i,VGUS_ON);
          }
        }
      }
      else if(ptMsg1->addr == 0xff)
      {
        if((ptMsg1->func == FUNC_WRITE) && (ptMsg1->reg == REG_SET_WEIGHT))
        {//��ÿ���ӻ������趨����
          u8 i;
          for(i=0;i<30;i++)
          {
            if(SlavePara.onoff[i] == 1)
            {
              info.addr = i + 1;
              info.func = ptMsg1->func;
              info.reg = ptMsg1->reg;
              info.value = SlavePara.value_set[i];
              res = modbus_send_frame(&txframe,info);
              if(res == MR_OK)
              {//��Ӧλ����ʾ��ȷͼ��
                Sdwe_writeIcon(PAGE1_SLAVE_STATE1 + i,VGUS_OFF);
              }
              else
              {//��Ӧλ����ʾ����ͼ��
                Sdwe_writeIcon(PAGE1_SLAVE_STATE1 + i,VGUS_ON);
              }
            }
          }
        }
        else if((ptMsg1->func == FUNC_WRITE) && (ptMsg1->reg == REG_RESET))
        {
          info.addr = BROADCAST;
          info.func = ptMsg1->func;
          info.reg = ptMsg1->reg;
          info.value = 0;
          modbus_send_frame(&txframe,info);
        }
        else if((ptMsg1->func == FUNC_WRITE) && (ptMsg1->reg == REG_START))
        {
          info.addr = BROADCAST;
          info.func = ptMsg1->func;
          info.reg = ptMsg1->reg;
          info.value = ptMsg1->value;
          modbus_send_frame(&txframe,info);
        }
      }
      else
      {
        info.addr = ptMsg1->addr + 1;
        info.func = ptMsg1->func;
        info.reg = ptMsg1->reg;
        info.value = ptMsg1->value;
        res = modbus_send_frame(&txframe,info);
        if(res == MR_OK)
        {//��Ӧλ����ʾ��ȷͼ��
          Sdwe_writeIcon(PAGE1_SLAVE_STATE1 + info.addr - 1,VGUS_OFF);
        }
        else
        {//��Ӧλ����ʾ����ͼ��
          Sdwe_writeIcon(PAGE1_SLAVE_STATE1 + info.addr - 1,VGUS_ON);
        }
      }
    }
    else
    {
      if(SlavePara.onoff[index] == 1)
      {
        info.addr = index + 1;
        info.func = FUNC_READ;
        info.reg = REG_GET_WEIGHT;
        info.value = 0;
        res = modbus_send_frame(&txframe,info);
        if(res == MR_OK)
        {
          if(readSlave.func == FUNC_READ)
          {
            if(readSlave.reg == REG_GET_WEIGHT)
            {//��ȡ����������ʾ����Ļ
              SlavePara.value_sample[readSlave.addr - 1] = readSlave.value;
              Sdwe_disDigi(PAGE1_SAMPLE_VALUE1 + readSlave.addr - 1,SlavePara.value_sample[index] / 10);
              Sdwe_writeIcon(PAGE1_SLAVE_STATE1 + readSlave.addr - 1,VGUS_OFF);//��Ӧλ����ȷ����ͼ��
              Sdwe_disDigi(PAGE1_ECHO_WEIGHT + readSlave.addr - 1,readSlave.set_value / 10);
            }
          }
        }
        else
        {//��Ӧλ����ʾ����ͼ��
          Sdwe_writeIcon(PAGE1_SLAVE_STATE1 + index,VGUS_ON);
          if(info.func == FUNC_READ)
          {
            if(info.reg == REG_GET_WEIGHT)
            {//��ȡ����������ʾ����Ļ
              Sdwe_disDigi(PAGE1_ECHO_WEIGHT + index,0);
              //            Sdwe_writeIcon(PAGE1_SLAVE_ONOFF1 + index,0);
            }
          }
        }
        vTaskDelay(80);
      }
      index++;
      if(index >= 30)
      {
        index = 0;
        info.addr = 0xFF;
        info.func = FUNC_WRITE;
        info.reg = REG_START;
        info.value = isWork;
        modbus_send_frame(&txframe,info);
      }
    }
    Task_iwdg_refresh(TASK_MsgPro);
  }
}

/*
*********************************************************************************************************
*	�� �� ��: vTaskRev485
*	����˵��: ��������Ҳ����������ȼ�����������������ɨ�衣
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 4  
*********************************************************************************************************
*/
static void vTaskRev485(void *pvParameters)
{
  SLAVE *ptMsg;
  /* ��ʼ���ṹ��ָ�� */
  ptMsg = &slave_info;
  /* ��ʼ������ */
  ptMsg->addr = 0;
  ptMsg->reg = 0;
  ptMsg->value = 0;
  while(1)
  {
    isWork = GetDeviceState();
    vTaskDelay(50);
    if(isWork != old_isWork)
    {
      old_isWork = isWork;
      ptMsg->addr = BROADCAST;
      ptMsg->func = FUNC_WRITE;
      ptMsg->reg = REG_START;
      ptMsg->value = isWork;
      if(xQueueSend(xQueue_message,                  /* ��Ϣ���о�� */
                    (void *) &ptMsg,           /* ���ͽṹ��ָ�����ptMsg�ĵ�ַ */
                    (TickType_t)10) == pdPASS )
      {
        printf("isWork is %d ���з��ͳɹ�\r\n",isWork);
      }
    }
    Task_iwdg_refresh(TASK_Rev485);
  }
}

static void vTaskReadDisk(void *pvParameters)
{
  BaseType_t xResult;
  FRESULT result;
  FATFS fs;
  DIR DirInf;
  FILINFO FileInf;
  uint32_t cnt = 0;
  char lfname[256];
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(200); /* �������ȴ�ʱ��Ϊ200ms */
  while(1)
  {
    xResult = xSemaphoreTake(xSemaphore_readDisk, (TickType_t)xMaxBlockTime);
    if(xResult == pdTRUE)
    {
      if(usb_disk_flag == 1)
      {
        DiskFile_init();
        Sdwe_disPicture(PAGE_U);//��ת��U���ļ�ҳ��
        memset(read_file_select,0,10);//����ļ�ѡ��Buff
        Sdwe_ClearReadDisk();//����ļ�ѡ��ͼ��
        /* �����ļ�ϵͳ */
        result = f_mount(&fs, "2:", 0);			/* Mount a logical drive */
        if (result != FR_OK)
        {
          printf("�����ļ�ϵͳʧ�� (%s)\r\n", FR_Table[result]);
        }
        /* �򿪸��ļ��� */
        result = f_opendir(&DirInf, "2:"); /* ���������������ӵ�ǰĿ¼��ʼ */
        if (result != FR_OK)
        {
          printf("�򿪸�Ŀ¼ʧ��  (%s)\r\n", FR_Table[result]);
          return;
        }
          /* ��ȡ��ǰ�ļ����µ��ļ���Ŀ¼ */
        #if _USE_LFN
          FileInf.lfname = lfname;
          FileInf.lfsize = 256;
        #endif
          printf("����        |  �ļ���С | ���ļ��� | ���ļ���\r\n");
          for(cnt = 0; ;cnt++)
          {
            result = f_readdir(&DirInf,&FileInf); 		/* ��ȡĿ¼��������Զ����� */
            if (result != FR_OK || FileInf.fname[0] == 0)
            {
              break;
            }
            if (FileInf.fname[0] == '.')
            {
              continue;
            }
            /* �ж����ļ�������Ŀ¼ */
            if (FileInf.fattrib & AM_DIR)
            {
              printf("(0x%02d)Ŀ¼  ", FileInf.fattrib);
            }
            else
            {
              printf("(0x%02d)�ļ�  ", FileInf.fattrib);
              if((strstr(FileInf.fname,".CSV")) && (strlen(FileInf.fname) <= 10))
              {//�ļ�ΪCSV������ļ�������С��10�ַ�
                if(Disk_File.filenum < 10)
                {
                  memcpy(Disk_File.filename[Disk_File.filenum],FileInf.fname,10);
                  Sdwe_disString(PAGE_U_TEXT_FILENAME1 + Disk_File.filenum * 10,FileInf.fname,10);
                  Disk_File.filenum++;
                }
              }
            }
            /* ��ӡ�ļ���С, ���4G */
            printf(" %10d", FileInf.fsize);
            printf("  %s |", FileInf.fname);	/* ���ļ��� */
            printf("  %s\r\n", (char *)FileInf.lfname);	/* ���ļ��� */
          }
          /* ж���ļ�ϵͳ */
          f_mount(NULL, "2:", 0);
      }
      else
      {
        Sdwe_disString(PAGE_HISTORY_TEXT_FILE_WARN,"��U��",strlen("��U��"));
        bsp_StartHardTimer(1 ,500000, (void *)TIM_CallBack1);
      }
    }
    Task_iwdg_refresh(TASK_ReadDisk);
  }
}

void vTaskManageCapacity(void *pvParameters)
{
  BaseType_t xResult;
  u8 buf[10];
  float complete_p,uncomplete_p;
  float complete_m,uncomplete_m;
  float p_value = 0.0,old_p_value = 0.0;
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(200); /* �������ȴ�ʱ��Ϊ200ms */
  init_product_para(product_para);
  while(1)
  {
    xResult = xSemaphoreTake(xSemaphore_pluse, (TickType_t)xMaxBlockTime);
    if(xResult == pdTRUE)
    {
      pluse_count++;
      if((pluse_count % 1000) == 0)
      {
        Sdwe_disDigi(PAGE_PRODUCT_KILOCOUNT,pluse_count % 1000);
      }
      product_para.pulse_count = pluse_count;
      p_value = product_per_meter(&product_para);
      p_value = get_float_1bit(p_value);//ȡ1λС����
      if(p_value != old_p_value)
      {//����ֵ�б仯ʱ�ű��ֲ���ʾ
        if(grade == 0)
        {//A��
          product_para.product_a = p_value;//A�����
          Sdwe_disDigi(PAGE_PRODUCT_A,(int)(p_value * 10));
        }
        else if(grade == 0)
        {//B��
          product_para.product_b = p_value;//A�����
          Sdwe_disDigi(PAGE_PRODUCT_B,(int)(p_value * 10));
        }
        complete_p = product_complete_meter(&product_para);//����ɲ���
        uncomplete_p = product_uncomplete_meter(&product_para);//δ��ɲ���
        complete_m = product_complete_kilo(&product_para);//���������
        uncomplete_m = product_uncomplete_kilo(&product_para);//δ�������
        Sdwe_disDigi(PAGE_PRODUCT_COMPLETE,(int)(complete_p * 10));
        Sdwe_disDigi(PAGE_PRODUCT_UNCOMPLETE,(int)(uncomplete_p * 10));
        Sdwe_disDigi(PAGE_PRODUCT_COMPLETE_W,(int)(complete_m * 10));
        Sdwe_disDigi(PAGE_PRODUCT_UNCOMPLETE_W,(int)(uncomplete_m * 10));
      }
      memset(buf,0,10);
      sprintf(buf,"%d",pluse_count);
    }
  }
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void AppTaskCreate (void)
{
  xTaskCreate( vTaskTaskLED,   	/* ������  */
              "vTaskTaskLED",     	/* ������    */
              64,               	/* ����ջ��С����λword��Ҳ����4�ֽ� */
              NULL,              	/* �������  */
              1,                 	/* �������ȼ�*/
              &xHandleTaskLED );  /* ������  */
  xTaskCreate( vTaskTaskRFID,   	/* ������  */
              "vTaskTaskRFID",     	/* ������    */
              64,               	/* ����ջ��С����λword��Ҳ����4�ֽ� */
              NULL,              	/* �������  */
              1,                 	/* �������ȼ�*/
              &xHandleTaskRFID );  /* ������  */
  xTaskCreate( vTaskMsgPro,     		/* ������  */
              "vTaskMsgPro",   		/* ������    */
              256,             		/* ����ջ��С����λword��Ҳ����4�ֽ� */
              NULL,           		/* �������  */
              2,               		/* �������ȼ�*/
              &xHandleTaskMsgPro );  /* ������  */
  xTaskCreate( vTaskRev485,     		/* ������  */
              "vTaskRev485",   		/* ������    */
              512,            		/* ����ջ��С����λword��Ҳ����4�ֽ� */
              NULL,           		/* �������  */
              3,              		/* �������ȼ�*/
              &xHandleTaskRev485 );   /* ������  */
  xTaskCreate( vTaskTaskLCD,   	/* ������  */
              "vTaskLCD",     	/* ������    */
              1024,               	/* ����ջ��С����λword��Ҳ����4�ֽ� */
              NULL,              	/* �������  */
              4,                 	/* �������ȼ�*/
              &xHandleTaskLCD );  /* ������  */
  xTaskCreate( vTaskMassStorage,    		/* ������  */
              "vTaskMassStorage",  		/* ������    */
              1024,         		/* ����ջ��С����λword��Ҳ����4�ֽ� */
              NULL,        		/* �������  */
              5,           		/* �������ȼ�*/
              &xHandleTaskMassStorage ); /* ������  */
  xTaskCreate( vTaskReadDisk,    		/* ������  */
              "vTaskReadDisk",  		/* ������    */
              512,         		/* ����ջ��С����λword��Ҳ����4�ֽ� */
              NULL,        		/* �������  */
              6,           		/* �������ȼ�*/
              &xHandleTaskReadDisk); /* ������  */
  xTaskCreate( vTaskManageCapacity,    		/* ������  */
              "vTaskManageCapacity",  		/* ������    */
              256,         		/* ����ջ��С����λword��Ҳ����4�ֽ� */
              NULL,        		/* �������  */
              7,           		/* �������ȼ�*/
              &xHandleTaskManageCapacity); /* ������  */
}

/*
*********************************************************************************************************
*	�� �� ��: AppObjCreate
*	����˵��: ��������ͨ�Ż���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void AppObjCreate (void)
{
  /* ������ֵ�ź������״δ����ź�������ֵ��0 */
  xSemaphore_lcd = xSemaphoreCreateBinary();
  
  if(xSemaphore_lcd == NULL)
  {
    printf("xSemaphore_lcd fault\r\n");
    /* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
  }
  /* ������ֵ�ź������״δ����ź�������ֵ��0 */
  xSemaphore_rs485 = xSemaphoreCreateBinary();
  
  if(xSemaphore_rs485 == NULL)
  {
    printf("xSemaphore_rs485 fault\r\n");
    /* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
  }
  
  /* ������ֵ�ź������״δ����ź�������ֵ��0 */
  xSemaphore_pluse = xSemaphoreCreateBinary();
  
  if(xSemaphore_pluse == NULL)
  {
    printf("xSemaphore_pluse fault\r\n");
    /* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
  }
  
  xSemaphore_readDisk = xSemaphoreCreateBinary();
  
  if(xSemaphore_readDisk == NULL)
  {
    printf("xSemaphore_readDisk fault\r\n");
    /* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
  }
  xSemaphore_download = xSemaphoreCreateBinary();
  
  if(xSemaphore_download == NULL)
  {
    printf("xSemaphore_download fault\r\n");
    /* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
  }
  idwgEventGroup = xEventGroupCreate();
  if(idwgEventGroup == NULL)
  {
    printf("idwgEventGroup fault\r\n");
  }
  /* ����10��uint8_t����Ϣ���� */
  xQueue_message = xQueueCreate(10, sizeof(struct SLAVE *));
  if(xQueue_message == NULL)
  {
    printf("xQueue_message fault\r\n");
    printf("xQueue_message create failure\r\n");
    /* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
  }
  xTimerUser = xTimerCreate("Timer",          /* ��ʱ������ */
                             500,    /* ��ʱ������,��λʱ�ӽ��� */
                             pdTRUE,          /* ������ */
                             (void *)0,      /* ��ʱ��ID */
                             UserTimerCallback); /* ��ʱ���ص����� */
    
  if(xTimerUser != NULL)
  {
    
    if(xTimerStart(xTimerUser, 500) != pdPASS)
    {
      printf("xSemaphore_rs485 fault\r\n");
      /* ��ʱ����û�н��뼤��״̬ */
    }
  }
}

void TIM_CallBack1(void)
{
  Sdwe_clearString(PAGE_HISTORY_TEXT_FILE_WARN);
  Sdwe_clearString(PAGE_U_TEXT_READ_STATE);
}

void UserTimerCallback(TimerHandle_t xTimer)
{//��ʱʱ��500ms
  static u16 sample_time = 0;
  u16 speed_1,speed_2,speed;
  EventBits_t uxBits;
  uxBits = xEventGroupWaitBits(idwgEventGroup, /* �¼���־���� */
                               IWDG_BIT_ALL,            /* �ȴ�bit0��bit1������ */
                               pdTRUE,             /* �˳�ǰbit0��bit1�������������bit0��bit1�������òű�ʾ���˳���*/
                               pdTRUE,             /* ����ΪpdTRUE��ʾ�ȴ�bit1��bit0��������*/
                               1); 	 /* �ȴ��ӳ�ʱ�� */
  if((uxBits & IWDG_BIT_ALL) == IWDG_BIT_ALL)
  {
    IWDG_Feed();
  }
  if(isWork == 1)
  {
    if(sample_time == 0)
    {
      speed_1 = pluse_count;
    }
    else if(sample_time >= 120)
    {//����1�����ڵ�������
      sample_time = 0;
      speed_2 = pluse_count;
      speed = speed_2 - speed_1;
      Sdwe_disDigi(PAGE_PRODUCT_SPEED,speed);//��ʾ�ٶ�
    }
    else
    {
      sample_time++;
    }
  }
}

void Task_iwdg_refresh(u8 task)
{
  EventBits_t uxBits;
  if(task == TASK_LED)
  {
    uxBits = IWDG_BIT_LED;
  }
  else if(task == TASK_LCD)
  {
    uxBits = IWDG_BIT_LCD;
  }
  else if(task == TASK_MassStorage)
  {
    uxBits = IWDG_BIT_MassStorage;
  }
  else if(task == TASK_MsgPro)
  {
    uxBits = IWDG_BIT_MsgPro;
  }
  else if(task == TASK_Rev485)
  {
    uxBits = IWDG_BIT_Rev485;
  }
  else if(task == TASK_ReadDisk)
  {
    uxBits = IWDG_BIT_ReadDisk;
  }
  xEventGroupSetBits(idwgEventGroup,uxBits);
}
