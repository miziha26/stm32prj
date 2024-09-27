#ifndef BEEP_H
#define BEEP_H
#include "stm32f10x.h"


void Beep_Init(void);
void Beep_Toggle(void);

#define BEEP_ON  GPIO_SetBits(GPIOA,GPIO_Pin_8)
#define BEEP_OFF GPIO_ResetBits(GPIOA,GPIO_Pin_8)

#endif

