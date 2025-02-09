#include "main.h"                  // Device header



RTC_t sys_time ={0};

/***************************************
*函数名			：rtc_init
*函数功能		：RTC初始化配置函数
*函数参数		：RTC_t time
*函数返回值	：无
*函数描述		：
****************************************/
void rtc_init(void)
{
	//PWR寄存器使能(APB1)
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	//PWR_CR的DBP位解除对备份域写访问
	PWR_BackupAccessCmd(ENABLE);
	
	//使能LSI时钟
	RCC_LSICmd(ENABLE);
	//等待时钟就绪
	while(!(RCC_GetFlagStatus(RCC_FLAG_LSIRDY)));
	//选择LSI时钟
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	//使能RTC
	RCC_RTCCLKCmd(ENABLE);
	
//	/*RTC时钟源设置*/
//	//使能外部震荡器
//	RCC->BDCR |= (1<<0);
//	//等待外部振荡器就绪
//	while(!(RCC->BDCR & (1<<1)));
//	//选择外部低速时钟源
//	RCC->BDCR |= (1<<8);
//	//使能RTC时钟
//	RCC->BDCR |= (1<<15);
	
	
	
	//解除写保护
	RTC_WriteProtectionCmd(DISABLE);
	
	//进入初始化模式
	RTC_EnterInitMode();
	//等待进入完成
	while(!(RTC_GetFlagStatus(RTC_FLAG_INITF)));
	
	//设置24小时制
	RTC_InitTypeDef RTC_InitStruct = {0};
	RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;
	RTC_InitStruct.RTC_AsynchPrediv = 0x7f;
	RTC_InitStruct.RTC_SynchPrediv = 0xf9;
	RTC_Init(&RTC_InitStruct);
	
	//设置时间和日期
	rtc_get_sys();
	rtc_weekday_get();
	
	if(RTC->BKP0R != 0xff)
	{
		rtc_set_time(sys_time);
		
		RTC->BKP0R = 0xff;
	}
	
	
	
	//退出初始化模式
	RTC_ExitInitMode();
	//打开写保护
	RTC_WriteProtectionCmd(ENABLE);
}



/***************************************
*函数名			：rtc_set_time
*函数功能		：RTC设置时间和日期函数
*函数参数		：RTC_t
*函数返回值	：无
*函数描述		：
****************************************/
void rtc_set_time(RTC_t time)
{

	//配置时间
	RTC_TimeTypeDef RTC_TimeStruct= {0};
	RTC_TimeStruct.RTC_Hours = time.hour;
	RTC_TimeStruct.RTC_Minutes = time.min;
	RTC_TimeStruct.RTC_Seconds = time.sec;
	RTC_SetTime(RTC_Format_BIN,&RTC_TimeStruct);

	
	//配置日期
	RTC_DateTypeDef RTC_DateStruct = {0};
	RTC_DateStruct.RTC_WeekDay = time.week;
	RTC_DateStruct.RTC_Year = time.year;
	RTC_DateStruct.RTC_Month = time.mon;
	RTC_DateStruct.RTC_Date = time.day;
	RTC_SetDate(RTC_Format_BIN,&RTC_DateStruct);
}


/***************************************
*函数名     ：rtc_set_custom_time
*函数功能    ：手动设置RTC时间
*函数参数    ：RTC_t time
*函数返回值  ：无
*函数描述    ：在系统运行时修改RTC时间
****************************************/
void rtc_set_custom_time(RTC_t time)
{
    //解除写保护
    RTC_WriteProtectionCmd(DISABLE);
    
    //进入初始化模式
    RTC_EnterInitMode();
    //等待进入完成
    while(!(RTC_GetFlagStatus(RTC_FLAG_INITF)));
    
    //设置时间和日期
    rtc_set_time(time);
    
    //退出初始化模式
    RTC_ExitInitMode();
    //打开写保护
    RTC_WriteProtectionCmd(ENABLE);
}



