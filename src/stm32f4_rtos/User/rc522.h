#ifndef  __RC522_H__
#define  __RC522_H__

#include "stm32f4xx.h"

#define GPIO_PORT_RFID_POWER  GPIOD
#define GPIO_PIN_RFID_POWER	GPIO_Pin_3

#define RFID_POWER_ON()    GPIO_SetBits(GPIO_PORT_RFID_POWER,GPIO_PIN_RFID_POWER)
#define RFID_POWER_OFF()   GPIO_ResetBits(GPIO_PORT_RFID_POWER,GPIO_PIN_RFID_POWER)

#define REQUEST_TYPE_IDLE       0x26
#define REQUEST_TYPE_ALL        0x52

#define COLLISION_GRADE_1       0x93 
#define COLLISION_GRADE_2       0x95 
#define COLLISION_GRADE_3       0x97 

#define Mifare1_S50     0x0004
#define Mifare1_S70     0x0002
#define Mifare1_Light     0x0010

typedef enum 
{
  FUNC_IDLE = 0,       //A°à
  FUNC_CLASS_A,
  FUNC_CLASS_B,
  FUNC_REPAIR
}CARD_FUNCTION;

extern u8 rfid_rev_flag;
extern u8 rfid_rev_buf[20];
extern u8 rfid_rev_cnt;

void bsp_Init_RFID(void);
void rc522_cmd_request(u8 mode);
void rc522_cmd_anticoll(u8 cmd);
void rc522_cmd_select(u8 *uid);
u8 rc522_find(u8 *buf,u8 len);
u8 rc522_card_id(u8 *buf,u8 len,u8 *card);
#endif