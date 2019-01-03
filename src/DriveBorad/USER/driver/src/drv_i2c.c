/**
  ****************** (C) COPYRIGHT 2016 Nova  *******************
  * @file     drv_i2c.c
	* @auther   Liurongxue
  * @version  V1.0
  * @date     2/21/2017
  * @brief    This file provides the functions of I2C. 
  ***************************************************************
  * @attention
  *
  * History<br>
  * version   date            auther         description<br>
  * v1.0.0    2017.2.21       Liurongxue     Initial Version<br>
  ***************************************************************
  */

#include "drv_i2c.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* I2C Bus1 */
#define _I2C_BUS1_SCL_RCC_AHB                 RCC_APB2Periph_GPIOE
#define _I2C_BUS1_SCL_GPIO                    GPIOE
#define _I2C_BUS1_SCL_GPIO_PIN                GPIO_Pin_15
#define _I2C_BUS1_SDA_RCC_AHB                 RCC_APB2Periph_GPIOE
#define _I2C_BUS1_SDA_GPIO                    GPIOE
#define _I2C_BUS1_SDA_GPIO_PIN                GPIO_Pin_0

/* Private constants ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u8 _gI2C_Bus_Licensed_Flag[EN_I2C_BUS_NUM] = {0};    /* I2C bus has been registered flag, 0x00:not registered  0x01:registered */

void  _Set_Sda_Direction(u8 dir);

/**
  * @brief  : Control SDA output status
  * @param  : *i2c :I2C BUS level:I2C SDA status
  * @return : None
  * @note   : None
  */
static void _BusSda(const I2C_Bus_t *i2c, u8 level)
{
  switch(i2c->index){
    case EN_I2C_BUS1 :
    {
      if(level)
      {
        GPIO_WriteBit(_I2C_BUS1_SDA_GPIO, _I2C_BUS1_SDA_GPIO_PIN, Bit_SET);
      }
      else
      {
        GPIO_WriteBit(_I2C_BUS1_SDA_GPIO, _I2C_BUS1_SDA_GPIO_PIN, Bit_RESET);
      }
      break;
    }
    default :
      break;
  }
}

/**
  * @brief  : Control SCL output status
  * @param  : *i2c :I2C BUS level:I2C SCL status
  * @return : None
  * @note   : None
  */
static void _BusScl(const I2C_Bus_t *i2c, u8 level)
{
  switch(i2c->index){
    case EN_I2C_BUS1 :
    {
      if(level)
      {
        GPIO_WriteBit(_I2C_BUS1_SCL_GPIO, _I2C_BUS1_SCL_GPIO_PIN, Bit_SET);
      }
      else
      {
        GPIO_WriteBit(_I2C_BUS1_SCL_GPIO, _I2C_BUS1_SCL_GPIO_PIN, Bit_RESET);
      }
      break;
    }
    default :
      break;
  }
}

/**
  * @brief  : Read SDA input status
  * @param  : *i2c :I2C bus level:I2C SDA status
  * @return : None
  * @note   : None
  */
static u8 _ReadSda(const I2C_Bus_t *i2c)
{
  u8 value;
  
  switch(i2c->index){
    case EN_I2C_BUS1 :
    {
//      _Set_Sda_Direction(0);
      value = _I2C_BUS1_SDA_GPIO->IDR & _I2C_BUS1_SDA_GPIO_PIN;
//      _Set_Sda_Direction(1);
      break;
    }
    default :
      break;
  }
  
  return value;
}

static void _TimeDelayUs(u32 time_is_over)
{ 
  while(time_is_over)
  {
    time_is_over--;
  }
}

/**
  * @brief  : I2C delay time unit:hz
  * @param  : *i2c :i2c bus
  * @return : None
  * @note   : None
  */
static void _I2C_Delay(const I2C_Bus_t *i2c)
{
  vu32 us = (1000 * 1000) / i2c->speed;
  
  if(us == 0)
  {
    us = 10;
  }
  _TimeDelayUs(us);
}

/**
  * @brief  : I2C start
  * @param  : *i2c :i2c bus
  * @return : None
  * @note   : None
  */
void I2C_Start(const I2C_Bus_t *i2c)
{
  _BusSda(i2c, 1);
  _BusScl(i2c, 1);
  _I2C_Delay(i2c);
  _BusSda(i2c, 0);
  _I2C_Delay(i2c);
  _I2C_Delay(i2c);
  _I2C_Delay(i2c); 
  _BusScl(i2c, 0);
} 

/**
  * @brief  : I2C stop
  * @param  : *i2c :i2c bus
  * @return : None
  * @note   : None
  */
void I2C_Stop(const I2C_Bus_t *i2c)
{
  _BusSda(i2c, 0);
  _BusScl(i2c, 0);
  _I2C_Delay(i2c);
  _BusScl(i2c, 1);
  _I2C_Delay(i2c);
  _BusSda(i2c, 1);
  _I2C_Delay(i2c);
}

/**
  * @brief  : Wait I2C ack
  * @param  : *i2c :i2c bus
  * @return : True: ack   False:no ack
  * @note   : None
  */
bool I2C_WaitAck(const I2C_Bus_t *i2c)
{
  u32 time_out_over = i2c->ack_time_out; 

  _I2C_Delay(i2c);
  _BusScl(i2c, 1);
  _I2C_Delay(i2c);
  _BusSda(i2c, 1);
  _I2C_Delay(i2c);
  while(_ReadSda(i2c))
  {   
    time_out_over--;
    if(!time_out_over)
    {
      I2C_Stop(i2c);
      return false;
    }
  }   
  _BusScl(i2c, 0);
  _I2C_Delay(i2c);
  
  return true;
}

