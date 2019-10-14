#include "includes.h"

TaskHandle_t xHandleTaskLCD = NULL;
TaskHandle_t xHandleTaskADC = NULL;
TaskHandle_t xHandleTaskLED = NULL;
TaskHandle_t xHandleTaskRFID = NULL;
TaskHandle_t xHandleTaskMassStorage = NULL;
TaskHandle_t xHandleTaskReadDisk = NULL;
TaskHandle_t xHandleTaskMsgPro = NULL;
TaskHandle_t xHandleTaskRev485 = NULL;
TaskHandle_t xHandleTaskManageCapacity = NULL;
TaskHandle_t xHandleTaskMotorControl = NULL;
TaskHandle_t xHandleTaskFreq = NULL;
TaskHandle_t xHandleTaskKey = NULL;

SemaphoreHandle_t  xSemaphore_lcd = NULL;
SemaphoreHandle_t  xSemaphore_rs485 = NULL;
SemaphoreHandle_t  xSemaphore_pluse = NULL;
SemaphoreHandle_t  xSemaphore_encoder = NULL;
SemaphoreHandle_t  xSemaphore_download = NULL;
SemaphoreHandle_t  xSemaphore_readDisk = NULL;
SemaphoreHandle_t  xSemaphore_debug = NULL;
EventGroupHandle_t idwgEventGroup = NULL;
TimerHandle_t xTimerUser = NULL;

xQueueHandle xQueue_MQTT_Recv;  /**< MQTT接收消息队列 */
xQueueHandle xQueue_MQTT_Transmit;
xQueueHandle xQueue_esp8266_Cmd;

SLAVE slave_info;
SLAVE readSlave;
QueueHandle_t xQueue_message = NULL;
u8 download_num = 0;
u8 working_stage = WORK_STAGE_IDLE;
u8 isWork = 0,old_isWork = 0;
u8 usb_disk_flag = 0;
u8 old_usb_disk_flag = 0;
u16 sample_time = 0;
u16 pluse_count = 0;

u8 card_record = 0,old_card_record = 0;

u8 card_config = CARD_DISABLE;
u8 isDevicePeriod = 0;
extern void DemoFatFS(void);
extern const char * FR_Table[];
extern u8 modbus_send_frame(m_frame_typedef * fx,SLAVE info);

u8 pwm_flag = 0;

u8 readDisk = DISK_IDLE;        //��ȡU�̹��ܣ�����/���ļ���/���ļ�
u8 readFilenum = 0;

u8 old_system_state = 0xff;
float pluse_step_src[3] = {0,0,0};
float speed_diff[3] = {0,0,0};
u8 symbol[3] = {0,0,0};

float servo_per_wei_src;//�ŷ���������ڲ�����ÿγ
float servo_diff;//�ŷ����������γ��
float servo_speed_diff;
u8 symbol_servo = 0;

u8 clear_text_flag = 0;

