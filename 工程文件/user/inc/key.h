#ifndef KEY_H
#define KEY_H
#include "main.h"

//�ж�key1-----PA0,key2-----PC13�İ���״̬,key2����ʱio��Ϊ�͵�ƽ��key1����ʱioΪ�ߵ�ƽ,ͳһ�궨��Ϊ����Ϊ1
#define KEY1  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)
#define KEY2  (!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13))



void key_init(void);
u8 key_scan(void);

#endif






