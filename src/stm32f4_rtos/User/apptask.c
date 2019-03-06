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
TaskHandle_t xHandleTaskFreq = NULL;

SemaphoreHandle_t  xSemaphore_lcd = NULL;
SemaphoreHandle_t  xSemaphore_rs485 = NULL;
SemaphoreHandle_t  xSemaphore_pluse = NULL;
SemaphoreHandle_t  xSemaphore_encoder = NULL;

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

u8 readDisk = DISK_IDLE;        //读取U盘功能，空闲/读文件名/读文件
u8 readFilenum = 0;

u8 old_system_state = 0xff;
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
            if(device_info.period_para.period_enable_onoff == 1)
              isDevicePeriod = get_period_state(&rtc_time,&device_info); 
            else
              isDevicePeriod = 0;
          }
        }
        else if(lcd_rev_buf[3] == 0x83)
        {//读变量存储器返回
          var_addr = (lcd_rev_buf[4] << 8) + lcd_rev_buf[5];
          if(var_addr < 0x1000)
          {//地址分成两段，提高检索效率
            if(var_addr == MAIN_PAGE_KEY_JINGSHA)
            {//经纱管理
              if(isDevicePeriod == 0)
              {
                if(device_info.func_onoff.jingsha)
                {
                  Sdwe_disPicture(PAGE_1);
                  Init_JINGSHA_GUI();
                }
                else
                  SDWE_WARNNING(MAIN_PAGE_WARNNING,"请联系厂商");
              }
              else
                SDWE_WARNNING(MAIN_PAGE_WARNNING,"试用期结束");
            }
            else if(var_addr == MAIN_PAGE_KEY_WEIMI)
            {//纬密
//              pwm_flag = 1;
              if(isDevicePeriod == 0)
              {
                if(device_info.func_onoff.weimi)
                {
                  Sdwe_disPicture(PAGE_WEIMI);
                  sdew_weimi_page1(&weimi_para);
                }
                else
                  SDWE_WARNNING(MAIN_PAGE_WARNNING,"请联系厂商");
              }
              else
                SDWE_WARNNING(MAIN_PAGE_WARNNING,"试用期结束");
            }
            else if(var_addr == MAIN_PAGE_KEY_CHANNENG)
            {//产能
              if(isDevicePeriod == 0)
              {
                if(device_info.func_onoff.channeng)
                {
                  Sdwe_disPicture(PAGE_CHANNENG);
                  Sdwe_product_page(&product_para);
                  card_config = READ_PERMISSION;
                }
                else
                  SDWE_WARNNING(MAIN_PAGE_WARNNING,"请联系厂商");
              }
              else
                SDWE_WARNNING(MAIN_PAGE_WARNNING,"试用期结束");
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
                Sdwe_writeIcon(i + PAGE_HISTORY_SELECT,file_select[i]);
              }
              if(usb_disk_flag == 0)
                Sdwe_writeIcon(PAGE_HISTORY_ICON_U_STATE,VGUS_OFF);  
              else
                Sdwe_writeIcon(PAGE_HISTORY_ICON_U_STATE,VGUS_ON);
            }
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
            }
            else if(var_addr == PAGE2_KEY_SAVE)
            {//第二页保存
              memset(input_password_buf,0,10);
              Sdwe_clearString(PAGE2_FILE_TEXT_DIS);
              Sdwe_clearString(PAGE2_FILE_TEXT_WARN);
            }
            else if(var_addr == PAGE3_KEY_SAVE)
            {//第三页保存
              memset(input_password_buf,0,10);
              Sdwe_clearString(PAGE3_FILE_TEXT_DIS);
              Sdwe_clearString(PAGE3_FILE_TEXT_WARN);
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
              W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
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
              W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
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
              JingSha_File.weight_value[var_addr - 0x0320] = value;
              W25QXX_Write((u8 *)&JingSha_File,(u32)(W25QXX_ADDR_JINGSHA + JINGSHA_SIZE * device_info.page_count_select),sizeof(JingSha_File));
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
              if(strcmp((char const*)input_password_buf,"111111") == 0)
//              if(memcmp(&input_password_buf,&device_info.regin_in.password,device_info.regin_in.password_len) == 0)
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
              
              if(strcmp((char const*)input_password_buf,"111111") == 0)
//              if(memcmp(&input_password_buf,&device_info.regin_in.password,device_info.regin_in.password_len) == 0)
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
              
              if(strcmp((char const*)input_password_buf,"111111") == 0)
//              if(memcmp(&input_password_buf,&device_info.regin_in.password,device_info.regin_in.password_len) == 0)
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
            else if((var_addr == PAGE1_FILE_TEXT_IMPORT) || (var_addr == PAGE2_FILE_TEXT_IMPORT) || (var_addr == PAGE3_FILE_TEXT_IMPORT) || (var_addr == PAGE_HISTORY2_IMPORT))
            {//文件名录入，去掉确认按钮，按OK后默认确认
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
                {//无重复文件
                  if(device_info.page_count_all >= 10)
                  {//去掉第一个文件，后面文件依次上移
                    u8 *file_read;
                    file_read = mymalloc(SRAMIN,1024);
                    W25QXX_Read(file_read,(u32)W25QXX_ADDR_JINGSHA + JINGSHA_SIZE,JINGSHA_SIZE * 9);//读出2-10文件的数据
                    W25QXX_Write(file_read,(u32)W25QXX_ADDR_JINGSHA,JINGSHA_SIZE * 9);//再写进1-9文件地址
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
                    W25QXX_Write((u8 *)&file_write,(u32)(W25QXX_ADDR_JINGSHA + JINGSHA_SIZE * 9),sizeof(file_write));//写当前页面数据到文件10地址
                    
                    //产能数据保存
                    peiliao_para.complete_meter = (u32)product_para.product_complete;
                    peiliao_para.complete_work_time = product_para.total_work_time;
                    W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO + CHANNENG_SIZE * 9,sizeof(peiliao_para));
                    
                    //纬密数据保存
                    W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI + WEIMI_SIZE * 9,sizeof(weimi_para));
                    if(var_addr == PAGE1_FILE_TEXT_IMPORT)//按下第一页保存按钮
                      Sdwe_disPicture(PAGE_1);//跳转到页面1
                    else if(var_addr == PAGE2_FILE_TEXT_IMPORT)//按下第二页保存按钮
                      Sdwe_disPicture(PAGE_2);//跳转到页面2
                    else if(var_addr == PAGE3_FILE_TEXT_IMPORT)//按下第三页保存按钮
                      Sdwe_disPicture(PAGE_3);//跳转到页面3
                    else if(var_addr == PAGE_HISTORY2_IMPORT)//按下第三页保存按钮
                    {
                      for(i=0;i<10;i++)
                      {
                        Sdwe_writeIcon(i + PAGE_HISTORY_SELECT,file_select[i]);
                      }
                      Sdwe_disPicture(PAGE_HISTORY);//跳转到页面3
                      Sdwe_refresh_allname(device_info.page_count_all);
                    }
                    myfree(SRAMIN,file_read);
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
                    W25QXX_Write((u8 *)&file_write,(u32)(W25QXX_ADDR_JINGSHA + JINGSHA_SIZE * device_info.page_count_all),sizeof(file_write));
                    //产能数据保存
                    peiliao_para.complete_meter = (u32)product_para.product_complete;
                    peiliao_para.complete_work_time = product_para.total_work_time;
                    W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO + CHANNENG_SIZE * device_info.page_count_all,sizeof(peiliao_para));
                    
                    //纬密数据保存
                    W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI + WEIMI_SIZE * device_info.page_count_all,sizeof(weimi_para));
                    
                    device_info.page_count_all++;
                    W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
                    if(var_addr == PAGE1_FILE_TEXT_IMPORT)
                      Sdwe_disPicture(PAGE_1);
                    else if(var_addr == PAGE2_FILE_TEXT_IMPORT)
                      Sdwe_disPicture(PAGE_2);
                    else if(var_addr == PAGE3_FILE_TEXT_IMPORT)
                      Sdwe_disPicture(PAGE_3);
                    else if(var_addr == PAGE_HISTORY2_IMPORT)//按下第三页保存按钮
                    {
                      Sdwe_disPicture(PAGE_HISTORY);//跳转到页面3
                      for(i=0;i<10;i++)
                      {
                        Sdwe_writeIcon(i + PAGE_HISTORY_SELECT,file_select[i]);
                      }
                      Sdwe_refresh_allname(device_info.page_count_all);
                    }
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
                  else if(var_addr == PAGE_HISTORY2_IMPORT)//按下第三页保存按钮
                    Sdwe_disString(PAGE_HISTORY2_WARNNING,"文件名重复",strlen("文件名重复"));
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
                else if(var_addr == PAGE_HISTORY2_IMPORT)//按下第三页保存按钮
                    Sdwe_disString(PAGE_HISTORY2_WARNNING,"文件名重复",strlen("文件名重复"));
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
                SDWE_WARNNING(PAGE_HISTORY_TEXT_FILE_WARN,"未选择文件");
              }
              else
              {
                if(device_info.page_count_all == 0)
                {//没有保存的文件
                  SDWE_WARNNING(PAGE_HISTORY_TEXT_FILE_WARN,"无文件");
                }
                else
                {
                  if(cnt > 1)
                  {//选择文件超过1个
                    SDWE_WARNNING(PAGE_HISTORY_TEXT_FILE_WARN,"只能选一个");
                  }
                  else
                  {
                    if(var_addr == PAGE_HISTORY_KEY_SELECT)
                    {//调用文件
                      JINGSHA_FILE file_read;
                      u8 i;
                      W25QXX_Read((u8 *)&file_read,(u32)(W25QXX_ADDR_JINGSHA + JINGSHA_SIZE * num),sizeof(file_read));//读出num的数据
                      W25QXX_Read((u8 *)&peiliao_para,(u32)(W25QXX_ADDR_PEILIAO + CHANNENG_SIZE * num),sizeof(peiliao_para));//读出num的数据
                      W25QXX_Read((u8 *)&weimi_para,(u32)(W25QXX_ADDR_WEIMI + WEIMI_SIZE * num),sizeof(weimi_para));//读出num的数据
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
                      SDWE_WARNNING(PAGE_HISTORY_TEXT_FILE_WARN,"调用成功");
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
              W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
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
            else if(var_addr == PAGE_HISTORY_KEY_ADD)
            {//
              
            }
            else if(var_addr == PAGE_HISTORY_KEY_READ)
            {
              readDisk = DISK_FILENAME;
              xSemaphoreGive(xSemaphore_readDisk);
            }
            else if((var_addr >= PAGE_U_ICON_SELECT1) && (var_addr <= PAGE_U_ICON_SELECT1 + 9))
            {//选择U盘文件
              value = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              Disk_File.fileselect[var_addr - PAGE_U_ICON_SELECT1] = value;
              printf("Disk file %d,%d\r\n",var_addr - PAGE_HISTORY_SELECT + 1,value);
            }
            else if(var_addr == PAGE_U_KEY_READ)
            {//读取U盘文件
              u8 i;
              u8 num;
              u16 cnt = 0;//文件选择标志，某个文件选择，对应的位置1
              for(i=0;i<10;i++)
              {
                if(Disk_File.fileselect[i] == 1)
                {
                  num = i;
                  cnt++;
                }
              }
              if(cnt == 0)
              {//未选择文件
                SDWE_WARNNING(PAGE_U_TEXT_READ_STATE,"未选择文件");
              }
              else if(cnt == 1)
              {//只能选择一个文件
                if(Disk_File.filenum == 0)
                {//没有保存的文件
                  SDWE_WARNNING(PAGE_U_TEXT_READ_STATE,"无文件");
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
                SDWE_WARNNING(PAGE_U_TEXT_READ_STATE,"只能选1个");
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
              SDWE_WARNNING(PAGE_DEVICE_WARNNING,"驱动板复位");
            }
            else if(var_addr == PAGE_PRODUCT_PEILIAO)
            {//
              Sdwe_peiliao_page(&peiliao_para);
              card_config = CARD_DISABLE;
            }
            else if(var_addr == PAGE_PRODUCT_CLEAR)
            {//产量清零，重新开始生产
              total_meter_gross = (u32)(peiliao_para.total_meter_set * (1 + (float)peiliao_para.loss / 100));
              total_weight_gross = (u32)(peiliao_para.total_weitht_set * (1 + peiliao_para.loss / 100.0));//总重量设置含损耗
              init_product_para(&product_para,&peiliao_para);//重新设置生产任务后，产能清零
              peiliao_para.add_meter_set = 0;//重新设置生产任务后，补单数清零
              Sdwe_disDigi(PAGE_PRODUCT_ADD_METER,(int)(peiliao_para.add_meter_set * 10),4);
              Sdwe_product_page(&product_para);
              W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
              W25QXX_Write((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
              plan_complete = 0;
              old_plan_complete = 0;
              
              Sdwe_clearString(PAGE_PRODUCT_RFID_WARNNING);//清除产能完成显示
            }
            else if(var_addr == PAGE_PRODUCT_QUIT)
            {//退出产能页面，卡功能禁止
              card_config = CARD_DISABLE;
            }
            else if(var_addr == PAGE_PEILIAO_QUIT)
            {//退出胚料页面，进入产能页面，卡连续读
              card_config = READ_PERMISSION;
            }
            else if(var_addr == PAGE_PRODUCT_JINGSHA)
            {//经纱设置
              float meter;
              meter = (float)((lcd_rev_buf[7] << 8) + lcd_rev_buf[8]) / 10;
              peiliao_para.latitude_weight = meter;
              peiliao_para.final_weight = final_per_meter(&peiliao_para);
              Sdwe_disDigi(PAGE_PRODUCT_FINAL,(int)(peiliao_para.final_weight * 10),2);
              W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
            }
            else if(var_addr == PAGE_PRODUCT_WEISHA)
            {//纬纱设置
              float meter;
              meter = (float)((lcd_rev_buf[7] << 8) + lcd_rev_buf[8]) / 10;
              peiliao_para.longitude_weight = meter;
              peiliao_para.final_weight = final_per_meter(&peiliao_para);
              Sdwe_disDigi(PAGE_PRODUCT_FINAL,(int)(peiliao_para.final_weight * 10),2);
              W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
            }
            else if(var_addr == PAGE_PRODUCT_RUBBER)
            {//橡胶设置
              float meter;
              meter = (float)((lcd_rev_buf[7] << 8) + lcd_rev_buf[8]) / 10;
              peiliao_para.rubber_weight = meter;
              peiliao_para.final_weight = final_per_meter(&peiliao_para);
              Sdwe_disDigi(PAGE_PRODUCT_FINAL,(int)(peiliao_para.final_weight * 10),2);
              W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
            }
            else if(var_addr == PAGE_PRODUCT_ZHIJI)
            {//织机条数
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              peiliao_para.loom_num = cnt;
              W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
            }
            else if(var_addr == PAGE_PRODUCT_LOSS)
            {//损耗
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              peiliao_para.loss = cnt;
              total_meter_gross = (u32)(peiliao_para.total_meter_set * (1 + peiliao_para.loss / 100.0));
              total_weight_gross = (u32)(peiliao_para.total_weitht_set * (1 + peiliao_para.loss / 100.0));
              W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
            }
            else if(var_addr == PAGE_PRODUCT_TOTAL_METER)
            {//生产任务米设置
              u32 cnt;
              float percent;
              cnt = (lcd_rev_buf[7] << 24) + (lcd_rev_buf[8] << 16) + (lcd_rev_buf[9] << 8) + lcd_rev_buf[10];
              peiliao_para.total_meter_set = cnt;
              percent = 1 + (float)peiliao_para.loss / 100.0;
              total_meter_gross = (u32)(peiliao_para.total_meter_set * percent);
              init_product_para(&product_para,&peiliao_para);//重新设置生产任务后，产能清零
              peiliao_para.add_meter_set = 0;//重新设置生产任务后，补单数清零
              Sdwe_disDigi(PAGE_PRODUCT_ADD_METER,(int)(peiliao_para.add_meter_set * 10),4);
              Sdwe_product_page(&product_para);
              W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
              W25QXX_Write((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
              plan_complete = 0;
              old_plan_complete = 0;
              
              Sdwe_clearString(PAGE_PRODUCT_RFID_WARNNING);//清除产能完成显示
            }
            else if(var_addr == PAGE_PRODUCT_TOTAL_WEIGHT)
            {//生产任务重量设置
              u32 cnt;
              float percent;
              cnt = (lcd_rev_buf[7] << 24) + (lcd_rev_buf[8] << 16) + (lcd_rev_buf[9] << 8) + lcd_rev_buf[10];
              peiliao_para.total_weitht_set = cnt;
              percent = 1 + (float)peiliao_para.loss / 100.0;
              total_weight_gross = (u32)(peiliao_para.total_weitht_set * percent);
              init_product_para(&product_para,&peiliao_para);//重新设置生产任务后，产能清零
              peiliao_para.add_meter_set = 0;//重新设置生产任务后，补单数清零
              Sdwe_disDigi(PAGE_PRODUCT_ADD_METER,(int)(peiliao_para.add_meter_set * 10),4);
              Sdwe_product_page(&product_para);
              W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
              W25QXX_Write((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
              plan_complete = 0;
              old_plan_complete = 0;
              Sdwe_clearString(PAGE_PRODUCT_RFID_WARNNING);//清除产能完成显示
            }
            else if(var_addr == PAGE_PRODUCT_KAIDU)
            {//开度设置
              float cnt;
              cnt = (float)((lcd_rev_buf[7] << 8) + lcd_rev_buf[8]) / 10;
              peiliao_para.kaidu_set = cnt;
              W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
            }
            else if(var_addr == PAGE_PRODUCT_WEIMI)
            {//纬密设置
              float cnt;
              cnt = (float)((lcd_rev_buf[7] << 8) + lcd_rev_buf[8]) / 10;
              if(cnt > 0)
              {
                peiliao_para.weimi_set = cnt;
                W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
                weimi_para.wei_cm_set[0] = cnt;//胚料页面纬厘米和纬密页面段1纬厘米相同
                weimi_para.wei_inch_set[0] = weimi_para.wei_cm_set[0] * 2.54;
                //转换为纬/inch显示
                W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI,sizeof(weimi_para));
              }
            }
            else if(var_addr == PAGE_PRODUCT_ADD_METER)
            {//补单设置
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 24) + (lcd_rev_buf[8] << 16) + (lcd_rev_buf[9] << 8) + lcd_rev_buf[10];
              peiliao_para.add_meter_set = cnt;
              W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
            }
            else if(var_addr == PAGE_PRODUCT_WEISHU_DIS)
            {//纬密显示设置
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              peiliao_para.weimi_dis_set = cnt;
              W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
            }
            else if((var_addr >= PAGE_STOP_OFF) && (var_addr <= PAGE_STOP_OFF + 11))
            {//停机原因选择
              if(device_info.system_state == SYS_NORMAL)
              {//只有系统正常时，才能选择停机原因
                device_info.system_state = var_addr - PAGE_STOP_OFF + 1;
                Sdwe_disString(PAGE1_SYSTEM_STATE,(u8 *)system_state_dis[device_info.system_state],strlen(system_state_dis[device_info.system_state]));
                W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
                Sdwe_writeIcon(var_addr - PAGE_STOP_OFF + PAGE_STOP_ON,VGUS_ON);//图标显示选中
                printf("System stop is num %d.\r\n",device_info.system_state);
              }
              else
              {
                if(device_info.system_state == (var_addr - PAGE_STOP_OFF + 1))
                {//取消选择停机
                  Sdwe_writeIcon(var_addr - PAGE_STOP_OFF + PAGE_STOP_ON,VGUS_OFF);
                  device_info.system_state = SYS_NORMAL;
                  Sdwe_disString(PAGE1_SYSTEM_STATE,(u8 *)system_state_dis[device_info.system_state],strlen(system_state_dis[device_info.system_state]));
                  W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
                  printf("System normal.\r\n");
                }
                else
                {
                  SDWE_WARNNING(PAGE_STOP_WARNNING,"无效操作");
                }
              }
            }
            
            /****************************恢复出厂默认值************************************/
            else if(var_addr == PAGE_CONFIG_DEFAULT)
            {
              default_device_para();
              SDWE_WARNNING(PAGE_DEVICE_WARNNING,"恢复默认");
            }
            /****************************齿轮比设置************************************/
            else if(var_addr == PAGE_CONFIG_RATIO_DISPLAY)
            {
              Sdwe_ratio_display(&device_info);
            }
            else if(var_addr == PAGE_CONFIG_RATIO1)
            {//齿轮比1
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              device_info.ratio.GEAR1 = cnt;
              W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
              servomotor_step = MotorStepCount(&device_info,&weimi_para,MotorProcess.current_seg);//获取段号对应的脉冲数/纬
            }
            else if(var_addr == PAGE_CONFIG_RATIO2)
            {//齿轮比2
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              device_info.ratio.GEAR2 = cnt;
              W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
              servomotor_step = MotorStepCount(&device_info,&weimi_para,MotorProcess.current_seg);//获取段号对应的脉冲数/纬
            }
            /****************************机器编号显示************************************/
            else if(var_addr == PAGE_CONFIG_DEVICE_ID)
            {//机器编号
              Sdwe_disString(PAGE_DEVICE_ID,device_info.device_id,device_info.device_id_len);
            }
            /*********************************************************************************/
            /****************************A/B班换班时间设置************************************/
            else if(var_addr == PAGE_CONFIG_TIME)
            {//换班时间设置
              Sdwe_change_class_time(&device_info);
            }
            else if(var_addr == PAGE_CHANGE_HOUR)
            {//换班时间小时设置
              value = ((lcd_rev_buf[7] << 8) + lcd_rev_buf[8]);
              if(value <= 23)
              {
                device_info.class_para.class_time_hour = value;
                W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
              }
              else
              {
                SDWE_WARNNING(PAGE_CHANGE_WARNNING,"超出范围");
              }
            }
            else if(var_addr == PAGE_CHANGE_MINUTE)
            {//换班时间分钟设置
              value = ((lcd_rev_buf[7] << 8) + lcd_rev_buf[8]);
              if(value <= 59)
              {
                device_info.class_para.class_time_minute = value;
                W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
              }
              else
              {
                SDWE_WARNNING(PAGE_CHANGE_WARNNING,"超出范围");
              }
            }
            else if(var_addr == PAGE_CHANGE_SWITCH)
            {//是否换班设置
              value = ((lcd_rev_buf[7] << 8) + lcd_rev_buf[8]);
              device_info.class_para.class_enable_onoff = value;
              W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
            }
            
            /****************************************************************************/
            /****************************员工卡权限设置*******************************/
            else if(var_addr == PAGE_CONFIG_CARD)
            {//员工卡设置
              card_config = WRITE_PERMISSION;
            }
            else if(var_addr == PAGE_CARD_A_INC)
            {//增加A班卡
              card_config = WRITE_INC_A;
            }
            else if(var_addr == PAGE_CARD_A_DEC)
            {//减少A班卡
              card_config = WRITE_DEC_A;
            }
            else if(var_addr == PAGE_CARD_B_INC)
            {//增加B班卡
              card_config = WRITE_INC_B;
            }
            else if(var_addr == PAGE_CARD_B_DEC)
            {//减少B班卡
              card_config = WRITE_DEC_B;
            }
            else if(var_addr == PAGE_CARD_REPAIR_INC)
            {//增加维护卡
              card_config = WRITE_INC_REPAIR;
            }
            else if(var_addr == PAGE_CARD_REPAIR_DEC)
            {//减少维护卡
              card_config = WRITE_DEC_REPAIR;
            }
            else if(var_addr == PAGE_CARD_QUIT)
            {//退出卡设置页面
              card_config = CARD_DISABLE;
            }
            /****************************************************************************/
            else if(var_addr == PAGE_PASSWORD_IMPORT)
            {//修改登录密码
              u8 llen;
              u8 buf[20],i;
              llen = lcd_rev_buf[6] * 2;//串口发送为字长
              memset(input_password_buf,0,10);
              memcpy(input_password_buf,lcd_rev_buf + 7,llen);
              input_password_len = get_valid_length(input_password_buf,llen);
              if(input_password_len > 0)
              {
                input_password_buf[input_password_len] = '\0';
                for(i=0;i<input_password_len;i++)
                  buf[i] = '*';
                Sdwe_disString(PAGE_PASSWORD_DIS,buf,input_password_len);//密码显示为*
                memcpy(device_info.regin_in.password,input_password_buf,input_password_len);//保存密码
                SDWE_WARNNING(PAGE_PASSWORD_WARNNING,"修改成功");
                W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
              }
              else
              {
                SDWE_WARNNING(PAGE_PASSWORD_WARNNING,"输入不能空");
              }
            }
            else if(var_addr == PAGE_DEVICE_ID)
            {//修改ID
              u8 llen;
              llen = lcd_rev_buf[6] * 2;//串口发送为字长
              memset(input_password_buf,0,10);
              memcpy(input_password_buf,lcd_rev_buf + 7,llen);
              input_password_len = get_valid_length(input_password_buf,llen);
              if(input_password_len > 0)
              {
                input_password_buf[input_password_len] = '\0';
                memcpy(device_info.device_id,input_password_buf,input_password_len);
                device_info.device_id_len = input_password_len;
                SDWE_WARNNING(PAGE_DEVICE_WARNNING,"修改成功");
                W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
              }
              else
              {
                SDWE_WARNNING(PAGE_DEVICE_WARNNING,"输入不能空");
              }
            }
            else if(var_addr == PAGE_CONFIG_PASSWORD)
            {//输入系统配置页面登录密码
              u8 llen;
              u8 buf[20],i;
              llen = lcd_rev_buf[6] * 2;//串口发送为字长
              memset(input_password_buf,0,10);
              memcpy(input_password_buf,lcd_rev_buf + 7,llen);
              input_password_len = get_valid_length(input_password_buf,llen);
              input_password_buf[input_password_len] = '\0';
              for(i=0;i<input_password_len;i++)
                buf[i] = '*';
              Sdwe_disString(PAGE_CONFIG_DIS,buf,input_password_len);//密码显示为*
              
              if(strcmp((char const*)input_password_buf,(char const *)device_info.period_para.period_password) == 0)
              {//特殊密码进入试用期页面
                Sdwe_disPicture(PAGE_PERIOD);
                Sdwe_period_page(&device_info);
              }
              else if(strcmp((char const*)input_password_buf,"138992") == 0)
              {//特殊密码进入隐藏页面
                Sdwe_disPicture(PAGE_HIDDEN);
                Sdwe_hidden_page(&device_info);
              }
              else if(memcmp(&input_password_buf,&device_info.regin_in.password,device_info.regin_in.password_len) == 0)
              {//特殊密码进入系统设置页面
                Sdwe_disPicture(PAGE_CONFIG);
              }
              else
              {//密码错误
                SDWE_WARNNING(PAGE_CONFIG_WARNNING,"密码错误");
              }
            }
            /******************************历史资料页面2***************************************/
            else if(var_addr == PAGE_HISTORY2_CANCEL)
            {
              
            }
            else if(var_addr == PAGE_HISTORY2_IMPORT)
            {
              
            }
            
            /**************************页面使能开关**************************************/
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
                SDWE_WARNNING(PAGE_PERIOD_WARNNING,"超出范围");
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
                SDWE_WARNNING(PAGE_PERIOD_WARNNING,"超出范围");
            }
            else if(var_addr == PAGE_PERIOD_PASSWORD_IMPORT)
            {
              u8 llen;
              u8 buf[20],i;
              llen = lcd_rev_buf[6] * 2;//串口发送为字长
              memset(input_password_buf,0,10);
              memcpy(input_password_buf,lcd_rev_buf + 7,llen);
              input_password_len = get_valid_length(input_password_buf,llen);
              input_password_buf[input_password_len] = '\0';
              for(i=0;i<input_password_len;i++)
                buf[i] = '*';
              Sdwe_disString(PAGE_PERIOD_PASSWORD_DIS,buf,input_password_len);//密码显示为*
              memcpy(device_info.period_para.period_password,input_password_buf,input_password_len);
              device_info.period_para.period_password_len = input_password_len;
              W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
            }
            /***********************************************************************/
          }
          else
          {
            if((var_addr >= PAGE_WEIMI_TOTALWEI_1) && (var_addr < PAGE_WEIMI_TOTALWEI_1 + 20))
            {//纬循环设置
              u32 cnt;
              cnt = (lcd_rev_buf[7] << 24) + (lcd_rev_buf[8] << 16) + (lcd_rev_buf[9] << 8) + lcd_rev_buf[10];
              if((var_addr - PAGE_WEIMI_TOTALWEI_1) == 0)
              {
                if(cnt == 0)
                {//段1纬循环不能为0
                  SDWE_WARNNING(PAGE_WEIMI_WARNNING,"不能为0");
                  Sdwe_disDigi(var_addr,weimi_para.total_wei_count[0],4);
                }
                else
                {
                  weimi_para.total_wei_count[(var_addr - PAGE_WEIMI_TOTALWEI_1)] = cnt;
                  W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI,sizeof(weimi_para));
                  if((var_addr - PAGE_WEIMI_TOTALWEI_1) == MotorProcess.current_seg)
                  {//如果修改的循环纬刚好是当前段号，立刻更新当前循环纬
                    MotorProcess.total_wei = weimi_para.total_wei_count[MotorProcess.current_seg];
                  }
                }
              }
              else
              {
                weimi_para.total_wei_count[(var_addr - PAGE_WEIMI_TOTALWEI_1)] = cnt;
                W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI,sizeof(weimi_para));
                if((var_addr - PAGE_WEIMI_TOTALWEI_1) == MotorProcess.current_seg)
                {//如果修改的循环纬刚好是当前段号，立刻更新当前循环纬
                  MotorProcess.total_wei = weimi_para.total_wei_count[MotorProcess.current_seg];
                }
              }
            }
            else if((var_addr >= PAGE_WEIMI_WEI_CM_1) && (var_addr < PAGE_WEIMI_WEI_CM_1 + 20))
            {//纬/cm设置
              float cnt;
              u8 seg_num;
              cnt = (float)((lcd_rev_buf[7] << 8) + lcd_rev_buf[8]) / 10.0;
              seg_num = (var_addr - PAGE_WEIMI_WEI_CM_1) / 2;
              weimi_para.wei_cm_set[seg_num] = cnt;
              weimi_para.wei_inch_set[seg_num] = weimi_para.wei_cm_set[seg_num] * 2.54;
              //转换为纬/inch显示
              Sdwe_disDigi(PAGE_WEIMI_WEI_INCH_1 + var_addr - PAGE_WEIMI_WEI_CM_1,(int)(weimi_para.wei_inch_set[seg_num] * 100),2);
              W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI,sizeof(weimi_para));
              
              if((seg_num) == (MotorProcess.current_seg / 2))
              {//如果改变的纬密为当前段号纬密，立刻更新步数/纬
                servomotor_step = MotorStepCount(&device_info,&weimi_para,MotorProcess.current_seg);//获取段号对应的脉冲数/纬
              }
                
              if((var_addr - PAGE_WEIMI_WEI_CM_1) == 0)
              {//第一段的纬密和胚料页面的纬密相同
                peiliao_para.weimi_set = cnt;
                W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
              }
            }
            else if((var_addr >= PAGE_WEIMI_MEDIANWEI_1) && (var_addr < PAGE_WEIMI_MEDIANWEI_1 + 20))
            {//纬过渡设置
              u32 cnt;
              cnt = (lcd_rev_buf[7] << 24) + (lcd_rev_buf[8] << 16) + (lcd_rev_buf[9] << 8) + lcd_rev_buf[10];
              weimi_para.total_wei_count[(var_addr - PAGE_WEIMI_MEDIANWEI_1) + 1] = cnt;
              W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI,sizeof(weimi_para));
              if((var_addr - PAGE_WEIMI_MEDIANWEI_1 + 1) == MotorProcess.current_seg)
              {//如果修改的纬过渡刚好是当前段号，立刻更新当前循环纬
                MotorProcess.total_wei = weimi_para.total_wei_count[MotorProcess.current_seg - 1];
              }
            }
            else if((var_addr >= PAGE_WEIMI_STEP1_SPEED) && (var_addr < PAGE_WEIMI_STEP1_SPEED + 20))
            {//送纬电机速度设置
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              if(cnt <= 300)
              {//当前电机速度小于主轴速度，为有效值
                weimi_para.step1_factor[(var_addr - PAGE_WEIMI_STEP1_SPEED) / 2] = cnt;
                W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI,sizeof(weimi_para));
                
                if(((var_addr - PAGE_WEIMI_STEP1_SPEED) / 2) == (MotorProcess.current_seg / 2))
                {//如果设置的送纬电机号段为当前号段，立刻更新送纬电机比例
                  MotorProcess.step1_factor = weimi_para.step1_factor[MotorProcess.current_seg / 2];
                }
              }
              else
              {
                Sdwe_disDigi(var_addr,weimi_para.step1_factor[(var_addr - PAGE_WEIMI_STEP1_SPEED) / 2],2);
                SDWE_WARNNING(PAGE_WEIMI_WARNNING,"速比太大");
              }
            }
            else if((var_addr >= PAGE_WEIMI_STEP2_SPEED) && (var_addr < PAGE_WEIMI_STEP2_SPEED + 20))
            {//底线电机速度设置
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              if(cnt <= 300)
              {
                weimi_para.step2_factor[(var_addr - PAGE_WEIMI_STEP2_SPEED) / 2] = cnt;
                W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI,sizeof(weimi_para));
                
                if(((var_addr - PAGE_WEIMI_STEP2_SPEED) / 2) == (MotorProcess.current_seg / 2))
                {//如果设置的送纬电机号段为当前号段，立刻更新送纬电机比例
                  MotorProcess.step2_factor = weimi_para.step2_factor[MotorProcess.current_seg / 2];
                }
              }
              else
              {
                Sdwe_disDigi(var_addr,weimi_para.step2_factor[(var_addr - PAGE_WEIMI_STEP2_SPEED) / 2],2);
                SDWE_WARNNING(PAGE_WEIMI_WARNNING,"速比太大");
              }
            }
            else if((var_addr >= PAGE_WEIMI_STEP3_SPEED) && (var_addr < PAGE_WEIMI_STEP3_SPEED + 20))
            {//电机3速度设置
              u16 cnt;
              cnt = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              if(cnt <= 300)
              {
                weimi_para.step3_factor[(var_addr - PAGE_WEIMI_STEP3_SPEED) / 2] = cnt;
                W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI,sizeof(weimi_para));
                
                if(((var_addr - PAGE_WEIMI_STEP3_SPEED) / 2) == (MotorProcess.current_seg / 2))
                {//如果设置的送纬电机号段为当前号段，立刻更新送纬电机比例
                  MotorProcess.step3_factor = weimi_para.step3_factor[MotorProcess.current_seg / 2];
                }
              }
              else
              {
                Sdwe_disDigi(var_addr,weimi_para.step3_factor[(var_addr - PAGE_WEIMI_STEP3_SPEED) / 2],2);
                SDWE_WARNNING(PAGE_WEIMI_WARNNING,"速比太大");
              }
            }
            else if((var_addr >= PAGE_WEIMI_STEP1_ADD1) && (var_addr < PAGE_WEIMI_STEP1_ADD1 + 10))
            {//送纬速度加1——10
              if(weimi_para.step1_factor[var_addr - PAGE_WEIMI_STEP1_ADD1] < 300)
              {//送纬电机速度小于200才能加
                u8 position;
                u8 speed;
                position = var_addr - PAGE_WEIMI_STEP1_ADD1;
                weimi_para.step1_factor[position] += 1;
                speed = weimi_para.step1_factor[position];
                W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI,sizeof(weimi_para));
                Sdwe_disDigi(PAGE_WEIMI_STEP1_SPEED + 2 * position,speed,2);
                
                if(((var_addr - PAGE_WEIMI_STEP1_ADD1) / 2) == (MotorProcess.current_seg / 2))
                {//如果设置的送纬电机号段为当前号段，立刻更新送纬电机比例
                  MotorProcess.step1_factor = weimi_para.step1_factor[MotorProcess.current_seg / 2];
                }
              }
            }
            else if((var_addr >= PAGE_WEIMI_STEP1_SUB1) && (var_addr < PAGE_WEIMI_STEP1_SUB1 + 10))
            {//送纬速度减1——10
              if(weimi_para.step1_factor[var_addr - PAGE_WEIMI_STEP1_SUB1] > 0)
              {//送纬电机速度大于0
                u8 position;
                u8 speed;
                position = var_addr - PAGE_WEIMI_STEP1_SUB1;
                weimi_para.step1_factor[position] -= 1;
                speed = weimi_para.step1_factor[position];
                W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI,sizeof(weimi_para));
                Sdwe_disDigi(PAGE_WEIMI_STEP1_SPEED + 2 * position,speed,2);
                
                if(((var_addr - PAGE_WEIMI_STEP1_SUB1) / 2) == (MotorProcess.current_seg / 2))
                {//如果设置的送纬电机号段为当前号段，立刻更新送纬电机比例
                  MotorProcess.step1_factor = weimi_para.step1_factor[MotorProcess.current_seg / 2];
                }
              }
            }
            else if((var_addr >= PAGE_WEIMI_STEP2_ADD1) && (var_addr < PAGE_WEIMI_STEP2_ADD1 + 10))
            {//底线速度加1——10
              if(weimi_para.step2_factor[var_addr - PAGE_WEIMI_STEP2_ADD1] < 300)
              {//送纬电机速度小于200才能减
                u8 position;
                u8 speed;
                position = var_addr - PAGE_WEIMI_STEP2_ADD1;
                weimi_para.step2_factor[position] += 1;
                speed = weimi_para.step2_factor[position];
                W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI,sizeof(weimi_para));
                Sdwe_disDigi(PAGE_WEIMI_STEP2_SPEED + 2 * position,speed,2);
                
                if(((var_addr - PAGE_WEIMI_STEP2_ADD1) / 2) == (MotorProcess.current_seg / 2))
                {//如果设置的送纬电机号段为当前号段，立刻更新送纬电机比例
                  MotorProcess.step2_factor = weimi_para.step2_factor[MotorProcess.current_seg / 2];
                }
              }
            }
            else if((var_addr >= PAGE_WEIMI_STEP2_SUB1) && (var_addr < PAGE_WEIMI_STEP2_SUB1 + 10))
            {//底线速度减1——10
              if(weimi_para.step2_factor[var_addr - PAGE_WEIMI_STEP2_SUB1] > 0)
              {//送纬电机速度大于0
                u8 position;
                u8 speed;
                position = var_addr - PAGE_WEIMI_STEP2_SUB1;
                weimi_para.step2_factor[position] -= 1;
                speed = weimi_para.step2_factor[position];
                W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI,sizeof(weimi_para));
                Sdwe_disDigi(PAGE_WEIMI_STEP2_SPEED + 2 * position,speed,2);
                
                if(((var_addr - PAGE_WEIMI_STEP2_SUB1) / 2) == (MotorProcess.current_seg / 2))
                {//如果设置的送纬电机号段为当前号段，立刻更新送纬电机比例
                  MotorProcess.step2_factor = weimi_para.step2_factor[MotorProcess.current_seg / 2];
                }
              }
            }
            else if((var_addr >= PAGE_WEIMI_STEP3_ADD1) && (var_addr < PAGE_WEIMI_STEP3_ADD1 + 10))
            {//电机3速度加1——10
              if(weimi_para.step3_factor[var_addr - PAGE_WEIMI_STEP3_ADD1] < 200)
              {//电机3速度小于200才能减
                u8 position;
                u8 speed;
                position = var_addr - PAGE_WEIMI_STEP3_ADD1;
                weimi_para.step3_factor[position] += 1;
                speed = weimi_para.step3_factor[position];
                W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI,sizeof(weimi_para));
                Sdwe_disDigi(PAGE_WEIMI_STEP3_SPEED + 2 * position,speed,2);
                
                if(((var_addr - PAGE_WEIMI_STEP3_ADD1) / 2) == (MotorProcess.current_seg / 2))
                {//如果设置的电机3号段为当前号段，立刻更新送纬电机比例
                  MotorProcess.step3_factor = weimi_para.step3_factor[MotorProcess.current_seg / 2];
                }
              }
            }
            else if((var_addr >= PAGE_WEIMI_STEP3_SUB1) && (var_addr < PAGE_WEIMI_STEP3_SUB1 + 10))
            {//电机3速度减1——10
              if(weimi_para.step3_factor[var_addr - PAGE_WEIMI_STEP3_SUB1] > 0)
              {//电机3电机速度大于0
                u8 position;
                u8 speed;
                position = var_addr - PAGE_WEIMI_STEP3_SUB1;
                weimi_para.step3_factor[position] -= 1;
                speed = weimi_para.step3_factor[position];
                W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI,sizeof(weimi_para));
                Sdwe_disDigi(PAGE_WEIMI_STEP3_SPEED + 2 * position,speed,2);
                
                if(((var_addr - PAGE_WEIMI_STEP3_SUB1) / 2) == (MotorProcess.current_seg / 2))
                {//如果设置的电机3号段为当前号段，立刻更新送纬电机比例
                  MotorProcess.step3_factor = weimi_para.step3_factor[MotorProcess.current_seg / 2];
                }
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
            else if(var_addr == PAGE_WEIMI_MANUAL_FORWARD)
            {//手动向前
              u16 value;
              value = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              if(is_stop == 0)
              {//主轴停止时，才能手动控制
                if(value == 0x0001)
                {//第一次按下
                  servomotor_mode = MANUAL;
                  ServoMotorRunning(100);//一次发送100脉冲
                }
                else if(value == 0x0002)
                {//持续按下
                  servomotor_mode = MANUAL;
                  ServoMotorRunning(600);//一次发送100脉冲
                }
                else if(value == 0x0003)
                {//抬起
                  servomotor_mode = AUTO;
                }
              }
            }
            else if(var_addr == PAGE_WEIMI_MANUAL_BACKWARD)
            {//手动向后
              u16 value;
              value = (lcd_rev_buf[7] << 8) + lcd_rev_buf[8];
              if(is_stop == 0)
              {//主轴停止时，才能手动控制
                if(value == 0x0001)
                {//第一次按下
                  servomotor_mode = MANUAL;
                  TIM4_PWM_Config(FREQ_500KHZ,BACKWARD_PWM);//重新配置TIM4 CH2 PWM方向逆向
                  ServoMotorRunning(600);//一次发送100脉冲
                }
                else if(value == 0x0002)
                {//持续按下
                  servomotor_mode = MANUAL;
                  ServoMotorRunning(600);//一次发送100脉冲
                }
                else if(value == 0x0003)
                {//抬起
                  servomotor_mode = AUTO;
                  TIM4_PWM_Config(FREQ_500KHZ,FORWARD_PWM);//重新配置TIM4 CH2 PWM方向正向
                }
              }
            }
            else if(var_addr == PAGE_WEIMI_RESET)
            {
              MotorProcess.current_seg = 0;
              MotorProcess.current_wei = 0;
            }
            else if((var_addr >= PAGE_HISTORY_SELECT) && (var_addr <= (PAGE_HISTORY_SELECT + 9)))
            {//文件选择
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
  JINGSHA_FILE read_info;
  
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
        W25QXX_Read((u8 *)&read_info,(u32)(W25QXX_ADDR_JINGSHA + JINGSHA_SIZE * download_num),sizeof(read_info));//读出num的数据
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
          printf("打开文件失败\r\n");
          return;
        }
        char buf[1024] = "位置,设定张力\n",buf1[20];
        u16 len,i;
        for(i=0;i<30;i++)
        {
          sprintf(buf1,"%d#,%.2f\n",i + 1,(float)read_info.weight_value[i] / 1000.0);
          strcat(buf,buf1);
        }
        sprintf(buf,"%s时间日期,%02d/%02d/%02d,%02d:%02d:%02d\n\n",buf,read_info.year,
                read_info.month,read_info.day,read_info.hour,read_info.minute,read_info.second);
        
        PEILIAO_PARA PeiLiao_para;
        W25QXX_Read((u8 *)&PeiLiao_para,(u32)(W25QXX_ADDR_PEILIAO + CHANNENG_SIZE * download_num),sizeof(PeiLiao_para));
        sprintf(buf,"%s经纱,%.1f\n",buf,PeiLiao_para.latitude_weight);
        sprintf(buf,"%s纬纱,%.1f\n",buf,PeiLiao_para.longitude_weight);
        sprintf(buf,"%s橡胶,%.1f\n",buf,PeiLiao_para.rubber_weight);
        sprintf(buf,"%s成品,%.1f\n",buf,PeiLiao_para.final_weight);
        sprintf(buf,"%s织机条数,%d\n",buf,PeiLiao_para.loom_num);
        sprintf(buf,"%s损耗,%d\n",buf,PeiLiao_para.loss);
        sprintf(buf,"%s米任务,%d\n",buf,PeiLiao_para.total_meter_set);
        sprintf(buf,"%s重量任务,%.1f\n",buf,PeiLiao_para.total_weitht_set);
        sprintf(buf,"%s开度,%.1f\n",buf,PeiLiao_para.kaidu_set);
        sprintf(buf,"%s纬密,%.1f\n",buf,PeiLiao_para.weimi_set);
        sprintf(buf,"%s纬密显示,%d\n",buf,PeiLiao_para.weimi_dis_set);
        sprintf(buf,"%s补单数量,%d\n\n",buf,PeiLiao_para.add_meter_set);

        WEIMI_PARA WeiMi_para;
        W25QXX_Read((u8 *)&WeiMi_para,(u32)(W25QXX_ADDR_WEIMI + WEIMI_SIZE * download_num),sizeof(WeiMi_para));
        sprintf(buf,"%s,纬循环,纬厘米,纬英寸,过渡,送纬电机,底线电机\n",buf);
        sprintf(buf,"%s一段,%d,%.1f,%.1f,%d,%d,%d,%d\n",buf,WeiMi_para.total_wei_count[0],WeiMi_para.wei_cm_set[0],WeiMi_para.wei_inch_set[0],
                WeiMi_para.total_wei_count[1],WeiMi_para.step1_factor[0],WeiMi_para.step2_factor[0],WeiMi_para.step3_factor[0]);
        sprintf(buf,"%s二段,%d,%.1f,%.1f,%d,%d,%d,%d\n",buf,WeiMi_para.total_wei_count[2],WeiMi_para.wei_cm_set[1],WeiMi_para.wei_inch_set[1],
                WeiMi_para.total_wei_count[3],WeiMi_para.step1_factor[1],WeiMi_para.step2_factor[1],WeiMi_para.step3_factor[1]);
        sprintf(buf,"%s三段,%d,%.1f,%.1f,%d,%d,%dv\n",buf,WeiMi_para.total_wei_count[4],WeiMi_para.wei_cm_set[2],WeiMi_para.wei_inch_set[2],
                WeiMi_para.total_wei_count[5],WeiMi_para.step1_factor[2],WeiMi_para.step2_factor[2],WeiMi_para.step3_factor[2]);
        sprintf(buf,"%s四段,%d,%.1f,%.1f,%d,%d,%d,%d\n",buf,WeiMi_para.total_wei_count[6],WeiMi_para.wei_cm_set[3],WeiMi_para.wei_inch_set[3],
                WeiMi_para.total_wei_count[7],WeiMi_para.step1_factor[3],WeiMi_para.step2_factor[3],WeiMi_para.step3_factor[3]);
        sprintf(buf,"%s五段,%d,%.1f,%.1f,%d,%d,%d,%d\n",buf,WeiMi_para.total_wei_count[8],WeiMi_para.wei_cm_set[4],WeiMi_para.wei_inch_set[4],
                WeiMi_para.total_wei_count[9],WeiMi_para.step1_factor[4],WeiMi_para.step2_factor[4],WeiMi_para.step3_factor[4]);
        sprintf(buf,"%s六段,%d,%.1f,%.1f,%d,%d,%d,%d\n",buf,WeiMi_para.total_wei_count[10],WeiMi_para.wei_cm_set[5],WeiMi_para.wei_inch_set[5],
                WeiMi_para.total_wei_count[11],WeiMi_para.step1_factor[5],WeiMi_para.step2_factor[5],WeiMi_para.step3_factor[5]);
        sprintf(buf,"%s七段,%d,%.1f,%.1f,%d,%d,%d,%d\n",buf,WeiMi_para.total_wei_count[12],WeiMi_para.wei_cm_set[6],WeiMi_para.wei_inch_set[6],
                WeiMi_para.total_wei_count[13],WeiMi_para.step1_factor[6],WeiMi_para.step2_factor[6],WeiMi_para.step3_factor[6]);
        sprintf(buf,"%s八段,%d,%.1f,%.1f,%d,%d,%d,%d\n",buf,WeiMi_para.total_wei_count[14],WeiMi_para.wei_cm_set[7],WeiMi_para.wei_inch_set[7],
                WeiMi_para.total_wei_count[15],WeiMi_para.step1_factor[7],WeiMi_para.step2_factor[7],WeiMi_para.step3_factor[7]);
        sprintf(buf,"%s九段,%d,%.1f,%.1f,%d,%d,%d,%d\n",buf,WeiMi_para.total_wei_count[16],WeiMi_para.wei_cm_set[8],WeiMi_para.wei_inch_set[8],
                WeiMi_para.total_wei_count[17],WeiMi_para.step1_factor[8],WeiMi_para.step2_factor[8],WeiMi_para.step3_factor[8]);
        sprintf(buf,"%s十段,%d,%.1f,%.1f,%d,%d,%d,%d\n",buf,WeiMi_para.total_wei_count[18],WeiMi_para.wei_cm_set[9],WeiMi_para.wei_inch_set[9],
                WeiMi_para.total_wei_count[19],WeiMi_para.step1_factor[9],WeiMi_para.step2_factor[9],WeiMi_para.step3_factor[9]);
        
        len = strlen(buf);
        result = f_write(&file,buf,len,&bw);
        if(result == FR_OK)
        {
          SDWE_WARNNING(PAGE_HISTORY_TEXT_FILE_WARN,"写入成功");
          printf("%s.CSV文件写入成功\r\n",name_1);
        }
        else
        {
          SDWE_WARNNING(PAGE_HISTORY_TEXT_FILE_WARN,"写入失败");
          printf("%s.CSV文件写入失败\r\n",name_1);
        }
        /* 关闭文件*/
        f_close(&file);
        
        /* 卸载文件系统 */
        f_mount(NULL, "2:", 0);
      }
      else
      {
        SDWE_WARNNING(PAGE_HISTORY_TEXT_FILE_WARN,"无U盘");
        printf("U盘未插入\r\n");
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
    ServoMotorRunning(20);
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
      {//在产能页面连续读取卡
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
                  //获取到卡号后，判断卡号是否为A班卡、B班卡、维护卡、未注册卡
                  card_type = get_card_type(card_id);
                  if(card_type == FUNC_REPAIR)
                  {//维护卡
                    Sdwe_disPicture(PAGE_REPAIR);//维护卡进入维护页面
                    Sdwe_stop_page(&device_info);
                  }
                  else if(card_type == FUNC_CLASS_A)
                  {//A班卡
                    if(device_info.class_para.class_enable_onoff == 0)
                    {//如果换班功能未启用，则产量一直记在A班
                      card_record = 1;//A班
                      SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"未启动换班");
                    }
                    else
                    {
                      if(get_class_time(&rtc_time,&device_info) == CLASS_A)
                      {//A班卡打在A时间段
                        card_record = 1;//A班
                        if(old_card_record != card_record)
                        {
                          old_card_record = card_record;
                          SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"A班卡");
                        }
                        else
                        {
                          SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"重复打卡");
                        }
                      }
                      else
                      {//A班卡打在B时间段
                        SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"无效操作");
                      }
                    }
                  }
                  else if(card_type == FUNC_CLASS_B)
                  {//B班卡
                    if(device_info.class_para.class_enable_onoff == 0)
                    {//如果换班功能未启用，则产量一直记在A班
                      card_record = 1;//A班
                      SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"未启动换班");
                    }
                    else
                    {
                      if(get_class_time(&rtc_time,&device_info) == CLASS_B)
                      {//B班卡打在B时间段
                        card_record = 2;//B班
                        if(old_card_record != card_record)
                        {
                          old_card_record = card_record;
                          SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"B班卡");
                        }
                        else
                        {
                          SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"重复打卡");
                        }
                      }
                      else
                      {//A班卡打在B时间段
                        SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"无效操作");
                      }
                    }
                    
                  }
                  else
                  {//未注册卡
                    SDWE_WARNNING(PAGE_PRODUCT_RFID_WARNNING,"未注册卡");
                  }
                }
              }
            }
          }
        }
      }
      else
      {//在员工卡页面单次读取卡
        if(card_config > WRITE_PERMISSION)
        {
          rfid_rev_flag = 0;
          rfid_rev_cnt = 0;
          rc522_cmd_request(REQUEST_TYPE_ALL);//请求卡
          vTaskDelay(100);
          if(rfid_rev_flag)
          {//收到模块返回命令
            rfid_rev_flag = 0;
            if(rfid_rev_cnt == 8)
            {//
              isCard = (rfid_rev_buf[5] << 8) + rfid_rev_buf[4];
              if(isCard == Mifare1_S50)
              {//识别到s50卡
                printf("s50\r\n");
                rfid_rev_cnt = 0;
                rc522_cmd_anticoll(COLLISION_GRADE_1);//防碰撞获取卡片ID
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
                        {//卡片不存在，添加到数据库
                          inc_card_type(card_id,FUNC_CLASS_A);
                          SDWE_WARNNING(PAGE_CARD_WARNNING,"录入成功");
                        }
                        else
                        {
                          SDWE_WARNNING(PAGE_CARD_WARNNING,"卡片已存在");
                        }
                      }
                      else if(card_config == WRITE_DEC_A)
                      {
                        if(card_type == FUNC_CLASS_A)
                        {//卡片存在，删除
                          
                        }
                      }
                      else if(card_config == WRITE_INC_B)
                      {
                        if(card_type == FUNC_IDLE)
                        {//卡片不存在，添加到数据库
                          inc_card_type(card_id,FUNC_CLASS_B);
                          SDWE_WARNNING(PAGE_CARD_WARNNING,"录入成功");
                        }
                        else
                        {
                          SDWE_WARNNING(PAGE_CARD_WARNNING,"卡片已存在");
                        }
                      }
                      else if(card_config == WRITE_DEC_B)
                      {
                        
                      }
                      else if(card_config == WRITE_INC_REPAIR)
                      {
                        if(card_type == FUNC_IDLE)
                        {//卡片不存在，添加到数据库
                          inc_card_type(card_id,FUNC_REPAIR);
                          SDWE_WARNNING(PAGE_CARD_WARNNING,"录入成功");
                        }
                        else
                        {
                          SDWE_WARNNING(PAGE_CARD_WARNNING,"卡片已存在");
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
    Task_iwdg_refresh(TASK_RFID);
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
              {//获取到重量并显示到屏幕
                SlavePara.value_sample[readSlave.addr - 1] = readSlave.value;
                Sdwe_disDigi(PAGE1_SAMPLE_VALUE1 + readSlave.addr - 1,SlavePara.value_sample[index] / 10,2);
                Sdwe_writeIcon(PAGE1_SLAVE_STATE1 + readSlave.addr - 1,VGUS_OFF);//对应位置正确错误图标
                Sdwe_disDigi(PAGE1_ECHO_WEIGHT + readSlave.addr - 1,readSlave.set_value / 10,2);
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
    if((device_info.func_onoff.jingsha == 1) && (isDevicePeriod == 0))
    {
      isWork = GetDeviceState();
    }
    else
    {//产能功能未打开，不工作
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
        if(xQueueSend(xQueue_message,                  /* 消息队列句柄 */
                      (void *) &ptMsg,           /* 发送结构体指针变量ptMsg的地址 */
                      (TickType_t)10) == pdPASS )
        {
  //        printf("isWork is %d 队列发送成功\r\n",isWork);
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
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(200); /* 设置最大等待时间为200ms */
  while(1)
  {
    xResult = xSemaphoreTake(xSemaphore_readDisk, (TickType_t)xMaxBlockTime);
    if(xResult == pdTRUE)
    {
      if(usb_disk_flag == 1)
      {//U盘存在
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
        
        if(readDisk == DISK_FILENAME)
        {//显示U盘文件名
          DiskFile_init();
          Sdwe_disPicture(PAGE_U);//跳转至U盘文件页面
          memset(read_file_select,0,10);//清除文件选择Buff
          Sdwe_ClearReadDisk();//清除文件选择图标
          
          /* 读取当前文件夹下的文件和目录 */
        #if _USE_LFN
          FileInf.lfname = lfname;
          FileInf.lfsize = 256;
        #endif
          printf("属性        |  文件大小 | 短文件名 | 长文件名\r\n");
          for(cnt = 0; ;cnt++)
          {
            result = f_readdir(&DirInf,&FileInf); 		/* 读取目录项，索引会自动下移 */
            if(result != FR_OK || FileInf.fname[0] == 0)
            {
              break;
            }
            if(FileInf.fname[0] == '.')
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
              if(Disk_File.filenum < 10)
              {
                char *fn;
                //判断是长文件名还是短文件名如果是长文件名就取长文件名否则就去短文件名的信息
                fn = (*FileInf.lfname?FileInf.lfname:FileInf.fname);
                if(strstr(fn,".CSV"))
                {//文件为CSV表格
                  memcpy(Disk_File.filename[Disk_File.filenum],fn,strlen(fn));
//                  Disk_File.filename[Disk_File.filenum][strlen(fn) - 3] = '\0';
                  Sdwe_disString(PAGE_U_TEXT_FILENAME1 + Disk_File.filenum * 20,fn,strlen(fn));
                  Disk_File.filenum++;
                }
              }
            }
            /* 打印文件大小, 最大4G */
            printf(" %10d", FileInf.fsize);
            printf("  %s |", FileInf.fname);	/* 短文件名 */
            printf("  %s\r\n", (char *)FileInf.lfname);	/* 长文件名 */
          }
        }
        else if(readDisk == DISK_FILEDATA)
        {//读取U盘文件
          char name[20];
          u8 file_buf[1000];
//          file_buf = mymalloc(SRAMIN,1000);
          sprintf(name,"2:/%s",Disk_File.filename[readFilenum]);
          /* 打开文件 */
          result = f_open(&file,name,FA_OPEN_EXISTING | FA_READ);
          if (result !=  FR_OK)
          {
            printf("Don't Find File : %s\r\n",name);
            return;
          }
          
          /* 读取文件 */
          result = f_read(&file, &file_buf, sizeof(file_buf) - 1, &bw);
          if(result == FR_OK)
          {
            __set_PRIMASK(1); 
            read_from_disk((char *)file_buf);
            __set_PRIMASK(0); 
            SDWE_WARNNING(PAGE_U_TEXT_READ_STATE,"读取成功");
          }
          else
          {
            SDWE_WARNNING(PAGE_U_TEXT_READ_STATE,"读取失败");
          }
//          myfree(SRAMIN,file_buf);
          /* 关闭文件*/
          f_close(&file);
        }
        /* 卸载文件系统 */
        f_mount(NULL, "2:", 0);
      }
      else
      {
        SDWE_WARNNING(PAGE_HISTORY_TEXT_FILE_WARN,"无U盘");
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
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(200); /* 设置最大等待时间为200ms */
  bsp_InitCloseSW();
  total_meter_gross = (u32)(peiliao_para.total_meter_set * (1 + peiliao_para.loss / 100.0));//总米设置含损耗
  total_weight_gross = (u32)(peiliao_para.total_weitht_set * (1 + peiliao_para.loss / 100.0));//总重量设置含损耗
  if(total_meter_gross > 0)
  {
    if(product_para.product_complete >= total_meter_gross)
    {
      plan_complete = 1;
    }
  }
  if(total_weight_gross > 0)
  {
    if(product_para.weight_complete >= total_weight_gross)
    {
      plan_complete = 1;
    }
  }
//  vTaskDelay(2000);//显示屏上电后需要大概2s启动
  Sdwe_disString(PAGE1_SYSTEM_STATE,(u8 *)system_state_dis[device_info.system_state],strlen(system_state_dis[device_info.system_state]));
  while(1)
  {
    xResult = xSemaphoreTake(xSemaphore_pluse, (TickType_t)xMaxBlockTime);
    if(xResult == pdTRUE)
    {
      if((device_info.func_onoff.channeng == 1) && (isDevicePeriod == 0))
      {//产能使能、无试用期限
        pluse_count++;
        work_idle_time = 0;
        if(device_info.system_state == SYS_NORMAL)
        {//系统正常时进行产能计算
          switch(plan_complete)
          {
            case 0:
              RELAY_OPEN();//有脉冲信号后，继电器断开
              count++;
              product_para.weicount_kilowei = product_para.weicount_kilowei + count / peiliao_para.weimi_dis_set;
              if((count % (peiliao_para.weimi_dis_set)) == 0)
              {//纬纱/千纬
                count = 0;
                Sdwe_disDigi(PAGE_PRODUCT_KILOCOUNT,product_para.weicount_kilowei,4);
                W25QXX_Write((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
              }
              if((total_meter_gross == 0) && (total_weight_gross == 0))
              {
                
              }
              else
              {
                if(card_record == 2)
                {//B班
                  product_para.product_b = product_para.product_b + product_per_meter(&peiliao_para,1);//b班产量
                  p_value = get_float_1bit(product_para.product_b);
                }
                else
                {//A班
                  product_para.product_a = product_para.product_a + product_per_meter(&peiliao_para,1);//b班产量
                  p_value = get_float_1bit(product_para.product_a);
                }
                if(p_value != old_p_value)
                {//产量数据变化大于=0.1时，显示更新
                  old_p_value = p_value;
                  Sdwe_disDigi(PAGE_PRODUCT_B,(int)(product_para.product_b * 10),4);
                  Sdwe_disDigi(PAGE_PRODUCT_A,(int)(product_para.product_a * 10),4);
                  if(total_meter_gross > 0)
                  {
                    product_para.product_complete = product_complete_meter(&product_para);//已完成产量
                    product_para.product_uncomplete = total_meter_gross - product_para.product_complete;//未完成产量
                    Sdwe_disDigi(PAGE_PRODUCT_COMPLETE,(int)(product_para.product_complete * 10),4);
                    Sdwe_disDigi(PAGE_PRODUCT_UNCOMPLETE,(int)(product_para.product_uncomplete * 10),4);
                    if(product_para.product_uncomplete <= 0)
                    {//完成产量
                      RELAY_CLOSE();//产量完成后，继电器闭合
                      if(old_plan_complete == 0)
                      {
                        plan_complete = 1;
                      }
                      else
                      {
                        plan_complete = 2;
                      }
                      Sdwe_disString(PAGE_PRODUCT_RFID_WARNNING,"产量完成",strlen("产量完成"));
                      device_info.system_state = SYS_STOP;
                      Sdwe_disString(PAGE1_SYSTEM_STATE,(u8 *)system_state_dis[device_info.system_state],strlen(system_state_dis[device_info.system_state]));
                    }
                  }
                  else
                  {
                    product_para.product_complete = 0;//已完成产量
                    product_para.product_uncomplete = 0;//未完成产量
                    Sdwe_disDigi(PAGE_PRODUCT_COMPLETE,(int)(product_para.product_complete * 10),4);
                    Sdwe_disDigi(PAGE_PRODUCT_UNCOMPLETE,(int)(product_para.product_uncomplete * 10),4);
                  }
                  if(total_weight_gross > 0)
                  {
                    product_para.weight_complete = product_complete_kilo(&product_para,&peiliao_para);//已完成重量
                    product_para.weight_uncomplete = total_weight_gross - product_para.weight_complete;//未完成重量
                    Sdwe_disDigi(PAGE_PRODUCT_COMPLETE_W,(int)(product_para.weight_complete * 10),4);
                    Sdwe_disDigi(PAGE_PRODUCT_UNCOMPLETE_W,(int)(product_para.weight_uncomplete * 10),4);
                    if(product_para.weight_uncomplete <= 0)
                    {//完成重量
                      if(old_plan_complete == 0)
                      {
                        plan_complete = 1;
                      }
                      else
                      {
                        plan_complete = 2;
                      }
                      RELAY_CLOSE();//产量完成后，继电器闭合
                      Sdwe_disString(PAGE_PRODUCT_RFID_WARNNING,"产量完成",strlen("产量完成"));
                      device_info.system_state = SYS_STOP;//产能完成后系统停止运行
                      Sdwe_disString(PAGE1_SYSTEM_STATE,(u8 *)system_state_dis[device_info.system_state],strlen(system_state_dis[device_info.system_state]));
                    }
                  }
                  else
                  {
                    product_para.weight_complete = 0;//已完成重量
                    product_para.weight_uncomplete = 0;//未完成重量
                    Sdwe_disDigi(PAGE_PRODUCT_COMPLETE_W,(int)(product_para.weight_complete * 10),4);
                    Sdwe_disDigi(PAGE_PRODUCT_UNCOMPLETE_W,(int)(product_para.weight_uncomplete * 10),4);
                  }
                }
              }
              break;
            case 1:
              RELAY_CLOSE();//产量完成后，继电器闭合
              if(peiliao_para.add_meter_set != 0)
              {
                //在现有总产量基础上增加补单数量,补单数量包含损耗
                old_plan_complete = 1;
                total_meter_gross = (u32)(total_meter_gross + peiliao_para.add_meter_set * (1 + peiliao_para.loss / 100.0));
                plan_complete = 0;
              }
              break;
            case 2:
              RELAY_CLOSE();//产量完成后，继电器闭合
              break;
          }
        }
        else
        {//系统停机
          RELAY_CLOSE();
        }
      }
      else
      {
        RELAY_CLOSE();
      }
    }
    Task_iwdg_refresh(TASK_ManageCapacity);
  }
}

static void vTaskMotorControl(void *pvParameters)
{
  BaseType_t xResult;
  u16 pluse_step_src1,pluse_step_src2,pluse_step_src3;
  u16 servo_per_wei_src;//伺服电机过渡期不仅数每纬
  u16 servo_diff;//伺服电机过渡期纬差
  u8 symbol_servo = 0;
  u16 speed_diff1,speed_diff2,speed_diff3;
  u8 symbol1 = 0,symbol2 = 0,symbol3 = 0;
  u16 guodu;
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(200); /* 设置最大等待时间为200ms */
  bsp_InitStepMotor();
  vTaskDelay(200);
  
  TIM4_CH1_ConfigPwmOut(FREQ_500KHZ,10);
  TIM4_CH2_ConfigPwmOut(FREQ_500KHZ,10);
  TIM4_PWM_Config(FREQ_500KHZ,FORWARD_PWM);
  DIFF_G_init();

  get_weimi_para(&weimi_para,&device_info,&MotorProcess);//获取当前参数
  servomotor_step = MotorStepCount(&device_info,&weimi_para,MotorProcess.current_seg);//获取段号对应的脉冲数/纬
  valid_seg = get_valid_seg(&weimi_para);
  while(1)
  {
    xResult = xSemaphoreTake(xSemaphore_encoder, (TickType_t)xMaxBlockTime);
    if(xResult == pdTRUE)
    {
      if(device_info.func_onoff.channeng)
      {
        if(servomotor_mode == AUTO)
        {//自动模式下，才会增加纬号
          MotorProcess.current_wei++;//纬号增加
          weimi_para.real_wei_count[MotorProcess.current_seg] = MotorProcess.current_wei;
          if((MotorProcess.current_seg % 2) == 0)
          {//偶数段号取纬循环
            Sdwe_disDigi(PAGE_WEIMI_REALWEI_1 + MotorProcess.current_seg,MotorProcess.current_wei,4);
          }
          else
          {//奇数段号取过渡循环号
            Sdwe_disDigi(PAGE_WEIMI_REAL_MEDIAN_1 + MotorProcess.current_seg - 1,MotorProcess.current_wei,4);
          }
          
          if(step_motor_adjust)
          {//过渡纬号，步进电机调速，每纬调节一次转速
            guodu++;
            u16 step_speed1,step_speed2;//步进电机转速，转/分钟
            if(symbol1 == 0)//速度增加
              step_speed1 = (pluse_step_src1 + speed_diff1 / MotorProcess.total_wei * guodu);
            else//速度减少
              step_speed1 = (pluse_step_src1 - speed_diff1 / MotorProcess.total_wei * guodu);
            if(symbol2 == 0)//速度增加
              step_speed2 = (pluse_step_src2 + speed_diff2 / MotorProcess.total_wei * guodu);
            else//速度减少
              step_speed2 = (pluse_step_src2 - speed_diff2 / MotorProcess.total_wei * guodu);
            u16 sstep1,sstep2,sstep3;
            sstep1 = from_speed_step(step_speed1);
            sstep2 = from_speed_step(step_speed2);
            StepMotor_adjust_speed(STEPMOTOR2,(u32)sstep2);
            StepMotor_adjust_speed(STEPMOTOR1,(u32)sstep1); 
//            printf("sstep1 %d,sstep2 %d\r\n",sstep1,sstep2);
            if(symbol_servo == 0)//正数增加
              sstep3 = servo_per_wei_src + servo_diff / MotorProcess.total_wei * guodu;
            else//负数减少
              sstep3 = servo_per_wei_src - servo_diff / MotorProcess.total_wei * guodu;
            ServoMotorRunning(sstep3);
          }
          else
          {
            ServoMotorRunning(servomotor_step);//发送一纬对应的脉冲信号到伺服驱动器
          }
          if(MotorProcess.current_wei >= MotorProcess.total_wei)
          {//当前纬号超过纬循环
            MotorProcess.current_wei = 0;
            MotorProcess.current_seg++;//进入下一段号
            if(MotorProcess.current_seg >= 21)
            {
              MotorProcess.current_seg = 0;
            }
            MotorProcess.total_wei = weimi_para.total_wei_count[MotorProcess.current_seg];
            
            if((weimi_para.total_wei_count[MotorProcess.current_seg] > 0) &&
               (weimi_para.wei_cm_set[MotorProcess.current_seg / 2] > 0))
            {//下个段号纬循环和纬厘米设置大于0，进行下一个段号
              if((MotorProcess.current_seg % 2) == 0)
              {//到下个段号，改变脉冲/纬
                servomotor_step = MotorStepCount(&device_info,&weimi_para,MotorProcess.current_seg / 2);//计算伺服电机脉冲数
                step_motor_adjust = 0;
                MotorProcess.step1_factor = weimi_para.step1_factor[MotorProcess.current_seg / 2];
                MotorProcess.step2_factor = weimi_para.step2_factor[MotorProcess.current_seg / 2];
                MotorProcess.step3_factor = weimi_para.step3_factor[MotorProcess.current_seg / 2];
              }
              else
              {//到过渡号
                //计算步进电机每秒脉冲数
                //目标每分钟转速/60=每秒转速
                //每秒转速*360/1.8*8=步进电机每秒需要脉冲数
                int ratio_diff1,ratio_diff2,ratio_diff3;//相邻速比差
                valid_seg = get_valid_seg(&weimi_para);
                if(MotorProcess.current_seg == (valid_seg - 1))
                {//到达最大有效段
                  if(weimi_para.step1_factor[0] >= weimi_para.step1_factor[MotorProcess.current_seg / 2])
                  {
                    symbol1 = 0;//正数
                    ratio_diff1 = weimi_para.step1_factor[0] - weimi_para.step1_factor[MotorProcess.current_seg / 2];  
                  }
                  else
                  {
                    symbol1 = 1;
                    ratio_diff1 = weimi_para.step1_factor[MotorProcess.current_seg / 2] - weimi_para.step1_factor[0];  
                  }
                  if(weimi_para.step2_factor[0] >= weimi_para.step2_factor[MotorProcess.current_seg / 2])
                  {
                    symbol2 = 0;//正数
                    ratio_diff2 = weimi_para.step2_factor[0] - weimi_para.step2_factor[MotorProcess.current_seg / 2];  
                  }
                  else
                  {
                    symbol2 = 1;
                    ratio_diff2 = weimi_para.step2_factor[MotorProcess.current_seg / 2] - weimi_para.step2_factor[0];  
                  }
                  if(weimi_para.step3_factor[0] >= weimi_para.step3_factor[MotorProcess.current_seg / 2])
                  {
                    symbol3 = 0;//正数
                    ratio_diff3 = weimi_para.step3_factor[0] - weimi_para.step3_factor[MotorProcess.current_seg / 2];  
                  }
                  else
                  {
                    symbol3 = 1;
                    ratio_diff3 = weimi_para.step3_factor[MotorProcess.current_seg / 2] - weimi_para.step3_factor[0];  
                  }
                  
                  u16 value1,value2;
                  value1 = MotorStepCount(&device_info,&weimi_para,0);
                  value2 = MotorStepCount(&device_info,&weimi_para,MotorProcess.current_seg / 2);
                  if(value1 >= value2)
                  {//第一段大于最后一段步数差
                    symbol_servo = 0;
                    servo_diff = value1 - value2;
                  }
                  else
                  {
                    symbol_servo = 1;
                    servo_diff = value2 - value1;
                  }
                }
                else
                {
                  if(weimi_para.step1_factor[MotorProcess.current_seg / 2 + 1] >= weimi_para.step1_factor[MotorProcess.current_seg / 2])
                  {
                    symbol1 = 0;
                    ratio_diff1 = weimi_para.step1_factor[MotorProcess.current_seg / 2 + 1] - weimi_para.step1_factor[MotorProcess.current_seg / 2];
                  }
                  else
                  {
                    symbol1 = 1;
                    ratio_diff1 = weimi_para.step1_factor[MotorProcess.current_seg / 2] - weimi_para.step1_factor[MotorProcess.current_seg / 2 + 1];
                  }
                  if(weimi_para.step2_factor[MotorProcess.current_seg / 2 + 1] >= weimi_para.step2_factor[MotorProcess.current_seg / 2])
                  {
                    symbol2 = 0;
                    ratio_diff2 = weimi_para.step2_factor[MotorProcess.current_seg / 2 + 1] - weimi_para.step2_factor[MotorProcess.current_seg / 2];
                  }
                  else
                  {
                    symbol2 = 1;
                    ratio_diff2 = weimi_para.step2_factor[MotorProcess.current_seg / 2] - weimi_para.step2_factor[MotorProcess.current_seg / 2 + 1];
                  }
                  if(weimi_para.step3_factor[MotorProcess.current_seg / 2 + 1] >= weimi_para.step3_factor[MotorProcess.current_seg / 2])
                  {
                    symbol3 = 0;
                    ratio_diff3 = weimi_para.step3_factor[MotorProcess.current_seg / 2 + 1] - weimi_para.step3_factor[MotorProcess.current_seg / 2];
                  }
                  else
                  {
                    symbol3 = 1;
                    ratio_diff3 = weimi_para.step3_factor[MotorProcess.current_seg / 2] - weimi_para.step3_factor[MotorProcess.current_seg / 2 + 1];
                  }
                  
                  u16 value1,value2;
                  value1 = MotorStepCount(&device_info,&weimi_para,MotorProcess.current_seg / 2 + 1);
                  value2 = MotorStepCount(&device_info,&weimi_para,MotorProcess.current_seg / 2);
                  if(value1 >= value2)
                  {//计算第一段和最后一段纬号差
                    symbol_servo = 0;
                    servo_diff = value1 - value2;
                  }
                  else
                  {
                    symbol_servo = 1;
                    servo_diff = value2 - value1;
                  }
                }
                speed_diff1 = speed_zhu * ratio_diff1 / 1000.0;//计算相邻段号步进电机运行频率差
                speed_diff2 = speed_zhu * ratio_diff2 / 1000.0;//计算相邻段号步进电机运行频率差
                speed_diff3 = speed_zhu * ratio_diff3 / 1000.0;//计算相邻段号步进电机运行频率差
                pluse_step_src1 = speed_zhu * weimi_para.step1_factor[(MotorProcess.current_seg - 1) / 2] / 1000.0;
                pluse_step_src2 = speed_zhu * weimi_para.step2_factor[(MotorProcess.current_seg - 1) / 2] / 1000.0;
                pluse_step_src3 = speed_zhu * weimi_para.step3_factor[(MotorProcess.current_seg - 1) / 2] / 1000.0;
                servo_per_wei_src = MotorStepCount(&device_info,&weimi_para,MotorProcess.current_seg / 2);
                step_motor_adjust = 1;//步进电机过渡调速标志
                guodu = 0;
              }
            }
            else
            {//下个段号纬循环等于0，前面所有的段号循环
              MotorProcess.current_seg = 0;
              MotorProcess.total_wei = weimi_para.total_wei_count[MotorProcess.current_seg];
              MotorProcess.step1_factor = weimi_para.step1_factor[MotorProcess.current_seg / 2];
              MotorProcess.step2_factor = weimi_para.step2_factor[MotorProcess.current_seg / 2];
              MotorProcess.step3_factor = weimi_para.step3_factor[MotorProcess.current_seg / 2];
              servomotor_step = MotorStepCount(&device_info,&weimi_para,MotorProcess.current_seg / 2);//计算伺服电机脉冲数
              step_motor_adjust = 0;
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
  u32 step1_count;
  u32 step2_count;
  u32 step3_count;
  ENC_Init();//编码器测速初始化
  while(1)
  {
    vTaskDelay(10);
    speed_zhu = ENC_Calc_Average_Speed();
    printf("speed_zhu is %d\r\n",speed_zhu);
    if(speed_zhu > 0)
    {
      if(device_info.func_onoff.weimi)
      {
        is_stop = 1;
        if(is_stop != old_is_stop)
        {//主轴速度大于0时，步进电机开始运行
          old_is_stop = is_stop;
          StepMotor_start(STEPMOTOR1);
          StepMotor_start(STEPMOTOR2);
          StepMotor_start(STEPMOTOR3);
        }
        if(step_motor_adjust == 0)//过渡期不按照速比控制步进电机
        {
          step1_count = from_speed_step((float)speed_zhu * MotorProcess.step1_factor / 1000.0);//计算步进电机脉冲频率
          step2_count = from_speed_step((float)speed_zhu * MotorProcess.step2_factor / 1000.0);//计算步进电机脉冲频率
          step3_count = from_speed_step((float)speed_zhu * MotorProcess.step3_factor / 1000.0);//计算步进电机脉冲频率
          if(step1_count == 0)
          {//送纬电机速度设置为0
            step1_stop = 0;
            if(step1_stop != old_step1_stop)
            {//停止送纬电机工作
              old_step1_stop = step1_stop;
              StepMotor_stop(STEPMOTOR1);
            }
          }
          else
          {//送纬电机速度设置大于0
            step1_stop = 0xff;
            if(step1_stop != old_step1_stop)
            {//开启送纬电机工作
              old_step1_stop = step1_stop;
              StepMotor_start(STEPMOTOR1);
            }
            StepMotor_adjust_speed(STEPMOTOR1,step1_count);
          }
          if(step2_count == 0)
          {//送纬电机速度设置为0
            step2_stop = 0;
            if(step2_stop != old_step2_stop)
            {//停止送纬电机工作
              old_step2_stop = step2_stop;
              StepMotor_stop(STEPMOTOR2);
            }
          }
          else
          {//送纬电机速度设置大于0
            step2_stop = 0xff;
            if(step2_stop != old_step2_stop)
            {//开启送纬电机工作
              old_step2_stop = step2_stop;
              StepMotor_start(STEPMOTOR2);
            }
            StepMotor_adjust_speed(STEPMOTOR2,step2_count);
          }
          if(step3_count == 0)
          {//电机3速度设置为0
            step3_stop = 0;
            if(step3_stop != old_step3_stop)
            {//停止电机3工作
              old_step3_stop = step3_stop;
              StepMotor_stop(STEPMOTOR3);
            }
          }
          else
          {//电机3速度设置大于0
            step3_stop = 0xff;
            if(step3_stop != old_step3_stop)
            {//开启电机3工作
              old_step3_stop = step3_stop;
              StepMotor_start(STEPMOTOR3);
            }
            StepMotor_adjust_speed(STEPMOTOR3,step3_count);
          }
        }
      }
    }
    else
    {
      is_stop = 0;
      if(is_stop != old_is_stop)
      {//主轴速度为0时，停止步进电机
        old_is_stop = is_stop;
        StepMotor_stop(STEPMOTOR1);
        StepMotor_stop(STEPMOTOR2);
        StepMotor_stop(STEPMOTOR3);
      }
    }
    Task_iwdg_refresh(TASK_Freq);
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
              512,               	/* 任务栈大小，单位word，也就是4字节 */
              NULL,              	/* 任务参数  */
              2,                 	/* 任务优先级*/
              &xHandleTaskRFID );  /* 任务句柄  */
  xTaskCreate( vTaskMsgPro,     		/* 任务函数  */
              "vTaskMsgPro",   		/* 任务名    */
              256,             		/* 任务栈大小，单位word，也就是4字节 */
              NULL,           		/* 任务参数  */
              3,               		/* 任务优先级*/
              &xHandleTaskMsgPro );  /* 任务句柄  */
  xTaskCreate( vTaskRev485,     		/* 任务函数  */
              "vTaskRev485",   		/* 任务名    */
              512,            		/* 任务栈大小，单位word，也就是4字节 */
              NULL,           		/* 任务参数  */
              4,              		/* 任务优先级*/
              &xHandleTaskRev485 );   /* 任务句柄  */
  xTaskCreate( vTaskTaskLCD,   	/* 任务函数  */
              "vTaskLCD",     	/* 任务名    */
              1024,               	/* 任务栈大小，单位word，也就是4字节 */
              NULL,              	/* 任务参数  */
              5,                 	/* 任务优先级*/
              &xHandleTaskLCD );  /* 任务句柄  */
  xTaskCreate( vTaskMassStorage,    		/* 任务函数  */
              "vTaskMassStorage",  		/* 任务名    */
              1024,         		/* 任务栈大小，单位word，也就是4字节 */
              NULL,        		/* 任务参数  */
              6,           		/* 任务优先级*/
              &xHandleTaskMassStorage ); /* 任务句柄  */
  xTaskCreate( vTaskReadDisk,    		/* 任务函数  */
              "vTaskReadDisk",  		/* 任务名    */
              1234,         		/* 任务栈大小，单位word，也就是4字节 */
              NULL,        		/* 任务参数  */
              7,           		/* 任务优先级*/
              &xHandleTaskReadDisk); /* 任务句柄  */
  xTaskCreate( vTaskManageCapacity,    		/* 任务函数  */
              "vTaskManageCapacity",  		/* 任务名    */
              512,         		/* 任务栈大小，单位word，也就是4字节 */
              NULL,        		/* 任务参数  */
              8,           		/* 任务优先级*/
              &xHandleTaskManageCapacity); /* 任务句柄  */
  xTaskCreate( vTaskMotorControl,    		/* 任务函数  */
              "vTaskMotorControl",  		/* 任务名    */
              256,         		/* 任务栈大小，单位word，也就是4字节 */
              NULL,        		/* 任务参数  */
              9,           		/* 任务优先级*/
              &xHandleTaskMotorControl); /* 任务句柄  */
  xTaskCreate( vTaskFreq,    		/* 任务函数  */
              "vTaskFreq",  		/* 任务名    */
              128,         		/* 任务栈大小，单位word，也就是4字节 */
              NULL,        		/* 任务参数  */
              10,           		/* 任务优先级*/
              &xHandleTaskFreq); /* 任务句柄  */
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
  
  xSemaphore_encoder = xSemaphoreCreateBinary();
  
  if(xSemaphore_encoder == NULL)
  {
    printf("xSemaphore_encoder fault\r\n");
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
                             1000,    /* 定时器周期,单位时钟节拍 */
                             pdTRUE,          /* 周期性 */
                             (void *)0,      /* 定时器ID */
                             UserTimerCallback); /* 定时器回调函数 */
    
  if(xTimerUser != NULL)
  {
    if(xTimerStart(xTimerUser,1000) != pdPASS)
    {
      printf("xSemaphore_rs485 fault\r\n");
      /* 定时器还没有进入激活状态 */
    }
  }
}

void TIM_CallBack1(void)
{
  Sdwe_clearString(PAGE_HISTORY_TEXT_FILE_WARN);//清除历史资料页面显示提醒
  Sdwe_clearString(PAGE_U_TEXT_READ_STATE);//清除U盘页面读取状态提醒
  Sdwe_clearString(PAGE_PRODUCT_RFID_WARNNING);//清除产能页面RFID提醒
  Sdwe_clearString(PAGE_STOP_WARNNING);//清除停机页面提醒
  Sdwe_clearString(PAGE_CARD_WARNNING);//清除读卡提醒
  Sdwe_clearString(PAGE_DEVICE_WARNNING);//清除设备ID修改提醒
  Sdwe_clearString(PAGE_WEIMI_WARNNING);//清除纬密页面提醒
  Sdwe_clearString(MAIN_PAGE_WARNNING);//清除主页提醒
}

void UserTimerCallback(TimerHandle_t xTimer)
{//定时时间1s
  static u16 speed_1 = 0,speed_2 = 0;
  static u8 timefor10s = 0;
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
  timefor10s++;
  if(timefor10s >= 10)
  {//每10秒获取一次时间
    timefor10s = 0;
    Sdwe_readRTC();
  }
  if(sample_time == 0)
  {
    speed_1 = pluse_count;
    sample_time++;
  }
  else if(sample_time >= 2)
  {//计算2s内的脉冲数
    sample_time = 0;
    speed_2 = pluse_count;
    pluse_count = 0;
    product_para.speed = (speed_2 - speed_1) * 30;//2秒内的脉冲数*30转换为1分钟脉冲数
    Sdwe_disDigi(PAGE_PRODUCT_SPEED,product_para.speed,2);//显示速度
  }
  else
  {
    sample_time++;
  }
//  Sdwe_disDigi(PAGE_PRODUCT_SPEED,speed_zhu,2);//显示速度
  if(work_idle_time < 1000)
    work_idle_time++;
  if(work_idle_time < 10)
  {//10s内无脉冲认为是工作时间
    work_idle = PLUSE_WORK;
    device_info.system_state = SYS_NORMAL;
    if(old_system_state != device_info.system_state)
    {
      old_system_state = device_info.system_state;
      Sdwe_disString(PAGE1_SYSTEM_STATE,(u8 *)system_state_dis[device_info.system_state],strlen(system_state_dis[device_info.system_state]));
    }
  }
  else if(work_idle_time < 300)
  {//10~300s无脉冲认为是空闲时间，不计入工作时间
    work_idle = PLUSE_IDLE;
  }
  else
  {//大于5分钟无脉冲认为停机
    work_idle = PLUSE_STOP;
    device_info.system_state = SYS_STOP;
    if(old_system_state != device_info.system_state)
    {
      old_system_state = device_info.system_state;
      Sdwe_disString(PAGE1_SYSTEM_STATE,(u8 *)system_state_dis[device_info.system_state],strlen(system_state_dis[device_info.system_state]));
    }
  }
  if(work_idle != PLUSE_IDLE)
  {
    if(device_info.system_state == SYS_NORMAL)
    {
      product_para.total_work_time++;
      if((product_para.total_work_time % 60) == 0)
      {//每分钟刷新一次开机时间显示
        u16 hour,min;
        hour = product_para.total_work_time / 3600;
        min = product_para.total_work_time % 3600 / 60;
        Sdwe_disDigi(PAGE_PRODUCT_TIME_ON_HOUR,hour,2);
        Sdwe_disDigi(PAGE_PRODUCT_TIME_ON_MIN,min,2);
      }
    }
    else
    {
      product_para.total_stop_time++;//停机总时间
      if((product_para.total_stop_time % 60) == 0)
      {//每分钟刷新一次停机时间显示
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
    }
  }
  printf("speed_zhu is %d\r\n",speed_zhu);
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
