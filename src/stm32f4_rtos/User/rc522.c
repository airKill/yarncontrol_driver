#include "includes.h"

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
  sendbuf[5] = checksum;
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
  sendbuf[6] = checksum;
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
    checksum ^= sendbuf[i];
  }
  sendbuf[8] = checksum;
  sendbuf[9] = 0x03;
  UART5ToPC(sendbuf,7);
}
