#ifndef _BSP_SERVOMOTOR_H_
#define _BSP_SERVOMOTOR_H

#define RCC_SERVOMOTOR 	 (RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD)

//#define GPIO_PORT_SERVOMOTOR1      GPIOB
//#define GPIO_PIN_SERVOMOTOR1	 GPIO_Pin_14

#define GPIO_PORT_SERVOMOTOR2      GPIOD
#define GPIO_PIN_SERVOMOTOR2	 GPIO_Pin_12

//#define GPIO_PORT_SERVOMOTOR3      GPIOD
//#define GPIO_PIN_SERVOMOTOR3	 GPIO_Pin_15

#define GPIO_PORT_SERVOMOTOR4      GPIOD
#define GPIO_PIN_SERVOMOTOR4	 GPIO_Pin_13

//#define GPIO_PORT_SERVOMOTOR_G      GPIOB
//#define GPIO_PIN_SERVOMOTOR_G	 GPIO_Pin_15
//
//#define GPIO_PORT_SERVOMOTOR_G0      GPIOD
//#define GPIO_PIN_SERVOMOTOR_G0	 GPIO_Pin_14
//
//#define DIFF_G_H()   GPIO_SetBits(GPIO_PORT_SERVOMOTOR_G,GPIO_PIN_SERVOMOTOR_G)
//#define DIFF_G_L()   GPIO_ResetBits(GPIO_PORT_SERVOMOTOR_G,GPIO_PIN_SERVOMOTOR_G)
//
//#define DIFF_G0_H()   GPIO_SetBits(GPIO_PORT_SERVOMOTOR_G0,GPIO_PIN_SERVOMOTOR_G0)
//#define DIFF_G0_L()   GPIO_ResetBits(GPIO_PORT_SERVOMOTOR_G0,GPIO_PIN_SERVOMOTOR_G0)

#define FREQ_500KHZ         84      //PWM频率=84MHZ/2/84=500KHZ
#define FREQ_200KHZ         210      //PWM频率=84MHZ/2/210=200KHZ
#define FREQ_7_5KHZ         5600      //PWM频率=84MHZ/2/5600=7.5KHZ
#define FREQ_37_5KHZ         1120      //PWM频率=84MHZ/2/1120=37.5KHZ
#define FREQ_75KHZ         560      //PWM频率=84MHZ/2/560=75KHZ

#define FORWARD_PWM TIM_OCMode_PWM2         //伺服电机正向
#define BACKWARD_PWM TIM_OCMode_PWM1        //伺服电机反向

#define SERVO_FORWARD()  GPIO_SetBits(GPIO_PORT_SERVOMOTOR4,GPIO_PIN_SERVOMOTOR4)
#define SERVO_BACKWARD() GPIO_ResetBits(GPIO_PORT_SERVOMOTOR4,GPIO_PIN_SERVOMOTOR4)

void ServoMotorRunning(u16 freq,u16 stepnum,u8 dir);
void TIM4_CH1_ConfigPwmOut(u32 freq,u16 num);
void TIM4_CH1_StartPwmOut(void);
void TIM4_CH1_StopPwmOut(void);
void TIM4_CH1_GPIO_Configuration( void );
void TIM4_PWM_Config(u32 period,u8 dir);
void TIM4_CH1_PWMDMA_Config(u16 period,u16 cnt);
void TIM4_CH2_ConfigPwmOut(u32 freq,u16 num);
void TIM4_CH2_StartPwmOut(void);
void TIM4_CH2_StopPwmOut(void);
void TIM4_CH2_GPIO_Configuration(void);
void TIM4_CH2_PWMDMA_Config(u16 period,u16 cnt);
//void DIFF_G_init(void);
void TIM4_MANUAL_PWM_Config(u32 period);
void TIM4_MANUAL_PWM_Stop(void);
void SEVRO_PWM_Continue(u32 per);
void ServoMotor_adjust_speed(u32 value);
void ServoMotor_start(u32 per);
void Sevro_PWM_Config(void);
u16 get_ServoMotor_freq(u16 value);
#endif
