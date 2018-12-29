#include "bsp.h"

u8 lcd_rev_buf[50];
u16 lcd_rev_len = 0;

void bsp_InitSdwe050c17(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* 打开GPIO时钟 */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* 设为输出口 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽模式 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 上下拉电阻不使能 */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO口最大速度 */
  GPIO_InitStructure.GPIO_Pin = LCD_POWER_PIN;
  GPIO_Init(LCD_POWER_PORT, &GPIO_InitStructure);
  
//  LCD_POWER_OFF();
  LCD_POWER_ON();
}

void Init_GUI(void)
{
  u8 i;
  u8 name[20],name_1[20];
  u8 name_len;
  memset(name,0,20);
  memset(name_1,0,20);
  memcpy(name_1,SlavePara.filename,SlavePara.filename_len);
  sprintf(name,"%d号机-%s",device_info.master_id,name_1);
  name_len = strlen(name);
  Sdwe_disString(PAGE1_TEXT_FILE_NUM,name,name_len);
  Sdwe_disString(PAGE1_SYSTEM_STATE,"系统正常...",strlen("系统正常..."));
  for(i=0;i<30;i++)
  {
    Sdwe_disDigi(PAGE1_SET_VALUE1 + i,SlavePara.value_set[i] / 10);
    vTaskDelay(5);
    Sdwe_writeIcon(PAGE1_SLAVE_ONOFF1 + i,SlavePara.onoff[i]);
    vTaskDelay(5);
  }
}

//调用图片
//addr：0x03
void Sdwe_disPicture(u8 picture)
{
  u8 sendbuf[20];
  sendbuf[0] = 0xA5;
  sendbuf[1] = 0x5A;
  sendbuf[2] = 0x04; 
  sendbuf[3] = VGUS_REGISTER_WRITE;
  sendbuf[4] = VGUS_PIC_ID;
  sendbuf[5] = 0x00;
  sendbuf[6] = picture;
  UART4ToPC(sendbuf,7);
}

//在指定位置显示数字  
void Sdwe_disDigi(u16 addr,u16 data)
{
  u8 sendbuf[20];
  sendbuf[0] = 0xA5;
  sendbuf[1] = 0x5A;
  sendbuf[2] = 0x05; 
  sendbuf[3] = VGUS_VARIABLE_WRITE;
  sendbuf[4] = (addr & 0xff00) >> 8;
  sendbuf[5] = addr & 0x00ff;
  sendbuf[6] = (data & 0xff00) >> 8;
  sendbuf[7] = data & 0x00ff;
  UART4ToPC(sendbuf,8);
}

//在指定位置显示字符
//addr：变量存储器地址
//*str:要显示的字符指针
//len:要显示的字符长度
void Sdwe_disString(u16 addr,u8 *str,u16 len)
{
  u8 i,sendbuf[50];
  sendbuf[0] = 0xA5;
  sendbuf[1] = 0x5A;
  sendbuf[2] = len + 5; 
  sendbuf[3] = VGUS_VARIABLE_WRITE;
  sendbuf[4] = (addr & 0xff00) >> 8;
  sendbuf[5] = addr & 0x00ff;
  for(i=0;i<len;i++)
    sendbuf[6 + i] = str[i];
  sendbuf[6 + len] = 0xFF;
  sendbuf[7 + len] = 0xFF;
  UART4ToPC(sendbuf,8 + len);
}

void Sdwe_clearString(u16 addr)
{
  u8 sendbuf[10];
  sendbuf[0] = 0xA5;
  sendbuf[1] = 0x5A;
  sendbuf[2] = 5; 
  sendbuf[3] = VGUS_VARIABLE_WRITE;
  sendbuf[4] = (addr & 0xff00) >> 8;
  sendbuf[5] = addr & 0x00ff;
  sendbuf[6] = 0xFF;
  sendbuf[7] = 0xFF;
  UART4ToPC(sendbuf,8);
}

