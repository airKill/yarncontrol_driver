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
*	函 数 名: vTaskTaskLCD
*	功能说明: 接口消息处理。
*	形    参: pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: 1  (数值越小优先级越低，这个跟uCOS相反)
*********************************************************************************************************
*/
void vTaskTaskLCD(void *pvParameters)
{
  u16 var_addr,value;
  SLAVE *ptMsg;
  /* 初始化结构体指针 */
  ptMsg = &slave_info;
  /* 初始化数组 */
  ptMsg->addr = 0;
  ptMsg->reg = 0;
  ptMsg->value = 0;
  
  BaseType_t xResult;
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(300); /* 设置最大等待时间为300ms */
//  LCD_POWER_ON();
  vTaskDelay(500);
  
  while(1)
  {
    xResult = xSemaphoreTake(xSemaphore_lcd, (TickType_t)xMaxBlockTime);
    if(xResult == pdTRUE)
    {
      /* 接收到同步信号 */
      if((lcd_rev_buf[0] == 0xA5) && (lcd_rev_buf[1] == 0x5A) && (lcd_rev_buf[2] == (lcd_rev_len - 3)))
      {//指令格式正确
        if(lcd_rev_buf[3] == 0x81)
        {//读寄存器返回
          var_addr = lcd_rev_buf[4];
          if(var_addr == 0x20)
          {//日期时间信息
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
        {//读变量存储器返回
          var_addr = (lcd_rev_buf[4] << 8) + lcd_rev_buf[5];
          if(var_addr == MAIN_PAGE_KEY_JINGSHA)
          {//经纱管理
            vTaskDelay(100);
            Init_JINGSHA_GUI();
          }
          else if(var_addr == MAIN_PAGE_KEY_WEIMI)
          {//纬密
            
          }
          else if(var_addr == MAIN_PAGE_KEY_CHANNENG)
          {//产能
            
          }
          else if(var_addr == MAIN_PAGE_KEY_SYS_CONFIG)
          {//系统配置
            
          }
          else if(var_addr == PAGE_FILE_KEY)
          {//历史资料页面
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
          {//第一页修改设定
            memset(input_password_buf,0,10);
            Sdwe_clearString(PAGE1_SECRET_TEXT_DIS);
            Sdwe_clearString(PAGE1_SECRET_TEXT_WARN);
          }
          else if(var_addr == PAGE2_KEY_SET_WEIGHT)
          {//第二页修改设定
            memset(input_password_buf,0,10);
            Sdwe_clearString(PAGE2_SECRET_TEXT_DIS);
            Sdwe_clearString(PAGE2_SECRET_TEXT_WARN);
          }
          else if(var_addr == PAGE3_KEY_SET_WEIGHT)
          {//第三页修改设定
            memset(input_password_buf,0,10);
            Sdwe_clearString(PAGE3_SECRET_TEXT_DIS);
            Sdwe_clearString(PAGE3_SECRET_TEXT_WARN);
          }
          else if(var_addr == PAGE1_KEY_LEFT)
          {//第一页向左
          
          }
          else if(var_addr == PAGE2_KEY_LEFT)
          {//第二页向左
            vTaskDelay(100);
            Init_JINGSHA_GUI();
          }
          else if(var_addr == PAGE3_KEY_LEFT)
          {//第三页向左
            vTaskDelay(100);
            Init_JINGSHA_GUI();
          }
          else if(var_addr == PAGE1_KEY_SAVE)
          {//第一页保存
            memset(input_password_buf,0,10);
            Sdwe_clearString(PAGE1_FILE_TEXT_DIS);
            Sdwe_clearString(PAGE1_FILE_TEXT_WARN);
            Sdwe_readRTC();
          }
          else if(var_addr == PAGE2_KEY_SAVE)
          {//第二页保存
            memset(input_password_buf,0,10);
            Sdwe_clearString(PAGE2_FILE_TEXT_DIS);
            Sdwe_clearString(PAGE2_FILE_TEXT_WARN);
            Sdwe_readRTC();
          }
          else if(var_addr == PAGE3_KEY_SAVE)
          {//第三页保存
            memset(input_password_buf,0,10);
            Sdwe_clearString(PAGE3_FILE_TEXT_DIS);
            Sdwe_clearString(PAGE3_FILE_TEXT_WARN);
            Sdwe_readRTC();
          }
          else if(var_addr == PAGE1_KEY_RIGHT)
          {//第一页向右
            vTaskDelay(100);
            Init_JINGSHA_GUI();
          }
          else if(var_addr == PAGE2_KEY_RIGHT)
          {//第二页向右
            vTaskDelay(100);
            Init_JINGSHA_GUI();
          }
          else if(var_addr == PAGE3_KEY_RIGHT)
          {//第三页向右
            vTaskDelay(100);
            Init_JINGSHA_GUI();
          }
          else if(var_addr == PAGE1_SECRET_KEY_CANCEL)
          {//第一页取消输入密码
          
          }
          else if(var_addr == PAGE2_SECRET_KEY_CANCEL)
          {//第二页取消输入密码
          
          }
          else if(var_addr == PAGE3_SECRET_KEY_CANCEL)
          {//第三页取消输入密码
          
          }
          else if(var_addr == PAGE1_SECRET_KEY_ENTER)
          {//第一页确认输入密码
          
          }
          else if(var_addr == PAGE2_SECRET_KEY_ENTER)
          {//第二页确认输入密码
            
          }
          else if(var_addr == PAGE3_SECRET_KEY_ENTER)
          {//第三页确认输入密码

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
                {//文件名长度相同
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
                  {//每个字符相同
                    repeat = 1;
                    break;
                  } 
                }
              }
              if(repeat == 0)
              {//无重复文件
                if(device_info.page_count_all >= 10)
                {//去掉第一个文件，后面文件依次上移
                  u8 file_read[1024];
                  __set_PRIMASK(1);
                  W25QXX_Read(file_read,(u32)W25QXX_ADDR_FILE + FILE_SIZE,FILE_SIZE * 9);//读出2-10文件的数据
                  W25QXX_Write(file_read,(u32)W25QXX_ADDR_FILE,FILE_SIZE * 9);//再写进1-9文件地址
                  __set_PRIMASK(0);
                  file_write.filename_len = input_password_len;                //文件名长度               
                  memcpy(file_write.filename,input_password_buf,input_password_len);//文件名
                  file_write.year = rtc_time.year;          //日期时间      
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
                  W25QXX_Write((u8 *)&file_write,(u32)(W25QXX_ADDR_FILE + FILE_SIZE * 9),sizeof(file_write));//写当前页面数据到文件10地址
                  __set_PRIMASK(0);
                  if(var_addr == PAGE1_FILE_KEY_ENTER)//按下第一页保存按钮
                    Sdwe_disPicture(PAGE_1);//跳转到页面1
                  else if(var_addr == PAGE2_FILE_KEY_ENTER)//按下第二页保存按钮
                    Sdwe_disPicture(PAGE_2);//跳转到页面2
                  else if(var_addr == PAGE3_FILE_KEY_ENTER)//按下第三页保存按钮
                    Sdwe_disPicture(PAGE_3);//跳转到页面3
                }
                else
                {
                  /******************文件储存格式********************
                  文件名长度                1byte   
                  文件名（最大10Byte）      10byte
                  日期时间（YY MM DD HH MM SS） 6byte
                  各驱动板张力设定值        30byte
                  **************************************************/
                  file_write.filename_len = input_password_len;                //文件名长度               
                  memcpy(file_write.filename,input_password_buf,input_password_len);//文件名
                  file_write.year = rtc_time.year;          //日期时间      
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
              {//有重复文件名
                if(var_addr == PAGE1_FILE_KEY_ENTER)
                  Sdwe_disString(PAGE1_FILE_TEXT_WARN,"文件名重复",strlen("文件名重复"));
                else if(var_addr == PAGE2_FILE_KEY_ENTER)
                  Sdwe_disString(PAGE2_FILE_TEXT_WARN,"文件名重复",strlen("文件名重复"));
                else if(var_addr == PAGE3_FILE_KEY_ENTER)
                  Sdwe_disString(PAGE3_FILE_TEXT_WARN,"文件名重复",strlen("文件名重复"));
              }
            }
            else
            {
              if(var_addr == PAGE1_FILE_KEY_ENTER)
                Sdwe_disString(PAGE1_FILE_TEXT_WARN,"文件名空",strlen("文件名空"));
              else if(var_addr == PAGE2_FILE_KEY_ENTER)
                Sdwe_disString(PAGE2_FILE_TEXT_WARN,"文件名空",strlen("文件名空"));
              else if(var_addr == PAGE3_FILE_KEY_ENTER)
                Sdwe_disString(PAGE3_FILE_TEXT_WARN,"文件名空",strlen("文件名空"));
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
            if(xQueueSend(xQueue_message,                  /* 消息队列句柄 */
                          (void *) &ptMsg,           /* 发送结构体指针变量ptMsg的地址 */
                          (TickType_t)10) == pdPASS )
            {
              /* 发送失败，即使等待了10个时钟节拍 */
              
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
            if(xQueueSend(xQueue_message,                  /* 消息队列句柄 */
                          (void *) &ptMsg,           /* 发送结构体指针变量ptMsg的地址 */
                          (TickType_t)10) == pdPASS )
            {
              /* 发送失败，即使等待了10个时钟节拍 */
              
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
            if(xQueueSend(xQueue_message,                  /* 消息队列句柄 */
                          (void *) &ptMsg,           /* 发送结构体指针变量ptMsg的地址 */
                          (TickType_t)10) == pdPASS )
            {
              /* 发送失败，即使等待了10个时钟节拍 */
              
            }
          }
          else if((var_addr >= PAGE1_SET_VALUE1) && (var_addr <= (PAGE1_SET_VALUE1 + 30)))
          {//设定值修改
            value = ((lcd_rev_buf[7] << 8) + lcd_rev_buf[8]) * 1000 / 100;//串口数据为两位数小数，单位kg，转换为g
            SlavePara.value_set[var_addr - 0x0320] = value;
            File_info.weight_value[var_addr - 0x0320] = value;
            __set_PRIMASK(1);
            W25QXX_Write((u8 *)&File_info,(u32)(W25QXX_ADDR_FILE + FILE_SIZE * device_info.page_count_select),sizeof(File_info));
            __set_PRIMASK(0);
            ptMsg->addr = var_addr - PAGE1_SET_VALUE1;
            ptMsg->func = FUNC_WRITE;
            ptMsg->reg = REG_SET_WEIGHT;
            ptMsg->value = value;
            if(xQueueSend(xQueue_message,                  /* 消息队列句柄 */
                          (void *) &ptMsg,           /* 发送结构体指针变量ptMsg的地址 */
                          (TickType_t)10) == pdPASS )
            {
              /* 发送失败，即使等待了10个时钟节拍 */
              printf("%d# value %.2f 队列发送成功\r\n",var_addr - 0x0320 + 1,(float)value / 1000);
            }
          }
          else if(var_addr == PAGE1_SECRET_TEXT_IMPORT)
          {//输入登录密码
            u8 llen;
            u8 buf[20],i;
            llen = lcd_rev_buf[6] * 2;//串口发送为字长
            memset(input_password_buf,0,10);
            memcpy(input_password_buf,lcd_rev_buf + 7,llen);
            input_password_len = get_valid_length(input_password_buf,llen);
            input_password_buf[input_password_len] = '\0';
            for(i=0;i<input_password_len;i++)
              buf[i] = '*';
            Sdwe_disString(PAGE1_SECRET_TEXT_DIS,buf,input_password_len);//密码显示为*
            
            if(strcmp(input_password_buf,"111111") == 0)
            {//密码正确
              Sdwe_disPicture(PAGE_1_SET_VALUE);
              vTaskDelay(10);
              Init_JINGSHA_GUI();
            }
            else
            {//密码错误
              Sdwe_disString(PAGE1_SECRET_TEXT_WARN,"密码错误",strlen("密码错误"));
            }
          }
          else if(var_addr == PAGE2_SECRET_TEXT_IMPORT)
          {//输入登录密码
            u8 llen;
            u8 buf[20],i;
            llen = lcd_rev_buf[6] * 2;//串口发送为字长
            memset(input_password_buf,0,10);
            memcpy(input_password_buf,lcd_rev_buf + 7,llen);
            input_password_len = get_valid_length(input_password_buf,llen);
            input_password_buf[input_password_len] = '\0';
            for(i=0;i<input_password_len;i++)
              buf[i] = '*';
            Sdwe_disString(PAGE2_SECRET_TEXT_DIS,buf,input_password_len);//密码显示为*
            
            if(strcmp(input_password_buf,"111111") == 0)
            {//密码正确
              Sdwe_disPicture(PAGE_2_SET_VALUE);
              vTaskDelay(10);
              Init_JINGSHA_GUI();
            }
            else
            {//密码错误
              Sdwe_disString(PAGE2_SECRET_TEXT_WARN,"密码错误",strlen("密码错误"));
            }
          }
          else if(var_addr == PAGE3_SECRET_TEXT_IMPORT)
          {//输入登录密码
            u8 llen;
            u8 buf[20],i;
            llen = lcd_rev_buf[6] * 2;//串口发送为字长
            memset(input_password_buf,0,10);
            memcpy(input_password_buf,lcd_rev_buf + 7,llen);
            input_password_len = get_valid_length(input_password_buf,llen);
            input_password_buf[input_password_len] = '\0';
            for(i=0;i<input_password_len;i++)
              buf[i] = '*';
            Sdwe_disString(PAGE3_SECRET_TEXT_DIS,buf,input_password_len);//密码显示为*
            
            if(strcmp(input_password_buf,"111111") == 0)
            {//密码正确
              Sdwe_disPicture(PAGE_3_SET_VALUE);
              vTaskDelay(10);
              Init_JINGSHA_GUI();
            }
            else
            {//密码错误
              Sdwe_disString(PAGE3_SECRET_TEXT_WARN,"密码错误",strlen("密码错误"));
            }
          }
          else if((var_addr == PAGE1_FILE_TEXT_IMPORT) || (var_addr == PAGE2_FILE_TEXT_IMPORT) || (var_addr == PAGE3_FILE_TEXT_IMPORT))
          {//文件名录入，去掉确认按钮，按OK后默认确认
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
              {//无重复文件
                if(device_info.page_count_all >= 10)
                {//去掉第一个文件，后面文件依次上移
                  u8 file_read[1024];
                  __set_PRIMASK(1);
                  W25QXX_Read(file_read,(u32)W25QXX_ADDR_FILE + FILE_SIZE,FILE_SIZE * 9);//读出2-10文件的数据
                  W25QXX_Write(file_read,(u32)W25QXX_ADDR_FILE,FILE_SIZE * 9);//再写进1-9文件地址
                  __set_PRIMASK(0);
                  file_write.filename_len = input_password_len;                //文件名长度               
                  memcpy(file_write.filename,input_password_buf,input_password_len);//文件名
                  file_write.year = rtc_time.year;          //日期时间      
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
                  W25QXX_Write((u8 *)&file_write,(u32)(W25QXX_ADDR_FILE + FILE_SIZE * 9),sizeof(file_write));//写当前页面数据到文件10地址
                  __set_PRIMASK(0);
                  if(var_addr == PAGE1_FILE_TEXT_IMPORT)//按下第一页保存按钮
                    Sdwe_disPicture(PAGE_1);//跳转到页面1
                  else if(var_addr == PAGE2_FILE_TEXT_IMPORT)//按下第二页保存按钮
                    Sdwe_disPicture(PAGE_2);//跳转到页面2
                  else if(var_addr == PAGE3_FILE_TEXT_IMPORT)//按下第三页保存按钮
                    Sdwe_disPicture(PAGE_3);//跳转到页面3
                }
                else
                {
                  /******************文件储存格式********************
                  文件名长度                1byte   
                  文件名（最大10Byte）      10byte
                  日期时间（YY MM DD HH MM SS） 6byte
                  各驱动板张力设定值        30byte
                  **************************************************/
                  file_write.filename_len = input_password_len;                //文件名长度               
                  memcpy(file_write.filename,input_password_buf,input_password_len);//文件名
                  file_write.year = rtc_time.year;          //日期时间      
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
              {//有重复文件名
                if(var_addr == PAGE1_FILE_TEXT_IMPORT)
                  Sdwe_disString(PAGE1_FILE_TEXT_WARN,"文件名重复",strlen("文件名重复"));
                else if(var_addr == PAGE2_FILE_TEXT_IMPORT)
                  Sdwe_disString(PAGE2_FILE_TEXT_WARN,"文件名重复",strlen("文件名重复"));
                else if(var_addr == PAGE3_FILE_TEXT_IMPORT)
                  Sdwe_disString(PAGE3_FILE_TEXT_WARN,"文件名重复",strlen("文件名重复"));
              }
            }
            else
            {
              if(var_addr == PAGE1_FILE_TEXT_IMPORT)
                Sdwe_disString(PAGE1_FILE_TEXT_WARN,"文件名空",strlen("文件名空"));
              else if(var_addr == PAGE2_FILE_TEXT_IMPORT)
                Sdwe_disString(PAGE2_FILE_TEXT_WARN,"文件名空",strlen("文件名空"));
              else if(var_addr == PAGE3_FILE_TEXT_IMPORT)
                Sdwe_disString(PAGE3_FILE_TEXT_WARN,"文件名空",strlen("文件名空"));
            }
          }
          else if((var_addr == PAGE_HISTORY_KEY_SELECT) || (var_addr == PAGE_HISTORY_KEY_DOWNLOAD))
          {//调用/下载
            u8 i;
            u16 ison = 0,num = 0,cnt = 0;//文件选择标志，某个文件选择，对应的位置1
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
            {//未选择文件
              Sdwe_disString(PAGE_HISTORY_TEXT_FILE_WARN,"未选择文件",strlen("未选择文件"));
              bsp_StartHardTimer(1 ,500000, (void *)TIM_CallBack1);
            }
            else
            {
              if(device_info.page_count_all == 0)
              {//没有保存的文件
                Sdwe_disString(PAGE_HISTORY_TEXT_FILE_WARN,"无文件",strlen("无文件"));
                bsp_StartHardTimer(1 ,500000, (void *)TIM_CallBack1);
              }
              else
              {
                if(cnt > 1)
                {//选择文件超过1个
                  Sdwe_disString(PAGE_HISTORY_TEXT_FILE_WARN,"只能选一个",strlen("只能选一个"));
                  bsp_StartHardTimer(1 ,500000, (void *)TIM_CallBack1);
                }
                else
                {
                  if(var_addr == PAGE_HISTORY_KEY_SELECT)
                  {//调用文件
                    FILE_INFO file_read;
                    u8 i;
                    __set_PRIMASK(1);
                    W25QXX_Read((u8 *)&file_read,(u32)(W25QXX_ADDR_FILE + FILE_SIZE * num),sizeof(file_read));//读出num的数据
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
                    Sdwe_disString(PAGE_HISTORY_TEXT_FILE_WARN,"调用成功",strlen("调用成功"));
                    bsp_StartHardTimer(1 ,500000, (void *)TIM_CallBack1);
                    ptMsg->addr = 0xff;
                    ptMsg->func = FUNC_WRITE;
                    ptMsg->reg = REG_SET_WEIGHT;
                    ptMsg->value = 0;
                    if(xQueueSend(xQueue_message,                  /* 消息队列句柄 */
                                  (void *) &ptMsg,           /* 发送结构体指针变量ptMsg的地址 */
                                  (TickType_t)10) == pdPASS )
                    {
                      /* 发送失败，即使等待了10个时钟节拍 */
                      printf("%d# value %.2f 队列发送成功\r\n",var_addr - 0x0320 + 1,(float)value / 1000);
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
          {//从机有效命令
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
            /* 向消息队列发数据，如果消息队列满了，等待10个时钟节拍 */
            ptMsg->addr = var_addr - PAGE1_SLAVE_ONOFF1;
            ptMsg->func = FUNC_WRITE;
            ptMsg->reg = REG_ONOFF;
            ptMsg->value = value;
            if(xQueueSend(xQueue_message,                  /* 消息队列句柄 */
                          (void *) &ptMsg,           /* 发送结构体指针变量ptMsg的地址 */
                          (TickType_t)10) == pdPASS )
            {
              /* 发送失败，即使等待了10个时钟节拍 */
              printf("%d# sw %d 队列发送成功\r\n",var_addr - PAGE1_SLAVE_ONOFF1 + 1,value);
            }
          }
          else if((var_addr >= PAGE_HISTORY_ICON_FILE1) && (var_addr <= (PAGE_HISTORY_ICON_FILE1 + 9)))
          {//文件选择
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
          {//选择U盘文件
            value = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
            Disk_File.fileselect[var_addr - PAGE_U_ICON_SELECT1] = value;
            printf("Disk file %d,%d\r\n",var_addr - PAGE_HISTORY_ICON_FILE1 + 1,value);
          }
          else if(var_addr == PAGE_U_KEY_READ)
          {//读取U盘文件
            u8 i;
            u16 num = 0,cnt = 0;//文件选择标志，某个文件选择，对应的位置1
            for(i=0;i<10;i++)
            {
              if(Disk_File.fileselect[i] == 1)
              {
                cnt++;
                num = i;
              }
            }
            if(cnt == 0)
            {//未选择文件
              Sdwe_disString(PAGE_U_TEXT_READ_STATE,"未选择文件",strlen("未选择文件"));
              bsp_StartHardTimer(1 ,500000, (void *)TIM_CallBack1);
            }
            else
            {
              if(Disk_File.filenum == 0)
              {//没有保存的文件
                Sdwe_disString(PAGE_U_TEXT_READ_STATE,"无文件",strlen("无文件"));
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
            if(xQueueSend(xQueue_message,                  /* 消息队列句柄 */
                          (void *) &ptMsg,           /* 发送结构体指针变量ptMsg的地址 */
                          (TickType_t)10) == pdPASS )
            {
              /* 发送失败，即使等待了10个时钟节拍 */
              printf("复位队列发送成功\r\n");
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
*	函 数 名: vTaskMassStorage
*	功能说明: 使用函数xQueueReceive接定时器中断发送的消息队列数据	
*	形    参: pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: 2  
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
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(5); /* 设置最大等待时间为5ms */
  usbh_OpenMassStorage();
  while(1)
  {
    USBH_Process(&USB_OTG_Core, &USB_Host);
    xResult = xSemaphoreTake(xSemaphore_download, (TickType_t)xMaxBlockTime);
    if(xResult == pdTRUE)
    {
      if(usb_disk_flag == 1)
      {//
        /* 挂载文件系统 */
        result = f_mount(&fs, "2:", 0);			/* Mount a logical drive */
        if (result != FR_OK)
        {
          printf("挂载文件系统失败 (%s)\r\n", FR_Table[result]);
        }
        /* 打开根文件夹 */
        result = f_opendir(&DirInf, "2:"); /* 如果不带参数，则从当前目录开始 */
        if (result != FR_OK)
        {
          printf("打开根目录失败  (%s)\r\n", FR_Table[result]);
          return;
        }
        __set_PRIMASK(1);
        W25QXX_Read((u8 *)&read_info,(u32)(W25QXX_ADDR_FILE + FILE_SIZE * download_num),sizeof(read_info));//读出num的数据
        __set_PRIMASK(0);
        u8 name_1[10],name_2[20];
        memset(name_1,0,10);
        memset(name_2,0,20);
        memcpy(name_1,read_info.filename,read_info.filename_len);
        sprintf(name_2,"2:/%d号机-%s.CSV",device_info.master_id,name_1);
  //      result = f_open(&file,"2:/22.CSV",FA_CREATE_ALWAYS | FA_WRITE);
        result = f_open(&file,name_2,FA_CREATE_ALWAYS | FA_WRITE);
        if(result != FR_OK)
        {
          printf("打开文件失败\r\n");
          return;
        }
        char buf[500] = "位置,设定张力\n",buf1[20];
        u16 len,i;
        for(i=0;i<30;i++)
        {
          sprintf(buf1,"%d#,%.2f\n",i + 1,(float)read_info.weight_value[i] / 1000.0);
          strcat(buf,buf1);
        }
        sprintf(buf,"%s时间日期,%02d/%02d/%02d,%02d:%02d:%02d",buf,read_info.year,
                read_info.month,read_info.day,read_info.hour,read_info.minute,read_info.second);
        len = strlen(buf);
        result = f_write(&file,buf,len,&bw);
        if(result == FR_OK)
        {
          Sdwe_disString(PAGE_HISTORY_TEXT_FILE_WARN,"写入成功",strlen("写入成功"));
          bsp_StartHardTimer(1 ,500000, (void *)TIM_CallBack1);
          printf("%s.CSV文件写入成功\r\n",name_1);
        }
        else
        {
          Sdwe_disString(PAGE_HISTORY_TEXT_FILE_WARN,"写入失败",strlen("写入失败"));
          bsp_StartHardTimer(1 ,500000, (void *)TIM_CallBack1);
          printf("%s.CSV文件写入失败\r\n",name_1);
        }
        /* 关闭文件*/
        f_close(&file);
        
        /* 卸载文件系统 */
        f_mount(NULL, "2:", 0);
      }
      else
      {
        Sdwe_disString(PAGE_HISTORY_TEXT_FILE_WARN,"无U盘",strlen("无U盘"));
        bsp_StartHardTimer(1 ,500000, (void *)TIM_CallBack1);
        printf("U盘未插入\r\n");
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
          {//1s内模块无回复退出
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
          {//卡号正确
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
*	函 数 名: vTaskMsgPro
*	功能说明: 使用函数xQueueReceive接定时器中断发送的消息队列数据
*	形    参: pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: 3  
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
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(10); /* 设置最大等待时间为200ms */
  while(1)
  {
    xResult = xQueueReceive(xQueue_message,                   /* 消息队列句柄 */
                            (void *)&ptMsg1,  		   /* 这里获取的是结构体的地址 */
                            (TickType_t)xMaxBlockTime);/* 设置阻塞时间 */
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
          {//对应位置显示正确图标
            Sdwe_writeIcon(PAGE1_SLAVE_STATE1 + i,VGUS_OFF);
          }
          else
          {//对应位置显示错误图标
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
          {//对应位置显示正确图标
            Sdwe_writeIcon(PAGE1_SLAVE_STATE1 + i,VGUS_OFF);
          }
          else
          {//对应位置显示错误图标
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
          {//对应位置显示正确图标
            Sdwe_writeIcon(PAGE1_SLAVE_STATE1 + i,VGUS_OFF);
          }
          else
          {//对应位置显示错误图标
            Sdwe_writeIcon(PAGE1_SLAVE_STATE1 + i,VGUS_ON);
          }
        }
      }
      else if(ptMsg1->addr == 0xff)
      {
        if((ptMsg1->func == FUNC_WRITE) && (ptMsg1->reg == REG_SET_WEIGHT))
        {//对每个从机设置设定张力
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
              {//对应位置显示正确图标
                Sdwe_writeIcon(PAGE1_SLAVE_STATE1 + i,VGUS_OFF);
              }
              else
              {//对应位置显示错误图标
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
        {//对应位置显示正确图标
          Sdwe_writeIcon(PAGE1_SLAVE_STATE1 + info.addr - 1,VGUS_OFF);
        }
        else
        {//对应位置显示错误图标
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
            {//获取到重量并显示到屏幕
              SlavePara.value_sample[readSlave.addr - 1] = readSlave.value;
              Sdwe_disDigi(PAGE1_SAMPLE_VALUE1 + readSlave.addr - 1,SlavePara.value_sample[index] / 10);
              Sdwe_writeIcon(PAGE1_SLAVE_STATE1 + readSlave.addr - 1,VGUS_OFF);//对应位置正确错误图标
              Sdwe_disDigi(PAGE1_ECHO_WEIGHT + readSlave.addr - 1,readSlave.set_value / 10);
            }
          }
        }
        else
        {//对应位置显示错误图标
          Sdwe_writeIcon(PAGE1_SLAVE_STATE1 + index,VGUS_ON);
          if(info.func == FUNC_READ)
          {
            if(info.reg == REG_GET_WEIGHT)
            {//获取到重量并显示到屏幕
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
*	函 数 名: vTaskRev485
*	功能说明: 启动任务，也就是最高优先级任务，这里用作按键扫描。
*	形    参: pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: 4  
*********************************************************************************************************
*/
static void vTaskRev485(void *pvParameters)
{
  SLAVE *ptMsg;
  /* 初始化结构体指针 */
  ptMsg = &slave_info;
  /* 初始化数组 */
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
      if(xQueueSend(xQueue_message,                  /* 消息队列句柄 */
                    (void *) &ptMsg,           /* 发送结构体指针变量ptMsg的地址 */
                    (TickType_t)10) == pdPASS )
      {
        printf("isWork is %d 队列发送成功\r\n",isWork);
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
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(200); /* 设置最大等待时间为200ms */
  while(1)
  {
    xResult = xSemaphoreTake(xSemaphore_readDisk, (TickType_t)xMaxBlockTime);
    if(xResult == pdTRUE)
    {
      if(usb_disk_flag == 1)
      {
        DiskFile_init();
        Sdwe_disPicture(PAGE_U);//跳转至U盘文件页面
        memset(read_file_select,0,10);//清除文件选择Buff
        Sdwe_ClearReadDisk();//清除文件选择图标
        /* 挂载文件系统 */
        result = f_mount(&fs, "2:", 0);			/* Mount a logical drive */
        if (result != FR_OK)
        {
          printf("挂载文件系统失败 (%s)\r\n", FR_Table[result]);
        }
        /* 打开根文件夹 */
        result = f_opendir(&DirInf, "2:"); /* 如果不带参数，则从当前目录开始 */
        if (result != FR_OK)
        {
          printf("打开根目录失败  (%s)\r\n", FR_Table[result]);
          return;
        }
          /* 读取当前文件夹下的文件和目录 */
        #if _USE_LFN
          FileInf.lfname = lfname;
          FileInf.lfsize = 256;
        #endif
          printf("属性        |  文件大小 | 短文件名 | 长文件名\r\n");
          for(cnt = 0; ;cnt++)
          {
            result = f_readdir(&DirInf,&FileInf); 		/* 读取目录项，索引会自动下移 */
            if (result != FR_OK || FileInf.fname[0] == 0)
            {
              break;
            }
            if (FileInf.fname[0] == '.')
            {
              continue;
            }
            /* 判断是文件还是子目录 */
            if (FileInf.fattrib & AM_DIR)
            {
              printf("(0x%02d)目录  ", FileInf.fattrib);
            }
            else
            {
              printf("(0x%02d)文件  ", FileInf.fattrib);
              if((strstr(FileInf.fname,".CSV")) && (strlen(FileInf.fname) <= 10))
              {//文件为CSV表格，且文件名长度小于10字符
                if(Disk_File.filenum < 10)
                {
                  memcpy(Disk_File.filename[Disk_File.filenum],FileInf.fname,10);
                  Sdwe_disString(PAGE_U_TEXT_FILENAME1 + Disk_File.filenum * 10,FileInf.fname,10);
                  Disk_File.filenum++;
                }
              }
            }
            /* 打印文件大小, 最大4G */
            printf(" %10d", FileInf.fsize);
            printf("  %s |", FileInf.fname);	/* 短文件名 */
            printf("  %s\r\n", (char *)FileInf.lfname);	/* 长文件名 */
          }
          /* 卸载文件系统 */
          f_mount(NULL, "2:", 0);
      }
      else
      {
        Sdwe_disString(PAGE_HISTORY_TEXT_FILE_WARN,"无U盘",strlen("无U盘"));
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
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(200); /* 设置最大等待时间为200ms */
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
      p_value = get_float_1bit(p_value);//取1位小数点
      if(p_value != old_p_value)
      {//产量值有变化时才保持并显示
        if(grade == 0)
        {//A班
          product_para.product_a = p_value;//A班产量
          Sdwe_disDigi(PAGE_PRODUCT_A,(int)(p_value * 10));
        }
        else if(grade == 0)
        {//B班
          product_para.product_b = p_value;//A班产量
          Sdwe_disDigi(PAGE_PRODUCT_B,(int)(p_value * 10));
        }
        complete_p = product_complete_meter(&product_para);//已完成产量
        uncomplete_p = product_uncomplete_meter(&product_para);//未完成产量
        complete_m = product_complete_kilo(&product_para);//已完成重量
        uncomplete_m = product_uncomplete_kilo(&product_para);//未完成重量
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
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void AppTaskCreate (void)
{
  xTaskCreate( vTaskTaskLED,   	/* 任务函数  */
              "vTaskTaskLED",     	/* 任务名    */
              64,               	/* 任务栈大小，单位word，也就是4字节 */
              NULL,              	/* 任务参数  */
              1,                 	/* 任务优先级*/
              &xHandleTaskLED );  /* 任务句柄  */
  xTaskCreate( vTaskTaskRFID,   	/* 任务函数  */
              "vTaskTaskRFID",     	/* 任务名    */
              64,               	/* 任务栈大小，单位word，也就是4字节 */
              NULL,              	/* 任务参数  */
              1,                 	/* 任务优先级*/
              &xHandleTaskRFID );  /* 任务句柄  */
  xTaskCreate( vTaskMsgPro,     		/* 任务函数  */
              "vTaskMsgPro",   		/* 任务名    */
              256,             		/* 任务栈大小，单位word，也就是4字节 */
              NULL,           		/* 任务参数  */
              2,               		/* 任务优先级*/
              &xHandleTaskMsgPro );  /* 任务句柄  */
  xTaskCreate( vTaskRev485,     		/* 任务函数  */
              "vTaskRev485",   		/* 任务名    */
              512,            		/* 任务栈大小，单位word，也就是4字节 */
              NULL,           		/* 任务参数  */
              3,              		/* 任务优先级*/
              &xHandleTaskRev485 );   /* 任务句柄  */
  xTaskCreate( vTaskTaskLCD,   	/* 任务函数  */
              "vTaskLCD",     	/* 任务名    */
              1024,               	/* 任务栈大小，单位word，也就是4字节 */
              NULL,              	/* 任务参数  */
              4,                 	/* 任务优先级*/
              &xHandleTaskLCD );  /* 任务句柄  */
  xTaskCreate( vTaskMassStorage,    		/* 任务函数  */
              "vTaskMassStorage",  		/* 任务名    */
              1024,         		/* 任务栈大小，单位word，也就是4字节 */
              NULL,        		/* 任务参数  */
              5,           		/* 任务优先级*/
              &xHandleTaskMassStorage ); /* 任务句柄  */
  xTaskCreate( vTaskReadDisk,    		/* 任务函数  */
              "vTaskReadDisk",  		/* 任务名    */
              512,         		/* 任务栈大小，单位word，也就是4字节 */
              NULL,        		/* 任务参数  */
              6,           		/* 任务优先级*/
              &xHandleTaskReadDisk); /* 任务句柄  */
  xTaskCreate( vTaskManageCapacity,    		/* 任务函数  */
              "vTaskManageCapacity",  		/* 任务名    */
              256,         		/* 任务栈大小，单位word，也就是4字节 */
              NULL,        		/* 任务参数  */
              7,           		/* 任务优先级*/
              &xHandleTaskManageCapacity); /* 任务句柄  */
}

/*
*********************************************************************************************************
*	函 数 名: AppObjCreate
*	功能说明: 创建任务通信机制
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void AppObjCreate (void)
{
  /* 创建二值信号量，首次创建信号量计数值是0 */
  xSemaphore_lcd = xSemaphoreCreateBinary();
  
  if(xSemaphore_lcd == NULL)
  {
    printf("xSemaphore_lcd fault\r\n");
    /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
  }
  /* 创建二值信号量，首次创建信号量计数值是0 */
  xSemaphore_rs485 = xSemaphoreCreateBinary();
  
  if(xSemaphore_rs485 == NULL)
  {
    printf("xSemaphore_rs485 fault\r\n");
    /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
  }
  
  /* 创建二值信号量，首次创建信号量计数值是0 */
  xSemaphore_pluse = xSemaphoreCreateBinary();
  
  if(xSemaphore_pluse == NULL)
  {
    printf("xSemaphore_pluse fault\r\n");
    /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
  }
  
  xSemaphore_readDisk = xSemaphoreCreateBinary();
  
  if(xSemaphore_readDisk == NULL)
  {
    printf("xSemaphore_readDisk fault\r\n");
    /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
  }
  xSemaphore_download = xSemaphoreCreateBinary();
  
  if(xSemaphore_download == NULL)
  {
    printf("xSemaphore_download fault\r\n");
    /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
  }
  idwgEventGroup = xEventGroupCreate();
  if(idwgEventGroup == NULL)
  {
    printf("idwgEventGroup fault\r\n");
  }
  /* 创建10个uint8_t型消息队列 */
  xQueue_message = xQueueCreate(10, sizeof(struct SLAVE *));
  if(xQueue_message == NULL)
  {
    printf("xQueue_message fault\r\n");
    printf("xQueue_message create failure\r\n");
    /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
  }
  xTimerUser = xTimerCreate("Timer",          /* 定时器名字 */
                             500,    /* 定时器周期,单位时钟节拍 */
                             pdTRUE,          /* 周期性 */
                             (void *)0,      /* 定时器ID */
                             UserTimerCallback); /* 定时器回调函数 */
    
  if(xTimerUser != NULL)
  {
    
    if(xTimerStart(xTimerUser, 500) != pdPASS)
    {
      printf("xSemaphore_rs485 fault\r\n");
      /* 定时器还没有进入激活状态 */
    }
  }
}

void TIM_CallBack1(void)
{
  Sdwe_clearString(PAGE_HISTORY_TEXT_FILE_WARN);
  Sdwe_clearString(PAGE_U_TEXT_READ_STATE);
}

void UserTimerCallback(TimerHandle_t xTimer)
{//定时时间500ms
  static u16 sample_time = 0;
  u16 speed_1,speed_2,speed;
  EventBits_t uxBits;
  uxBits = xEventGroupWaitBits(idwgEventGroup, /* 事件标志组句柄 */
                               IWDG_BIT_ALL,            /* 等待bit0和bit1被设置 */
                               pdTRUE,             /* 退出前bit0和bit1被清除，这里是bit0和bit1都被设置才表示“退出”*/
                               pdTRUE,             /* 设置为pdTRUE表示等待bit1和bit0都被设置*/
                               1); 	 /* 等待延迟时间 */
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
    {//计算1分钟内的脉冲数
      sample_time = 0;
      speed_2 = pluse_count;
      speed = speed_2 - speed_1;
      Sdwe_disDigi(PAGE_PRODUCT_SPEED,speed);//显示速度
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
