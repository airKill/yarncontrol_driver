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

void Init_JINGSHA_GUI(void)
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

void Init_CHANNENG_GUI(void)
{
  
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

//产量页面
void Sdwe_peiliao_page(PRODUCT_PARA *para)
{
  float meter,unmeter;
  float weight,unweight;
  Sdwe_disDigi(PAGE_PRODUCT_A,(int)(para->product_a * 10));
  Sdwe_disDigi(PAGE_PRODUCT_B,(int)(para->product_b * 10));
  meter = product_complete_meter(para);
  unmeter = product_uncomplete_meter(para);
  Sdwe_disDigi(PAGE_PRODUCT_UNCOMPLETE,(int)(unmeter * 10));
  Sdwe_disDigi(PAGE_PRODUCT_COMPLETE,(int)(meter * 10));
  Sdwe_disDigi(PAGE_PRODUCT_KILOCOUNT,0);
  Sdwe_disDigi(PAGE_PRODUCT_SPEED,0);
  
  Sdwe_disDigi(PAGE_PRODUCT_TIME_ON,0);
  Sdwe_disDigi(PAGE_PRODUCT_TIME_OFF,0);
  weight = product_complete_kilo(para);
  unweight = product_uncomplete_kilo(para);
  Sdwe_disDigi(PAGE_PRODUCT_UNCOMPLETE_W,(int)(weight * 10));
  Sdwe_disDigi(PAGE_PRODUCT_COMPLETE_W,(int)(unweight * 10));
}

//胚料页面
void Sdwe_product_page(PRODUCT_PARA *para)
{
  Sdwe_disDigi(PAGE_PRODUCT_JINGSHA,(int)(para->latitude_weight * 10));
  Sdwe_disDigi(PAGE_PRODUCT_WEISHA,(int)(para->longitude_weight * 10));
  Sdwe_disDigi(PAGE_PRODUCT_RUBBER,(int)(para->rubber_weight * 10));
  Sdwe_disDigi(PAGE_PRODUCT_FINAL,(int)(para->final_weight * 10));
  Sdwe_disDigi(PAGE_PRODUCT_ZHIJI,(int)(para->loom_num));
  Sdwe_disDigi(PAGE_PRODUCT_LOSS,(int)(para->loss));
  Sdwe_disDigi(PAGE_PRODUCT_TOTAL_METER,(int)(para->total_meter_set));
  Sdwe_disDigi(PAGE_PRODUCT_TOTAL_WEIGHT,(int)(para->total_weitht_set));
  Sdwe_disDigi(PAGE_PRODUCT_KAIDU,(int)(para->kaidu_set));
  Sdwe_disDigi(PAGE_PRODUCT_WEIMI,(int)(para->weimi_set * 10));
  Sdwe_disDigi(PAGE_PRODUCT_WEISHU_DIS,(int)(para->weimi_dis_set));
}

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
