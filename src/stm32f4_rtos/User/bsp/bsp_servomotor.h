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

extern u16 DMA1_MEM_LEN;

void TIM4_PWM_SETPMOTOR(void);
void bsp_InitServoMotor(void);
void bsp_io_ServoMotor(void);
void TIM4_PWM_Init(u16 arr,u16 psc);
void DMA1_Stream0_CH2_Cmd(void (*Fuc)(uint16_t *,int32_t,int32_t),uint16_t *DataBuf,int32_t BufSize,int32_t MemoryInc);
void TIM4_PWMDMA_Config(uint16_t *DataBuf,int32_t BufSize,int32_t MemoryInc);
void DMA1_Stream0_CH2Init(void);
void TIM4_CH1_PWM_Config(void);
u16 DMA_send_feedback(void);
void DMA_PWM_Enable(void);
#endif