#ifndef USART_H
#define USART_H
#include "main.h"


void usart1Init(u32 baud);
void USART1_SendChar(uint8_t ch);
void USART1_SendString(char *str);

#endif