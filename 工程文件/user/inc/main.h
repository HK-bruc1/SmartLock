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
#include "stdlib.h"
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
#include "esp32.h"
#include "scheduledTasks.h"



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

//设置音量和亮度的结构体
typedef struct set
{
	u8 open_flag;
	u8 voice;
	u8 light;
	
}SET_VAL_t;

//接收ESP32信息的结构体
typedef struct
{
	u8 buff[1024];//接收需要的字符串
	u16 len; //收到的数据的长度
	u8 flag;//表示数据接收完成的标志
}USART2_RECSTR;


//所有外部变量声明
extern USART_t usart1;
extern volatile u32 tim9_count [10];
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
extern SET_VAL_t set_data;
extern u8 voice_cmd[];
extern USART2_RECSTR esp32rec;
extern u8 autoCloseTimerFlag;
extern u8 wifi_connect_flag;
extern u8 mqtt_connect_flag;
extern u8 wifi_working_flag;
extern u8 wifi_check_flag;



#endif