u16 device_speed_volate = 0;
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
  Sdwe_writeIcon(PAGE_MAIN_DACOUT_ADD,device_speed_volate);
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
              isDevicePeriod = get_period_state(&rtc_time,&device_info); 
            else
              isDevicePeriod = 0;
          }
        }
        else if(lcd_rev_buf[3] == 0x83)
        {//�������洢������
          var_addr = (lcd_rev_buf[4] << 8) + lcd_rev_buf[5];
          if(var_addr < 0x1000)
          {//��ַ�ֳ����Σ���߼���Ч��
            if(var_addr == MAIN_PAGE_KEY_JINGSHA)
            {//��ɴ����
              if(isDevicePeriod == 0)
              {
                if(device_info.func_onoff.jingsha)
                {
                  Sdwe_disPicture(PAGE_1);
                  Init_JINGSHA_GUI();
                }
                else
                  SDWE_WARNNING(MAIN_PAGE_WARNNING,"����ϵ����",1);
              }
              else
                SDWE_WARNNING(MAIN_PAGE_WARNNING,"�����ڽ���",1);
            }
            else if(var_addr == MAIN_PAGE_KEY_WEIMI)
            {//γ��
//              pwm_flag = 1;
              if(isDevicePeriod == 0)
              {
                if(device_info.func_onoff.weimi)
                {
                  Sdwe_disPicture(PAGE_WEIMI);
                  sdew_weimi_page1(&weimi_para);
                }
                else
                  SDWE_WARNNING(MAIN_PAGE_WARNNING,"����ϵ����",1);
              }
              else
                SDWE_WARNNING(MAIN_PAGE_WARNNING,"�����ڽ���",1);
            }
            else if(var_addr == MAIN_PAGE_KEY_CHANNENG)
            {//����
              if(isDevicePeriod == 0)
              {
                if(device_info.func_onoff.channeng)
                {
                  Sdwe_disPicture(PAGE_CHANNENG);
                  Sdwe_product_page(&product_para);
                  card_config = READ_PERMISSION;
//                  card_config = CARD_DISABLE;
                }
                else
                  SDWE_WARNNING(MAIN_PAGE_WARNNING,"����ϵ����",1);
              }
              else
                SDWE_WARNNING(MAIN_PAGE_WARNNING,"�����ڽ���",1);
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
                Sdwe_writeIcon(i + PAGE_HISTORY_SELECT,file_select[i]);
              }
              if(usb_disk_flag == 0)
                Sdwe_writeIcon(PAGE_HISTORY_ICON_U_STATE,VGUS_OFF);  
              else
                Sdwe_writeIcon(PAGE_HISTORY_ICON_U_STATE,VGUS_ON);
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
            }
            else if(var_addr == PAGE2_KEY_SAVE)
            {//�ڶ�ҳ����
              memset(input_password_buf,0,10);
              Sdwe_clearString(PAGE2_FILE_TEXT_DIS);
              Sdwe_clearString(PAGE2_FILE_TEXT_WARN);
            }
            else if(var_addr == PAGE3_KEY_SAVE)
            {//����ҳ����
              memset(input_password_buf,0,10);
              Sdwe_clearString(PAGE3_FILE_TEXT_DIS);
              Sdwe_clearString(PAGE3_FILE_TEXT_WARN);
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
              JingSha_File.weight_value[var_addr - 0x0320] = value;
              u8 no = 0;
              no = isFileSelect();
              if(no == 1)
              {
                W25QXX_Write((u8 *)&JingSha_File,(u32)(W25QXX_ADDR_JINGSHA + JINGSHA_SIZE * device_info.page_count_select),sizeof(JingSha_File));
              }
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
            else if((var_addr >= PAGE1_SET_COMPARE_VALUE) && (var_addr <= (PAGE1_SET_COMPARE_VALUE + 30)))
            {//�Ƚ�ֵ�޸�
              value = ((lcd_rev_buf[7] << 8) + lcd_rev_buf[8]) * 1000 / 100;//��������Ϊ��λ��С������λkg��ת��Ϊg
              SlavePara.value_compare[var_addr - PAGE1_SET_COMPARE_VALUE] = value;
              JingSha_File.weight_compare[var_addr - PAGE1_SET_COMPARE_VALUE] = value;
              u8 no = 0;
              no = isFileSelect();
              if(no == 1)
              {
                W25QXX_Write((u8 *)&JingSha_File,(u32)(W25QXX_ADDR_JINGSHA + JINGSHA_SIZE * device_info.page_count_select),sizeof(JingSha_File));
              }
            }
            else if((var_addr >= PAGE1_SENSE_PRECISION) && (var_addr <= (PAGE1_SENSE_PRECISION + 30)))
            {//��������Χ����
              value = ((lcd_rev_buf[7] << 8) + lcd_rev_buf[8]) * 1000 / 100;//��������Ϊ��λ��С������λkg��ת��Ϊg
              SlavePara.value_precision[var_addr - PAGE1_SENSE_PRECISION] = value;
              JingSha_File.weight_precision[var_addr - PAGE1_SENSE_PRECISION] = value;
              u8 no = 0;
              no = isFileSelect();
              if(no == 1)
              {
                W25QXX_Write((u8 *)&JingSha_File,(u32)(W25QXX_ADDR_JINGSHA + JINGSHA_SIZE * device_info.page_count_select),sizeof(JingSha_File));
              }
              ptMsg->addr = var_addr - PAGE1_SENSE_PRECISION;
              ptMsg->func = FUNC_WRITE;
              ptMsg->reg = REG_PRECISION;
              ptMsg->value = value;
              if(xQueueSend(xQueue_message,                  /* ��Ϣ���о�� */
                            (void *) &ptMsg,           /* ���ͽṹ��ָ�����ptMsg�ĵ�ַ */
                            (TickType_t)10) == pdPASS )
              {
                /* ����ʧ�ܣ���ʹ�ȴ���10��ʱ�ӽ��� */
//                printf("%d# sw %d ���з��ͳɹ�\r\n",var_addr - PAGE1_SLAVE_ONOFF1 + 1,value);
              }
            }
            else if((var_addr >= PAGE1_CLEAR_SENSE) && (var_addr <= (PAGE1_CLEAR_SENSE + 30)))
            {//��������������
              ptMsg->addr = var_addr - PAGE1_CLEAR_SENSE;
              ptMsg->func = FUNC_WRITE;
              ptMsg->reg = REG_CLEAR;
              ptMsg->value = 1;
              if(xQueueSend(xQueue_message,                  /* ��Ϣ���о�� */
                            (void *) &ptMsg,           /* ���ͽṹ��ָ�����ptMsg�ĵ�ַ */
                            (TickType_t)10) == pdPASS )
              {
                /* ����ʧ�ܣ���ʹ�ȴ���10��ʱ�ӽ��� */
//                printf("%d# sw %d ���з��ͳɹ�\r\n",var_addr - PAGE1_SLAVE_ONOFF1 + 1,value);
              }
            }
            else if(var_addr == PAGE1_SENSE_ALL_CLEAR)
            {//���д���������
              ptMsg->addr = BROADCAST;
              ptMsg->func = FUNC_WRITE;
              ptMsg->reg = REG_CLEAR;
              ptMsg->value = 1;
              if(xQueueSend(xQueue_message,                  /* ��Ϣ���о�� */
                            (void *) &ptMsg,           /* ���ͽṹ��ָ�����ptMsg�ĵ�ַ */
                            (TickType_t)10) == pdPASS )
              {
                /* ����ʧ�ܣ���ʹ�ȴ���10��ʱ�ӽ��� */
//                printf("%d# sw %d ���з��ͳɹ�\r\n",var_addr - PAGE1_SLAVE_ONOFF1 + 1,value);
              }
            }
            else if(var_addr == PAGE1_SENSE_PASSWORD)
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
              Sdwe_disString(PAGE1_SENSE_DIS,buf,input_password_len);//������ʾΪ*
              if(strcmp((char const*)input_password_buf,"899170") == 0)
//              if(memcmp(&input_password_buf,&device_info.regin_in.password,device_info.regin_in.password_len) == 0)
              {//������ȷ
                Sdwe_disPicture(PAGE_SENSE_CLEAR);
                vTaskDelay(10);
                init_sense_clear();
              }
              else
              {//�������
                Sdwe_disString(PAGE1_SENSE_WARNNING,"�������",strlen("�������"));
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
              if(strcmp((char const*)input_password_buf,"111111") == 0)
//              if(memcmp(&input_password_buf,&device_info.regin_in.password,device_info.regin_in.password_len) == 0)
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
              
              if(strcmp((char const*)input_password_buf,"111111") == 0)
//              if(memcmp(&input_password_buf,&device_info.regin_in.password,device_info.regin_in.password_len) == 0)
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
              
              if(strcmp((char const*)input_password_buf,"111111") == 0)
//              if(memcmp(&input_password_buf,&device_info.regin_in.password,device_info.regin_in.password_len) == 0)
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
            else if((var_addr == PAGE1_FILE_TEXT_IMPORT) || (var_addr == PAGE2_FILE_TEXT_IMPORT) || (var_addr == PAGE3_FILE_TEXT_IMPORT) || (var_addr == PAGE_HISTORY2_IMPORT))
            {//�ļ���¼�룬ȥ��ȷ�ϰ�ť����OK��Ĭ��ȷ��
              u8 llen;
              llen = lcd_rev_buf[6] * 2;
              memcpy(input_password_buf,lcd_rev_buf + 7,llen);
              input_password_len = get_valid_length(input_password_buf,llen);
              input_password_buf[input_password_len] = '\0';
              
              u8 i;
              JINGSHA_FILE file_write;
              memset(file_write.filename,0,10);
              if(input_password_len > 0)
              {
                u8 repeat = 0;
                for(i=0;i<device_info.page_count_all;i++)
                {
                  u8 name_buf[10];
                  W25QXX_Read(name_buf,(u32)W25QXX_ADDR_JINGSHA + JINGSHA_SIZE * i + 1,10);
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
                    W25QXX_Read(file_read,(u32)W25QXX_ADDR_JINGSHA + JINGSHA_SIZE,JINGSHA_SIZE * 9);//����2-10�ļ�������
                    W25QXX_Write(file_read,(u32)W25QXX_ADDR_JINGSHA,JINGSHA_SIZE * 9);//��д��1-9�ļ���ַ
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
                    W25QXX_Write((u8 *)&file_write,(u32)(W25QXX_ADDR_JINGSHA + JINGSHA_SIZE * 9),sizeof(file_write));//д��ǰҳ�����ݵ��ļ�10��ַ
                    
                    //�������ݱ���
                    peiliao_para.complete_meter = (u32)product_para.product_complete;
                    peiliao_para.complete_work_time = product_para.total_work_time;
                    W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO + CHANNENG_SIZE * 9,sizeof(peiliao_para));
                    
                    //γ�����ݱ���
                    W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI + WEIMI_SIZE * 9,sizeof(weimi_para));
                    if(var_addr == PAGE1_FILE_TEXT_IMPORT)//���µ�һҳ���水ť
                      Sdwe_disPicture(PAGE_1);//��ת��ҳ��1
                    else if(var_addr == PAGE2_FILE_TEXT_IMPORT)//���µڶ�ҳ���水ť
                      Sdwe_disPicture(PAGE_2);//��ת��ҳ��2
                    else if(var_addr == PAGE3_FILE_TEXT_IMPORT)//���µ���ҳ���水ť
                      Sdwe_disPicture(PAGE_3);//��ת��ҳ��3
                    else if(var_addr == PAGE_HISTORY2_IMPORT)//���µ���ҳ���水ť
                    {
                      for(i=0;i<10;i++)
                      {
                        Sdwe_writeIcon(i + PAGE_HISTORY_SELECT,file_select[i]);
                      }
                      Sdwe_disPicture(PAGE_HISTORY);//��ת��ҳ��3
                      Sdwe_refresh_allname(device_info.page_count_all);
                    }
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
                    W25QXX_Write((u8 *)&file_write,(u32)(W25QXX_ADDR_JINGSHA + JINGSHA_SIZE * device_info.page_count_all),sizeof(file_write));
                    //�������ݱ���
                    peiliao_para.complete_meter = (u32)product_para.product_complete;
                    peiliao_para.complete_work_time = product_para.total_work_time;
                    W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO + CHANNENG_SIZE * device_info.page_count_all,sizeof(peiliao_para));
                    
                    //γ�����ݱ���
                    W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI + WEIMI_SIZE * device_info.page_count_all,sizeof(weimi_para));
                    
                    device_info.page_count_all++;
                    W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
                    if(var_addr == PAGE1_FILE_TEXT_IMPORT)
                      Sdwe_disPicture(PAGE_1);
                    else if(var_addr == PAGE2_FILE_TEXT_IMPORT)
                      Sdwe_disPicture(PAGE_2);
                    else if(var_addr == PAGE3_FILE_TEXT_IMPORT)
                      Sdwe_disPicture(PAGE_3);
                    else if(var_addr == PAGE_HISTORY2_IMPORT)//���µ���ҳ���水ť
                    {
                      Sdwe_disPicture(PAGE_HISTORY);//��ת��ҳ��3
                      for(i=0;i<10;i++)
                      {
                        Sdwe_writeIcon(i + PAGE_HISTORY_SELECT,file_select[i]);
                      }
                      Sdwe_refresh_allname(device_info.page_count_all);
                    }
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
                  else if(var_addr == PAGE_HISTORY2_IMPORT)//���µ���ҳ���水ť
                    Sdwe_disString(PAGE_HISTORY2_WARNNING,"�ļ����ظ�",strlen("�ļ����ظ�"));
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
                else if(var_addr == PAGE_HISTORY2_IMPORT)//���µ���ҳ���水ť
                    Sdwe_disString(PAGE_HISTORY2_WARNNING,"�ļ����ظ�",strlen("�ļ����ظ�"));
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
                SDWE_WARNNING(PAGE_HISTORY_TEXT_FILE_WARN,"δѡ���ļ�",2);
              }
              else
              {
                if(device_info.page_count_all == 0)
                {//û�б�����ļ�
                  SDWE_WARNNING(PAGE_HISTORY_TEXT_FILE_WARN,"���ļ�",2);
                }
                else
                {
                  if(cnt > 1)
                  {//ѡ���ļ�����1��
                    SDWE_WARNNING(PAGE_HISTORY_TEXT_FILE_WARN,"ֻ��ѡһ��",2);
                  }
                  else
                  {
                    if(var_addr == PAGE_HISTORY_KEY_SELECT)
                    {//�����ļ�
//                      JINGSHA_FILE file_read;
                      u8 i;
//                      memset((u8 *)&file_read,0,sizeof(file_read));
                      W25QXX_Read((u8 *)&JingSha_File,(u32)(W25QXX_ADDR_JINGSHA + JINGSHA_SIZE * num),sizeof(JingSha_File));//����num������
                      W25QXX_Read((u8 *)&peiliao_para,(u32)(W25QXX_ADDR_PEILIAO + CHANNENG_SIZE * num),sizeof(peiliao_para));//����num������
                      W25QXX_Read((u8 *)&weimi_para,(u32)(W25QXX_ADDR_WEIMI + WEIMI_SIZE * num),sizeof(weimi_para));//����num������
                      device_info.page_count_select = num;
                      SlavePara.filename_len = JingSha_File.filename_len;
                      for(i=0;i<JingSha_File.filename_len;i++)
                      {
                        SlavePara.filename[i] = JingSha_File.filename[i];
                      }
                      for(i=0;i<30;i++)
                      {
                        SlavePara.value_set[i] = JingSha_File.weight_value[i];
                      }
                      W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
                      Init_JINGSHA_GUI();
                      SDWE_WARNNING(PAGE_HISTORY_TEXT_FILE_WARN,"���óɹ�",2);
                      ptMsg->addr = 0xff;
                      ptMsg->func = FUNC_WRITE;
                      ptMsg->reg = REG_SET_WEIGHT;
                      ptMsg->value = 0;
                      if(xQueueSend(xQueue_message,                  /* ��Ϣ���о�� */
                                    (void *) &ptMsg,           /* ���ͽṹ��ָ�����ptMsg�ĵ�ַ */
                                    (TickType_t)10) == pdPASS )
                      {
                        /* ����ʧ�ܣ���ʹ�ȴ���10��ʱ�ӽ��� */
//                        printf("%d# value %.2f ���з��ͳɹ�\r\n",var_addr - 0x0320 + 1,(float)value / 1000);
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
//                printf("%d# sw %d ���з��ͳɹ�\r\n",var_addr - PAGE1_SLAVE_ONOFF1 + 1,value);
              }
            }
            else if(var_addr == PAGE_HISTORY_KEY_ADD)
            {//
              
            }
            else if(var_addr == PAGE_HISTORY_KEY_READ)
            {
              readDisk = DISK_FILENAME;
              xSemaphoreGive(xSemaphore_readDisk);
            }
            else if((var_addr >= PAGE_U_ICON_SELECT1) && (var_addr <= PAGE_U_ICON_SELECT1 + 9))
            {//ѡ��U���ļ�
              value = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              Disk_File.fileselect[var_addr - PAGE_U_ICON_SELECT1] = value;
              printf("Disk file %d,%d\r\n",var_addr - PAGE_HISTORY_SELECT + 1,value);
            }
            else if(var_addr == PAGE_U_KEY_READ)
            {//��ȡU���ļ�
              u8 i;
              u8 num;
              u16 cnt = 0;//�ļ�ѡ���־��ĳ���ļ�ѡ�񣬶�Ӧ��λ��1
              for(i=0;i<10;i++)
              {
                if(Disk_File.fileselect[i] == 1)
                {
                  num = i;
                  cnt++;
                }
              }
              if(cnt == 0)
              {//δѡ���ļ�
                SDWE_WARNNING(PAGE_U_TEXT_READ_STATE,"δѡ���ļ�",3);
              }
              else if(cnt == 1)
              {//ֻ��ѡ��һ���ļ�
                if(Disk_File.filenum == 0)
                {//û�б�����ļ�
                  SDWE_WARNNING(PAGE_U_TEXT_READ_STATE,"���ļ�",3);
                }
                else
                {
                  readDisk = DISK_FILEDATA;
                  readFilenum = num;
                  xSemaphoreGive(xSemaphore_readDisk);
                }
              }
              else
              {//
                SDWE_WARNNING(PAGE_U_TEXT_READ_STATE,"ֻ��ѡ1��",3);
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
              SDWE_WARNNING(PAGE_DEVICE_WARNNING,"�����帴λ",4);
            }
//            else if(var_addr == PAGE_PRODUCT_PEILIAO)
//            {//
//              //��������һ��ʱ��󣬻��������ҳ�����ݴ�����ÿ�ν�������ҳ������´Ӵ洢���ж�ȡ�������ݽ��в���
//              W25QXX_Read((u8 *)&peiliao_para,(u32)(W25QXX_ADDR_PEILIAO + PEILIAO_SIZE * device_info.page_count_select),sizeof(peiliao_para));//���ϱ�������
//              Sdwe_peiliao_page(peiliao_para);
//              card_config = CARD_DISABLE;
//            }
            else if(var_addr == PAGE_PEILIAO_PASSWORD)
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
              Sdwe_disString(PAGE_PEILIAO_DIS,buf,input_password_len);//������ʾΪ*
              
              if(memcmp(&input_password_buf,"222222",6) == 0)
              {//�����������ϵͳ����ҳ��
                Sdwe_disPicture(PAGE_PEILIAO);
                W25QXX_Read((u8 *)&peiliao_para,(u32)(W25QXX_ADDR_PEILIAO + PEILIAO_SIZE * device_info.page_count_select),sizeof(peiliao_para));//���ϱ�������
                Sdwe_peiliao_page(peiliao_para);
                card_config = CARD_DISABLE;
              }
              else
              {//�������
                SDWE_WARNNING(PAGE_PEILIAO_WARNNING,"�������",5);
              }
            }
//            else if(var_addr == PAGE_PRODUCT_CLEAR)
//            {//�������㣬���¿�ʼ����
//              total_meter_gross = (u32)(peiliao_para.total_meter_set * (1 + (float)peiliao_para.loss / 100));
//              total_weight_gross = (u32)(peiliao_para.total_weitht_set * (1 + peiliao_para.loss / 100.0));//���������ú����
//              init_product_para(&product_para,peiliao_para);//����������������󣬲�������
//              peiliao_para.add_meter_set = 0;//����������������󣬲���������
//              Sdwe_disDigi(PAGE_PRODUCT_ADD_METER,peiliao_para.add_meter_set,4);
//              Sdwe_product_page(&product_para);
//              W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
//              W25QXX_Write((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
//              plan_complete = 0;
//              old_plan_complete = 0;
//              
//              Sdwe_clearString(PAGE_PRODUCT_RFID_WARNNING);//������������ʾ
//            }
            else if(var_addr == PAGE_CLEAR_PASSWORD)
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
              Sdwe_disString(PAGE_CLEAR_DIS,buf,input_password_len);//������ʾΪ*
              
              if(memcmp(&input_password_buf,"222222",6) == 0)
              {//�����������ϵͳ����ҳ��
//                Sdwe_disPicture(PAGE_CHANNENG_CLEAR);
                Sdwe_disPicture(PAGE_CHANNENG);
                total_meter_gross = (u32)(peiliao_para.total_meter_set * (1 + (float)peiliao_para.loss / 100));
                total_weight_gross = (u32)(peiliao_para.total_weitht_set * (1 + peiliao_para.loss / 100.0));//���������ú����
                init_product_para(&product_para,peiliao_para);//����������������󣬲�������
                peiliao_para.add_meter_set = 0;//����������������󣬲���������
                Sdwe_disDigi(PAGE_PRODUCT_ADD_METER,peiliao_para.add_meter_set,4);
                Sdwe_product_page(&product_para);
                W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
                W25QXX_Write((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
                plan_complete = 0;
                old_plan_complete = 0;
                
                Sdwe_clearString(PAGE_PRODUCT_RFID_WARNNING);//������������ʾ
              }
              else
              {//�������
                SDWE_WARNNING(PAGE_CLEAR_WARNNING,"�������",5);
              }
            }
            else if(var_addr == PAGE_PRODUCT_QUIT)
            {//�˳�����ҳ�棬�����ܽ�ֹ
              card_config = CARD_DISABLE;
            }
            else if(var_addr == PAGE_PEILIAO_QUIT)
            {//�˳�����ҳ�棬�������ҳ�棬��������
              card_config = READ_PERMISSION;
//              card_config = CARD_DISABLE;
            }
            else if(var_addr == PAGE_PRODUCT_JINGSHA)
            {//��ɴ����
              u32 meter;
              meter = (float)((lcd_rev_buf[7] << 8) + lcd_rev_buf[8]);
              peiliao_para.latitude_weight = meter;
              peiliao_para.final_weight = final_per_meter(peiliao_para);
              Sdwe_disDigi(PAGE_PRODUCT_FINAL,peiliao_para.final_weight,2);
              u8 no = 0;
              no = isFileSelect();
              if(no == 1)
                W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO + PEILIAO_SIZE * device_info.page_count_select,sizeof(peiliao_para));
              u8 publish_topic;
              publish_topic = TOPIC_PEILIAO;
              xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
            }
            else if(var_addr == PAGE_PRODUCT_WEISHA)
            {//γɴ����
              u32 meter;
              meter = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              peiliao_para.longitude_weight = meter;
              peiliao_para.final_weight = final_per_meter(peiliao_para);
              Sdwe_disDigi(PAGE_PRODUCT_FINAL,peiliao_para.final_weight,2);
              u8 no = 0;
              no = isFileSelect();
              if(no == 1)
                W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO + PEILIAO_SIZE * device_info.page_count_select,sizeof(peiliao_para));
              u8 publish_topic;
              publish_topic = TOPIC_PEILIAO;
              xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
            }
            else if(var_addr == PAGE_PRODUCT_RUBBER)
            {//������
              u32 meter;
              meter = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              peiliao_para.rubber_weight = meter;
              peiliao_para.final_weight = final_per_meter(peiliao_para);
              Sdwe_disDigi(PAGE_PRODUCT_FINAL,peiliao_para.final_weight,2);
              u8 no = 0;
              no = isFileSelect();
              if(no == 1)
                W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO + PEILIAO_SIZE * device_info.page_count_select,sizeof(peiliao_para));
              u8 publish_topic;
              publish_topic = TOPIC_PEILIAO;
              xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
            }
            else if(var_addr == PAGE_PRODUCT_ZHIJI)
            {//֯������
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              peiliao_para.loom_num = cnt;
              u8 no = 0;
              no = isFileSelect();
              if(no == 1)
                W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO + PEILIAO_SIZE * device_info.page_count_select,sizeof(peiliao_para));
              u8 publish_topic;
              publish_topic = TOPIC_PEILIAO;
              xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
            }
            else if(var_addr == PAGE_PRODUCT_LOSS)
            {//���
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              peiliao_para.loss = cnt;
              total_meter_gross = (u32)(peiliao_para.total_meter_set * (1 + peiliao_para.loss / 100.0));
              total_weight_gross = (u32)(peiliao_para.total_weitht_set * (1 + peiliao_para.loss / 100.0));
              u8 no = 0;
              no = isFileSelect();
              if(no == 1)
                W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO + PEILIAO_SIZE * device_info.page_count_select,sizeof(peiliao_para));
              u8 publish_topic;
              publish_topic = TOPIC_PEILIAO;
              xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
            }
            else if(var_addr == PAGE_PRODUCT_TOTAL_METER)
            {//��������������
              u32 cnt;
              float percent;
              cnt = (lcd_rev_buf[7] << 24) + (lcd_rev_buf[8] << 16) + (lcd_rev_buf[9] << 8) + lcd_rev_buf[10];
              peiliao_para.total_meter_set = cnt;
              percent = 1 + (float)peiliao_para.loss / 100.0;
              total_meter_gross = (u32)(peiliao_para.total_meter_set * percent);
              init_product_para(&product_para,peiliao_para);//����������������󣬲�������
              peiliao_para.add_meter_set = 0;//����������������󣬲���������
              Sdwe_disDigi(PAGE_PRODUCT_ADD_METER,peiliao_para.add_meter_set,4);
              Sdwe_product_page(&product_para);
              u8 no = 0;
              no = isFileSelect();
              if(no == 1)
                W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO + PEILIAO_SIZE * device_info.page_count_select,sizeof(peiliao_para));
              W25QXX_Write((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
              plan_complete = 0;
              old_plan_complete = 0;
              
              Sdwe_clearString(PAGE_PRODUCT_RFID_WARNNING);//������������ʾ
              u8 publish_topic;
              publish_topic = TOPIC_PEILIAO;
              xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
            }
            else if(var_addr == PAGE_PRODUCT_TOTAL_WEIGHT)
            {//����������������
              u32 cnt;
              float percent;
              cnt = (lcd_rev_buf[7] << 24) + (lcd_rev_buf[8] << 16) + (lcd_rev_buf[9] << 8) + lcd_rev_buf[10];
              peiliao_para.total_weitht_set = cnt;
              percent = 1 + (float)peiliao_para.loss / 100.0;
              total_weight_gross = (u32)(peiliao_para.total_weitht_set * percent);
              init_product_para(&product_para,peiliao_para);//����������������󣬲�������
              peiliao_para.add_meter_set = 0;//����������������󣬲���������
              Sdwe_disDigi(PAGE_PRODUCT_ADD_METER,peiliao_para.add_meter_set,4);
              Sdwe_product_page(&product_para);
              u8 no = 0;
              no = isFileSelect();
              if(no == 1)
                W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO + PEILIAO_SIZE * device_info.page_count_select,sizeof(peiliao_para));
              W25QXX_Write((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
              plan_complete = 0;
              old_plan_complete = 0;
              Sdwe_clearString(PAGE_PRODUCT_RFID_WARNNING);//������������ʾ
              u8 publish_topic;
              publish_topic = TOPIC_PEILIAO;
              xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
            }
            else if(var_addr == PAGE_PRODUCT_KAIDU)
            {//��������
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              peiliao_para.kaidu_set = cnt;
              u8 no = 0;
              no = isFileSelect();
              if(no == 1)
                W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO + PEILIAO_SIZE * device_info.page_count_select,sizeof(peiliao_para));
              u8 publish_topic;
              publish_topic = TOPIC_PEILIAO;
              xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
            }
            else if(var_addr == PAGE_PRODUCT_WEIMI)
            {//γ������
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              if(cnt > 0)
              {
                peiliao_para.weimi_set = cnt;
                u8 no = 0;
                no = isFileSelect();
                if(no == 1)
                  W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO + PEILIAO_SIZE * device_info.page_count_select,sizeof(peiliao_para));
                weimi_para.wei_cm_set[0] = cnt / 10;//����ҳ��γ���׺�γ��ҳ���1γ������ͬ
                weimi_para.wei_inch_set[0] = weimi_para.wei_cm_set[0] * 2.54;
                //ת��Ϊγ/inch��ʾ
                if(no == 1)
                  W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI,sizeof(weimi_para));
              }
              u8 publish_topic;
              publish_topic = TOPIC_PEILIAO;
              xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
            }
            else if(var_addr == PAGE_PRODUCT_ADD_METER)
            {//��������
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 24) + (lcd_rev_buf[8] << 16) + (lcd_rev_buf[9] << 8) + lcd_rev_buf[10];
              peiliao_para.add_meter_set = cnt;
              u8 no = 0;
              no = isFileSelect();
              if(no == 1)
                W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO + PEILIAO_SIZE * device_info.page_count_select,sizeof(peiliao_para));
              u8 publish_topic;
              publish_topic = TOPIC_PEILIAO;
              xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
            }
            else if(var_addr == PAGE_PRODUCT_WEISHU_DIS)
            {//γ����ʾ����
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              peiliao_para.weimi_dis_set = cnt;
              u8 no = 0;
              no = isFileSelect();
              if(no == 1)
                W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO + PEILIAO_SIZE * device_info.page_count_select,sizeof(peiliao_para));
              u8 publish_topic;
              publish_topic = TOPIC_PEILIAO;
              xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
            }
            else if((var_addr >= PAGE_STOP_OFF) && (var_addr <= PAGE_STOP_OFF + 11))
            {//ͣ��ԭ��ѡ��
              if((device_info.system_state == SYS_STOP) || (device_info.system_state == SYS_IDLE))
              {//ֻ��ϵͳ����ʱ������ѡ��ͣ��ԭ��
                device_info.system_state = var_addr - PAGE_STOP_OFF + 1;
                Sdwe_disString(PAGE1_SYSTEM_STATE,(u8 *)system_state_dis[device_info.system_state],strlen(system_state_dis[device_info.system_state]));
                W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
                Sdwe_writeIcon(var_addr - PAGE_STOP_OFF + PAGE_STOP_ON,VGUS_ON);//ͼ����ʾѡ��
                printf("System stop is num %d.\r\n",device_info.system_state);
                RELAY_CLOSE();
              }
              else
              {
                if(device_info.system_state == (var_addr - PAGE_STOP_OFF + 1))
                {//ȡ��ѡ��ͣ��
                  Sdwe_writeIcon(var_addr - PAGE_STOP_OFF + PAGE_STOP_ON,VGUS_OFF);
                  device_info.system_state = SYS_STOP;
                  Sdwe_disString(PAGE1_SYSTEM_STATE,(u8 *)system_state_dis[device_info.system_state],strlen(system_state_dis[device_info.system_state]));
                  W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
                  printf("System normal.\r\n");
                  RELAY_OPEN();
                }
                else
                {
                  SDWE_WARNNING(PAGE_STOP_WARNNING,"��Ч����",6);
                }
              }
              u8 publish_topic;
              publish_topic = TOPIC_STOP;
              xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
            }
            
            /****************************�ָ�����Ĭ��ֵ************************************/
            else if(var_addr == PAGE_CONFIG_DEFAULT)
            {
              default_device_para();
              SDWE_WARNNING(PAGE_DEVICE_WARNNING,"�ָ�Ĭ��",4);
            }
            /****************************���ֱ�����************************************/
            else if(var_addr == PAGE_CONFIG_RATIO_DISPLAY)
            {
              Sdwe_ratio_display(&device_info);
            }
            else if(var_addr == PAGE_CONFIG_RATIO1)
            {//���ֱ�1
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              device_info.ratio.GEAR1 = cnt;
              W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
            }
            /****************************���������ʾ************************************/
            else if(var_addr == PAGE_CONFIG_DEVICE_ID)
            {//�������
              Sdwe_disString(PAGE_DEVICE_ID,device_info.device_id,device_info.device_id_len);
            }
            /*********************************************************************************/
            /****************************ͣ����ת����������************************************/
            else if(var_addr == PAGE_CONFIG_SEVRO_PLUSE_PAGE)
            {//ͣ��ʱ�ŷ����Ҫ��תһ����������
              Sdwe_sevro_pulse_display(&device_info);
            }
            else if(var_addr == PAGE_CONFIG_SEVRO_PLUSE)
            {//
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              device_info.sevro_stop_pluse = cnt;
              W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
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
                SDWE_WARNNING(PAGE_CHANGE_WARNNING,"������Χ",7);
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
                SDWE_WARNNING(PAGE_CHANGE_WARNNING,"������Χ",7);
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
                SDWE_WARNNING(PAGE_PASSWORD_WARNNING,"�޸ĳɹ�",8);
                W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
              }
              else
              {
                SDWE_WARNNING(PAGE_PASSWORD_WARNNING,"���벻�ܿ�",8);
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
                SDWE_WARNNING(PAGE_DEVICE_WARNNING,"�޸ĳɹ�",4);
                W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
              }
              else
              {
                SDWE_WARNNING(PAGE_DEVICE_WARNNING,"���벻�ܿ�",4);
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
              else if(strcmp((char const*)input_password_buf,"102608") == 0)
              {//���������������ҳ��
                Sdwe_disPicture(PAGE_HIDDEN);
                Sdwe_hidden_page(&device_info);
              }
              else if(memcmp(&input_password_buf,&device_info.regin_in.password,device_info.regin_in.password_len) == 0)
              {//�����������ϵͳ����ҳ��
                Sdwe_disPicture(PAGE_CONFIG);
              }
              else if(memcmp(&input_password_buf,"111111",6) == 0)
              {//�����������ϵͳ����ҳ��
                Sdwe_disPicture(PAGE_CONFIG);
              }
              else if(strcmp((char const*)input_password_buf,"190403") == 0)
              {
                Sdwe_disPicture(PAGE_CONFIG);
              }
              else
              {//�������
                SDWE_WARNNING(PAGE_CONFIG_WARNNING,"�������",5);
              }
            }
            /******************************��ʷ����ҳ��2***************************************/
            else if(var_addr == PAGE_HISTORY2_CANCEL)
            {
              
            }
            else if(var_addr == PAGE_HISTORY2_IMPORT)
            {
              
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
                SDWE_WARNNING(PAGE_PERIOD_WARNNING,"������Χ",9);
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
                SDWE_WARNNING(PAGE_PERIOD_WARNNING,"������Χ",9);
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
            /*****************************�����ٶ�DAC�������*********************************/
            else if(var_addr == PAGE_MAIN_DACOUT_ADD)
            {
              value = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              Dac1_Set_Vol(value);
              device_speed_volate = value;
              RTC_WriteBackupRegister(RTC_BKP_DR13,device_speed_volate);
            }
            /***********************************************************************/
          }
          else
          {
            if((var_addr >= PAGE_WEIMI_TOTALWEI_1) && (var_addr < PAGE_WEIMI_TOTALWEI_1 + 20))
            {//γѭ������
              u32 cnt;
              cnt = (lcd_rev_buf[7] << 24) + (lcd_rev_buf[8] << 16) + (lcd_rev_buf[9] << 8) + lcd_rev_buf[10];
              if((var_addr - PAGE_WEIMI_TOTALWEI_1) == 0)
              {//�޸ĵ�һ��γѭ��
                if(cnt == 0)
                {//��1γѭ������Ϊ0
                  SDWE_WARNNING(PAGE_WEIMI_WARNNING,"����Ϊ0",10);
                  Sdwe_disDigi(var_addr,weimi_para.total_wei_count[0],4);
                }
                else
                {
                  weimi_para.total_wei_count[(var_addr - PAGE_WEIMI_TOTALWEI_1)] = cnt;
                  u8 no = 0;
                  no = isFileSelect();
                  if(no == 1)
                    W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI + WEIMI_SIZE * device_info.page_count_select,sizeof(weimi_para));
                  if((var_addr - PAGE_WEIMI_TOTALWEI_1) == MotorProcess.current_seg)
                  {//����޸ĵ�ѭ��γ�պ��ǵ�ǰ�κţ����̸��µ�ǰѭ��γ
//                    MotorProcess.total_wei = weimi_para.total_wei_count[MotorProcess.current_seg];
//                    MotorProcess.song_total_wei[0] = weimi_para.total_wei_count[MotorProcess.songwei_seg[0]];
//                    MotorProcess.song_total_wei[1] = weimi_para.total_wei_count[MotorProcess.songwei_seg[1]];
//                    MotorProcess.song_total_wei[2] = weimi_para.total_wei_count[MotorProcess.songwei_seg[2]];
                    reset_seg_to_1();
                  }
                }
              }
              else
              {
                weimi_para.total_wei_count[(var_addr - PAGE_WEIMI_TOTALWEI_1)] = cnt;
                u8 no = 0;
                no = isFileSelect();
                if(no == 1)
                  W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI + WEIMI_SIZE * device_info.page_count_select,sizeof(weimi_para));
                if((var_addr - PAGE_WEIMI_TOTALWEI_1) == MotorProcess.current_seg)
                {//����޸ĵ�ѭ��γ�պ��ǵ�ǰ�κţ����̸��µ�ǰѭ��γ
//                  MotorProcess.total_wei = weimi_para.total_wei_count[MotorProcess.current_seg];
//                  MotorProcess.song_total_wei[0] = weimi_para.total_wei_count[MotorProcess.songwei_seg[0]];
//                  MotorProcess.song_total_wei[1] = weimi_para.total_wei_count[MotorProcess.songwei_seg[1]];
//                  MotorProcess.song_total_wei[2] = weimi_para.total_wei_count[MotorProcess.songwei_seg[2]];
                  reset_seg_to_1();
                }
              }
              u8 publish_topic;
              publish_topic = TOPIC_WEIMI;
              xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
            }
            else if((var_addr >= PAGE_WEIMI_WEI_CM_1)  && (var_addr < PAGE_WEIMI_WEI_CM_1 + 20))
            {//γ/cm����
              float cnt;
              u8 seg_num;
              cnt = (float)((lcd_rev_buf[7] << 8) + lcd_rev_buf[8]) / 10.0;
              seg_num = (var_addr - PAGE_WEIMI_WEI_CM_1) / 2;
              if((seg_num == 0) && (cnt == 0))
              {//��1��ͬ���������е�γ�ܣ���������Ϊ0
                Sdwe_disDigi(var_addr,weimi_para.wei_cm_set[0] *10,2);
              }
              else
              {
                weimi_para.wei_cm_set[seg_num] = cnt;
                weimi_para.wei_inch_set[seg_num] = weimi_para.wei_cm_set[seg_num] * 2.54;
                //ת��Ϊγ/inch��ʾ
                Sdwe_disDigi(PAGE_WEIMI_WEI_INCH_1 + var_addr - PAGE_WEIMI_WEI_CM_1,(int)(weimi_para.wei_inch_set[seg_num] * 100),2);
                u8 no = 0;
                no = isFileSelect();
                if(no == 1)
                  W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI + WEIMI_SIZE * device_info.page_count_select,sizeof(weimi_para));
                
                if(seg_num == MotorProcess.current_seg)
                {//����ı��γ��Ϊ��ǰ�κ�γ�ܣ����̸��²���/γ
                  if(cnt == 0)
                  {
                    reset_seg_to_1();
                  }
                  else
                    MotorProcess.wei_cm_set = weimi_para.wei_cm_set[MotorProcess.current_seg];
                }
                  
                if((var_addr - PAGE_WEIMI_WEI_CM_1) == 0)
                {//��һ�ε�γ�ܺ�����ҳ���γ����ͬ
                  peiliao_para.weimi_set = cnt * 10;
                  u8 no = 0;
                  no = isFileSelect();
                  if(no == 1)
                  {
                    W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO + PEILIAO_SIZE * device_info.page_count_select,sizeof(peiliao_para));
                    W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI + WEIMI_SIZE * device_info.page_count_select,sizeof(weimi_para));
                  }
                }
                valid_seg[0] = get_valid_seg(weimi_para);
                max_seg = get_max_type(valid_seg);
                u8 publish_topic;
                publish_topic = TOPIC_WEIMI;
                xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
              }
            }
            else if((var_addr >= PAGE_WEIMI_MEDIANWEI_1) && (var_addr < PAGE_WEIMI_MEDIANWEI_1 + 20))
            {//γ��������
              u32 cnt;
              cnt = (lcd_rev_buf[7] << 24) + (lcd_rev_buf[8] << 16) + (lcd_rev_buf[9] << 8) + lcd_rev_buf[10];
              weimi_para.total_wei_count[(var_addr - PAGE_WEIMI_MEDIANWEI_1) + 1] = cnt;
              u8 no = 0;
              no = isFileSelect();
              if(no == 1)
                W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI + WEIMI_SIZE * device_info.page_count_select,sizeof(weimi_para));
              if((var_addr - PAGE_WEIMI_MEDIANWEI_1 + 1) == MotorProcess.current_seg)
              {//����޸ĵ�γ���ɸպ��ǵ�ǰ�κţ����̸��µ�ǰѭ��γ
                if(cnt == 0)
                  MotorProcess.total_wei = weimi_para.total_wei_count[MotorProcess.current_seg];
                else
                  reset_seg_to_1();
              }
              u8 publish_topic;
              publish_topic = TOPIC_WEIMI;
              xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
            }
            else if((var_addr >= PAGE_WEIMI_STEP1_SPEED) && (var_addr < PAGE_WEIMI_STEP1_SPEED + 20))
            {//��γ1����ٶ�����
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              if(cnt <= 300)
              {//��ǰ����ٶ�С�������ٶȣ�Ϊ��Чֵ
                weimi_para.step_factor[0][(var_addr - PAGE_WEIMI_STEP1_SPEED) / 2] = cnt;
                u8 no = 0;
                no = isFileSelect();
                if(no == 1)
                  W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI + WEIMI_SIZE * device_info.page_count_select,sizeof(weimi_para));
                
                if(((var_addr - PAGE_WEIMI_STEP1_SPEED) / 2) == (MotorProcess.songwei_seg[0]))
                {//������õ���γ����Ŷ�Ϊ��ǰ�ŶΣ����̸�����γ�������
                  if(cnt == 0)
                  {
                    weisha_reset_to_seg1(0);
                  }
                  else
                    MotorProcess.step_factor[0] = weimi_para.step_factor[0][MotorProcess.songwei_seg[0]];
//                  reset_seg_to_1();
                }
                valid_seg[1] = get_songwei0_maxseg(weimi_para);
                max_seg = get_max_type(valid_seg);
              }
              else
              {
                Sdwe_disDigi(var_addr,weimi_para.step_factor[0][(var_addr - PAGE_WEIMI_STEP1_SPEED) / 2],2);
                SDWE_WARNNING(PAGE_WEIMI_WARNNING,"�ٱ�̫��",10);
              }
              u8 publish_topic;
              publish_topic = TOPIC_WEISHA;
              xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
            }
            else if((var_addr >= PAGE_WEIMI_STEP2_SPEED) && (var_addr < PAGE_WEIMI_STEP2_SPEED + 20))
            {//��γ2����ٶ�����
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              if(cnt <= 300)
              {
                weimi_para.step_factor[1][(var_addr - PAGE_WEIMI_STEP2_SPEED) / 2] = cnt;
                u8 no = 0;
                no = isFileSelect();
                if(no == 1)
                  W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI + WEIMI_SIZE * device_info.page_count_select,sizeof(weimi_para));
                
                if(((var_addr - PAGE_WEIMI_STEP2_SPEED) / 2) == (MotorProcess.songwei_seg[1]))
                {//������õ���γ����Ŷ�Ϊ��ǰ�ŶΣ����̸�����γ�������
                  if(cnt == 0)
                  {
                    weisha_reset_to_seg1(1);
                  }
                  else
                    MotorProcess.step_factor[1] = weimi_para.step_factor[1][MotorProcess.songwei_seg[1]];
//                  reset_seg_to_1();
                }
                valid_seg[2] = get_songwei1_maxseg(weimi_para);
                max_seg = get_max_type(valid_seg);
              }
              else
              {
                Sdwe_disDigi(var_addr,weimi_para.step_factor[1][(var_addr - PAGE_WEIMI_STEP2_SPEED) / 2],2);
                SDWE_WARNNING(PAGE_WEIMI_WARNNING,"�ٱ�̫��",10);
              }
              u8 publish_topic;
              publish_topic = TOPIC_WEISHA;
              xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
            }
            else if((var_addr >= PAGE_WEIMI_STEP3_SPEED) && (var_addr < PAGE_WEIMI_STEP3_SPEED + 20))
            {//���ߵ���ٶ�����
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              if(cnt <= 300)
              {
                weimi_para.step_factor[2][(var_addr - PAGE_WEIMI_STEP3_SPEED) / 2] = cnt;
                u8 no = 0;
                no = isFileSelect();
                if(no == 1)
                  W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI + WEIMI_SIZE * device_info.page_count_select,sizeof(weimi_para));
                
                if(((var_addr - PAGE_WEIMI_STEP3_SPEED) / 2) == (MotorProcess.songwei_seg[2]))
                {//������õ���γ����Ŷ�Ϊ��ǰ�ŶΣ����̸�����γ�������
                  if(cnt == 0)
                  {
                    weisha_reset_to_seg1(2);
                  }
                  else
                    MotorProcess.step_factor[2] = weimi_para.step_factor[2][MotorProcess.songwei_seg[2]];
//                  reset_seg_to_1();
                }
                valid_seg[3] = get_songwei2_maxseg(weimi_para);
                max_seg = get_max_type(valid_seg);
              }
              else
              {
                Sdwe_disDigi(var_addr,weimi_para.step_factor[2][(var_addr - PAGE_WEIMI_STEP3_SPEED) / 2],2);
                SDWE_WARNNING(PAGE_WEIMI_WARNNING,"�ٱ�̫��",10);
              }
              u8 publish_topic;
              publish_topic = TOPIC_WEISHA;
              xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
            }
            else if((var_addr >= PAGE_WEIMI_STEP1_ADD1) && (var_addr < PAGE_WEIMI_STEP1_ADD1 + 10))
            {//��γ1�ٶȼ�1����10
              if(weimi_para.step_factor[0][var_addr - PAGE_WEIMI_STEP1_ADD1] < 300)
              {//��γ����ٶ�С��200���ܼ�
                u8 position;
                u8 speed;
                position = var_addr - PAGE_WEIMI_STEP1_ADD1;
                weimi_para.step_factor[0][position] += 1;
                speed = weimi_para.step_factor[0][position];
                u8 no = 0;
                no = isFileSelect();
                if(no == 1)
                  W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI + WEIMI_SIZE * device_info.page_count_select,sizeof(weimi_para));
                Sdwe_disDigi(PAGE_WEIMI_STEP1_SPEED + 2 * position,speed,2);
                
                if(((var_addr - PAGE_WEIMI_STEP1_ADD1) / 2) == (MotorProcess.current_seg / 2))
                {//������õ���γ����Ŷ�Ϊ��ǰ�ŶΣ����̸�����γ�������
                  MotorProcess.step_factor[0] = weimi_para.step_factor[0][MotorProcess.current_seg / 2];
//                  reset_seg_to_1();
                }
              }
              u8 publish_topic;
              publish_topic = TOPIC_WEISHA;
              xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
            }
            else if((var_addr >= PAGE_WEIMI_STEP1_SUB1) && (var_addr < PAGE_WEIMI_STEP1_SUB1 + 10))
            {//��γ1�ٶȼ�1����10
              if(weimi_para.step_factor[0][var_addr - PAGE_WEIMI_STEP1_SUB1] > 0)
              {//��γ����ٶȴ���0
                u8 position;
                u8 speed;
                position = var_addr - PAGE_WEIMI_STEP1_SUB1;
                weimi_para.step_factor[0][position] -= 1;
                speed = weimi_para.step_factor[0][position];
                W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI,sizeof(weimi_para));
                Sdwe_disDigi(PAGE_WEIMI_STEP1_SPEED + 2 * position,speed,2);
                
                if(((var_addr - PAGE_WEIMI_STEP1_SUB1) / 2) == (MotorProcess.current_seg / 2))
                {//������õ���γ����Ŷ�Ϊ��ǰ�ŶΣ����̸�����γ�������
                  MotorProcess.step_factor[0] = weimi_para.step_factor[0][MotorProcess.current_seg / 2];
//                  reset_seg_to_1();
                }
              }
              u8 publish_topic;
              publish_topic = TOPIC_WEISHA;
              xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
            }
            else if((var_addr >= PAGE_WEIMI_STEP2_ADD1) && (var_addr < PAGE_WEIMI_STEP2_ADD1 + 10))
            {//��γ2�ٶȼ�1����10
              if(weimi_para.step_factor[1][var_addr - PAGE_WEIMI_STEP2_ADD1] < 300)
              {//��γ����ٶ�С��200���ܼ�
                u8 position;
                u8 speed;
                position = var_addr - PAGE_WEIMI_STEP2_ADD1;
                weimi_para.step_factor[1][position] += 1;
                speed = weimi_para.step_factor[1][position];
                u8 no = 0;
                no = isFileSelect();
                if(no == 1)
                  W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI + WEIMI_SIZE * device_info.page_count_select,sizeof(weimi_para));
                Sdwe_disDigi(PAGE_WEIMI_STEP2_SPEED + 2 * position,speed,2);
                
                if(((var_addr - PAGE_WEIMI_STEP2_ADD1) / 2) == (MotorProcess.current_seg / 2))
                {//������õ���γ����Ŷ�Ϊ��ǰ�ŶΣ����̸�����γ�������
                  MotorProcess.step_factor[1] = weimi_para.step_factor[1][MotorProcess.current_seg / 2];
//                  reset_seg_to_1();
                }
              }
              u8 publish_topic;
              publish_topic = TOPIC_WEISHA;
              xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
            }
            else if((var_addr >= PAGE_WEIMI_STEP2_SUB1) && (var_addr < PAGE_WEIMI_STEP2_SUB1 + 10))
            {//��γ2�ٶȼ�1����10
              if(weimi_para.step_factor[1][var_addr - PAGE_WEIMI_STEP2_SUB1] > 0)
              {//��γ����ٶȴ���0
                u8 position;
                u8 speed;
                position = var_addr - PAGE_WEIMI_STEP2_SUB1;
                weimi_para.step_factor[1][position] -= 1;
                speed = weimi_para.step_factor[1][position];
                u8 no = 0;
                no = isFileSelect();
                if(no == 1)
                  W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI + WEIMI_SIZE * device_info.page_count_select,sizeof(weimi_para));
                Sdwe_disDigi(PAGE_WEIMI_STEP2_SPEED + 2 * position,speed,2);
                
                if(((var_addr - PAGE_WEIMI_STEP2_SUB1) / 2) == (MotorProcess.current_seg / 2))
                {//������õ���γ����Ŷ�Ϊ��ǰ�ŶΣ����̸�����γ�������
                  MotorProcess.step_factor[1] = weimi_para.step_factor[1][MotorProcess.current_seg / 2];
//                  reset_seg_to_1();
                }
              }
              u8 publish_topic;
              publish_topic = TOPIC_WEISHA;
              xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
            }
            else if((var_addr >= PAGE_WEIMI_STEP3_ADD1) && (var_addr < PAGE_WEIMI_STEP3_ADD1 + 10))
            {//���3�ٶȼ�1����10
              if(weimi_para.step_factor[2][var_addr - PAGE_WEIMI_STEP3_ADD1] < 200)
              {//���3�ٶ�С��200���ܼ�
                u8 position;
                u8 speed;
                position = var_addr - PAGE_WEIMI_STEP3_ADD1;
                weimi_para.step_factor[2][position] += 1;
                speed = weimi_para.step_factor[2][position];
                u8 no = 0;
                no = isFileSelect();
                if(no == 1)
                  W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI + WEIMI_SIZE * device_info.page_count_select,sizeof(weimi_para));
                Sdwe_disDigi(PAGE_WEIMI_STEP3_SPEED + 2 * position,speed,2);
                
                if(((var_addr - PAGE_WEIMI_STEP3_ADD1) / 2) == (MotorProcess.current_seg / 2))
                {//������õĵ��3�Ŷ�Ϊ��ǰ�ŶΣ����̸�����γ�������
                  MotorProcess.step_factor[2] = weimi_para.step_factor[2][MotorProcess.current_seg / 2];
//                  reset_seg_to_1();
                }
              }
              u8 publish_topic;
              publish_topic = TOPIC_WEISHA;
              xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
            }
            else if((var_addr >= PAGE_WEIMI_STEP3_SUB1) && (var_addr < PAGE_WEIMI_STEP3_SUB1 + 10))
            {//���3�ٶȼ�1����10
              if(weimi_para.step_factor[2][var_addr - PAGE_WEIMI_STEP3_SUB1] > 0)
              {//���3����ٶȴ���0
                u8 position;
                u8 speed;
                position = var_addr - PAGE_WEIMI_STEP3_SUB1;
                weimi_para.step_factor[2][position] -= 1;
                speed = weimi_para.step_factor[2][position];
                u8 no = 0;
                no = isFileSelect();
                if(no == 1)
                  W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI + WEIMI_SIZE * device_info.page_count_select,sizeof(weimi_para));
                Sdwe_disDigi(PAGE_WEIMI_STEP3_SPEED + 2 * position,speed,2);
                
                if(((var_addr - PAGE_WEIMI_STEP3_SUB1) / 2) == (MotorProcess.current_seg / 2))
                {//������õĵ��3�Ŷ�Ϊ��ǰ�ŶΣ����̸�����γ�������
                  MotorProcess.step_factor[2] = weimi_para.step_factor[2][MotorProcess.current_seg / 2];
//                  reset_seg_to_1();
                }
              }
              u8 publish_topic;
              publish_topic = TOPIC_WEISHA;
              xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
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
            else if(var_addr == PAGE_WEIMI_MANUAL_FORWARD)
            {//�ֶ���ǰ
              u16 value;
              value = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              if(is_stop == 0)
              {//����ֹͣʱ�������ֶ�����
                if(value == 0x0001)
                {//��һ�ΰ���
                  servomotor_mode = MANUAL;
//                  RELAY_CLOSE();
                  SERVO_FORWARD();
                  TIM4_MANUAL_PWM_Config(FREQ_37_5KHZ);
                  SERVO_ENABLE();
                }
                else if(value == 0x0002)
                {//��������
                  servomotor_mode = MANUAL;
                }
                else if(value == 0x0003)
                {//̧��
                  servomotor_mode = AUTO;
                  TIM4_MANUAL_PWM_Stop();
                  SERVO_FORWARD();
//                  RELAY_OPEN();
                }
              }
            }
            else if(var_addr == PAGE_WEIMI_MANUAL_BACKWARD)
            {//�ֶ����
              u16 value;
              value = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              if(is_stop == 0)
              {//����ֹͣʱ�������ֶ�����
                if(value == 0x0001)
                {//��һ�ΰ���
                  servomotor_mode = MANUAL;
//                  RELAY_CLOSE();
                  SERVO_BACKWARD();
                  TIM4_MANUAL_PWM_Config(FREQ_37_5KHZ);
                  SERVO_ENABLE();
                }
                else if(value == 0x0002)
                {//��������
                  servomotor_mode = MANUAL;
                }
                else if(value == 0x0003)
                {//̧��
                  servomotor_mode = AUTO;
//                  RELAY_OPEN();
                  TIM4_MANUAL_PWM_Stop();
                  SERVO_FORWARD();
                }
              }
            }
            else if(var_addr == PAGE_WEIMI_RESET)
            {
              reset_seg_to_1();
//              StepMotor_stop(STEPMOTOR1);
//              StepMotor_stop(STEPMOTOR2);
//              StepMotor_stop(STEPMOTOR3);
            }
            else if((var_addr >= PAGE_HISTORY_SELECT) && (var_addr <= (PAGE_HISTORY_SELECT + 9)))
            {//�ļ�ѡ��
              value = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              file_select[var_addr - PAGE_HISTORY_SELECT] = value;
              device_info.file_select[var_addr - PAGE_HISTORY_SELECT] = file_select[var_addr - PAGE_HISTORY_SELECT];
              W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
              if(value == 0)
              {
                printf("file %d off\r\n",var_addr - PAGE_HISTORY_SELECT + 1);
              }
              else
              {
                printf("file %d on\r\n",var_addr - PAGE_HISTORY_SELECT + 1);
              }
            }
            else if(var_addr == PAGE_WEIMI1_SECRET_INPUT)
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
              Sdwe_disString(PAGE_WEIMI1_SECRET_DISPLAY,buf,input_password_len);//������ʾΪ*
              if(strcmp((char const*)input_password_buf,"000000") == 0)
              {//������ȷ
                Sdwe_disPicture(PAGE_WEIMI1_XIUGAI);
                vTaskDelay(10);
              }
              else
              {//�������
                Sdwe_disString(PAGE_WEIMI1_SECRET_WARNING,"�������",strlen("�������"));
              }
            }
            else if(var_addr == PAGE_WEIMI2_SECRET_INPUT)
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
              Sdwe_disString(PAGE_WEIMI2_SECRET_DISPLAY,buf,input_password_len);//������ʾΪ*
              if(strcmp((char const*)input_password_buf,"000000") == 0)
              {//������ȷ
                Sdwe_disPicture(PAGE_WEIMI2_XIUGAI);
                vTaskDelay(10);
              }
              else
              {//�������
                Sdwe_disString(PAGE_WEIMI2_SECRET_WARNING,"�������",strlen("�������"));
              }
            }
            else if(var_addr == PAGE_WEISHA1_SECRET_INPUT)
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
              Sdwe_disString(PAGE_WEISHA1_SECRET_DISPLAY,buf,input_password_len);//������ʾΪ*
              if(strcmp((char const*)input_password_buf,"000000") == 0)
              {//������ȷ
                Sdwe_disPicture(PAGE_WEISHA1_XIUGAI);
                vTaskDelay(10);
              }
              else
              {//�������
                Sdwe_disString(PAGE_WEISHA1_SECRET_WARNING,"�������",strlen("�������"));
              }
            }
            else if(var_addr == PAGE_WEISHA2_SECRET_INPUT)
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
              Sdwe_disString(PAGE_WEISHA2_SECRET_DISPLAY,buf,input_password_len);//������ʾΪ*
              if(strcmp((char const*)input_password_buf,"000000") == 0)
              {//������ȷ
                Sdwe_disPicture(PAGE_WEISHA2_XIUGAI);
                vTaskDelay(10);
              }
              else
              {//�������
                Sdwe_disString(PAGE_WEISHA2_SECRET_WARNING,"�������",strlen("�������"));
              }
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
  JINGSHA_FILE read_info;
  
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
        W25QXX_Read((u8 *)&read_info,(u32)(W25QXX_ADDR_JINGSHA + JINGSHA_SIZE * download_num),sizeof(read_info));//����num������
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
        char buf[1024] = "λ��,�趨����\n",buf1[20];
        u16 len,i;
        for(i=0;i<30;i++)
        {
          sprintf(buf1,"%d#,%.2f\n",i + 1,(float)read_info.weight_value[i] / 1000.0);
          strcat(buf,buf1);
        }
        sprintf(buf,"%sʱ������,%02d/%02d/%02d,%02d:%02d:%02d\n\n",buf,read_info.year,
                read_info.month,read_info.day,read_info.hour,read_info.minute,read_info.second);
        
        PEILIAO_PARA PeiLiao_para;
        W25QXX_Read((u8 *)&PeiLiao_para,(u32)(W25QXX_ADDR_PEILIAO + CHANNENG_SIZE * download_num),sizeof(PeiLiao_para));
        sprintf(buf,"%s��ɴ,%.1f\n",buf,(float)PeiLiao_para.latitude_weight / 10);
        sprintf(buf,"%sγɴ,%.1f\n",buf,(float)PeiLiao_para.longitude_weight / 10);
        sprintf(buf,"%s��,%.1f\n",buf,(float)PeiLiao_para.rubber_weight / 10);
        sprintf(buf,"%s��Ʒ,%.1f\n",buf,(float)PeiLiao_para.final_weight / 10);
        sprintf(buf,"%s֯������,%d\n",buf,PeiLiao_para.loom_num);
        sprintf(buf,"%s���,%d\n",buf,PeiLiao_para.loss);
        sprintf(buf,"%s������,%d\n",buf,PeiLiao_para.total_meter_set);
        sprintf(buf,"%s��������,%d\n",buf,PeiLiao_para.total_weitht_set);
        sprintf(buf,"%s����,%.1f\n",buf,(float)PeiLiao_para.kaidu_set / 10);
        sprintf(buf,"%sγ��,%.1f\n",buf,(float)PeiLiao_para.weimi_set / 10);
        sprintf(buf,"%sγ����ʾ,%d\n",buf,PeiLiao_para.weimi_dis_set);
        sprintf(buf,"%s��������,%d\n\n",buf,PeiLiao_para.add_meter_set);

        WEIMI_PARA WeiMi_para;
        W25QXX_Read((u8 *)&WeiMi_para,(u32)(W25QXX_ADDR_WEIMI + WEIMI_SIZE * download_num),sizeof(WeiMi_para));
        sprintf(buf,"%s,γѭ��,γ����,γӢ��,����,��γһ,��γ��,����\n",buf);
        sprintf(buf,"%sһ��,%d,%.1f,%.1f,%d,%d,%d,%d\n",buf,WeiMi_para.total_wei_count[0],WeiMi_para.wei_cm_set[0],WeiMi_para.wei_inch_set[0],
                WeiMi_para.total_wei_count[1],WeiMi_para.step_factor[0][0],WeiMi_para.step_factor[1][0],WeiMi_para.step_factor[2][0]);
        sprintf(buf,"%s����,%d,%.1f,%.1f,%d,%d,%d,%d\n",buf,WeiMi_para.total_wei_count[2],WeiMi_para.wei_cm_set[1],WeiMi_para.wei_inch_set[1],
                WeiMi_para.total_wei_count[3],WeiMi_para.step_factor[0][1],WeiMi_para.step_factor[1][1],WeiMi_para.step_factor[2][1]);
        sprintf(buf,"%s����,%d,%.1f,%.1f,%d,%d,%d,%d\n",buf,WeiMi_para.total_wei_count[4],WeiMi_para.wei_cm_set[2],WeiMi_para.wei_inch_set[2],
                WeiMi_para.total_wei_count[5],WeiMi_para.step_factor[0][2],WeiMi_para.step_factor[1][2],WeiMi_para.step_factor[2][2]);
        sprintf(buf,"%s�Ķ�,%d,%.1f,%.1f,%d,%d,%d,%d\n",buf,WeiMi_para.total_wei_count[6],WeiMi_para.wei_cm_set[3],WeiMi_para.wei_inch_set[3],
                WeiMi_para.total_wei_count[7],WeiMi_para.step_factor[0][3],WeiMi_para.step_factor[1][3],WeiMi_para.step_factor[2][3]);
        sprintf(buf,"%s���,%d,%.1f,%.1f,%d,%d,%d,%d\n",buf,WeiMi_para.total_wei_count[8],WeiMi_para.wei_cm_set[4],WeiMi_para.wei_inch_set[4],
                WeiMi_para.total_wei_count[9],WeiMi_para.step_factor[0][4],WeiMi_para.step_factor[1][4],WeiMi_para.step_factor[2][4]);
        sprintf(buf,"%s����,%d,%.1f,%.1f,%d,%d,%d,%d\n",buf,WeiMi_para.total_wei_count[10],WeiMi_para.wei_cm_set[5],WeiMi_para.wei_inch_set[5],
                WeiMi_para.total_wei_count[11],WeiMi_para.step_factor[0][5],WeiMi_para.step_factor[1][5],WeiMi_para.step_factor[2][5]);
        sprintf(buf,"%s�߶�,%d,%.1f,%.1f,%d,%d,%d,%d\n",buf,WeiMi_para.total_wei_count[12],WeiMi_para.wei_cm_set[6],WeiMi_para.wei_inch_set[6],
                WeiMi_para.total_wei_count[13],WeiMi_para.step_factor[0][6],WeiMi_para.step_factor[1][6],WeiMi_para.step_factor[2][6]);
        sprintf(buf,"%s�˶�,%d,%.1f,%.1f,%d,%d,%d,%d\n",buf,WeiMi_para.total_wei_count[14],WeiMi_para.wei_cm_set[7],WeiMi_para.wei_inch_set[7],
                WeiMi_para.total_wei_count[15],WeiMi_para.step_factor[0][7],WeiMi_para.step_factor[1][7],WeiMi_para.step_factor[2][7]);
        sprintf(buf,"%s�Ŷ�,%d,%.1f,%.1f,%d,%d,%d,%d\n",buf,WeiMi_para.total_wei_count[16],WeiMi_para.wei_cm_set[8],WeiMi_para.wei_inch_set[8],
                WeiMi_para.total_wei_count[17],WeiMi_para.step_factor[0][8],WeiMi_para.step_factor[1][8],WeiMi_para.step_factor[2][8]);
        sprintf(buf,"%sʮ��,%d,%.1f,%.1f,%d,%d,%d,%d\n",buf,WeiMi_para.total_wei_count[18],WeiMi_para.wei_cm_set[9],WeiMi_para.wei_inch_set[9],
                WeiMi_para.total_wei_count[19],WeiMi_para.step_factor[0][9],WeiMi_para.step_factor[1][9],WeiMi_para.step_factor[2][9]);
        
        len = strlen(buf);
        result = f_write(&file,buf,len,&bw);
        if(result == FR_OK)
        {
          SDWE_WARNNING(PAGE_HISTORY_TEXT_FILE_WARN,"д��ɹ�",2);
          printf("%s.CSV�ļ�д��ɹ�\r\n",name_1);
        }
        else
        {
          SDWE_WARNNING(PAGE_HISTORY_TEXT_FILE_WARN,"д��ʧ��",2);
          printf("%s.CSV�ļ�д��ʧ��\r\n",name_1);
        }
        /* �ر��ļ�*/
        f_close(&file);
        
        /* ж���ļ�ϵͳ */
        f_mount(NULL, "2:", 0);
      }
      else
      {
        SDWE_WARNNING(PAGE_HISTORY_TEXT_FILE_WARN,"��U��",2);
        printf("U��δ����\r\n");
      }
    }
    else
    {
      if(usb_disk_flag != old_usb_disk_flag)
      {
        old_usb_disk_flag = usb_disk_flag;
        if(usb_disk_flag == 0)
          Sdwe_writeIcon(PAGE_HISTORY_ICON_U_STATE,VGUS_OFF);  
        else
          Sdwe_writeIcon(PAGE_HISTORY_ICON_U_STATE,VGUS_ON);  
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

static void vTaskTaskADC(void *pvParameters)
{
  float power_adc;
  vTaskDelay(200);
  while(1)
  {
    power_adc = (float)Get_Adc_Average(1) / 4096 * 3.3 * 2;
    if(power_adc <= 4.9)
    {//��ѹ����4.5V��Ϊ����
//      LCD_POWER_OFF();//��ʾ��̫�ĵ磬�ȹر���ʾ��
      W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
//      W25QXX_Write((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
      printf("�ϵ籣��\r\n");
      while(1);
    }
    vTaskDelay(1);
    Task_iwdg_refresh(TASK_LED);
  }
}

static void vTaskTaskRFID(void *pvParameters)
{
  u16 isCard;
  u32 card_id;
  u8 card_type = FUNC_IDLE;
  bsp_Init_RFID();
  vTaskDelay(200);
  while(1)
  {
    if(card_config == CARD_DISABLE)
    {
      vTaskDelay(100);
    }
    else
    {
      if(speed_zhu == 0)
      {
        if(card_config == READ_PERMISSION)
        {//�ڲ���ҳ��������ȡ��
          rfid_rev_flag = 0;
          rfid_rev_cnt = 0;
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
                rfid_rev_cnt = 0;
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
                        SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"δ��������",11);
                      }
                      else
                      {
                        if(get_class_time(&rtc_time,&device_info) == CLASS_A)
                        {//A�࿨����Aʱ���
                          card_record = 1;//A��
                          if(old_card_record != card_record)
                          {
                            old_card_record = card_record;
                            SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"A�࿨",11);
                          }
                          else
                          {
                            SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"�ظ���",11);
                          }
                        }
                        else
                        {//A�࿨����Bʱ���
                          SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"��Ч����",11);
                        }
                      }
                    }
                    else if(card_type == FUNC_CLASS_B)
                    {//B�࿨
                      if(device_info.class_para.class_enable_onoff == 0)
                      {//������๦��δ���ã������һֱ����A��
                        card_record = 1;//A��
                        SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"δ��������",11);
                      }
                      else
                      {
                        if(get_class_time(&rtc_time,&device_info) == CLASS_B)
                        {//B�࿨����Bʱ���
                          card_record = 2;//B��
                          if(old_card_record != card_record)
                          {
                            old_card_record = card_record;
                            SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"B�࿨",11);
                          }
                          else
                          {
                            SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"�ظ���",11);
                          }
                        }
                        else
                        {//A�࿨����Bʱ���
                          SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"��Ч����",11);
                        }
                      }
                      
                    }
                    else
                    {//δע�Ῠ
                      SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"δע�Ῠ",11);
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
            rfid_rev_cnt = 0;
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
                  rfid_rev_cnt = 0;
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
                            SDWE_WARNNING(PAGE_CARD_WARNNING,"¼��ɹ�",12);
                          }
                          else
                          {
                            SDWE_WARNNING(PAGE_CARD_WARNNING,"��Ƭ�Ѵ���",12);
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
                            SDWE_WARNNING(PAGE_CARD_WARNNING,"¼��ɹ�",12);
                          }
                          else
                          {
                            SDWE_WARNNING(PAGE_CARD_WARNNING,"��Ƭ�Ѵ���",12);
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
                            SDWE_WARNNING(PAGE_CARD_WARNNING,"¼��ɹ�",12);
                          }
                          else
                          {
                            SDWE_WARNNING(PAGE_CARD_WARNNING,"��Ƭ�Ѵ���",12);
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
      else
      {
        vTaskDelay(100);
      }
    }
    Task_iwdg_refresh(TASK_RFID);
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
        else if((ptMsg1->func == FUNC_WRITE) && (ptMsg1->reg == REG_CLEAR))
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
      if(device_info.func_onoff.jingsha)
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
                Sdwe_disDigi(PAGE_ECHO_SENSE_ZERO1 + readSlave.addr - 1,readSlave.hx711_offset,2);
                if(SlavePara.value_compare[readSlave.addr - 1] > 0)
                {//ֻ������ֵ���ò�Ϊ0ʱ���Ƚϲ���Ч
                  if(readSlave.value >= SlavePara.value_compare[readSlave.addr - 1])
                  {
                    RELAY_CLOSE();//�ɼ����������趨����ʱ���պϼ̵���������ֹͣ����
                  }
                }
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
//      isWork = GetDeviceState();
      if((speed_zhu > 50) || (speed_zhu1 > 50))
        isWork = 1;
      else
        isWork = 0;
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
      if(device_info.func_onoff.jingsha)
      {
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
    }
    Task_iwdg_refresh(TASK_Rev485);
  }
}

