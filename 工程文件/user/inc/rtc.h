#ifndef _RTC_H
#define _RTC_H
#include "main.h"  

typedef struct rtc
{
	u8 year;
	u8 mon;
	u8 day;
	u8 week;
	u8 hour;
	u8 min;
	u8 sec;
	
}RTC_t;



void rtc_init(void);
void rtc_set_time(RTC_t time);
void rtc_get_sys(void);
void rtc_weekday_get(void);

void rtc_alarmA_init9(RTC_t alarm_time);
void rtc_WakeUp_init(void);
void rtc_set_custom_time(RTC_t time);
#endif




