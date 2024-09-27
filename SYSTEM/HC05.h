#ifndef HC05_H
#define HC05_H

#include <stdio.h>
#include <stdarg.h>
#include "stm32f10x.h"

extern int key1,key2,key3,key4,key5;
 
void USART2_Init(void);
void USART2_SendByte(uint8_t Byte);
void USART2_SendArray(uint8_t *Array, uint16_t Length);
void USART2_SendString(char *String);
void USART2_SendNumber(uint32_t Number, uint8_t Length);
void USART2_Printf(char *format, ...);
 
uint8_t USART2_GetRxFlag(void);
uint8_t USART2_GetRxData(void);
 
 
//和手机相关联	GUI
void get_slave_data(uint8_t data);
void lanya_receive(void);
uint8_t checksum(void);
void Int_to_Byte(int i,uint8_t *byte);
void Float_to_Byte(float f,uint8_t *byte);
void Short_to_Byte(short s,uint8_t *byte);
 
void lanya_transmit(void); 
 
#endif