static void vTaskReadDisk(void *pvParameters)
{
  BaseType_t xResult;
  FRESULT result;
  FATFS fs;
  FIL file;
  uint32_t bw;
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
      {//U�̴���
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
        
        if(readDisk == DISK_FILENAME)
        {//��ʾU���ļ���
          DiskFile_init();
          Sdwe_disPicture(PAGE_U);//��ת��U���ļ�ҳ��
          memset(read_file_select,0,10);//����ļ�ѡ��Buff
          Sdwe_ClearReadDisk();//����ļ�ѡ��ͼ��
          
          /* ��ȡ��ǰ�ļ����µ��ļ���Ŀ¼ */
        #if _USE_LFN
          FileInf.lfname = lfname;
          FileInf.lfsize = 256;
        #endif
          printf("����        |  �ļ���С | ���ļ��� | ���ļ���\r\n");
          for(cnt = 0; ;cnt++)
          {
            result = f_readdir(&DirInf,&FileInf); 		/* ��ȡĿ¼��������Զ����� */
            if(result != FR_OK || FileInf.fname[0] == 0)
            {
              break;
            }
            if(FileInf.fname[0] == '.')
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
              if(Disk_File.filenum < 10)
              {
                char *fn;
                //�ж��ǳ��ļ������Ƕ��ļ�������ǳ��ļ�����ȡ���ļ��������ȥ���ļ�������Ϣ
                fn = (*FileInf.lfname?FileInf.lfname:FileInf.fname);
                if(strstr(fn,".CSV"))
                {//�ļ�ΪCSV���
                  memcpy(Disk_File.filename[Disk_File.filenum],fn,strlen(fn));
//                  Disk_File.filename[Disk_File.filenum][strlen(fn) - 3] = '\0';
                  Sdwe_disString(PAGE_U_TEXT_FILENAME1 + Disk_File.filenum * 20,fn,strlen(fn));
                  Disk_File.filenum++;
                }
              }
            }
            /* ��ӡ�ļ���С, ���4G */
            printf(" %10d", FileInf.fsize);
            printf("  %s |", FileInf.fname);	/* ���ļ��� */
            printf("  %s\r\n", (char *)FileInf.lfname);	/* ���ļ��� */
          }
        }
        else if(readDisk == DISK_FILEDATA)
        {//��ȡU���ļ�
          char name[20];
          u8 file_buf[1000];
//          file_buf = mymalloc(SRAMIN,1000);
          sprintf(name,"2:/%s",Disk_File.filename[readFilenum]);
          /* ���ļ� */
          result = f_open(&file,name,FA_OPEN_EXISTING | FA_READ);
          if (result !=  FR_OK)
          {
            printf("Don't Find File : %s\r\n",name);
            return;
          }
          
          /* ��ȡ�ļ� */
          result = f_read(&file, &file_buf, sizeof(file_buf) - 1, &bw);
          if(result == FR_OK)
          {
            __set_PRIMASK(1); 
            read_from_disk((char *)file_buf);
            __set_PRIMASK(0); 
            SDWE_WARNNING(PAGE_U_TEXT_READ_STATE,"��ȡ�ɹ�",3);
          }
          else
          {
            SDWE_WARNNING(PAGE_U_TEXT_READ_STATE,"��ȡʧ��",3);
          }
//          myfree(SRAMIN,file_buf);
          /* �ر��ļ�*/
          f_close(&file);
        }
        /* ж���ļ�ϵͳ */
        f_mount(NULL, "2:", 0);
      }
      else
      {
        SDWE_WARNNING(PAGE_HISTORY_TEXT_FILE_WARN,"��U��",2);
      }
    }
    Task_iwdg_refresh(TASK_ReadDisk);
  }
}

