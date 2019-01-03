#ifndef  __RC522_H__
#define  __RC522_H__

#include "stm32f4xx.h"

#define REQUEST_TYPE_IDLE       0x26
#define REQUEST_TYPE_ALL        0x52

#define COLLISION_GRADE_1       0x93 
#define COLLISION_GRADE_2       0x95 
#define COLLISION_GRADE_3       0x97 

void rc522_cmd_request(u8 mode);
void rc522_cmd_anticoll(u8 cmd);
void rc522_cmd_select(u8 *uid);
#endif