void Sdwe_writeIcon(u16 addr,u16 state)
{
  u8 sendbuf[10];
  sendbuf[0] = 0xA5;
  sendbuf[1] = 0x5A;
  sendbuf[2] = 5; 
  sendbuf[3] = VGUS_VARIABLE_WRITE;
  sendbuf[4] = (addr & 0xff00) >> 8;
  sendbuf[5] = addr & 0x00ff;
  sendbuf[6] = (state & 0xff00) >> 8;
  sendbuf[7] = state & 0x00ff;
  UART4ToPC(sendbuf,8);
}

void Sdwe_ClearReadDisk(void)
{
  u8 i;
  for(i=0;i<10;i++)
    Sdwe_writeIcon(PAGE_U_ICON_SELECT1,0);
}

void Sdwe_readIcon(u16 addr)
{
  u8 sendbuf[10];
  sendbuf[0] = 0xA5;
  sendbuf[1] = 0x5A;
  sendbuf[2] = 4; 
  sendbuf[3] = VGUS_VARIABLE_WRITE;
  sendbuf[4] = (addr & 0xff00) >> 8;
  sendbuf[5] = addr & 0x00ff;
  sendbuf[6] = 1;
  UART4ToPC(sendbuf,7);
}

void Sdwe_readRTC(void)
{
  u8 sendbuf[10];
  sendbuf[0] = 0xA5;
  sendbuf[1] = 0x5A;
  sendbuf[2] = 3; 
  sendbuf[3] = VGUS_REGISTER_READ;
  sendbuf[4] = 0x20;
  sendbuf[5] = 7;
  UART4ToPC(sendbuf,6);
}

void Sdwe_clear_filename(u8 file_count)
{
  Sdwe_clearString(PAGE_HISTORY_TEXT_FILENAME1 + file_count * 20);
  Sdwe_clearString(PAGE_HISTORY_TEXT_FILENAME1 + file_count * 20 + 10);
}

