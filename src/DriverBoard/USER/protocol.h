#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#define MAX_LINK_TIME   8000

typedef struct
{
  u8 onoff;
  u16 weight;
}MOTOR;

extern MOTOR motor;
extern float Weight_absolute;
extern u8 message0;
extern u8 rev_buf[10];
extern u8 rev_len;

extern u8 start_stop;
extern u16 heart_cnt;

void uart_protocol(void);
u8* uart_check_cmd(char *str);
void rs485_revProtocol(u8 *buf,u16 len);

void rs485_ack_onoff(u8 num,u8 onoff);
void rs485_ack_set_value(u8 num,u16 weight);
void rs485_ack_weight(u8 num,u16 weight);
void rs485_ack_start_stop(u8 num,u16 state);
void uart1_debug(void);
u8* uart1_check_cmd(char *str);
#endif