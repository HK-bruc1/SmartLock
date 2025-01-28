#ifndef MAIN_H
#define MAIN_H
//��ֹ�ظ����������ظ����壨��ṹ�壩


//����ͷ�ļ�����

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



//�궨��,�ֿ������ʼ��ַ
#define ZK_ADDR 0x020000
//��һ�ο�����־λ��ֵ��������һ�α�־λҲ�У�ֻҪ���ڲ�ͬ��ַ���У�
#define OPEN_FLAG 0xff





//�������ڴ��ڴ������ݵĽṹ��
typedef struct usart
{
    u8 usart_buff[100];
    u32 len;
    u8 usart_flag;
}USART_t;


//�����ⲿ��������
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