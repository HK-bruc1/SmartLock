#ifndef KEY_H
#define KEY_H
#include "main.h"

//判断key1-----PA0,key2-----PC13的按键状态,key2按下时io口为低电平，key1按下时io为高电平,统一宏定义为按下为1
#define KEY1  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)
#define KEY2  (!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13))



void key_init(void);
u8 key_scan(void);

#endif