void vTaskManageCapacity(void *pvParameters)
{
  u16 count = 0;
  BaseType_t xResult;
  u16 p_value = 0,old_p_value = 1;
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(200); /* �������ȴ�ʱ��Ϊ200ms */
  bsp_InitCloseSW();
  total_meter_gross = (u32)(peiliao_para.total_meter_set * (1 + peiliao_para.loss / 100.0));//�������ú����
  total_weight_gross = (u32)(peiliao_para.total_weitht_set * (1 + peiliao_para.loss / 100.0));//���������ú����
  if(total_meter_gross > 0)
  {
    if(product_para.product_complete >= total_meter_gross * 10)
    {
      plan_complete = 1;
    }
  }
  if(total_weight_gross > 0)
  {
    if(product_para.weight_complete >= total_weight_gross * 10)
    {
      plan_complete = 1;
    }
  }
  Sdwe_disString(PAGE1_SYSTEM_STATE,(u8 *)system_state_dis[device_info.system_state],strlen(system_state_dis[device_info.system_state]));
  while(1)
  {
    xResult = xSemaphoreTake(xSemaphore_pluse, (TickType_t)xMaxBlockTime);
    if(xResult == pdTRUE)
    {
      if((device_info.func_onoff.channeng == 1) && (isDevicePeriod == 0))
      {//����ʹ�ܡ�����������
        pluse_count++;
        work_idle_time = 0;
        if(device_info.system_state == SYS_NORMAL)
        {//ϵͳ����ʱ���в��ܼ���
          switch(plan_complete)
          {
            case 0:
              RELAY_OPEN();//�������źź󣬼̵����Ͽ�
              count++;
              printf("wei\r\n");
              product_para.weicount_kilowei = product_para.weicount_kilowei + count / peiliao_para.weimi_dis_set;
              if((count % (peiliao_para.weimi_dis_set)) == 0)
              {//γɴ/ǧγ
                count = 0;
                Sdwe_disDigi(PAGE_PRODUCT_KILOCOUNT,product_para.weicount_kilowei,4);
//                W25QXX_Write((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
              }
              if((total_meter_gross == 0) && (total_weight_gross == 0))
              {
                
              }
              else
              {
                if(card_record == 2)
                {//B��
                  product_para.product_b = product_para.product_b + product_per_meter(peiliao_para,1) * 10;//b�����
                  p_value = get_float_1bit(product_para.product_b);
                }
                else
                {//A��
                  product_para.product_a = product_para.product_a + product_per_meter(peiliao_para,1) * 10;//b�����
                  p_value = get_float_1bit(product_para.product_a);
                }
                if(p_value != old_p_value)
                {//�������ݱ仯����=0.1ʱ����ʾ����
                  old_p_value = p_value;
                  Sdwe_disDigi(PAGE_PRODUCT_B,product_para.product_b,4);
                  Sdwe_disDigi(PAGE_PRODUCT_A,product_para.product_a,4);
                  if(total_meter_gross > 0)
                  {
                    product_para.product_complete = product_complete_meter(product_para);//����ɲ���
                    product_para.product_uncomplete = total_meter_gross * 10 - product_para.product_complete;//δ��ɲ���
                    Sdwe_disDigi(PAGE_PRODUCT_COMPLETE,product_para.product_complete,4);
                    Sdwe_disDigi(PAGE_PRODUCT_UNCOMPLETE,product_para.product_uncomplete,4);
                    if((product_para.product_uncomplete <= 0) || (product_para.product_complete >= total_meter_gross * 10))
                    {//��ɲ���
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
                      device_info.system_state = SYS_STOP;
                      Sdwe_disString(PAGE1_SYSTEM_STATE,(u8 *)system_state_dis[device_info.system_state],strlen(system_state_dis[device_info.system_state]));
                    }
                  }
                  else
                  {
                    product_para.product_complete = 0;//����ɲ���
                    product_para.product_uncomplete = 0;//δ��ɲ���
                    Sdwe_disDigi(PAGE_PRODUCT_COMPLETE,product_para.product_complete,4);
                    Sdwe_disDigi(PAGE_PRODUCT_UNCOMPLETE,product_para.product_uncomplete,4);
                  }
                  if(total_weight_gross > 0)
                  {
                    product_para.weight_complete = product_complete_kilo(product_para,peiliao_para);//���������
                    product_para.weight_uncomplete = total_weight_gross * 10 - product_para.weight_complete;//δ�������
                    Sdwe_disDigi(PAGE_PRODUCT_COMPLETE_W,product_para.weight_complete,4);
                    Sdwe_disDigi(PAGE_PRODUCT_UNCOMPLETE_W,product_para.weight_uncomplete,4);
                    if((product_para.weight_uncomplete <= 0) || (product_para.weight_complete >= total_weight_gross * 10))
                    {//�������
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
                      device_info.system_state = SYS_STOP;//������ɺ�ϵͳֹͣ����
                      Sdwe_disString(PAGE1_SYSTEM_STATE,(u8 *)system_state_dis[device_info.system_state],strlen(system_state_dis[device_info.system_state]));
                    }
                  }
                  else
                  {
                    product_para.weight_complete = 0;//���������
                    product_para.weight_uncomplete = 0;//δ�������
                    Sdwe_disDigi(PAGE_PRODUCT_COMPLETE_W,product_para.weight_complete,4);
                    Sdwe_disDigi(PAGE_PRODUCT_UNCOMPLETE_W,product_para.weight_uncomplete,4);
                  }
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
//    printf("-->free %f K", xPortGetFreeHeapSize()/1024.0); 
    Task_iwdg_refresh(TASK_ManageCapacity);
  }
}

static void vTaskMotorControl(void *pvParameters)
{
  BaseType_t xResult;
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(200); /* �������ȴ�ʱ��Ϊ200ms */
  vTaskDelay(200);
  TIM4_CH1_ConfigPwmOut(FREQ_500KHZ,10);
  get_weimi_para(&weimi_para,&device_info,&MotorProcess);//��ȡ��ǰ����
  valid_seg[0] = get_valid_seg(weimi_para);
  valid_seg[1] = get_songwei0_maxseg(weimi_para);
  valid_seg[2] = get_songwei1_maxseg(weimi_para);
  valid_seg[3] = get_songwei2_maxseg(weimi_para);
  max_seg = get_max_type(valid_seg);
//  servomotor_guodu = device_info.weimi_info.guodu_flag[0];
//  stepmotor_guodu[0] = device_info.weimi_info.guodu_flag[1];
//  stepmotor_guodu[1] = device_info.weimi_info.guodu_flag[2];
//  stepmotor_guodu[2] = device_info.weimi_info.guodu_flag[3];
  if(max_seg == 0)
  {
    if(servomotor_guodu == 0)
      Sdwe_disDigi(PAGE_WEIMI_REALWEI_1 + MotorProcess.current_seg * 2,MotorProcess.current_wei,4);
    else
      Sdwe_disDigi(PAGE_WEIMI_REAL_MEDIAN_1 + MotorProcess.current_seg * 2,MotorProcess.current_wei,4);
  }
  else if(max_seg == 1)
  {
    if(stepmotor_guodu[0] == 0)
      Sdwe_disDigi(PAGE_WEIMI_REALWEI_1 + MotorProcess.songwei_seg[0] * 2,MotorProcess.song_current_wei[0],4);
    else
      Sdwe_disDigi(PAGE_WEIMI_REAL_MEDIAN_1 + MotorProcess.songwei_seg[0] * 2,MotorProcess.song_current_wei[0],4);
  }
  else if(max_seg == 2)
  {
    if(stepmotor_guodu[1] == 0)
      Sdwe_disDigi(PAGE_WEIMI_REALWEI_1 + MotorProcess.songwei_seg[1] * 2,MotorProcess.song_current_wei[1],4);
    else
      Sdwe_disDigi(PAGE_WEIMI_REAL_MEDIAN_1 + MotorProcess.songwei_seg[1] * 2,MotorProcess.song_current_wei[1],4);
  }
  else
  {
    if(stepmotor_guodu[2] == 0)
      Sdwe_disDigi(PAGE_WEIMI_REALWEI_1 + MotorProcess.songwei_seg[2] * 2,MotorProcess.song_current_wei[2],4);
    else
      Sdwe_disDigi(PAGE_WEIMI_REAL_MEDIAN_1 + MotorProcess.songwei_seg[2] * 2,MotorProcess.song_current_wei[2],4);
  }
  while(1)
  {
    xResult = xSemaphoreTake(xSemaphore_encoder, (TickType_t)xMaxBlockTime);
    if(xResult == pdTRUE)
    {
      if(device_info.func_onoff.channeng)
      {
        if(servomotor_mode == AUTO)
        {//�Զ�ģʽ�£��Ż�����γ��
          if(fault_weimi_flag == 0)
          {
            MotorProcess.current_wei++;//γ������
            
            weimi_para.real_wei_count[MotorProcess.current_seg] = MotorProcess.current_wei;
            if(MotorProcess.step_factor[0] > 0)
              MotorProcess.song_current_wei[0]++;
            if(MotorProcess.step_factor[1] > 0)
              MotorProcess.song_current_wei[1]++;
            if(MotorProcess.step_factor[2] > 0)
              MotorProcess.song_current_wei[2]++;
            /*************���ڵ��籣��*********************/
            write_bkp_para(&MotorProcess);
            /*************************************************/
            if(max_seg == 0)
            {
              if(servomotor_guodu == 0)
                Sdwe_disDigi(PAGE_WEIMI_REALWEI_1 + MotorProcess.current_seg * 2,MotorProcess.current_wei,4);
              else
                Sdwe_disDigi(PAGE_WEIMI_REAL_MEDIAN_1 + MotorProcess.current_seg * 2,MotorProcess.current_wei,4);
            }
            else if(max_seg == 1)
            {
              if(stepmotor_guodu[0] == 0)
                Sdwe_disDigi(PAGE_WEIMI_REALWEI_1 + MotorProcess.songwei_seg[0] * 2,MotorProcess.song_current_wei[0],4);
              else
                Sdwe_disDigi(PAGE_WEIMI_REAL_MEDIAN_1 + MotorProcess.songwei_seg[0] * 2,MotorProcess.song_current_wei[0],4);
            }
            else if(max_seg == 2)
            {
              if(stepmotor_guodu[1] == 0)
                Sdwe_disDigi(PAGE_WEIMI_REALWEI_1 + MotorProcess.songwei_seg[1] * 2,MotorProcess.song_current_wei[1],4);
              else
                Sdwe_disDigi(PAGE_WEIMI_REAL_MEDIAN_1 + MotorProcess.songwei_seg[1] * 2,MotorProcess.song_current_wei[1],4);
            }
            else
            {
              if(stepmotor_guodu[2] == 0)
                Sdwe_disDigi(PAGE_WEIMI_REALWEI_1 + MotorProcess.songwei_seg[2] * 2,MotorProcess.song_current_wei[2],4);
              else
                Sdwe_disDigi(PAGE_WEIMI_REAL_MEDIAN_1 + MotorProcess.songwei_seg[2] * 2,MotorProcess.song_current_wei[2],4);
            }
            
            /**********************************************************************************/
            if(servomotor_guodu > 0)
            {
              float sevro_speed;
              u32 sevrostep;//�������ת�٣�ת/����
              if(symbol_servo == 0)//�ٶ�����
                sevro_speed = speed_zhu * (servo_per_wei_src + (float)servo_speed_diff / MotorProcess.total_wei * MotorProcess.current_wei);
              else//�ٶȼ���
                sevro_speed = speed_zhu * (servo_per_wei_src - (float)servo_speed_diff / MotorProcess.total_wei * MotorProcess.current_wei);
              sevrostep = get_sevro_step(sevro_speed);//�����ٶȶ�Ӧ��������
              ServoMotor_adjust_speed(sevrostep); //��ʱ������PWM
              //                printf("motor%d speed %d guodu is %d\r\n",i,speed_zhu,sstep);
            }
            if(MotorProcess.current_wei >= MotorProcess.total_wei)
            {
              MotorProcess.current_wei = 0;
              if(MotorProcess.current_seg == 9)
                MotorProcess.wei_cm_set = weimi_para.wei_cm_set[0];
              else
                MotorProcess.wei_cm_set = weimi_para.wei_cm_set[MotorProcess.current_seg + 1];
              if(MotorProcess.wei_cm_set >= 2)
              {//�ٱȴ���0
                if(servomotor_guodu == 0)
                {//����ǹ��ɶΣ���γ��Ϊ�ι���ѭ����
                  MotorProcess.total_wei = weimi_para.total_wei_count[MotorProcess.current_seg * 2 + 1];
                  if(MotorProcess.total_wei == 0)//����ѭ��������Ϊ0������
                  {
                    fault_weimi_flag = 1;
                    continue;
                  }
                  if(MotorProcess.current_seg == 9)
                  {
                    if(weimi_para.wei_cm_set[0] < weimi_para.wei_cm_set[MotorProcess.current_seg])
                    {//γ��ԽС�ٶ�Խ��
                      symbol_servo = 0;
                      servo_diff = 1.0 / weimi_para.wei_cm_set[0] - 1.0 / weimi_para.wei_cm_set[MotorProcess.current_seg];
                    }
                    else
                    {
                      symbol_servo = 1;
                      servo_diff = 1.0 / weimi_para.wei_cm_set[MotorProcess.current_seg] - 1.0 / weimi_para.wei_cm_set[0];
                    }
                  }
                  else
                  {
                    if(weimi_para.wei_cm_set[MotorProcess.current_seg + 1] < weimi_para.wei_cm_set[MotorProcess.current_seg])
                    {
                      symbol_servo = 0;
                      servo_diff = 1.0 / weimi_para.wei_cm_set[MotorProcess.current_seg + 1] - 1.0 / weimi_para.wei_cm_set[MotorProcess.current_seg];
                    }
                    else
                    {
                      symbol_servo = 1;
                      servo_diff = 1.0 / weimi_para.wei_cm_set[MotorProcess.current_seg] - 1.0 / weimi_para.wei_cm_set[MotorProcess.current_seg + 1];
                    }
                  }
                  servo_speed_diff = servo_diff * device_info.ratio.GEAR1;//�������ڶκŲ����������Ƶ�ʲ�
                  servo_per_wei_src = 1.0 / weimi_para.wei_cm_set[MotorProcess.current_seg] * device_info.ratio.GEAR1;
                  servomotor_guodu = 1;
//                  device_info.weimi_info.guodu_flag[0] = servomotor_guodu;
                }
                else
                {//����ǶΣ���γ��Ϊ��ѭ����
                  MotorProcess.current_seg++;//������һ�κ�
                  if(MotorProcess.current_seg >= 10)
                  {
                    MotorProcess.current_seg = 0;
                  }
                  MotorProcess.total_wei = weimi_para.total_wei_count[MotorProcess.current_seg * 2];
                  if(MotorProcess.total_wei == 0)//ѭ��������Ϊ0������
                  {
                    fault_weimi_flag = 1;
                    continue;
                  }
                  servomotor_guodu = 0;
//                  device_info.weimi_info.guodu_flag[0] = servomotor_guodu;
                }
              }
              else
              {
                MotorProcess.wei_cm_set = weimi_para.wei_cm_set[0];
                if(servomotor_guodu == 0)
                {
                  if(MotorProcess.current_seg == 0)
                  {
                    MotorProcess.total_wei = weimi_para.total_wei_count[0];
                  }
                  else
                  {
                    MotorProcess.total_wei = weimi_para.total_wei_count[MotorProcess.current_seg * 2 + 1];
                    if(MotorProcess.total_wei == 0)//����ѭ��������Ϊ0������
                    {
                      fault_weimi_flag = 1;
                      continue;
                    }
                    if(weimi_para.wei_cm_set[0] < weimi_para.wei_cm_set[MotorProcess.current_seg])
                    {
                      symbol_servo = 0;
                      servo_diff = 1.0 / weimi_para.wei_cm_set[0] - 1.0 / weimi_para.wei_cm_set[MotorProcess.current_seg];
                    }
                    else
                    {
                      symbol_servo = 1;
                      servo_diff = (1.0 / weimi_para.wei_cm_set[MotorProcess.current_seg]) - (1.0 / weimi_para.wei_cm_set[0]);
                    }
                    servo_speed_diff = servo_diff * device_info.ratio.GEAR1;//�������ڶκŲ����������Ƶ�ʲ�
                    servo_per_wei_src = 1.0 / weimi_para.wei_cm_set[MotorProcess.current_seg] * device_info.ratio.GEAR1;
                    servomotor_guodu = 1;
//                    device_info.weimi_info.guodu_flag[0] = servomotor_guodu;
                  }
                }
                else
                {//����ǶΣ���γ��Ϊ��ѭ����
                  MotorProcess.current_seg = 0;//������һ�κ�
                  MotorProcess.total_wei = weimi_para.total_wei_count[0];
                  servomotor_guodu = 0;
//                  device_info.weimi_info.guodu_flag[0] = servomotor_guodu;
                }
              }
            }
            
            u8 i;
            u16 ratio_diff[3];
            for(i=0;i<3;i++)
            {
              if(stepmotor_guodu[i] > 0)
              {
                float step_speed;
                u32 sstep;//�������ת�٣�ת/����
                if(symbol[i] == 0)//�ٶ�����
                  step_speed = speed_zhu * (pluse_step_src[i] + (float)speed_diff[i] / MotorProcess.song_total_wei[i] * MotorProcess.song_current_wei[i]);
                else//�ٶȼ���
                  step_speed = speed_zhu * (pluse_step_src[i] - (float)speed_diff[i] / MotorProcess.song_total_wei[i] * MotorProcess.song_current_wei[i]);
                sstep = from_speed_step(step_speed);//�����ٶȶ�Ӧ��������
                StepMotor_adjust_speed(STEPMOTOR1 + i,sstep); //��ʱ������PWM
//                printf("motor%d speed %d guodu is %d\r\n",i,speed_zhu,sstep);
              }
              if(MotorProcess.song_current_wei[i] >= MotorProcess.song_total_wei[i])
              {
                MotorProcess.song_current_wei[i] = 0;
                if(MotorProcess.songwei_seg[i] == 9)
                  MotorProcess.step_factor[i] = weimi_para.step_factor[i][0];
                else
                  MotorProcess.step_factor[i] = weimi_para.step_factor[i][MotorProcess.songwei_seg[i] + 1];
                if(MotorProcess.step_factor[i] >= 2)
                {//�ٱȴ���0
                  if(stepmotor_guodu[i] == 0)
                  {//����ǹ��ɶΣ���γ��Ϊ�ι���ѭ����
                    MotorProcess.song_total_wei[i] = weimi_para.total_wei_count[MotorProcess.songwei_seg[i] * 2 + 1];
                    if(MotorProcess.song_total_wei[i] == 0)//����ѭ��������Ϊ0������
                    {
                      fault_weimi_flag = 1;
                      continue;
                    }
                    if(MotorProcess.songwei_seg[i] == 9)
                    {
                      if(weimi_para.step_factor[i][0] >= weimi_para.step_factor[i][MotorProcess.songwei_seg[i]])
                      {
                        symbol[i] = 0;
                        ratio_diff[i] = weimi_para.step_factor[i][0] - weimi_para.step_factor[i][MotorProcess.songwei_seg[i]];
                      }
                      else
                      {
                        symbol[i] = 1;
                        ratio_diff[i] = weimi_para.step_factor[i][MotorProcess.songwei_seg[i]] - weimi_para.step_factor[i][0];
                      }
                    }
                    else
                    {
                      if(weimi_para.step_factor[i][MotorProcess.songwei_seg[i] + 1] >= weimi_para.step_factor[i][MotorProcess.songwei_seg[i]])
                      {
                        symbol[i] = 0;
                        ratio_diff[i] = weimi_para.step_factor[i][MotorProcess.songwei_seg[i] + 1] - weimi_para.step_factor[i][MotorProcess.songwei_seg[i]];
                      }
                      else
                      {
                        symbol[i] = 1;
                        ratio_diff[i] = weimi_para.step_factor[i][MotorProcess.songwei_seg[i]] - weimi_para.step_factor[i][MotorProcess.songwei_seg[i] + 1];
                      }
                    }
                    speed_diff[i] = ratio_diff[i] * SPEED_RADIO[i];//�������ڶκŲ����������Ƶ�ʲ�
                    pluse_step_src[i] = weimi_para.step_factor[i][MotorProcess.songwei_seg[i]] * SPEED_RADIO[i];
                    stepmotor_guodu[i] = 1;
//                    device_info.weimi_info.guodu_flag[i + 1] = stepmotor_guodu[0];
                  }
                  else
                  {//����ǶΣ���γ��Ϊ��ѭ����
                    MotorProcess.songwei_seg[i]++;//������һ�κ�
                    if(MotorProcess.songwei_seg[i] >= 10)
                    {
                      MotorProcess.songwei_seg[i] = 0;
                    }
                    MotorProcess.song_total_wei[i] = weimi_para.total_wei_count[MotorProcess.songwei_seg[i] * 2];
                    if(MotorProcess.song_total_wei[i] == 0)//ѭ��������Ϊ0������
                    {
                      fault_weimi_flag = 1;
                      continue;
                    }
                    stepmotor_guodu[i] = 0;
//                    device_info.weimi_info.guodu_flag[i + 1] = stepmotor_guodu[0];
                  }
                }
                else
                {
                  MotorProcess.step_factor[i] = weimi_para.step_factor[i][0];
                  if(stepmotor_guodu[i] == 0)
                  {
                    if(MotorProcess.songwei_seg[i] == 0)
                    {
                      MotorProcess.song_total_wei[i] = weimi_para.total_wei_count[0];
                    }
                    else
                    {
                      MotorProcess.song_total_wei[i] = weimi_para.total_wei_count[MotorProcess.songwei_seg[i] * 2 + 1];
                      if(MotorProcess.song_total_wei[i] == 0)//����ѭ��������Ϊ0������
                      {
                        fault_weimi_flag = 1;
                        continue;
                      }
                      if(weimi_para.step_factor[i][0] >= weimi_para.step_factor[i][MotorProcess.songwei_seg[i]])
                      {
                        symbol[i] = 0;
                        ratio_diff[i] = weimi_para.step_factor[i][0] - weimi_para.step_factor[i][MotorProcess.songwei_seg[i]];
                      }
                      else
                      {
                        symbol[i] = 1;
                        ratio_diff[i] = weimi_para.step_factor[i][MotorProcess.songwei_seg[i]] - weimi_para.step_factor[i][0];
                      }
                      speed_diff[i] = ratio_diff[i] * SPEED_RADIO[i];//�������ڶκŲ����������Ƶ�ʲ�
                      pluse_step_src[i] = weimi_para.step_factor[i][MotorProcess.songwei_seg[i]] * SPEED_RADIO[i];
                      stepmotor_guodu[i] = 1;
//                      device_info.weimi_info.guodu_flag[i + 1] = stepmotor_guodu[0];
                    }
                  }
                  else
                  {//����ǶΣ���γ��Ϊ��ѭ����
                    MotorProcess.songwei_seg[i] = 0;//������һ�κ�
                    MotorProcess.song_total_wei[i] = weimi_para.total_wei_count[0];
                    stepmotor_guodu[i] = 0;
//                    device_info.weimi_info.guodu_flag[i + 1] = stepmotor_guodu[0];
                  }
                }
              }
            }
          }
        }
      }
      xSemaphoreGive(xSemaphore_pluse);
    }
    Task_iwdg_refresh(TASK_MotorControl);
  }
}

static void vTaskFreq(void *pvParameters)
{
  u8 step1_stop = 0,old_step1_stop = 0xff;
  u8 step2_stop = 0,old_step2_stop = 0xff;
  u8 step3_stop = 0,old_step3_stop = 0xff;
  u8 servo_stop = 0,old_servo_stop = 0xff;
  u32 step1_count;
  u32 step2_count;
  u32 step3_count;
  u32 servo_count;
  float step_speed;
  u32 sstep;//�������ת�٣�ת/����
  float servo_speed;
  u32 servostep;//�������ת�٣�ת/����
  static portTickType xLastWakeTime;  
  const portTickType xFrequency = pdMS_TO_TICKS(10);  
  
  xLastWakeTime = xTaskGetTickCount(); 

  vTaskDelay(300);
  ENC_Init();//���������ٳ�ʼ��
  while(1)
  {
    vTaskDelayUntil( &xLastWakeTime,xFrequency); //������ʱ
    speed_zhu = ENC_Calc_Average_Speed();
//    printf("speed_zhu is %d\r\n",speed_zhu);
    if(speed_zhu > 0)
    {
      if(device_info.func_onoff.weimi)
      {
        if(fault_weimi_flag == 0)
        {
          is_stop = 1;
          __set_PRIMASK(1);
          step1_count = from_speed_step((float)speed_zhu * MotorProcess.step_factor[0] * SPEED_RADIO[0]);//���㲽���������Ƶ�ʣ�ʵ���ٱ���Ҫ��10�ٳ��ٷֱȣ�����1000��
          step2_count = from_speed_step((float)speed_zhu * MotorProcess.step_factor[1] * SPEED_RADIO[1]);//���㲽���������Ƶ��
          step3_count = from_speed_step((float)speed_zhu * MotorProcess.step_factor[2] * SPEED_RADIO[2]);//���㲽���������Ƶ��
//          servo_count = get_sevro_step((float)speed_zhu / MotorProcess.wei_cm_set * device_info.ratio.GEAR1);//���㲽���������Ƶ��
          if(stepmotor_guodu[0] == 0)
          {
//            step1_count = from_speed_step((float)speed_zhu * MotorProcess.step_factor[0] * SPEED_RADIO[0]);//���㲽���������Ƶ�ʣ�ʵ���ٱ���Ҫ��10�ٳ��ٷֱȣ�����1000��
            if(step1_count == 0)
            {//��γ����ٶ�����Ϊ0
              step1_stop = 0;
              if(step1_stop != old_step1_stop)
              {//ֹͣ��γ�������
                old_step1_stop = step1_stop;
                StepMotor_stop(STEPMOTOR1);
              }
            }
            else
            {//��γ����ٶ����ô���0
              step1_stop = 0xff;
              if(step1_stop != old_step1_stop)
              {//������γ�������
                old_step1_stop = step1_stop;
                StepMotor_start(STEPMOTOR1,step1_count);
              }
              StepMotor_adjust_speed(STEPMOTOR1,step1_count);
//              printf("step1_count %d\r\n",step1_count);
            }
          }
          else
          {
            if(symbol[0] == 0)//�ٶ�����
              step_speed = speed_zhu * (pluse_step_src[0] + (float)speed_diff[0] / MotorProcess.song_total_wei[0] * MotorProcess.song_current_wei[0]);
            else//�ٶȼ���
              step_speed = speed_zhu * (pluse_step_src[0] - (float)speed_diff[0] / MotorProcess.song_total_wei[0] * MotorProcess.song_current_wei[0]);
            sstep = from_speed_step(step_speed);//�����ٶȶ�Ӧ��������
            StepMotor_adjust_speed(STEPMOTOR1,sstep); //��ʱ������PWM
          }
          if(stepmotor_guodu[1] == 0)
          {
//            step2_count = from_speed_step((float)speed_zhu * MotorProcess.step_factor[1] * SPEED_RADIO[1]);//���㲽���������Ƶ��
            if(step2_count == 0)
            {//��γ����ٶ�����Ϊ0
              step2_stop = 0;
              if(step2_stop != old_step2_stop)
              {//ֹͣ��γ�������
                old_step2_stop = step2_stop;
                StepMotor_stop(STEPMOTOR2);
              }
            }
            else
            {//��γ����ٶ����ô���0
              step2_stop = 0xff;
              if(step2_stop != old_step2_stop)
              {//������γ�������
                old_step2_stop = step2_stop;
                StepMotor_start(STEPMOTOR2,step2_count);
              }
              StepMotor_adjust_speed(STEPMOTOR2,step2_count);
//              printf("step2_count %d\r\n",step2_count);
            }
          }
          else
          {
            if(symbol[1] == 0)//�ٶ�����
              step_speed = speed_zhu * (pluse_step_src[1] + (float)speed_diff[1] / MotorProcess.song_total_wei[1] * MotorProcess.song_current_wei[1]);
            else//�ٶȼ���
              step_speed = speed_zhu * (pluse_step_src[1] - (float)speed_diff[1] / MotorProcess.song_total_wei[1] * MotorProcess.song_current_wei[1]);
            sstep = from_speed_step(step_speed);//�����ٶȶ�Ӧ��������
            StepMotor_adjust_speed(STEPMOTOR2,sstep); //��ʱ������PWM
          }
          if(stepmotor_guodu[2] == 0)
          {
//            step3_count = from_speed_step((float)speed_zhu * MotorProcess.step_factor[2] * SPEED_RADIO[2]);//���㲽���������Ƶ��
            if(step3_count == 0)
            {//���3�ٶ�����Ϊ0
              step3_stop = 0;
              if(step3_stop != old_step3_stop)
              {//ֹͣ���3����
                old_step3_stop = step3_stop;
                StepMotor_stop(STEPMOTOR3);
              }
            }
            else
            {//���3�ٶ����ô���0
              step3_stop = 0xff;
              if(step3_stop != old_step3_stop)
              {//�������3����
                old_step3_stop = step3_stop;
                StepMotor_start(STEPMOTOR3,step3_count);
              }
              StepMotor_adjust_speed(STEPMOTOR3,step3_count);
//              printf("step3_count %d\r\n",step3_count);
            }
          }
          else
          {
            if(symbol[2] == 0)//�ٶ�����
              step_speed = speed_zhu * (pluse_step_src[2] + (float)speed_diff[2] / MotorProcess.song_total_wei[2] * MotorProcess.song_current_wei[2]);
            else//�ٶȼ���
              step_speed = speed_zhu * (pluse_step_src[2] - (float)speed_diff[2] / MotorProcess.song_total_wei[2] * MotorProcess.song_current_wei[2]);
            sstep = from_speed_step(step_speed);//�����ٶȶ�Ӧ��������
            StepMotor_adjust_speed(STEPMOTOR3,sstep); //��ʱ������PWM
          }
          if(servomotor_guodu == 0)
          {
            if(MotorProcess.wei_cm_set == 0)
              servo_count = 0;
            else
              servo_count = get_sevro_step((float)speed_zhu / MotorProcess.wei_cm_set * device_info.ratio.GEAR1);//���㲽���������Ƶ��
            if(servo_count == 0)
            {//���3�ٶ�����Ϊ0
              servo_stop = 0;
              if(servo_stop != old_servo_stop)
              {//ֹͣ���3����
                old_servo_stop = servo_stop;
                TIM4_MANUAL_PWM_Stop();
              }
            }
            else
            {//���3�ٶ����ô���0
              servo_stop = 0xff;
              if(servo_stop != old_servo_stop)
              {//�������3����
                old_servo_stop = servo_stop;
                TIM4_MANUAL_PWM_Config(servo_count);
              }
              ServoMotor_adjust_speed(servo_count);
//              printf("step3_count %d\r\n",step3_count);
            }
          }
          else
          {
            if(symbol_servo == 0)//�ٶ�����
              servo_speed = speed_zhu * (servo_per_wei_src + (float)servo_speed_diff / MotorProcess.total_wei * MotorProcess.current_wei);
            else//�ٶȼ���
              servo_speed = speed_zhu * (servo_per_wei_src - (float)servo_speed_diff / MotorProcess.total_wei * MotorProcess.current_wei);
            servostep = get_sevro_step(servo_speed);//�����ٶȶ�Ӧ��������
            ServoMotor_adjust_speed(servostep); //��ʱ������PWM
          }
          if(is_stop != old_is_stop)
          {//�����ٶȴ���0ʱ�����������ʼ����
            old_is_stop = is_stop;
//            SERVO_ENABLE();
            ServoMotor_start(servo_count);
            StepMotor_start(STEPMOTOR2,step2_count);
            StepMotor_start(STEPMOTOR3,step3_count);
            StepMotor_start(STEPMOTOR1,step1_count);
          }
          __set_PRIMASK(0);
        }
        else
        {
          is_stop = 0;
          if(is_stop != old_is_stop)
          {//�����ٶ�Ϊ0ʱ��ֹͣ�������
            old_is_stop = is_stop;
//            SERVO_DISABLE();
            TIM4_MANUAL_PWM_Stop();
            StepMotor_stop(STEPMOTOR2);
            StepMotor_stop(STEPMOTOR3);
            StepMotor_stop(STEPMOTOR1);
          }
        }
      }
      else
      {
        is_stop = 0;
        if(is_stop != old_is_stop)
        {//�����ٶ�Ϊ0ʱ��ֹͣ�������
          old_is_stop = is_stop;
//          SERVO_DISABLE();
          TIM4_MANUAL_PWM_Stop();
          StepMotor_stop(STEPMOTOR2);
          StepMotor_stop(STEPMOTOR3);
          StepMotor_stop(STEPMOTOR1);
        }
      }
    }
    else
    {
      is_stop = 0;
      if(is_stop != old_is_stop)
      {//�����ٶ�Ϊ0ʱ��ֹͣ�������
        old_is_stop = is_stop;
//        SERVO_DISABLE();
        TIM4_MANUAL_PWM_Stop();
        StepMotor_stop(STEPMOTOR2);
        StepMotor_stop(STEPMOTOR3);
        StepMotor_stop(STEPMOTOR1);
      }
    }
    Task_iwdg_refresh(TASK_Freq);
  }
}

/**
******************************************************************************
* @brief  �������߳�
******************************************************************************
**/
void vEsp8266_Main_Task(void *ptr)
{
  esp8266_gpio_init();
  vTaskDelay(3000 / portTICK_RATE_MS);
  while(net_device_send_cmd("AT\r\n", "OK"))
  {
    vTaskDelay(500);
    printf("WIFI DEVICE LOST\r\n");
  }
  while(net_device_send_cmd("ATE0\r\n", "OK"));
  {
    vTaskDelay(500);
    printf("CLOSE ECHO FAILURE\r\n");
  }
  while(ESP8266_Net_Mode_Choose(STA))
  {
    vTaskDelay(500);
    printf("wait for STA...\r\n");
  }
  while(ESP8266_JoinAP(WIFI_SSID,WIFI_PASSWORD))
  {
    vTaskDelay(500);
    printf("wait for AP...\r\n");
  }
  printf("connect to AP success!\r\n");
  wifi_flag = WIFI_CONNECT;
  ESP8266_Enable_MultipleId(DISABLE);
  while(1)
  {
    if((g_esp_status_t.esp_hw_status_e != ESP_HW_CONNECT_OK) && (g_esp_status_t.esp_hw_status_e != ESP_HW_RECONFIG))
    {
      g_esp_status_t.esp_hw_status_e = check_esp8266_status();
    }
    switch(g_esp_status_t.esp_hw_status_e)
    {
    case ESP_HW_RESERVE_0:
    case ESP_HW_RESERVE_1:
    case ESP_HW_CONNECT_OK:
      break;
      
      /* ���IP */
    case ESP_HW_GET_IPADDR:
      getTcpConnect();
      break;
      
      /* ʧȥ���ӣ��Ͽ�tcp���� */
    case ESP_HW_DISCONNECT:
      esp_error_t.err_esp_disconnect++;
      g_esp_status_t.esp_net_work_e = ESP_NETWORK_FAILED;
      //            SetLedConnectNet(LED_NETWORK_OK);
      printf("TCP CLOSEED\r\n");
      vTaskDelay(3000);//3����������ӷ�����
      /* ��������TCP������ */
      if(g_esp_status_t.esp_hw_status_e != ESP_HW_RECONFIG)	//�����������״̬���������������
      {
        getTcpConnect();
      }
      break;
    case ESP_HW_LOST_WIFI:
      wifi_flag = NO_CONNECT;
      esp_error_t.err_esp_lostwifi++;
      printf("ESP8266 Lost\r\n");			//�豸��ʧ
      ESP8266_JoinAP(WIFI_SSID,WIFI_PASSWORD);
      break;
    case ESP_HW_BUSY_STUS:
      printf("--%s",netDeviceInfo_t.cmd_resp);
      break;
    case ESP_HW_NOT_RESP:
      esp_error_t.err_esp_notresp++;
      printf("Esp8266 send timeOut!\r\n");			//�豸��ʧ
      break;
    default:
      break;
    }
    if(g_esp_status_t.esp_net_work_e == 0 )
    {
      esp_error_t.err_esp_network++;
    }
    if(esp_error_t.err_esp_disconnect > ESP_ERROR_CNT || esp_error_t.err_esp_lostwifi >
       ESP_ERROR_CNT || esp_error_t.err_esp_notresp > ESP_ERROR_CNT || esp_error_t.err_esp_network > ESP_ERROR_CNT )
    {
      //RESTART SYSTEM;
      Mqtt_status_step = MQTT_IDLE;
      esp_error_t.err_esp_disconnect = 0;
      esp_error_t.err_esp_lostwifi = 0;
      esp_error_t.err_esp_notresp = 0;
      esp_error_t.err_esp_network = 0;
      //						ReconfigWIFI();
    }
    
    printf("esp_hw_status_e=%d,esp_net_work_e=%d",g_esp_status_t.esp_hw_status_e,g_esp_status_t.esp_net_work_e);
    vTaskDelay(2000);
    if(wifi_flag != old_wifi_flag)
    {
      old_wifi_flag = wifi_flag;
      Sdwe_writeIcon(PAGE_WIFI_STATE,wifi_flag);
    }
  }
}

void vMQTT_Handler_Task(void *ptr)
{
  int rc = -1;
  while(1)
  {
    switch(Mqtt_status_step)
    {
    case MQTT_IDLE:
      {
        wifi_flag = WIFI_CONNECT;
        MqttHandle();
        memset(mqttSubscribeData,0,sizeof(mqttSubscribeData));
        Mqtt_status_step = MQTT_CONNECT;
        g_transmit = 0;
        printf("mqtt satus is idle");
      }
      break;
    case MQTT_CONNECT:
      {
        if(g_esp_status_t.esp_hw_status_e == ESP_HW_CONNECT_OK)
        {
          lastSendOutTime = xTaskGetTickCount();
          rc = MQTT_Connect();
          if(rc == -1)
          {
            vTaskDelay(200 / portTICK_RATE_MS);
            continue;
          }
          else
          {
            /* ������ӳɹ�������붩��״̬*/
            g_esp_status_t.esp_net_work_e = ESP_NETWORK_SUCCESS;
            Mqtt_status_step = MQTT_SUBSCRB;
            memset(mqttSubscribeData,0,sizeof(mqttSubscribeData));
          }
        }
        vTaskDelay(200 / portTICK_RATE_MS);
      }
      break;
    case MQTT_SUBSCRB:
      {
        lastSendOutTime =  xTaskGetTickCount();
        rc = MQTT_Subscribe(MQTT_TOPIC);
        if(rc != -1)
        {
          /* ������ĳɹ�������뷢��״̬*/
          Mqtt_status_step = MQTT_PUBLSH;
          wifi_flag = SEVER_CONNECT_OK;
          memset(mqttSubscribeData,0,sizeof(mqttSubscribeData));
        }
        else
        {
          printf("MQTT Subscribe Error");
        }
        vTaskDelay(200 / portTICK_RATE_MS);
      }
      break;
    case MQTT_PUBLSH:
      {
        g_transmit = 1;
        MQTT_Read();
        if(g_esp_status_t.esp_net_work_e != ESP_NETWORK_SUCCESS)
        {
          printf("mqtt connect is failed");
          Mqtt_status_step = MQTT_IDLE;
          wifi_flag = WIFI_CONNECT;
        }
      }
      break;
    default:
      break;
    }
    vTaskDelay(1000);
  }
}

/**
******************************************************************************
* @brief  ����ESP8266���ڷ��������߳�
******************************************************************************
**/
void vAnalysisUartData(void *ptr)
{
  signed portBASE_TYPE pd;
  //    xSemaphore_CmdLine = xSemaphoreCreateMutex();
  while(1)
  {
    pd = xQueueReceive(xQueue_esp8266_Cmd, gUsartReciveLineBuf, portMAX_DELAY);
    if(pd != pdTRUE)
    {
      printf("pd != pdTRUE\r\n");
      break;
    }
    if(strstr(gUsartReciveLineBuf,"CLOSED") != NULL)
    {
      g_esp_status_t.esp_hw_status_e = ESP_HW_DISCONNECT;
      g_esp_status_t.esp_net_work_e = ESP_NETWORK_FAILED;
      printf("TCP CLOSED!");
    }
    else if(strstr(gUsartReciveLineBuf,"+IPD") != NULL)
    {
      /* ������������ */
      Handle_Internet_Data(gUsartReciveLineBuf);
    }
    else
    {
      if(g_esp_status_t.esp_hw_status_e != ESP_HW_CONNECT_OK || g_esp_status_t.esp_hw_status_e != ESP_HW_RECONFIG)
      {
        /* ESP8266ָ������ */
        esp8266_cmd_handle(gUsartReciveLineBuf);
      }
      else
      {
        printf("MCU unknown how prosess!\r\n");
        netDeviceInfo_t.cmd_hdl = NULL;
        memset(gUsartReciveLineBuf,0,sizeof(gUsartReciveLineBuf));
      }
    }
    memset(gUsartReciveLineBuf,0,sizeof(gUsartReciveLineBuf));
//    vTaskDelay(20 / portTICK_RATE_MS);
  }
}

void vMQTT_Recive_Task(void *ptr)
{
  MQTT_Recv_t mqtt_recv_t;
  signed portBASE_TYPE pd;
  u16 i;
  while(1)
  {
    pd = xQueueReceive(xQueue_MQTT_Recv, &mqtt_recv_t, 20 / portTICK_RATE_MS);
    if(pd != NULL)
    {
      u8 buf[50];
      memset(buf,0,50);
      memcpy(buf,mqtt_recv_t.receivedTopic.lenstring.data,mqtt_recv_t.receivedTopic.lenstring.len);
      printf("mqtt recv topic:%s,length:%d\r\n",buf,mqtt_recv_t.payloadlen_in);
      for(i=0;i<mqtt_recv_t.payloadlen_in;i++)
        printf("%c",mqtt_recv_t.payload_in[i]);
      printf("\r\n");
    }
    vTaskDelay(20);
  }
}

void vMQTT_Tranmit_Task(void *ptr)
{
  u8 sendbuf[200],sendlen = 0;
  u8 func;
  signed portBASE_TYPE pd;
  while(1)
  {
    pd = xQueueReceive(xQueue_MQTT_Transmit, &func, 200);
    if(pd != NULL)
    {
      if(func == TOPIC_WEIMI)
      {
        sendlen = WeimiMQTTPackage(sendbuf);
        MQTT_Package_Publish(MQTT_TOPIC_WEIMI,sendbuf,sendlen);
      }
      else if(func == TOPIC_WEISHA)
      {
        sendlen = WeishaMQTTPackage(sendbuf);
        MQTT_Package_Publish(MQTT_TOPIC_WEISHA,sendbuf,sendlen);
      }
      else if(func == TOPIC_CHANNENG)
      {
        sendlen = ChannengMQTTPackage(sendbuf);
        MQTT_Package_Publish(MQTT_TOPIC_CHANNENG,sendbuf,sendlen);
      }
      else if(func == TOPIC_PEILIAO)
      {
        sendlen = PeiliaoMQTTPackage(sendbuf);
        MQTT_Package_Publish(MQTT_TOPIC_PEILIAO,sendbuf,sendlen);
      }
      else if(func == TOPIC_STOP)
      {
        sendlen = SystemStateMQTTPackage(sendbuf);
        MQTT_Package_Publish(MQTT_TOPIC_STOP,sendbuf,sendlen);
      }
    }
    vTaskDelay(1000);
  }
}

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
        case KEY_DOWN_K1:
          if(is_stop == 0)
          {
            servomotor_mode = MANUAL;
            SERVO_FORWARD();
            TIM4_MANUAL_PWM_Config(FREQ_37_5KHZ);
            SERVO_ENABLE();
          }
          break;
        case KEY_LONG_K1:
          break;
        case KEY_UP_K1:
          if(is_stop == 0)
          {
            servomotor_mode = AUTO;
            TIM4_MANUAL_PWM_Stop();
            SERVO_FORWARD();
          }
          break;  
      }
    }
    vTaskDelay(10);
    bsp_KeyScan();
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
  xTaskCreate( vTaskTaskKey,            /* ������  */
              "vTaskTaskKey",           /* ������    */
              128,                     /* stack��С����λword��Ҳ����4�ֽ� */
              NULL,                    /* �������  */
              2,                       /* �������ȼ�*/
              &xHandleTaskKey);  /* ������  */
  
  xTaskCreate( vTaskTaskLCD,   	/* ������  */
              "vTaskLCD",     	/* ������    */
              1224,               	/* ����ջ��С����λword��Ҳ����4�ֽ� */
              NULL,              	/* �������  */
              3,                 	/* �������ȼ�*/
              &xHandleTaskLCD );  /* ������  */
  xTaskCreate( vTaskMsgPro,     		/* ������  */
              "vTaskMsgPro",   		/* ������    */
              128,             		/* ����ջ��С����λword��Ҳ����4�ֽ� */
              NULL,           		/* �������  */
              4,               		/* �������ȼ�*/
              &xHandleTaskMsgPro );  /* ������  */
  xTaskCreate( vTaskRev485,     		/* ������  */
              "vTaskRev485",   		/* ������    */
              128,            		/* ����ջ��С����λword��Ҳ����4�ֽ� */
              NULL,           		/* �������  */
              5,              		/* �������ȼ�*/
              &xHandleTaskRev485 );   /* ������  */
  xTaskCreate( vTaskTaskRFID,   	/* ������  */
              "vTaskTaskRFID",     	/* ������    */
              512,               	/* ����ջ��С����λword��Ҳ����4�ֽ� */
              NULL,              	/* �������  */
              6,                 	/* �������ȼ�*/
              &xHandleTaskRFID );  /* ������  */
  xTaskCreate( vTaskMassStorage,    		/* ������  */
              "vTaskMassStorage",  		/* ������    */
              1024,         		/* ����ջ��С����λword��Ҳ����4�ֽ� */
              NULL,        		/* �������  */
              7,           		/* �������ȼ�*/
              &xHandleTaskMassStorage ); /* ������  */
  xTaskCreate( vTaskReadDisk,    		/* ������  */
              "vTaskReadDisk",  		/* ������    */
              1124,         		/* ����ջ��С����λword��Ҳ����4�ֽ� */
              NULL,        		/* �������  */
              8,           		/* �������ȼ�*/
              &xHandleTaskReadDisk); /* ������  */
  xTaskCreate( vTaskManageCapacity,    		/* ������  */
              "vTaskManageCapacity",  		/* ������    */
              512,         		/* ����ջ��С����λword��Ҳ����4�ֽ� */
              NULL,        		/* �������  */
              9,           		/* �������ȼ�*/
              &xHandleTaskManageCapacity); /* ������  */
  xTaskCreate( vTaskMotorControl,    		/* ������  */
              "vTaskMotorControl",  		/* ������    */
              512,         		/* ����ջ��С����λword��Ҳ����4�ֽ� */
              NULL,        		/* �������  */
              10,           		/* �������ȼ�*/
              &xHandleTaskMotorControl); /* ������  */
  xTaskCreate( vTaskFreq,    		/* ������  */
              "vTaskFreq",  		/* ������    */
              256,         		/* ����ջ��С����λword��Ҳ����4�ֽ� */
              NULL,        		/* �������  */
              11,           		/* �������ȼ�*/
              &xHandleTaskFreq); /* ������  */
    xTaskCreate( vAnalysisUartData,   	"vAnalysisUartData",  	256, NULL, 12, NULL);
    xTaskCreate( vEsp8266_Main_Task,   	"vEsp8266_Main_Task",  	128, NULL, 13, NULL);
    xTaskCreate( vMQTT_Handler_Task,   	"vMQTT_Handler_Task",  	256, NULL, 14, NULL);
    xTaskCreate( vMQTT_Recive_Task,   	"vMQTT_Recive_Task",  	256, NULL, 15, NULL);
    xTaskCreate( vMQTT_Tranmit_Task,   	"vMQTT_Tranmit_Task",  	128, NULL, 16, NULL);
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
  xQueue_esp8266_Cmd = xQueueCreate(1, 1000);
  xQueue_MQTT_Recv = xQueueCreate(5, sizeof(MQTT_Recv_t));
  xQueue_MQTT_Transmit = xQueueCreate(5, sizeof(char));
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
  
  xSemaphore_encoder = xSemaphoreCreateBinary();
  
  if(xSemaphore_encoder == NULL)
  {
    printf("xSemaphore_encoder fault\r\n");
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
  xQueue_message = xQueueCreate(1, sizeof(struct SLAVE *));
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
  if(clear_text_flag == 1)
    Sdwe_clearString(MAIN_PAGE_WARNNING);//�����ҳ����
  else if(clear_text_flag == 2)
    Sdwe_clearString(PAGE_HISTORY_TEXT_FILE_WARN);//�����ʷ����ҳ����ʾ����
  else if(clear_text_flag == 3)
    Sdwe_clearString(PAGE_U_TEXT_READ_STATE);//���U��ҳ���ȡ״̬����
  else if(clear_text_flag == 4)
    Sdwe_clearString(PAGE_DEVICE_WARNNING);//����豸ID�޸�����
  else if(clear_text_flag == 5)
    Sdwe_clearString(PAGE_CONFIG_WARNNING);
  else if(clear_text_flag == 6)
    Sdwe_clearString(PAGE_STOP_WARNNING);//���ͣ��ҳ������
  else if(clear_text_flag == 7)
    Sdwe_clearString(PAGE_CHANGE_WARNNING);
  else if(clear_text_flag == 8)
    Sdwe_clearString(PAGE_PASSWORD_WARNNING);
  else if(clear_text_flag == 9)
    Sdwe_clearString(PAGE_PERIOD_WARNNING);
  else if(clear_text_flag == 10)
    Sdwe_clearString(PAGE_WEIMI_WARNNING);
  else if(clear_text_flag == 11)
    Sdwe_clearString(PAGE_PRODUCT_RFID_WARNNING);//�������ҳ��RFID����
  else if(clear_text_flag == 12)
    Sdwe_clearString(PAGE_CARD_WARNNING);//�����������
}

void UserTimerCallback(TimerHandle_t xTimer)
{//��ʱʱ��1s
  static u16 speed_1 = 0,speed_2 = 0;
  static u8 timefor10s = 0;
  static u16 timefor5min = 0;
  static u16 timefor10min = 0;
  static u16 timefor20min = 0;
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
  timefor10s++;
  if(timefor10s >= 10)
  {//ÿ10���ȡһ��ʱ��
    timefor10s = 0;
    Sdwe_readRTC();
  }
  timefor5min++;
  if(timefor5min >= 300)
  {
    timefor5min = 0;
    u8 publish_topic;
    publish_topic = TOPIC_CHANNENG;
    xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
  }
  if(sample_time == 0)
  {
    speed_1 = pluse_count;
    sample_time++;
  }
  else if(sample_time >= 2)
  {//����2s�ڵ�������
    sample_time = 0;
    speed_2 = pluse_count;
    speed_zhu1 = (speed_2 - speed_1) * 30;//2���ڵ�������*30ת��Ϊ1����������
    pluse_count = 0;
    if(speed_zhu > 0)
    {
      Sdwe_disDigi(PAGE_PRODUCT_SPEED,speed_zhu,2);//��ʾ�ٶ�
    }
    else
    {
      Sdwe_disDigi(PAGE_PRODUCT_SPEED,speed_zhu1,2);//��ʾ�ٶ�
    }
  }
  else
  {
    sample_time++;
  }
  if(work_idle_time < 1000)
    work_idle_time++;
  if(work_idle_time < 10)
  {//10s����������Ϊ�ǹ���ʱ��
    if((device_info.system_state == SYS_STOP) || (device_info.system_state == SYS_IDLE))
      device_info.system_state = SYS_NORMAL;
    if(old_system_state != device_info.system_state)
    {
      old_system_state = device_info.system_state;
      Sdwe_disString(PAGE1_SYSTEM_STATE,(u8 *)system_state_dis[device_info.system_state],strlen(system_state_dis[device_info.system_state]));
      u8 publish_topic;
      publish_topic = TOPIC_STOP;
      xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
    }
  }
  else if(work_idle_time < 310)
  {//10~300s��������Ϊ�ǿ���ʱ�䣬�����빤��ʱ��
    if(device_info.system_state == SYS_NORMAL)
      device_info.system_state = SYS_IDLE;
    if(old_system_state != device_info.system_state)
    {
      old_system_state = device_info.system_state;
      Sdwe_disString(PAGE1_SYSTEM_STATE,(u8 *)system_state_dis[device_info.system_state],strlen(system_state_dis[device_info.system_state]));
      u8 publish_topic;
      publish_topic = TOPIC_STOP;
      xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
    }
  }
  else
  {//����5������������Ϊͣ��
    if((device_info.system_state == SYS_NORMAL) || (device_info.system_state > SYS_REPLACE_PAN))
      device_info.system_state = SYS_STOP;
    if(old_system_state != device_info.system_state)
    {
      old_system_state = device_info.system_state;
      Sdwe_disString(PAGE1_SYSTEM_STATE,(u8 *)system_state_dis[device_info.system_state],strlen(system_state_dis[device_info.system_state]));
      u8 publish_topic;
      publish_topic = TOPIC_STOP;
      xQueueSend(xQueue_MQTT_Transmit,(void *)&publish_topic,(TickType_t)10);
    }
  }
  if(device_info.system_state == SYS_NORMAL)
  {
    product_para.total_work_time++;
    if((product_para.total_work_time % 60) == 0)
    {//ÿ����ˢ��һ�ο���ʱ����ʾ
      u16 hour,min;
      hour = product_para.total_work_time / 3600;
      min = product_para.total_work_time % 3600 / 60;
      Sdwe_disDigi(PAGE_PRODUCT_TIME_ON_HOUR,hour,2);
      Sdwe_disDigi(PAGE_PRODUCT_TIME_ON_MIN,min,2);
    }
    timefor10min++;
    if(timefor10min >= 600)
    {//����״���£�ÿ10���ӱ���һ�β�������
      timefor10min = 0;
      W25QXX_Write((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
    }
  }
  else if(device_info.system_state == SYS_IDLE)
  {
    
  }
  else
  {
    product_para.total_stop_time++;//ͣ����ʱ��
    if((product_para.total_stop_time % 60) == 0)
    {//ÿ����ˢ��һ��ͣ��ʱ����ʾ
      u16 hour,min;
      hour = product_para.total_stop_time / 3600;
      min = product_para.total_stop_time % 3600 / 60;
      Sdwe_disDigi(PAGE_PRODUCT_TIME_OFF_HOUR,hour,2);
      Sdwe_disDigi(PAGE_PRODUCT_TIME_OFF_MIN,min,2);
    }
    if(device_info.system_state != SYS_STOP)
    {
      device_info.stop_para.stop_time[device_info.system_state - 1]++;
    }
    timefor20min++;
    if(timefor20min >= 600)
    {//����״���£�ÿ10���ӱ���һ��ʱ������
      timefor20min = 0;
      W25QXX_Write((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
      W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
    }
  }
//  printf("speed_zhu is %d\r\n",speed_zhu);
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
  else if(task == TASK_RFID)
  {
    uxBits = IWDG_BIT_RFID;
  }
  else if(task == TASK_MotorControl)
  {
    uxBits = IWDG_BIT_MotorControl;
  }
  else if(task == TASK_ManageCapacity)
  {
    uxBits = IWDG_BIT_ManageCapacity;
  }
  else if(task == TASK_Freq)
  {
    uxBits = IWDG_BIT_Freq;
  }
  xEventGroupSetBits(idwgEventGroup,uxBits);
}
