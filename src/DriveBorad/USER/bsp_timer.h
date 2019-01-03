#ifndef __BSP_TIMER_H_
#define __BSP_TIMER_H_

void bsp_StartHardTimer(u8 _CC, u32 _uiTimeOut, void * _pCallBack);
void bsp_InitHardTimer(void);

extern void (*s_TIM_CallBack1)(void);
extern void (*s_TIM_CallBack2)(void);
extern void (*s_TIM_CallBack3)(void);
extern void (*s_TIM_CallBack4)(void);

void TIM_CallBack1(void);
void TIM_CallBack2(void);
void TIM_CallBack3(void);
void TIM_CallBack4(void);
#endif