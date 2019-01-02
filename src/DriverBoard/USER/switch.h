#ifndef __SWITCH_H
#define __SWITCH_H

#define SW1_GPIO_PIN                 GPIO_Pin_9
#define SW1_GPIO_PORT                GPIOB

#define SW2_GPIO_PIN                 GPIO_Pin_8
#define SW2_GPIO_PORT                GPIOB

#define SW3_GPIO_PIN                 GPIO_Pin_5
#define SW3_GPIO_PORT                GPIOB

#define SW4_GPIO_PIN                 GPIO_Pin_4
#define SW4_GPIO_PORT                GPIOB

#define SW5_GPIO_PIN                 GPIO_Pin_3
#define SW5_GPIO_PORT                GPIOB

#define SW6_GPIO_PIN                 GPIO_Pin_15
#define SW6_GPIO_PORT                GPIOA

#define SW7_GPIO_PIN                 GPIO_Pin_12
#define SW7_GPIO_PORT                GPIOA

#define SW8_GPIO_PIN                 GPIO_Pin_11
#define SW8_GPIO_PORT                GPIOA

#define READ_SW1()    GPIO_ReadInputDataBit(SW1_GPIO_PORT,SW1_GPIO_PIN)
#define READ_SW2()    GPIO_ReadInputDataBit(SW2_GPIO_PORT,SW2_GPIO_PIN)
#define READ_SW3()    GPIO_ReadInputDataBit(SW3_GPIO_PORT,SW3_GPIO_PIN)
#define READ_SW4()    GPIO_ReadInputDataBit(SW4_GPIO_PORT,SW4_GPIO_PIN)
#define READ_SW5()    GPIO_ReadInputDataBit(SW5_GPIO_PORT,SW5_GPIO_PIN)
#define READ_SW6()    GPIO_ReadInputDataBit(SW6_GPIO_PORT,SW6_GPIO_PIN)
#define READ_SW7()    GPIO_ReadInputDataBit(SW7_GPIO_PORT,SW7_GPIO_PIN)
#define READ_SW8()    GPIO_ReadInputDataBit(SW8_GPIO_PORT,SW8_GPIO_PIN)

extern u8 device_addr;

void switch_init(void);
u8 read_switch(void);

#endif