void Sdwe_refresh_filename(FILE_INFO file,u8 file_count)
{
  u8 i;
  u8 time_buf[30];
  u8 len;
  u8 name[20],name_1[20];
  u8 name_len;
  memset(time_buf,0,30);
  len = sizeof(FILE_INFO);
  __set_PRIMASK(1);
  W25QXX_Read((u8 *)&file,(u32)(W25QXX_ADDR_FILE + FILE_SIZE * file_count),len);
  __set_PRIMASK(0);
  if(file.filename_len <= 10)
  {
    memset(name,0,20);
    memset(name_1,0,20);
    memcpy(name_1,file.filename,file.filename_len);
    sprintf(name,"%d号机-%s",device_info.master_id,name_1);
    name_len = strlen(name);
    Sdwe_disString(PAGE_HISTORY_TEXT_FILENAME1 + file_count * 20,name,name_len);//显示文件名
    sprintf(time_buf,"20%02d/%02d/%02d %02d:%02d:%02d",file.year,file.month,file.day,
            file.hour,file.minute,file.second);
    len = strlen(time_buf);
    Sdwe_disString(PAGE_HISTORY_TEXT_FILENAME1 + 10 + file_count * 20,time_buf,strlen(time_buf));//显示日期时间
  }
}
void Sdwe_refresh_allname(u8 file_count)
{
  u8 i;
  FILE_INFO file;
  for(i=0;i<10;i++)
    Sdwe_clear_filename(i);
  for(i=0;i<file_count;i++)
    Sdwe_refresh_filename(file,i);
}
//void Sdwe_protocol(u8 *buf,u16 len)
//{
//  u16 var_addr,value;
//  u16 command;
//
//  if((buf[0] == 0xA5) && (buf[1] == 0x5A) && (buf[2] == (len - 3)))
//  {//指令格式正确
//    if(buf[3] == 0x81)
//    {//读寄存器返回
//      
//    }
//    else if(buf[3] == 0x83)
//    {//读变量存储器返回
//      var_addr = (buf[4] << 8) + buf[5];
//      if((var_addr >= 0x0300) && (var_addr < 0x0400))
//      {//数据录入地址
//        value = ((buf[7] << 8) + buf[8]) * 1000 / 100;//串口数据为两位数小数，单位kg，转换为g
//        if((var_addr >= 0x030a) && (var_addr <= 0x0313))
//        {//1——10号从机，设定张力
//          SlavePara.value_set[var_addr - 0x030a] = value;
//          printf("value_set %d# %.2f\r\n",var_addr - 0x030a + 1,(float)value / 1000);
//        }
//        else if((var_addr >= 0x031e) && (var_addr <= 0x0327))
//        {//11——20号从机，设定张力
//          SlavePara.value_set[var_addr - 0x031e + 10] = value;
//          printf("value_set %d# %.2f\r\n",var_addr - 0x031e + 10 + 1,(float)value / 1000);
//        }
//        else if((var_addr >= 0x0332) && (var_addr <= 0x033b))
//        {//21——30号从机，设定张力
//          SlavePara.value_set[var_addr - 0x0332 + 20] = value;
//          printf("value_set %d# %.2f\r\n",var_addr - 0x0332 + 20 + 1,(float)value / 1000);
//        }
//        message_mode = WORK_STAGE_SET_COMPARE;
//      }
//      else if(var_addr == 0x0200)
//      {
//        u8 llen;
//        llen = buf[6] * 2;//串口发送为字长
//        memcpy(input_password_buf,buf + 7,len);
//        input_password_len = get_valid_length(input_password_buf,llen);
//      }
//      else if(var_addr == 0x0100)
//      {
//        u8 fault;
//        value = (buf[7] << 8) + buf[8];
//        if(value == 1)
//        {//登录按钮
//          if(input_password_len != 6)
//          {//密码错误
//            Sdwe_disString(VGUS_ADDR_LOGIN_ERROR,"ERROR!",strlen("ERROR!"));
//          }
//          else
//          {
//            fault = array_compare(input_password_buf,"111111",input_password_len);
//            if(fault == 0)
//            {//密码正确
//              Sdwe_disPicture(VGUS_PICTURE_INTERFACE);
//            }
//            else
//            {//密码错误
//              Sdwe_disString(VGUS_ADDR_LOGIN_ERROR,"ERROR!",strlen("ERROR!"));
//            }
//          }
//        }
//      }
//      else if(var_addr == 0x0101)
//      {
//        value = (buf[7] << 8) + buf[8];
//      }
//      else if(var_addr == 0x0102)
//      {//复位键
//        
//      }
//      else if(var_addr == 0x0103)
//      {//存储键
//        
//      }
//      else if((var_addr >= 0x0400) && (var_addr < 0x0500))
//      {//从机有效命令
//        value = (buf[7] << 8) + buf[8];
//        if(var_addr <= 0x41d)
//        {//从机开关设置
//          SlavePara.onoff[var_addr - 0x0400] = value;
//          printf("%d# %d\r\n",var_addr - 0x0400 + 1,value);
//        }
//        else if(var_addr == 0x430)
//        {//启动/停止按钮
//          if(value == 1)
//          {//启动
//            message_mode = WORK_STAGE_START;
////            Sdwe_disString(VGUS_ADDR_WORKING_STATE,"RUNNING...",strlen("RUNNING..."));
//          }
//          else 
//          {//停止
//            
//          }
//        }
//        else if(var_addr == 0x431)
//        {//全开/全关按钮
//          if(value == 1)
//          {//全开
//            
//          }
//          else
//          {//全关
//            
//          }
//        }
//      }
//    }
//  }
//}

u8 get_valid_length(u8 *buf,u8 len)
{
  u8 i,length;
  for(i=0;i<len;i++)
  {
    if(buf[i] == 0xff)
    {
      length = i;
      break;
    }
      
  }
  return length;
}

u8 array_compare(u8 *buf1,u8 *buf2,u8 len)
{
  u8 i,flag = 0;
  for(i=0;i<len;i++)
  {
    if(buf1[i] != buf2[i])
    {
      flag = 1;
      break;
    }
  }
  return flag;
}

u8 hex_to_decimal(u8 da)
{
  u8 tmp;
  tmp = (da / 16) * 10 + (da % 16);
  return tmp;
}
