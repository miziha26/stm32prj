#ifndef USART_H
#define USART_H

#include "stm32f10x.h"


void Init_UART1(uint32_t USART_BaudRate);
void UART1_Send_Datas(uint8_t* sendbuf,uint16_t n);
void USART1_IRQHandler(void);





#endif 