/**
  * @brief  : send no ack to I2C
  * @param  : *i2c :i2c bus
  * @return : None
  * @note   : None
  */
void I2C_SendNoAck(const I2C_Bus_t *i2c)
{ 
  _BusSda(i2c, 1);
  _I2C_Delay(i2c);
  _BusScl(i2c, 1);
  _I2C_Delay(i2c);
  _BusScl(i2c, 0);
  _I2C_Delay(i2c);
  _BusSda(i2c, 0);
  _I2C_Delay(i2c);
}

/**
  * @brief  : send ack to I2C
  * @param  : *i2c :i2c bus
  * @return : None
  * @note   : None
  */
void I2C_SendAck(const I2C_Bus_t *i2c)
{ 
  _BusSda(i2c, 0);
  _I2C_Delay(i2c);
  _BusScl(i2c, 1);
  _I2C_Delay(i2c);
  _BusScl(i2c, 0);
  _I2C_Delay(i2c);
  _BusSda(i2c, 1);
  _I2C_Delay(i2c);
}

/**
  * @brief  : I2C write one byte
  * @param  : *i2c :i2c bus
  * @return : None
  * @note   : None
  */
void I2C_WriteByte(const I2C_Bus_t *i2c, u8 byte)
{ 
  u8  i; 

  for(i = 0; i < 8; i++) 
  { 
    if(0x80 & byte)
    {
      _BusSda(i2c, 1);
    }      
    else 
    {
      _BusSda(i2c, 0);
    } 
    _I2C_Delay(i2c);   
    byte <<= 1; 
    _I2C_Delay(i2c);
    _BusScl(i2c, 1);
    _I2C_Delay(i2c); 
    _BusScl(i2c, 0);
    _I2C_Delay(i2c); 
  } 
}

/**
  * @brief  : I2C read one byte
  * @param  : *i2c :i2c bus
  * @return : None
  * @note   : None
  */
u8 I2C_ReadByte (const I2C_Bus_t *i2c) 
{ 
  u8  i; 
  u8  read_value = 0; 

  _BusSda(i2c, 1);
  for(i = 0; i < 8; i++) 
  { 
    read_value <<= 1;
    _BusScl(i2c, 1);
    _I2C_Delay(i2c); 
    if(_ReadSda(i2c))
    {
      read_value |= 0x01;
    }           
    _BusScl(i2c, 0);
    _I2C_Delay(i2c);     
  }
  return read_value;
}

/**
  * @brief  : I2C GPIO init
  * @param  : *i2c :i2c bus
  * @return : None
  * @note   : None
  */
void I2C_GPIO_Init(const I2C_Bus_t *i2c)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  switch(i2c->index){
    case EN_I2C_BUS1 :
    {
      if(!_gI2C_Bus_Licensed_Flag[EN_I2C_BUS1])
      {
        _gI2C_Bus_Licensed_Flag[EN_I2C_BUS1] = 0x01;
        
        RCC_APB2PeriphClockCmd(_I2C_BUS1_SDA_RCC_AHB, ENABLE);
        RCC_APB2PeriphClockCmd(_I2C_BUS1_SCL_RCC_AHB, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = _I2C_BUS1_SDA_GPIO_PIN;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_WriteBit(_I2C_BUS1_SDA_GPIO, _I2C_BUS1_SDA_GPIO_PIN, Bit_SET);
        GPIO_Init(_I2C_BUS1_SDA_GPIO, &GPIO_InitStructure);
            
        GPIO_InitStructure.GPIO_Pin = _I2C_BUS1_SCL_GPIO_PIN;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_WriteBit(_I2C_BUS1_SCL_GPIO, _I2C_BUS1_SCL_GPIO_PIN, Bit_SET);
        GPIO_Init(_I2C_BUS1_SCL_GPIO, &GPIO_InitStructure);
      }
      break;
    }
    default :
      break;
  }
}

/**
  * @brief  : I2C GPIO deInit
  * @param  : *i2c :i2c bus
  * @return : None
  * @note   : None
  */
void I2CC_GPIO_DeInit(const I2C_Bus_t *i2c)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  switch(i2c->index){
    case EN_I2C_BUS1 :
    {
      _gI2C_Bus_Licensed_Flag[EN_I2C_BUS1] = 0x00;
      
      RCC_APB2PeriphClockCmd(_I2C_BUS1_SDA_RCC_AHB, ENABLE);
      RCC_APB2PeriphClockCmd(_I2C_BUS1_SCL_RCC_AHB, ENABLE);
      
      GPIO_InitStructure.GPIO_Pin = _I2C_BUS1_SDA_GPIO_PIN;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(_I2C_BUS1_SDA_GPIO, &GPIO_InitStructure);
      
      GPIO_InitStructure.GPIO_Pin = _I2C_BUS1_SCL_GPIO_PIN;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(_I2C_BUS1_SCL_GPIO, &GPIO_InitStructure);
      break;
    }
    default :
      break;
  }
}

void _Set_Sda_Direction(u8 dir)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  if(dir)
  {
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  }
  else
  {
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  }
  GPIO_InitStructure.GPIO_Pin = _I2C_BUS1_SDA_GPIO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(_I2C_BUS1_SDA_GPIO, &GPIO_InitStructure);
}

