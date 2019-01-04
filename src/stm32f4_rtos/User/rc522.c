#include "includes.h"

u8 rfid_rev_flag = 0;
u8 rfid_rev_buf[20];
u8 rfid_rev_cnt = 0;

void rc522_cmd_request(u8 mode)
{
  u8 sendbuf[20],i;
  u8 checksum = 0;
  sendbuf[0] = 0x07;
  sendbuf[1] = 0x02;
  sendbuf[2] = 'A';
  sendbuf[3] = 0x01;
  sendbuf[4] = mode;
  for(i=0; i<=4; i++) 
  {
    checksum ^= sendbuf[i];
  }
  sendbuf[5] = ~checksum;
  sendbuf[6] = 0x03;
  UART5ToPC(sendbuf,7);
}

void rc522_cmd_anticoll(u8 cmd)
{
  u8 sendbuf[20],i;
  u8 checksum = 0;
  sendbuf[0] = 0x08;
  sendbuf[1] = 0x02;
  sendbuf[2] = 'B';
  sendbuf[3] = 0x02;
  sendbuf[4] = cmd;
  sendbuf[5] = 0x00;
  for(i=0; i<=5; i++)
  {
    checksum ^= sendbuf[i];
  }
  sendbuf[6] = ~checksum;
  sendbuf[7] = 0x03;
  UART5ToPC(sendbuf,8);
}

void rc522_cmd_select(u8 *uid)
{
  u8 sendbuf[20],i;
  u8 checksum = 0;
  sendbuf[0] = 0x0B;
  sendbuf[1] = 0x02;
  sendbuf[2] = 'B';
  sendbuf[3] = 0x05;
  sendbuf[4] = uid[0];
  sendbuf[5] = uid[1];
  sendbuf[6] = uid[2];
  sendbuf[7] = uid[3];
  for(i=0; i<=7; i++) 
  {
    checksum ^= ~sendbuf[i];
  }
  sendbuf[8] = checksum;
  sendbuf[9] = 0x03;
  UART5ToPC(sendbuf,7);
}

u8 rc522_find(u8 *buf,u8 len)
{
  u8 check = 0,i;
  u8 type = 0;
  u16 atq = 0;
  if(len == 8)
  {
    for(i=0;i<=5;i++)
    {
      check ^= buf[i];
    }
    check = ~check;
    if((check == buf[6]) && (buf[7] == 0x03))
    {//校验及帧尾正确
      atq = (buf[5] << 8) + buf[4];
      if(atq == Mifare1_S50)
      {
        type = Mifare1_S50;
      }
      else if(atq == Mifare1_S70)
      {
        type = Mifare1_S70;
      }
      else if(atq == Mifare1_Light)
      {
        type = Mifare1_Light;
      }
    }
  }
  return type;
}

u8 rc522_card_id(u8 *buf,u8 len,u8 *card)
{
  u8 check = 0,i;
  u8 type = 0;
  u16 err = 0;
  for(i=0;i<=7;i++)
  {
    check ^= buf[i];
  }
  check = ~check;
  if((check == buf[8]) && (buf[9] == 0x03))
  {//校验及帧尾正确
    if(len == 10)
    {
      card[0] = buf[4];
      card[1] = buf[5];
      card[2] = buf[6];
      card[3] = buf[7];
      err = 0;
    }
    else
      err = 1;
  }
  else
    err = 1;
  return err;
}
