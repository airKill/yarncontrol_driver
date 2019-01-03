#ifndef __HC08_H
#define __HC08_H
  
#include "stm32f10x.h"

void hc08_protocol(void);
u8* hc08_check_cmd(char *str);

#endif
