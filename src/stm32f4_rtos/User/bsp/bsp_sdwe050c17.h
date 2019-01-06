#ifndef _BSP_SDWe050C17_H_
#define _BSP_SDWe050C17_H

#include "protocol.h"
#include "product_cal.h"

#define LCD_POWER_PORT  GPIOD
#define LCD_POWER_PIN	GPIO_Pin_0

#define LCD_POWER_ON()    GPIO_SetBits(LCD_POWER_PORT,LCD_POWER_PIN)
#define LCD_POWER_OFF()    GPIO_ResetBits(LCD_POWER_PORT,LCD_POWER_PIN)

#define VGUS_REGISTER_WRITE     0x80
#define VGUS_REGISTER_READ      0x81

#define VGUS_VARIABLE_WRITE     0x82
#define VGUS_VARIABLE_READ      0x83

#define VGUS_EXPAND_CMD         0x85

#define VGUS_GET_VERSION        0x00    //读取版本号
#define VGUS_LED_NOW            0x01    //LED亮度控制，0x00-0x40
#define VGUS_BZ_TIME            0x02    //蜂鸣器鸣叫控制
#define VGUS_PIC_ID             0x03    //地址03-04
#define VGUS_TP_FLAG            0x05    //触摸屏坐标更新标志
#define VGUS_TP_STATUS          0x06    //触摸按压标志
#define VGUS_RUN_TIME           0x0C    //上电后运行时间
#define VGUS_RTC_COM_ADJ        0x1F    //用户申请修改RTC数据，VGUS修改后清零，年：月：日：星期：时：分：秒

#define PAGE_MAIN           1
#define PAGE_1              2
#define PAGE_1_SECRET       3
#define PAGE_1_SET_VALUE    4
#define PAGE_1_FILENAME     5
#define PAGE_2              6
#define PAGE_2_SECRET       7
#define PAGE_2_SET_VALUE    8
#define PAGE_2_FILENAME     9
#define PAGE_3              10
#define PAGE_3_SECRET       11
#define PAGE_3_SET_VALUE    12
#define PAGE_3_FILENAME     13
#define PAGE_HISTORY        14
#define PAGE_U              19

#define MAIN_PAGE_KEY_JINGSHA   0x0100
#define MAIN_PAGE_KEY_WEIMI     0x0101
#define MAIN_PAGE_KEY_CHANNENG  0x0102
#define MAIN_PAGE_KEY_SYS_CONFIG     0x0103

#define PAGE_FILE_KEY           0x0104
#define PAGE_START_STOP           0x0130

#define PAGE1_KEY_RESET         0x0131
#define PAGE1_ICON1_10_ONOFF          0x0450
#define PAGE2_ICON11_20_ONOFF         0x0451
#define PAGE3_ICON21_30_ONOFF         0x0452

#define PAGE1_ECHO_WEIGHT       0x0700

#define PAGE1_KEY_SET_WEIGHT    0x0105
#define PAGE1_KEY_LEFT          0x0106
#define PAGE1_KEY_SAVE          0x0107
#define PAGE1_KEY_RIGHT         0x0108
#define PAGE1_TEXT_FILE_NUM     0x0200
#define PAGE1_SYSTEM_STATE      0x020A
#define PAGE1_SAMPLE_VALUE1     0x0300
#define PAGE1_SET_VALUE1        0x0320
#define PAGE1_SLAVE_STATE1      0x0400
#define PAGE1_SLAVE_ONOFF1      0x0420
#define PAGE1_SECRET_KEY_CANCEL 0x0109
#define PAGE1_SECRET_KEY_ENTER  0x010A
#define PAGE1_SECRET_TEXT_DIS   0x0214
#define PAGE1_SECRET_TEXT_IMPORT   0x0219
#define PAGE1_SECRET_TEXT_WARN   0x021E
#define PAGE1_FILE_KEY_CANCEL     0x010B
#define PAGE1_FILE_KEY_ENTER     0x010C
#define PAGE1_FILE_TEXT_DIS       0x0223
#define PAGE1_FILE_TEXT_IMPORT       0x0223
#define PAGE1_FILE_TEXT_WARN       0x0228

#define PAGE2_KEY_SET_WEIGHT    0x010D
#define PAGE2_KEY_LEFT          0x010E
#define PAGE2_KEY_SAVE          0x010F
#define PAGE2_KEY_RIGHT         0x0110
#define PAGE2_SECRET_KEY_CANCEL 0x0111
#define PAGE2_SECRET_KEY_ENTER  0x0112
#define PAGE2_SECRET_TEXT_DIS   0x022D
#define PAGE2_SECRET_TEXT_IMPORT   0x0232
#define PAGE2_SECRET_TEXT_WARN    0x0237
#define PAGE2_FILE_KEY_CANCEL     0x0113
#define PAGE2_FILE_KEY_ENTER      0x0114
#define PAGE2_FILE_TEXT_DIS       0x023C
#define PAGE2_FILE_TEXT_IMPORT    0x023C
#define PAGE2_FILE_TEXT_WARN      0x0246

