#include "includes.h"

TaskHandle_t xHandleTaskLCD = NULL;
TaskHandle_t xHandleTaskLED = NULL;
TaskHandle_t xHandleTaskRFID = NULL;
TaskHandle_t xHandleTaskMassStorage = NULL;
TaskHandle_t xHandleTaskReadDisk = NULL;
TaskHandle_t xHandleTaskMsgPro = NULL;
TaskHandle_t xHandleTaskRev485 = NULL;
TaskHandle_t xHandleTaskManageCapacity = NULL;
TaskHandle_t xHandleTaskMotorControl = NULL;

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
u16 sample_time = 0;
u16 pluse_count = 0;

u8 card_record = 0,old_card_record = 0;

u8 card_config = CARD_DISABLE;
u8 isDevicePeriod = 0;
extern void DemoFatFS(void);
extern const char * FR_Table[];
extern u8 modbus_send_frame(m_frame_typedef * fx,SLAVE info);

u8 pwm_flag = 0;

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
            if(device_info.period_para.period_enable_onoff == 1)
            {
              isDevicePeriod = get_period_state(&rtc_time,&device_info); 
            }
            else
            {
              isDevicePeriod = 0;
            }
          }
        }
        else if(lcd_rev_buf[3] == 0x83)
        {//�������洢������
          var_addr = (lcd_rev_buf[4] << 8) + lcd_rev_buf[5];
          if(var_addr < 0x1000)
          {//��ַ�ֳ����Σ���߼���Ч��
            if(var_addr == MAIN_PAGE_KEY_JINGSHA)
            {//��ɴ����
              if(isDevicePeriod)
              {
                if(device_info.func_onoff.jingsha)
                {
                  Init_JINGSHA_GUI();
                }
                else
                {
                  SDWE_WARNNING(MAIN_PAGE_WARNNING,"����ϵ����");
                }
              }
              else
                SDWE_WARNNING(MAIN_PAGE_WARNNING,"�����ڽ���");
            }
            else if(var_addr == MAIN_PAGE_KEY_WEIMI)
            {//γ��
              pwm_flag = 1;
//              if(isDevicePeriod == 0)
//              {
//                if(device_info.func_onoff.weimi)
//                {
//                  sdew_weimi_page1(&weimi_para);
//                }
//                else
//                {
//                  SDWE_WARNNING(MAIN_PAGE_WARNNING,"����ϵ����");
//                }
//              }
//              else
//              {
//                SDWE_WARNNING(MAIN_PAGE_WARNNING,"�����ڽ���");
//              }
            }
            else if(var_addr == MAIN_PAGE_KEY_CHANNENG)
            {//����
              if(isDevicePeriod == 0)
              {
                if(device_info.func_onoff.channeng)
                {
                  Sdwe_product_page(&product_para);
                  card_config = READ_PERMISSION;
                }
                else
                {
                  SDWE_WARNNING(MAIN_PAGE_WARNNING,"����ϵ����");
                }
              }
              else
              {
                SDWE_WARNNING(MAIN_PAGE_WARNNING,"�����ڽ���");
              }
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
              //            Sdwe_readRTC();
            }
            else if(var_addr == PAGE2_KEY_SAVE)
            {//�ڶ�ҳ����
              memset(input_password_buf,0,10);
              Sdwe_clearString(PAGE2_FILE_TEXT_DIS);
              Sdwe_clearString(PAGE2_FILE_TEXT_WARN);
              //            Sdwe_readRTC();
            }
            else if(var_addr == PAGE3_KEY_SAVE)
            {//����ҳ����
              memset(input_password_buf,0,10);
              Sdwe_clearString(PAGE3_FILE_TEXT_DIS);
              Sdwe_clearString(PAGE3_FILE_TEXT_WARN);
              //            Sdwe_readRTC();
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
                  W25QXX_Read(name_buf,(u32)W25QXX_ADDR_JINGSHA + FILE_SIZE * i,11);
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
                    W25QXX_Read(file_read,(u32)W25QXX_ADDR_JINGSHA + FILE_SIZE,FILE_SIZE * 9);//����2-10�ļ�������
                    W25QXX_Write(file_read,(u32)W25QXX_ADDR_JINGSHA,FILE_SIZE * 9);//��д��1-9�ļ���ַ
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
                    W25QXX_Write((u8 *)&file_write,(u32)(W25QXX_ADDR_JINGSHA + FILE_SIZE * 9),sizeof(file_write));//д��ǰҳ�����ݵ��ļ�10��ַ
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
                    W25QXX_Write((u8 *)&file_write,(u32)(W25QXX_ADDR_JINGSHA + FILE_SIZE * device_info.page_count_all),sizeof(file_write));
                    device_info.page_count_all++;
                    W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
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
              W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
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
              W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
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
              W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
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
              W25QXX_Write((u8 *)&File_info,(u32)(W25QXX_ADDR_JINGSHA + FILE_SIZE * device_info.page_count_select),sizeof(File_info));
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
              //            if(strcmp((char const*)input_password_buf,"111111") == 0)
              if(memcmp(&input_password_buf,&device_info.regin_in.password,device_info.regin_in.password_len) == 0)
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
              
              //            if(strcmp((char const*)input_password_buf,"111111") == 0)
              if(memcmp(&input_password_buf,&device_info.regin_in.password,device_info.regin_in.password_len) == 0)
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
              
              //            if(strcmp((char const*)input_password_buf,"111111") == 0)
              if(memcmp(&input_password_buf,&device_info.regin_in.password,device_info.regin_in.password_len) == 0)
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
                  W25QXX_Read(name_buf,(u32)W25QXX_ADDR_JINGSHA + FILE_SIZE * i + 1,10);
                  if(strcmp((char const*)input_password_buf,(char const*)name_buf) == 0)
                  {
                    repeat = 1;
                    break;
                  }
                }
                if(repeat == 0)
                {//���ظ��ļ�
                  if(device_info.page_count_all >= 10)
                  {//ȥ����һ���ļ��������ļ���������
                    u8 *file_read;
                    file_read = mymalloc(SRAMIN,1024);
                    W25QXX_Read(file_read,(u32)W25QXX_ADDR_JINGSHA + FILE_SIZE,FILE_SIZE * 9);//����2-10�ļ�������
                    W25QXX_Write(file_read,(u32)W25QXX_ADDR_JINGSHA,FILE_SIZE * 9);//��д��1-9�ļ���ַ
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
                    W25QXX_Write((u8 *)&file_write,(u32)(W25QXX_ADDR_JINGSHA + FILE_SIZE * 9),sizeof(file_write));//д��ǰҳ�����ݵ��ļ�10��ַ
                    if(var_addr == PAGE1_FILE_TEXT_IMPORT)//���µ�һҳ���水ť
                      Sdwe_disPicture(PAGE_1);//��ת��ҳ��1
                    else if(var_addr == PAGE2_FILE_TEXT_IMPORT)//���µڶ�ҳ���水ť
                      Sdwe_disPicture(PAGE_2);//��ת��ҳ��2
                    else if(var_addr == PAGE3_FILE_TEXT_IMPORT)//���µ���ҳ���水ť
                      Sdwe_disPicture(PAGE_3);//��ת��ҳ��3
                    myfree(SRAMIN,file_read);
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
                    W25QXX_Write((u8 *)&file_write,(u32)(W25QXX_ADDR_JINGSHA + FILE_SIZE * device_info.page_count_all),sizeof(file_write));
                    device_info.page_count_all++;
                    W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
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
                SDWE_WARNNING(PAGE_HISTORY_TEXT_FILE_WARN,"δѡ���ļ�");
              }
              else
              {
                if(device_info.page_count_all == 0)
                {//û�б�����ļ�
                  SDWE_WARNNING(PAGE_HISTORY_TEXT_FILE_WARN,"���ļ�");
                }
                else
                {
                  if(cnt > 1)
                  {//ѡ���ļ�����1��
                    SDWE_WARNNING(PAGE_HISTORY_TEXT_FILE_WARN,"ֻ��ѡһ��");
                  }
                  else
                  {
                    if(var_addr == PAGE_HISTORY_KEY_SELECT)
                    {//�����ļ�
                      FILE_INFO file_read;
                      u8 i;
                      W25QXX_Read((u8 *)&file_read,(u32)(W25QXX_ADDR_JINGSHA + FILE_SIZE * num),sizeof(file_read));//����num������
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
                      W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
                      Init_JINGSHA_GUI();
                      SDWE_WARNNING(PAGE_HISTORY_TEXT_FILE_WARN,"���óɹ�");
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
              W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
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
              W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
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
              u16 cnt = 0;//�ļ�ѡ���־��ĳ���ļ�ѡ�񣬶�Ӧ��λ��1
              for(i=0;i<10;i++)
              {
                if(Disk_File.fileselect[i] == 1)
                {
                  cnt++;
                }
              }
              if(cnt == 0)
              {//δѡ���ļ�
                SDWE_WARNNING(PAGE_U_TEXT_READ_STATE,"δѡ���ļ�");
              }
              else
              {
                if(Disk_File.filenum == 0)
                {//û�б�����ļ�
                  SDWE_WARNNING(PAGE_U_TEXT_READ_STATE,"���ļ�");
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
            else if(var_addr == PAGE_PRODUCT_PEILIAO)
            {//
              Sdwe_peiliao_page(&peiliao_para);
              card_config = CARD_DISABLE;
            }
            else if(var_addr == PAGE_PRODUCT_CLEAR)
            {//�������㣬���¿�ʼ����
              TIM_Cmd(TIM1, ENABLE);
              TIM_CtrlPWMOutputs(TIM1, ENABLE);
              total_meter_gross = (u32)(peiliao_para.total_meter_set * (1 + (float)peiliao_para.loss / 100));
              init_product_para(&product_para,&peiliao_para);//����������������󣬲�������
              peiliao_para.add_meter_set = 0;//����������������󣬲���������
              Sdwe_disDigi(PAGE_PRODUCT_ADD_METER,(int)(peiliao_para.add_meter_set * 10),4);
              Sdwe_product_page(&product_para);
              W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
              W25QXX_Write((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
              plan_complete = 0;
              old_plan_complete = 0;
              
              Sdwe_clearString(PAGE_PRODUCT_RFID_WARNNING);//������������ʾ
            }
            else if(var_addr == PAGE_PRODUCT_QUIT)
            {//�˳�����ҳ�棬�����ܽ�ֹ
              card_config = CARD_DISABLE;
            }
            else if(var_addr == PAGE_PEILIAO_QUIT)
            {//�˳�����ҳ�棬�������ҳ�棬��������
              card_config = READ_PERMISSION;
            }
            else if(var_addr == PAGE_PRODUCT_JINGSHA)
            {//��ɴ����
              float meter;
              meter = (float)((lcd_rev_buf[7] << 8) + lcd_rev_buf[8]) / 10;
              peiliao_para.latitude_weight = meter;
              peiliao_para.final_weight = final_per_meter(&peiliao_para);
              Sdwe_disDigi(PAGE_PRODUCT_FINAL,(int)(peiliao_para.final_weight * 10),2);
              W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
            }
            else if(var_addr == PAGE_PRODUCT_WEISHA)
            {//γɴ����
              float meter;
              meter = (float)((lcd_rev_buf[7] << 8) + lcd_rev_buf[8]) / 10;
              peiliao_para.longitude_weight = meter;
              peiliao_para.final_weight = final_per_meter(&peiliao_para);
              Sdwe_disDigi(PAGE_PRODUCT_FINAL,(int)(peiliao_para.final_weight * 10),2);
              W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
            }
            else if(var_addr == PAGE_PRODUCT_RUBBER)
            {//������
              float meter;
              meter = (float)((lcd_rev_buf[7] << 8) + lcd_rev_buf[8]) / 10;
              peiliao_para.rubber_weight = meter;
              peiliao_para.final_weight = final_per_meter(&peiliao_para);
              Sdwe_disDigi(PAGE_PRODUCT_FINAL,(int)(peiliao_para.final_weight * 10),2);
              W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
            }
            else if(var_addr == PAGE_PRODUCT_ZHIJI)
            {//֯������
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              peiliao_para.loom_num = cnt;
              W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
            }
            else if(var_addr == PAGE_PRODUCT_LOSS)
            {//���
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              peiliao_para.loss = cnt;
              total_meter_gross = (u32)(peiliao_para.total_meter_set * (1 + peiliao_para.loss / 100.0));
              total_weight_gross = (u32)(peiliao_para.total_weitht_set * (1 + peiliao_para.loss / 100.0));
              W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
            }
            else if(var_addr == PAGE_PRODUCT_TOTAL_METER)
            {//��������������
              u32 cnt;
              cnt = (lcd_rev_buf[7] << 24) + (lcd_rev_buf[8] << 16) + (lcd_rev_buf[9] << 8) + lcd_rev_buf[10];
              peiliao_para.total_meter_set = cnt;
              total_meter_gross = (u32)(peiliao_para.total_meter_set * (1 + (float)peiliao_para.loss / 100));
              init_product_para(&product_para,&peiliao_para);//����������������󣬲�������
              peiliao_para.add_meter_set = 0;//����������������󣬲���������
              Sdwe_disDigi(PAGE_PRODUCT_ADD_METER,(int)(peiliao_para.add_meter_set * 10),4);
              Sdwe_product_page(&product_para);
              W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
              W25QXX_Write((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
              plan_complete = 0;
              old_plan_complete = 0;
              
              Sdwe_clearString(PAGE_PRODUCT_RFID_WARNNING);//������������ʾ
            }
            else if(var_addr == PAGE_PRODUCT_TOTAL_WEIGHT)
            {//����������������
              u32 cnt;
              cnt = (lcd_rev_buf[7] << 24) + (lcd_rev_buf[8] << 16) + (lcd_rev_buf[9] << 8) + lcd_rev_buf[10];
              peiliao_para.total_weitht_set = cnt;
              total_weight_gross = (u32)(peiliao_para.total_weitht_set * (1 + (float)peiliao_para.loss / 100));
              init_product_para(&product_para,&peiliao_para);//����������������󣬲�������
              peiliao_para.add_meter_set = 0;//����������������󣬲���������
              Sdwe_disDigi(PAGE_PRODUCT_ADD_METER,(int)(peiliao_para.add_meter_set * 10),4);
              Sdwe_product_page(&product_para);
              W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
              W25QXX_Write((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
              plan_complete = 0;
              old_plan_complete = 0;
              
              Sdwe_clearString(PAGE_PRODUCT_RFID_WARNNING);//������������ʾ
            }
            else if(var_addr == PAGE_PRODUCT_KAIDU)
            {//��������
              float cnt;
              cnt = (float)((lcd_rev_buf[7] << 8) + lcd_rev_buf[8]) / 10;
              peiliao_para.kaidu_set = cnt;
              W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
            }
            else if(var_addr == PAGE_PRODUCT_WEIMI)
            {//γ������
              float cnt;
              cnt = (float)((lcd_rev_buf[7] << 8) + lcd_rev_buf[8]) / 10;
              if(cnt > 0)
              {
                peiliao_para.weimi_set = cnt;
                W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
              }
            }
            else if(var_addr == PAGE_PRODUCT_ADD_METER)
            {//��������
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 24) + (lcd_rev_buf[8] << 16) + (lcd_rev_buf[9] << 8) + lcd_rev_buf[10];
              peiliao_para.add_meter_set = cnt;
              W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
            }
            else if(var_addr == PAGE_PRODUCT_WEISHU_DIS)
            {//γ����ʾ����
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              peiliao_para.weimi_dis_set = cnt;
              W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
            }
            else if((var_addr >= PAGE_STOP_OFF) && (var_addr <= PAGE_STOP_OFF + 11))
            {//ͣ��ԭ��ѡ��
              if(device_info.system_state == SYS_NORMAL)
              {//ֻ��ϵͳ����ʱ������ѡ��ͣ��ԭ��
                device_info.system_state = var_addr - PAGE_STOP_OFF + 1;
                Sdwe_disString(PAGE1_SYSTEM_STATE,(u8 *)system_state_dis[device_info.system_state],strlen(system_state_dis[device_info.system_state]));
                W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
                Sdwe_writeIcon(var_addr - PAGE_STOP_OFF + PAGE_STOP_ON,VGUS_ON);//ͼ����ʾѡ��
                printf("System stop is num %d.\r\n",device_info.system_state);
              }
              else
              {
                if(device_info.system_state == (var_addr - PAGE_STOP_OFF + 1))
                {//ȡ��ѡ��ͣ��
                  Sdwe_writeIcon(var_addr - PAGE_STOP_OFF + PAGE_STOP_ON,VGUS_OFF);
                  device_info.system_state = SYS_NORMAL;
                  Sdwe_disString(PAGE1_SYSTEM_STATE,(u8 *)system_state_dis[device_info.system_state],strlen(system_state_dis[device_info.system_state]));
                  W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
                  printf("System normal.\r\n");
                }
                else
                {
                  SDWE_WARNNING(PAGE_STOP_WARNNING,"��Ч����");
                }
              }
            }
            /****************************A/B�໻��ʱ������************************************/
            else if(var_addr == PAGE_CONFIG_TIME)
            {//����ʱ������
              Sdwe_change_class_time(&device_info);
            }
            else if(var_addr == PAGE_CHANGE_HOUR)
            {//����ʱ��Сʱ����
              value = ((lcd_rev_buf[7] << 8) + lcd_rev_buf[8]);
              if(value <= 23)
              {
                device_info.class_para.class_time_hour = value;
                W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
              }
              else
              {
                SDWE_WARNNING(PAGE_CHANGE_WARNNING,"������Χ");
              }
            }
            else if(var_addr == PAGE_CHANGE_MINUTE)
            {//����ʱ���������
              value = ((lcd_rev_buf[7] << 8) + lcd_rev_buf[8]);
              if(value <= 59)
              {
                device_info.class_para.class_time_minute = value;
                W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
              }
              else
              {
                SDWE_WARNNING(PAGE_CHANGE_WARNNING,"������Χ");
              }
            }
            else if(var_addr == PAGE_CHANGE_SWITCH)
            {//�Ƿ񻻰�����
              value = ((lcd_rev_buf[7] << 8) + lcd_rev_buf[8]);
              device_info.class_para.class_enable_onoff = value;
              W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
            }
            
            /****************************************************************************/
            /****************************Ա����Ȩ������*******************************/
            else if(var_addr == PAGE_CONFIG_CARD)
            {//Ա��������
              card_config = WRITE_PERMISSION;
            }
            else if(var_addr == PAGE_CARD_A_INC)
            {//����A�࿨
              card_config = WRITE_INC_A;
            }
            else if(var_addr == PAGE_CARD_A_DEC)
            {//����A�࿨
              card_config = WRITE_DEC_A;
            }
            else if(var_addr == PAGE_CARD_B_INC)
            {//����B�࿨
              card_config = WRITE_INC_B;
            }
            else if(var_addr == PAGE_CARD_B_DEC)
            {//����B�࿨
              card_config = WRITE_DEC_B;
            }
            else if(var_addr == PAGE_CARD_REPAIR_INC)
            {//����ά����
              card_config = WRITE_INC_REPAIR;
            }
            else if(var_addr == PAGE_CARD_REPAIR_DEC)
            {//����ά����
              card_config = WRITE_DEC_REPAIR;
            }
            else if(var_addr == PAGE_CARD_QUIT)
            {//�˳�������ҳ��
              card_config = CARD_DISABLE;
            }
            /****************************************************************************/
            else if(var_addr == PAGE_PASSWORD_IMPORT)
            {//�޸ĵ�¼����
              u8 llen;
              u8 buf[20],i;
              llen = lcd_rev_buf[6] * 2;//���ڷ���Ϊ�ֳ�
              memset(input_password_buf,0,10);
              memcpy(input_password_buf,lcd_rev_buf + 7,llen);
              input_password_len = get_valid_length(input_password_buf,llen);
              if(input_password_len > 0)
              {
                input_password_buf[input_password_len] = '\0';
                for(i=0;i<input_password_len;i++)
                  buf[i] = '*';
                Sdwe_disString(PAGE_PASSWORD_DIS,buf,input_password_len);//������ʾΪ*
                memcpy(device_info.regin_in.password,input_password_buf,input_password_len);//��������
                SDWE_WARNNING(PAGE_PASSWORD_WARNNING,"�޸ĳɹ�");
                W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
              }
              else
              {
                SDWE_WARNNING(PAGE_PASSWORD_WARNNING,"���벻�ܿ�");
              }
            }
            else if(var_addr == PAGE_DEVICE_ID)
            {//�޸�ID
              u8 llen;
              llen = lcd_rev_buf[6] * 2;//���ڷ���Ϊ�ֳ�
              memset(input_password_buf,0,10);
              memcpy(input_password_buf,lcd_rev_buf + 7,llen);
              input_password_len = get_valid_length(input_password_buf,llen);
              if(input_password_len > 0)
              {
                input_password_buf[input_password_len] = '\0';
                memcpy(device_info.device_id,input_password_buf,input_password_len);
                device_info.device_id_len = input_password_len;
                SDWE_WARNNING(PAGE_DEVICE_WARNNING,"�޸ĳɹ�");
                W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
              }
              else
              {
                SDWE_WARNNING(PAGE_DEVICE_WARNNING,"���벻�ܿ�");
              }
            }
            else if(var_addr == PAGE_CONFIG_PASSWORD)
            {//����ϵͳ����ҳ���¼����
              u8 llen;
              u8 buf[20],i;
              llen = lcd_rev_buf[6] * 2;//���ڷ���Ϊ�ֳ�
              memset(input_password_buf,0,10);
              memcpy(input_password_buf,lcd_rev_buf + 7,llen);
              input_password_len = get_valid_length(input_password_buf,llen);
              input_password_buf[input_password_len] = '\0';
              for(i=0;i<input_password_len;i++)
                buf[i] = '*';
              Sdwe_disString(PAGE_CONFIG_DIS,buf,input_password_len);//������ʾΪ*
              
              if(strcmp((char const*)input_password_buf,(char const *)device_info.period_para.period_password) == 0)
              {//�����������������ҳ��
                Sdwe_disPicture(PAGE_PERIOD);
                Sdwe_period_page(&device_info);
              }
              else if(strcmp((char const*)input_password_buf,"138992") == 0)
              {//���������������ҳ��
                Sdwe_disPicture(PAGE_HIDDEN);
                Sdwe_hidden_page(&device_info);
              }
              else if(memcmp(&input_password_buf,&device_info.regin_in.password,device_info.regin_in.password_len) == 0)
              {//�����������ϵͳ����ҳ��
                Sdwe_disPicture(PAGE_CONFIG);
              }
              else
              {//�������
                SDWE_WARNNING(PAGE_CONFIG_WARNNING,"�������");
              }
            }
            /**************************ҳ��ʹ�ܿ���**************************************/
            else if(var_addr == PAGE_HIDDEN_JINGSHA)
            {
              value = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              device_info.func_onoff.jingsha = value;
              W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
            }
            else if(var_addr == PAGE_HIDDEN_CHANNENG)
            {
              value = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              device_info.func_onoff.channeng = value;
              W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
            }
            else if(var_addr == PAGE_HIDDEN_WEIMI)
            {
              value = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              device_info.func_onoff.weimi = value;
              W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
            }
            /***********************************************************************/
            /***********************************************************************/
            else if(var_addr == PAGE_PERIOD_ENABLE)
            {
              value = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              device_info.period_para.period_enable_onoff = value;
              W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
            }
            else if(var_addr == PAGE_PERIOD_YEAR)
            {
              value = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              device_info.period_para.period_year = value;
              W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
            }
            else if(var_addr == PAGE_PERIOD_MONTH)
            {
              value = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              if((value > 0) && (value <= 12))
              {
                device_info.period_para.period_month = value;
                W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
              }
              else
                SDWE_WARNNING(PAGE_PERIOD_WARNNING,"������Χ");
            }
            else if(var_addr == PAGE_PERIOD_DAY)
            {
              value = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              if((value > 0) && (value <= 31))
              {
                device_info.period_para.period_day = value;
                W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
              }
              else
                SDWE_WARNNING(PAGE_PERIOD_WARNNING,"������Χ");
            }
            else if(var_addr == PAGE_PERIOD_PASSWORD_IMPORT)
            {
              u8 llen;
              u8 buf[20],i;
              llen = lcd_rev_buf[6] * 2;//���ڷ���Ϊ�ֳ�
              memset(input_password_buf,0,10);
              memcpy(input_password_buf,lcd_rev_buf + 7,llen);
              input_password_len = get_valid_length(input_password_buf,llen);
              input_password_buf[input_password_len] = '\0';
              for(i=0;i<input_password_len;i++)
                buf[i] = '*';
              Sdwe_disString(PAGE_PERIOD_PASSWORD_DIS,buf,input_password_len);//������ʾΪ*
              memcpy(device_info.period_para.period_password,input_password_buf,input_password_len);
              device_info.period_para.period_password_len = input_password_len;
              W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
            }
            /***********************************************************************/
          }
          else
          {
            if((var_addr >= PAGE_WEIMI_TOTALWEI_1) && (var_addr < PAGE_WEIMI_TOTALWEI_1 + 20))
            {//γѭ������
              u32 cnt;
              cnt = (lcd_rev_buf[7] << 24) + (lcd_rev_buf[8] << 16) + (lcd_rev_buf[9] << 8) + lcd_rev_buf[10];
              weimi_para.total_wei_count[(var_addr - PAGE_WEIMI_TOTALWEI_1) / 2] = cnt;
            }
            else if((var_addr >= PAGE_WEIMI_WEI_CM_1) && (var_addr < PAGE_WEIMI_WEI_CM_1 + 20))
            {//γ/cm����
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              weimi_para.wei_cm_set[(var_addr - PAGE_WEIMI_WEI_CM_1) / 2] = (float)cnt / 10.0;
              weimi_para.wei_inch_set[(var_addr - PAGE_WEIMI_WEI_CM_1) / 2] = weimi_para.wei_cm_set[(var_addr - PAGE_WEIMI_WEI_CM_1) / 2] * 2.54;
              //ת��Ϊγ/inch��ʾ
              Sdwe_disDigi(PAGE_WEIMI_WEI_INCH_1 + var_addr - PAGE_WEIMI_WEI_CM_1,(int)(weimi_para.wei_inch_set[(var_addr - PAGE_WEIMI_WEI_CM_1) / 2] * 10),2);
                
              if((var_addr - PAGE_WEIMI_WEI_CM_1) == 0)
              {//��һ�ε�γ�ܺ�����ҳ���γ����ͬ
                peiliao_para.weimi_set = (float)cnt / 10.0;
                W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
              }
            }
            else if((var_addr >= PAGE_WEIMI_MEDIANWEI_1) && (var_addr < PAGE_WEIMI_MEDIANWEI_1 + 20))
            {//γ��������
              u32 cnt;
              cnt = (lcd_rev_buf[7] << 24) + (lcd_rev_buf[8] << 16) + (lcd_rev_buf[9] << 8) + lcd_rev_buf[10];
              weimi_para.median_wei_count[(var_addr - PAGE_WEIMI_MEDIANWEI_1) / 2] = cnt;
            }
            else if((var_addr >= PAGE_WEIMI_STEP1_SPEED) && (var_addr < PAGE_WEIMI_MEDIANWEI_1 + 20))
            {//��γ����ٶ�����
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              weimi_para.step1_speed[(var_addr - PAGE_WEIMI_STEP1_SPEED) / 2] = cnt;
            }
            else if((var_addr >= PAGE_WEIMI_STEP2_SPEED) && (var_addr < PAGE_WEIMI_STEP2_SPEED + 20))
            {//���ߵ���ٶ�����
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              weimi_para.step1_speed[(var_addr - PAGE_WEIMI_STEP2_SPEED) / 2] = cnt;
            }
            else if((var_addr >= PAGE_WEIMI_STEP1_ADD1) && (var_addr < PAGE_WEIMI_STEP1_ADD1 + 10))
            {//��γ�ٶȼ�1����10
              if(weimi_para.step1_speed[var_addr - PAGE_WEIMI_STEP1_SPEED] < 9999)
              {//��γ����ٶ�С�������ٶȲ��ܼ�
                weimi_para.step1_speed[var_addr - PAGE_WEIMI_STEP1_SPEED] += 1;
                Sdwe_disDigi(var_addr - PAGE_WEIMI_STEP1_SPEED,weimi_para.step1_speed[var_addr - PAGE_WEIMI_STEP1_SPEED],2);
              }
            }
            else if((var_addr >= PAGE_WEIMI_STEP1_SUB1) && (var_addr < PAGE_WEIMI_STEP1_SUB1 + 10))
            {//��γ�ٶȼ�1����10
              if(weimi_para.step1_speed[var_addr - PAGE_WEIMI_STEP1_SPEED] > 1)
              {
                weimi_para.step1_speed[var_addr - PAGE_WEIMI_STEP1_SPEED] -= 1;
                Sdwe_disDigi(var_addr - PAGE_WEIMI_STEP1_SPEED,weimi_para.step1_speed[var_addr - PAGE_WEIMI_STEP1_SPEED],2);
              }
            }
            else if((var_addr >= PAGE_WEIMI_STEP2_ADD1) && (var_addr < PAGE_WEIMI_STEP2_ADD1 + 10))
            {//�����ٶȼ�1����10
              if(weimi_para.step2_speed[var_addr - PAGE_WEIMI_STEP2_SPEED] < 9999)
              {//��γ����ٶ�С�������ٶȲ��ܼ�
                weimi_para.step2_speed[var_addr - PAGE_WEIMI_STEP2_SPEED] += 1;
                Sdwe_disDigi(var_addr - PAGE_WEIMI_STEP2_SPEED,weimi_para.step2_speed[var_addr - PAGE_WEIMI_STEP2_SPEED],2);
              }
            }
            else if((var_addr >= PAGE_WEIMI_STEP2_SUB1) && (var_addr < PAGE_WEIMI_STEP2_SUB1 + 10))
            {//�����ٶȼ�1����10
              if(weimi_para.step2_speed[var_addr - PAGE_WEIMI_STEP2_SPEED] > 1)
              {
                weimi_para.step2_speed[var_addr - PAGE_WEIMI_STEP2_SPEED] -= 1;
                Sdwe_disDigi(var_addr - PAGE_WEIMI_STEP2_SPEED,weimi_para.step2_speed[var_addr - PAGE_WEIMI_STEP2_SPEED],2);
              }
            }
            else if(var_addr == PAGE_WEIMI_WEIMI1)
            {
              sdew_weimi_page1(&weimi_para);
            }
            else if(var_addr == PAGE_WEIMI_WEIMI2)
            {
              sdew_weimi_page2(&weimi_para);
            }
            else if(var_addr == PAGE_WEIMI_WEISHA1)
            {
              sdew_weisha_page1(&weimi_para);
            }
            else if(var_addr == PAGE_WEIMI_WEISHA2)
            {
              sdew_weisha_page2(&weimi_para);
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
        W25QXX_Read((u8 *)&read_info,(u32)(W25QXX_ADDR_JINGSHA + FILE_SIZE * download_num),sizeof(read_info));//����num������
        u8 name_1[10],name_2[20];
        u8 id[10];
        memset(id,0,10);
        memset(name_1,0,10);
        memset(name_2,0,20);
        memcpy(name_1,read_info.filename,read_info.filename_len);
        memcpy(id,device_info.device_id,device_info.device_id_len);
        sprintf((char *)name_2,"2:/%s-%s.CSV",id,name_1);
  //      result = f_open(&file,"2:/22.CSV",FA_CREATE_ALWAYS | FA_WRITE);
        result = f_open(&file,(char const*)name_2,FA_CREATE_ALWAYS | FA_WRITE);
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
          SDWE_WARNNING(PAGE_HISTORY_TEXT_FILE_WARN,"д��ɹ�");
          printf("%s.CSV�ļ�д��ɹ�\r\n",name_1);
        }
        else
        {
          SDWE_WARNNING(PAGE_HISTORY_TEXT_FILE_WARN,"д��ʧ��");
          printf("%s.CSV�ļ�д��ʧ��\r\n",name_1);
        }
        /* �ر��ļ�*/
        f_close(&file);
        
        /* ж���ļ�ϵͳ */
        f_mount(NULL, "2:", 0);
      }
      else
      {
        SDWE_WARNNING(PAGE_HISTORY_TEXT_FILE_WARN,"��U��");
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
  u32 card_id;
  u8 card_type = FUNC_IDLE;
  while(1)
  {
    if(card_config == CARD_DISABLE)
    {
      vTaskDelay(100);
    }
    else
    {
      if(card_config == READ_PERMISSION)
      {//�ڲ���ҳ��������ȡ��
        rfid_rev_flag = 0;
        rc522_cmd_request(REQUEST_TYPE_ALL);
        vTaskDelay(100);
        if(rfid_rev_flag)
        {
          rfid_rev_flag = 0;
          if(rfid_rev_cnt == 8)
          {
            isCard = (rfid_rev_buf[5] << 8) + rfid_rev_buf[4];
            if(isCard == Mifare1_S50)
            {
              printf("s50\r\n");
              rc522_cmd_anticoll(COLLISION_GRADE_1);
              vTaskDelay(100);
              if(rfid_rev_flag)
              {
                rfid_rev_flag = 0;
                if(rfid_rev_cnt == 10)
                {
                  card_id = (rfid_rev_buf[4] << 24) + (rfid_rev_buf[5] << 16) + (rfid_rev_buf[6] << 8) + rfid_rev_buf[7];
                  printf("%x ",rfid_rev_buf[4]);
                  printf("%x ",rfid_rev_buf[5]);
                  printf("%x ",rfid_rev_buf[6]);
                  printf("%x ",rfid_rev_buf[7]);
                  //��ȡ�����ź��жϿ����Ƿ�ΪA�࿨��B�࿨��ά������δע�Ῠ
                  card_type = get_card_type(card_id);
                  if(card_type == FUNC_REPAIR)
                  {//ά����
                    Sdwe_disPicture(PAGE_REPAIR);//ά��������ά��ҳ��
                    Sdwe_stop_page(&device_info);
                  }
                  else if(card_type == FUNC_CLASS_A)
                  {//A�࿨
                    if(device_info.class_para.class_enable_onoff == 0)
                    {//������๦��δ���ã������һֱ����A��
                      card_record = 1;//A��
                      SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"δ��������");
                    }
                    else
                    {
                      if(get_class_time(&rtc_time,&device_info) == CLASS_A)
                      {//A�࿨����Aʱ���
                        card_record = 1;//A��
                        if(old_card_record != card_record)
                        {
                          old_card_record = card_record;
                          SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"A�࿨");
                        }
                        else
                        {
                          SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"�ظ���");
                        }
                      }
                      else
                      {//A�࿨����Bʱ���
                        SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"��Ч����");
                      }
                    }
                  }
                  else if(card_type == FUNC_CLASS_B)
                  {//B�࿨
                    if(device_info.class_para.class_enable_onoff == 0)
                    {//������๦��δ���ã������һֱ����A��
                      card_record = 1;//A��
                      SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"δ��������");
                    }
                    else
                    {
                      if(get_class_time(&rtc_time,&device_info) == CLASS_B)
                      {//B�࿨����Bʱ���
                        card_record = 2;//B��
                        if(old_card_record != card_record)
                        {
                          old_card_record = card_record;
                          SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"B�࿨");
                        }
                        else
                        {
                          SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"�ظ���");
                        }
                      }
                      else
                      {//A�࿨����Bʱ���
                        SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"��Ч����");
                      }
                    }
                    
                  }
                  else
                  {//δע�Ῠ
                    SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"δע�Ῠ");
                  }
                }
              }
            }
          }
        }
      }
      else
      {//��Ա����ҳ�浥�ζ�ȡ��
        if(card_config > WRITE_PERMISSION)
        {
          rfid_rev_flag = 0;
          rc522_cmd_request(REQUEST_TYPE_ALL);//����
          vTaskDelay(100);
          if(rfid_rev_flag)
          {//�յ�ģ�鷵������
            rfid_rev_flag = 0;
            if(rfid_rev_cnt == 8)
            {//
              isCard = (rfid_rev_buf[5] << 8) + rfid_rev_buf[4];
              if(isCard == Mifare1_S50)
              {//ʶ��s50��
                printf("s50\r\n");
                rc522_cmd_anticoll(COLLISION_GRADE_1);//����ײ��ȡ��ƬID
                vTaskDelay(100);
                if(rfid_rev_flag)
                {
                  rfid_rev_flag = 0;
                  if(rfid_rev_cnt == 10)
                  {
                    card_id = (rfid_rev_buf[4] << 24) + (rfid_rev_buf[5] << 16) + (rfid_rev_buf[6] << 8) + rfid_rev_buf[7];
                    if(card_id != 0)
                    {
                      card_type = get_card_type(card_id);
                      if(card_config == WRITE_INC_A)
                      {
                        if(card_type == FUNC_IDLE)
                        {//��Ƭ�����ڣ���ӵ����ݿ�
                          inc_card_type(card_id,FUNC_CLASS_A);
                          SDWE_WARNNING(PAGE_CARD_WARNNING,"¼��ɹ�");
                        }
                        else
                        {
                          SDWE_WARNNING(PAGE_CARD_WARNNING,"��Ƭ�Ѵ���");
                        }
                      }
                      else if(card_config == WRITE_DEC_A)
                      {
                        if(card_type == FUNC_CLASS_A)
                        {//��Ƭ���ڣ�ɾ��
                          
                        }
                      }
                      else if(card_config == WRITE_INC_B)
                      {
                        if(card_type == FUNC_IDLE)
                        {//��Ƭ�����ڣ���ӵ����ݿ�
                          inc_card_type(card_id,FUNC_CLASS_B);
                          SDWE_WARNNING(PAGE_CARD_WARNNING,"¼��ɹ�");
                        }
                        else
                        {
                          SDWE_WARNNING(PAGE_CARD_WARNNING,"��Ƭ�Ѵ���");
                        }
                      }
                      else if(card_config == WRITE_DEC_B)
                      {
                        
                      }
                      else if(card_config == WRITE_INC_REPAIR)
                      {
                        if(card_type == FUNC_IDLE)
                        {//��Ƭ�����ڣ���ӵ����ݿ�
                          inc_card_type(card_id,FUNC_REPAIR);
                          SDWE_WARNNING(PAGE_CARD_WARNNING,"¼��ɹ�");
                        }
                        else
                        {
                          SDWE_WARNNING(PAGE_CARD_WARNNING,"��Ƭ�Ѵ���");
                        }
                      }
                      else if(card_config == WRITE_DEC_REPAIR)
                      {
                        
                      }
                    }
                  }
                }
              }
            }
          }
          card_config = WRITE_PERMISSION;
        }
        vTaskDelay(100);
      }
    }
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
              Sdwe_disDigi(PAGE1_SAMPLE_VALUE1 + readSlave.addr - 1,SlavePara.value_sample[index] / 10,2);
              Sdwe_writeIcon(PAGE1_SLAVE_STATE1 + readSlave.addr - 1,VGUS_OFF);//��Ӧλ����ȷ����ͼ��
              Sdwe_disDigi(PAGE1_ECHO_WEIGHT + readSlave.addr - 1,readSlave.set_value / 10,2);
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
              Sdwe_disDigi(PAGE1_ECHO_WEIGHT + index,0,2);
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
    if((device_info.func_onoff.jingsha == 1) && (isDevicePeriod == 0))
    {
      isWork = GetDeviceState();
    }
    else
    {//���ܹ���δ�򿪣�������
      isWork = 0;
    }
    vTaskDelay(50);
    if(isWork != old_isWork)
    {
      old_isWork = isWork;
      if(isWork)
      {
        sample_time = 0;
      }
      ptMsg->addr = BROADCAST;
      ptMsg->func = FUNC_WRITE;
      ptMsg->reg = REG_START;
      ptMsg->value = isWork;
      if(xQueueSend(xQueue_message,                  /* ��Ϣ���о�� */
                    (void *) &ptMsg,           /* ���ͽṹ��ָ�����ptMsg�ĵ�ַ */
                    (TickType_t)10) == pdPASS )
      {
//        printf("isWork is %d ���з��ͳɹ�\r\n",isWork);
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
                  Sdwe_disString(PAGE_U_TEXT_FILENAME1 + Disk_File.filenum * 10,(u8 *)FileInf.fname,10);
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
        SDWE_WARNNING(PAGE_HISTORY_TEXT_FILE_WARN,"��U��");
      }
    }
    Task_iwdg_refresh(TASK_ReadDisk);
  }
}

void vTaskManageCapacity(void *pvParameters)
{
  u16 count = 0;
  BaseType_t xResult;
  float p_value = 0.0,old_p_value = 0.0;
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(200); /* �������ȴ�ʱ��Ϊ200ms */
  bsp_InitStepMotor();
  total_meter_gross = (u32)(peiliao_para.total_meter_set * (1 + peiliao_para.loss / 100.0));
  total_weight_gross = (u32)(peiliao_para.total_weitht_set * (1 + peiliao_para.loss / 100.0));
  if(product_para.product_complete >= total_meter_gross)
  {
    plan_complete = 1;
  }
  else if(product_para.weight_complete >= total_weight_gross)
  {
    plan_complete = 1;
  }
  while(1)
  {
    xResult = xSemaphoreTake(xSemaphore_pluse, (TickType_t)xMaxBlockTime);
    if(xResult == pdTRUE)
    {
      if((device_info.func_onoff.channeng == 1) && (isDevicePeriod == 0))
      {//����ʹ�ܡ�����������
        work_idle_time = 0;
        if(device_info.system_state == SYS_NORMAL)
        {//ϵͳ����ʱ���в��ܼ���
          switch(plan_complete)
          {
            case 0:
              RELAY_OPEN();//�������źź󣬼̵����Ͽ�
              pluse_count++;
              count++;
              product_para.weicount_kilowei = product_para.weicount_kilowei + count / peiliao_para.weimi_dis_set;
              if((count % (peiliao_para.weimi_dis_set)) == 0)
              {//γɴ/ǧγ
                count = 0;
                Sdwe_disDigi(PAGE_PRODUCT_KILOCOUNT,product_para.weicount_kilowei,4);
                W25QXX_Write((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
              }
              if(card_record == 2)
              {//B��
                product_para.product_b = product_para.product_b + product_per_meter(&peiliao_para,1);//b�����
                p_value = get_float_1bit(product_para.product_b);
              }
              else
              {//A��
                product_para.product_a = product_para.product_a + product_per_meter(&peiliao_para,1);//b�����
                p_value = get_float_1bit(product_para.product_a);
              }
              if(p_value != old_p_value)
              {//�������ݱ仯����=0.1ʱ����ʾ����
                old_p_value = p_value;
                Sdwe_disDigi(PAGE_PRODUCT_B,(int)(product_para.product_b * 10),4);
                Sdwe_disDigi(PAGE_PRODUCT_A,(int)(product_para.product_a * 10),4);
                product_para.product_complete = product_complete_meter(&product_para);//����ɲ���
                product_para.product_uncomplete = total_meter_gross - product_para.product_complete;//δ��ɲ���
                product_para.weight_complete = product_complete_kilo(&product_para,&peiliao_para);//���������
                product_para.weight_uncomplete = total_weight_gross - product_para.weight_complete;//δ�������
                Sdwe_disDigi(PAGE_PRODUCT_COMPLETE,(int)(product_para.product_complete * 10),4);
                Sdwe_disDigi(PAGE_PRODUCT_UNCOMPLETE,(int)(product_para.product_uncomplete * 10),4);
                Sdwe_disDigi(PAGE_PRODUCT_COMPLETE_W,(int)(product_para.weight_complete * 100),4);
                Sdwe_disDigi(PAGE_PRODUCT_UNCOMPLETE_W,(int)(product_para.weight_uncomplete * 100),4);
                if(product_para.product_uncomplete <= 0)
                {//��ɲ���
                  TIM_CtrlPWMOutputs(TIM1, DISABLE);
                  TIM_Cmd(TIM1, DISABLE);
                  RELAY_CLOSE();//������ɺ󣬼̵����պ�
                  if(old_plan_complete == 0)
                  {
                    plan_complete = 1;
                  }
                  else
                  {
                    plan_complete = 2;
                  }
                  Sdwe_disString(PAGE_PRODUCT_RFID_WARNNING,"�������",strlen("�������"));
                }
                else if(product_para.weight_uncomplete <= 0)
                {//�������
                  TIM_CtrlPWMOutputs(TIM1, DISABLE);
                  TIM_Cmd(TIM1, DISABLE);
                  if(old_plan_complete == 0)
                  {
                    plan_complete = 1;
                  }
                  else
                  {
                    plan_complete = 2;
                  }
                  RELAY_CLOSE();//������ɺ󣬼̵����պ�
                  Sdwe_disString(PAGE_PRODUCT_RFID_WARNNING,"�������",strlen("�������"));
                }
              }
              break;
            case 1:
              RELAY_CLOSE();//������ɺ󣬼̵����պ�
              if(peiliao_para.add_meter_set != 0)
              {
                //�������ܲ������������Ӳ�������,���������������
                old_plan_complete = 1;
                total_meter_gross = (u32)(total_meter_gross + peiliao_para.add_meter_set * (1 + peiliao_para.loss / 100.0));
                plan_complete = 0;
              }
              break;
            case 2:
              RELAY_CLOSE();//������ɺ󣬼̵����պ�
              break;
          }
        }
        else
        {//ϵͳͣ��
          RELAY_CLOSE();
        }
      }
      else
      {
        RELAY_CLOSE();
      }
    }
  }
}

static void vTaskMotorControl(void *pvParameters)
{
  u16 servomotor = 84;
//  u16 feedback,i;
  TIM4ConfigPwmOut(servomotor,10);
  
  while(1)
  {
//    feedback = DMA_send_feedback();
//    if(feedback != 0)
//    {
//      printf("-> ");
//      printf("%d\r\n",DMA_send_feedback());
//    }
    if(pwm_flag == 1)
    {
      pwm_flag = 0;
      TIM4_PWMDMA_Config(servomotor,1000);
      TIM4StartPwmOut();
    }
    vTaskDelay(10);
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
              256,               	/* ����ջ��С����λword��Ҳ����4�ֽ� */
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
  xTaskCreate( vTaskMotorControl,    		/* ������  */
              "vTaskMotorControl",  		/* ������    */
              128,         		/* ����ջ��С����λword��Ҳ����4�ֽ� */
              NULL,        		/* �������  */
              8,           		/* �������ȼ�*/
              &xHandleTaskMotorControl); /* ������  */
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
                             1000,    /* ��ʱ������,��λʱ�ӽ��� */
                             pdTRUE,          /* ������ */
                             (void *)0,      /* ��ʱ��ID */
                             UserTimerCallback); /* ��ʱ���ص����� */
    
  if(xTimerUser != NULL)
  {
    if(xTimerStart(xTimerUser,1000) != pdPASS)
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
  Sdwe_clearString(PAGE_PRODUCT_RFID_WARNNING);
  Sdwe_clearString(PAGE_STOP_WARNNING);
  Sdwe_clearString(PAGE_CARD_WARNNING);
  Sdwe_clearString(PAGE_DEVICE_WARNNING);
}

void UserTimerCallback(TimerHandle_t xTimer)
{//��ʱʱ��1s
  static u16 speed_1 = 0,speed_2 = 0;
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
  Sdwe_readRTC();//ÿ���ȡһ��ʱ��
  if(sample_time == 0)
  {
    speed_1 = pluse_count;
    sample_time++;
  }
  else if(sample_time >= 2)
  {//����10s�ڵ�������
    sample_time = 0;
    speed_2 = pluse_count;
    pluse_count = 0;
    product_para.speed = (speed_2 - speed_1) * 30;//2���ڵ�������*30ת��Ϊ1����������
    Sdwe_disDigi(PAGE_PRODUCT_SPEED,product_para.speed,2);//��ʾ�ٶ�
  }
  else
  {
    sample_time++;
  }
  if(work_idle_time < 1000)
    work_idle_time++;
  if(work_idle_time < 10)
  {//10s����������Ϊ�ǹ���ʱ��
    work_idle = PLUSE_WORK;
    device_info.system_state = SYS_NORMAL;
  }
  else if(work_idle_time < 300)
  {//10~300s��������Ϊ�ǿ���ʱ�䣬�����빤��ʱ��
    work_idle = PLUSE_IDLE;
  }
  else
  {//����5������������Ϊͣ��
    work_idle = PLUSE_STOP;
    device_info.system_state = SYS_STOP;
  }
  if(work_idle != PLUSE_IDLE)
  {
    if(device_info.system_state == SYS_NORMAL)
    {
      product_para.total_work_time++;
      if((product_para.total_work_time % 60) == 0)
      {
        u8 on_time_buf[10];
        memset(on_time_buf,0,10);
        sprintf((char *)on_time_buf,"%04d:%02d",product_para.total_work_time / 3600,product_para.total_work_time % 3600 / 60);
        Sdwe_disString(PAGE_PRODUCT_TIME_ON,on_time_buf,strlen((char const*)on_time_buf));
      }
    }
    else
    {
      product_para.total_stop_time++;//ͣ����ʱ��
      device_info.stop_para.stop_time[device_info.system_state - 1]++;
      if((product_para.total_stop_time % 60) == 0)
      {
        u8 off_time_buf[10];
        memset(off_time_buf,0,10);
        sprintf((char *)off_time_buf,"%04d:%02d",product_para.total_stop_time / 3600,product_para.total_stop_time % 3600 / 60);
        Sdwe_disString(PAGE_PRODUCT_TIME_OFF,off_time_buf,strlen((char const*)off_time_buf));
      }
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