/***************************************
*函数名			：get_sys
*函数功能		：字符串获取系统时间日期函数
*函数参数		：
*函数返回值	：无
*函数描述		：获取编译时间
****************************************/
u8 time[]=__TIME__;
u8 data[]=__DATE__;
u8 month[12][4]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
void rtc_get_sys(void)
{
	u8 i,mon[4];
	sys_time.hour = (time[0] - '0') * 10 + (time[1] - '0');//提取小时
	sys_time.min = (time[3] - '0') * 10 + (time[4] - '0');//提取分钟
	sys_time.sec = (time[6] - '0') * 10 + (time[7] - '0');//提取秒
	
	if(data[4]<'0' || data[4]>'9')
	{
		data[4] = '0';
	}
	sys_time.year = (data[9] - '0') * 10 + (data[10] - '0');//提取年
	sys_time.day = (data[4] - '0') * 10 + (data[5] - '0');//提取日
	
	//提取月
	for(i=0;i<3;i++)
	{
		mon[i] = data[i];
	}
	mon[3] = '\0';
	for(i=0;i<12;i++)
	{
		if(strcmp((const char *)mon,(const char *)month[i]) == 0)
		{
			break;
		}
	}
			sys_time.mon = i+1;
}



/***************************星期获取**********************************/
void rtc_weekday_get(void)
{
	u8 mon[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
	u32 i;
	u32 days=0;
	for(i=2024;i<sys_time.year+2000;i++)
	{
		if((i%4==0&&i%100)||(i%400==0))
		{
			days+=366;
		}
		else
		{
			days+=365;
		}
	}
	
	if(((sys_time.year+2000)%4==0&&(sys_time.year+2000)%100)||((sys_time.year+2000)%400==0))
	{
		mon[2]+=1;
	}
	for(i=0;i<sys_time.mon;i++)
	{
		days+=mon[i];
	}
	
	days+=sys_time.day;
	
	sys_time.week=days%7;
	if(sys_time.week==0)
	{
		sys_time.week=7;
	}
}



















/***************************************
*函数名			：alarmA_init
*函数功能		：闹钟A配置函数
*函数参数		：
*函数返回值	：无
*函数描述		：
****************************************/
void rtc_alarmA_init9(RTC_t alarm_time)
{
	//禁止闹钟
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);
	
	//配置闹钟
	RTC_AlarmTypeDef RTC_AlarmStruct = {0};
	RTC_TimeTypeDef RTC_AlarmTime = {0};
	
	RTC_AlarmStruct.RTC_AlarmDateWeekDay = alarm_time.week;
	RTC_AlarmStruct.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay;
	RTC_AlarmStruct.RTC_AlarmMask = RTC_AlarmMask_Minutes;//掩码
	RTC_AlarmTime.RTC_Hours = alarm_time.hour;
	RTC_AlarmTime.RTC_Minutes = alarm_time.min;
	RTC_AlarmTime.RTC_Seconds = alarm_time.sec;
	
	RTC_AlarmStruct.RTC_AlarmTime = RTC_AlarmTime;
	RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A,&RTC_AlarmStruct);
	
	//打开闹钟中断
	RTC_ITConfig(RTC_IT_ALRA,ENABLE);
	//使能闹钟
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);
	//配置外部中断线17
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line17;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_InitStruct);
	
	//NVIC控制器配置
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	NVIC_InitStruct.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_Init(&NVIC_InitStruct);
}

/***************************************
*函数名			：rtc_WakeUp_init
*函数功能		：唤醒函数
*函数参数		：无
*函数返回值	：无
*函数描述		：
****************************************/
void rtc_WakeUp_init(void)
{
	//禁止唤醒使能
	RTC_WakeUpCmd(DISABLE);
	
	//配置时钟
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	
	//配置重载值
	RTC_SetWakeUpCounter(0);
	
	//打开中断
	RTC_ITConfig(RTC_IT_WUT,ENABLE);
	
	//清除标志位
	RTC_ClearITPendingBit(RTC_IT_WUT);
	//使能唤醒
	RTC_WakeUpCmd(ENABLE);

	//外部中断22
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line22;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_InitStruct);
	
	//NVIC控制器配置
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	NVIC_InitStruct.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	
	NVIC_Init(&NVIC_InitStruct);
}












