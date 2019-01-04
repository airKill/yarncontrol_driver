#ifndef  __RC522_H__
#define  __RC522_H__

#include "stm32f4xx.h"

#define REQUEST_TYPE_IDLE       0x26
#define REQUEST_TYPE_ALL        0x52

#define COLLISION_GRADE_1       0x93 
#define COLLISION_GRADE_2       0x95 
#define COLLISION_GRADE_3       0x97 

#define Mifare1_S50     0x0004
#define Mifare1_S70     0x0002
#define Mifare1_Light     0x0010

extern u8 rfid_rev_flag;
extern u8 rfid_rev_buf[20];
extern u8 rfid_rev_cnt;

void rc522_cmd_request(u8 mode);
void rc522_cmd_anticoll(u8 cmd);
void rc522_cmd_select(u8 *uid);
u8 rc522_find(u8 *buf,u8 len);
<<<<<<< HEAD
void rc522_card_id(u8 *buf,u8 len,u8 *card);
=======
u8 rc522_card_id(u8 *buf,u8 len,u8 *card);
>>>>>>> e91465d28df06ad15ef63bed982c7be4489bd12e
#endif