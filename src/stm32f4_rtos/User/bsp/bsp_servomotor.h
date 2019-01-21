#ifndef _BSP_SERVOMOTOR_H_
#define _BSP_SERVOMOTOR_H

#define RCC_SERVOMOTOR 	 (RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD)

#define GPIO_PORT_SERVOMOTOR1      GPIOB
#define GPIO_PIN_SERVOMOTOR1	 GPIO_Pin_14

#define GPIO_PORT_SERVOMOTOR2      GPIOD
#define GPIO_PIN_SERVOMOTOR2	 GPIO_Pin_12

#define GPIO_PORT_SERVOMOTOR3      GPIOD
#define GPIO_PIN_SERVOMOTOR3	 GPIO_Pin_15

#define GPIO_PORT_SERVOMOTOR4      GPIOD
#define GPIO_PIN_SERVOMOTOR4	 GPIO_Pin_13

#define GPIO_PORT_SERVOMOTOR_G      GPIOB
#define GPIO_PIN_SERVOMOTOR_G	 GPIO_Pin_15

#define GPIO_PORT_SERVOMOTOR_G0      GPIOD
#define GPIO_PIN_SERVOMOTOR_G0	 GPIO_Pin_14

#define DIFF_G_H()   GPIO_SetBits(GPIO_PORT_SERVOMOTOR_G,GPIO_PIN_SERVOMOTOR_G)
#define DIFF_G_L()   GPIO_ResetBits(GPIO_PORT_SERVOMOTOR_G,GPIO_PIN_SERVOMOTOR_G)

#define DIFF_G0_H()   GPIO_SetBits(GPIO_PORT_SERVOMOTOR_G0,GPIO_PIN_SERVOMOTOR_G0)
#define DIFF_G0_L()   GPIO_ResetBits(GPIO_PORT_SERVOMOTOR_G0,GPIO_PIN_SERVOMOTOR_G0)

#define FREQ_500KHZ         84      //PWM频率=84MHZ/2/84=500KHZ
#define FORWARD TIM_OCMode_PWM2         //伺服电机正向
#define BACKWARD TIM_OCMode_PWM1        //伺服电机反向

void ServoMotorRunning(u8 dir,u16 stepnum);
void TIM4_CH1_ConfigPwmOut(u32 freq,u16 num);
void TIM4_CH1_StartPwmOut(void);
void TIM4_CH1_StopPwmOut(void);
void TIM4_CH1_GPIO_Configuration( void );
void TIM4_PWM_Config(u32 period);
void TIM4_CH1_PWMDMA_Config(u16 period,u16 cnt);
void TIM4_CH2_ConfigPwmOut(u32 freq,u16 num);
void TIM4_CH2_StartPwmOut(void);
void TIM4_CH2_StopPwmOut(void);
void TIM4_CH2_GPIO_Configuration(void);
void TIM4_CH2_PWMDMA_Config(u16 period,u16 cnt);
void DIFF_G_init(void);
#endif