#define PAGE3_KEY_SET_WEIGHT    0x0115
#define PAGE3_KEY_LEFT          0x0116
#define PAGE3_KEY_SAVE          0x0117
#define PAGE3_KEY_RIGHT         0x0118
#define PAGE3_SECRET_KEY_CANCEL 0x0119
#define PAGE3_SECRET_KEY_ENTER  0x011A
#define PAGE3_SECRET_TEXT_DIS   0x024B
#define PAGE3_SECRET_TEXT_IMPORT   0x0250
#define PAGE3_SECRET_TEXT_WARN    0x0255
#define PAGE3_FILE_KEY_CANCEL     0x011B
#define PAGE3_FILE_KEY_ENTER      0x011C
#define PAGE3_FILE_TEXT_DIS       0x025A
#define PAGE3_FILE_TEXT_IMPORT    0x025A
#define PAGE3_FILE_TEXT_WARN      0x025F

#define PAGE_HISTORY_KEY_READ     0x011D
#define PAGE_HISTORY_KEY_SELECT   0x011E
#define PAGE_HISTORY_KEY_DOWNLOAD   0x011F
#define PAGE_HISTORY_TEXT_FILENAME1 0x0500
#define PAGE_HISTORY_TEXT_FILETIME  0x050A
#define PAGE_HISTORY_TEXT_FILE_WARN  0x05C8
#define PAGE_HISTORY_ICON_FILE1     0x0440
#define PAGE_HISTORY_ICON_U_STATE   0x044A

#define PAGE_U_KEY_READ     0x0120
#define PAGE_U_TEXT_FILENAME1  0x0600
#define PAGE_U_TEXT_READ_STATE  0x06C8
#define PAGE_U_ICON_SELECT1     0x044B

#define PAGE_PRODUCT_A 0x0800
#define PAGE_PRODUCT_B 0x0805
#define PAGE_PRODUCT_UNCOMPLETE  0x080A
#define PAGE_PRODUCT_COMPLETE  0x080F
#define PAGE_PRODUCT_KILOCOUNT   0x0814
#define PAGE_PRODUCT_SPEED   0x0819
#define PAGE_PRODUCT_TIME_ON    0x081E
#define PAGE_PRODUCT_TIME_OFF    0x0823
#define PAGE_PRODUCT_UNCOMPLETE_W  0x0828
#define PAGE_PRODUCT_COMPLETE_W  0x082D
#define PAGE_PRODUCT_JINGSHA       0x0832
#define PAGE_PRODUCT_WEISHA       0x0837
#define PAGE_PRODUCT_RUBBER       0x083C
#define PAGE_PRODUCT_FINAL        0x0841
#define PAGE_PRODUCT_ZHIJI        0x0846
#define PAGE_PRODUCT_LOSS        0x084B
#define PAGE_PRODUCT_TOTAL_METER        0x0850
#define PAGE_PRODUCT_TOTAL_WEIGHT        0x0855
#define PAGE_PRODUCT_KAIDU        0x085A
#define PAGE_PRODUCT_WEIMI        0x085F
#define PAGE_PRODUCT_WEISHU_DIS        0x0864

#define PAGE_PRODUCT_RFID_WARNNING  0x0900

#define PAGE_PRODUCT_PEILIAO    0x0140
#define PAGE_PRODUCT_CONTINUE    0x0141

#define PAGE_STOP_ON    0x0460
#define PAGE_STOP_OFF    0x0470

#define VGUS_ON   1
#define VGUS_OFF   0

extern u8 lcd_rev_buf[50];
extern u16 lcd_rev_len;

void bsp_InitSdwe050c17(void);
void Init_JINGSHA_GUI(void);
void Init_CHANNENG_GUI(void);
void Sdwe_disPicture(u8 picture);
void Sdwe_disDigi(u16 addr,u32 data,u8 bytes);
void Sdwe_disString(u16 addr,u8 *str,u16 len);
void Sdwe_protocol(u8 *buf,u16 len);
u8 get_valid_length(u8 *buf,u8 len);
u8 array_compare(u8 *buf1,u8 *buf2,u8 len);
void Sdwe_writeIcon(u16 addr,u16 state);
void Sdwe_readIcon(u16 addr);
void Sdwe_readRTC(void);
void Sdwe_refresh_filename(FILE_INFO file,u8 file_count);
void Sdwe_refresh_allname(u8 file_count);
void Sdwe_clearString(u16 addr);
void Sdwe_clear_filename(u8 file_count);
u8 hex_to_decimal(u8 da);
void Sdwe_ClearReadDisk(void);
void Sdwe_product_page(PRODUCT_PARA *para);
void Sdwe_peiliao_page(PRODUCT_PARA *para);
#endif
