#ifndef MAIN_H
#define MAIN_H
//防止重复包含导致重复定义（如结构体）


//所有头文件声明

#include "stm32f4xx.h"                  // Device header
#include "led.h"
#include "key.h"
#include "usart.h"
#include "nvic.h"
#include "string.h"
#include "stdio.h"
#include "selfprintf.h"
#include "tim.h"
#include "delay.h"
#include "door.h"
#include "at24cxx.h"
#include "w25q64.h"
#include "lcd.h"
#include "font.h"
#include "function.h"
#include "voice.h"
#include "iwdg.h"
#include "bs8116.h"
#include "rtc.h"
#include "mg200.h"
#include "rc522.h"



//宏定义,字库擦除起始地址
#define ZK_ADDR 0x020000
//第一次开机标志位的值（其他第一次标志位也行，只要存在不同地址就行）
#define OPEN_FLAG 0xff





//声明用于串口传输数据的结构体
typedef struct usart
{
    u8 usart_buff[100];
    u32 len;
    u8 usart_flag;
}USART_t;


//所有外部变量声明
extern USART_t usart1;
extern u16 tim9_count [10];
extern char at24c02_rec_str[];
extern u8 zk_flag;
extern u16 page_mode;
extern RTC_TimeTypeDef RTC_TimeStruct;
extern RTC_DateTypeDef RTC_DateStruct;
extern volatile u8 mg200_buff[8];
extern volatile u8 mg200_rec_flag;
extern u8 mg200_id[9];
extern u8 picc_passward[6];
extern u8 picc[9][4];
extern u8 picc_data[16];
extern u8 picc_id[4];
extern u8 picc_read_data[16];




#endif