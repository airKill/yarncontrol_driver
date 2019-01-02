#ifndef __MOTOR_H
#define __MOTOR_H

#define MOTOR_IN1_GPIO_PIN                 GPIO_Pin_5
#define MOTOR_IN1_GPIO_PORT                GPIOA

#define MOTOR_EN_GPIO_PIN                 GPIO_Pin_6
#define MOTOR_EN_GPIO_PORT                GPIOA

#define MOTOR_IN2_GPIO_PIN                 GPIO_Pin_4
#define MOTOR_IN2_GPIO_PORT                GPIOA

#define MOTOR_EN_H()     GPIO_SetBits(MOTOR_EN_GPIO_PORT,MOTOR_EN_GPIO_PIN)
#define MOTOR_EN_L()     GPIO_ResetBits(MOTOR_EN_GPIO_PORT,MOTOR_EN_GPIO_PIN)

#define MOTOR_IN1_H()     GPIO_SetBits(MOTOR_IN1_GPIO_PORT,MOTOR_IN1_GPIO_PIN)
#define MOTOR_IN1_L()     GPIO_ResetBits(MOTOR_IN1_GPIO_PORT,MOTOR_IN1_GPIO_PIN)

#define MOTOR_IN2_H()     GPIO_SetBits(MOTOR_IN2_GPIO_PORT,MOTOR_IN2_GPIO_PIN)
#define MOTOR_IN2_L()     GPIO_ResetBits(MOTOR_IN2_GPIO_PORT,MOTOR_IN2_GPIO_PIN)

#define MOTOR_STOP      0
#define MOTOR_FORWARD   1
#define MOTOR_REVERSE   2
#define MOTOR_BREAK     3

extern u8 motor_dir;
extern u8 old_motor_dir;

void motor_init(void);
void motor_control(u8 cmd);
void TIM3_PWM_Init(u16 arr,u16 psc);
void motor_speed(u16 speed);
#endif