#include "main.h"                  // Device header



RTC_t sys_time ={0};

/***************************************
*������			��rtc_init
*��������		��RTC��ʼ�����ú���
*��������		��RTC_t time
*��������ֵ	����
*��������		��
****************************************/
void rtc_init(void)
{
	//PWR�Ĵ���ʹ��(APB1)
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	//PWR_CR��DBPλ����Ա�����д����
	PWR_BackupAccessCmd(ENABLE);
	
	//ʹ��LSIʱ��
	RCC_LSICmd(ENABLE);
	//�ȴ�ʱ�Ӿ���
	while(!(RCC_GetFlagStatus(RCC_FLAG_LSIRDY)));
	//ѡ��LSIʱ��
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	//ʹ��RTC
	RCC_RTCCLKCmd(ENABLE);
	
//	/*RTCʱ��Դ����*/
//	//ʹ���ⲿ����
//	RCC->BDCR |= (1<<0);
//	//�ȴ��ⲿ��������
//	while(!(RCC->BDCR & (1<<1)));
//	//ѡ���ⲿ����ʱ��Դ
//	RCC->BDCR |= (1<<8);
//	//ʹ��RTCʱ��
//	RCC->BDCR |= (1<<15);
	
	
	
	//���д����
	RTC_WriteProtectionCmd(DISABLE);
	
	//�����ʼ��ģʽ
	RTC_EnterInitMode();
	//�ȴ��������
	while(!(RTC_GetFlagStatus(RTC_FLAG_INITF)));
	
	//����24Сʱ��
	RTC_InitTypeDef RTC_InitStruct = {0};
	RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;
	RTC_InitStruct.RTC_AsynchPrediv = 0x7f;
	RTC_InitStruct.RTC_SynchPrediv = 0xf9;
	RTC_Init(&RTC_InitStruct);
	
	//����ʱ�������
	rtc_get_sys();
	rtc_weekday_get();
	
	if(RTC->BKP0R != 0xff)
	{
		rtc_set_time(sys_time);
		
		RTC->BKP0R = 0xff;
	}
	
	
	
	//�˳���ʼ��ģʽ
	RTC_ExitInitMode();
	//��д����
	RTC_WriteProtectionCmd(ENABLE);
}



/***************************************
*������			��rtc_set_time
*��������		��RTC����ʱ������ں���
*��������		��RTC_t
*��������ֵ	����
*��������		��
****************************************/
void rtc_set_time(RTC_t time)
{

	//����ʱ��
	RTC_TimeTypeDef RTC_TimeStruct= {0};
	RTC_TimeStruct.RTC_Hours = time.hour;
	RTC_TimeStruct.RTC_Minutes = time.min;
	RTC_TimeStruct.RTC_Seconds = time.sec;
	RTC_SetTime(RTC_Format_BIN,&RTC_TimeStruct);

	
	//��������
	RTC_DateTypeDef RTC_DateStruct = {0};
	RTC_DateStruct.RTC_WeekDay = time.week;
	RTC_DateStruct.RTC_Year = time.year;
	RTC_DateStruct.RTC_Month = time.mon;
	RTC_DateStruct.RTC_Date = time.day;
	RTC_SetDate(RTC_Format_BIN,&RTC_DateStruct);
}


/***************************************
*������     ��rtc_set_custom_time
*��������    ���ֶ�����RTCʱ��
*��������    ��RTC_t time
*��������ֵ  ����
*��������    ����ϵͳ����ʱ�޸�RTCʱ��
****************************************/
void rtc_set_custom_time(RTC_t time)
{
    //���д����
    RTC_WriteProtectionCmd(DISABLE);
    
    //�����ʼ��ģʽ
    RTC_EnterInitMode();
    //�ȴ��������
    while(!(RTC_GetFlagStatus(RTC_FLAG_INITF)));
    
    //����ʱ�������
    rtc_set_time(time);
    
    //�˳���ʼ��ģʽ
    RTC_ExitInitMode();
    //��д����
    RTC_WriteProtectionCmd(ENABLE);
}



/***************************************
*������			��get_sys
*��������		���ַ�����ȡϵͳʱ�����ں���
*��������		��
*��������ֵ	����
*��������		����ȡ����ʱ��
****************************************/
u8 time[]=__TIME__;
u8 data[]=__DATE__;
u8 month[12][4]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
void rtc_get_sys(void)
{
	u8 i,mon[4];
	sys_time.hour = (time[0] - '0') * 10 + (time[1] - '0');//��ȡСʱ
	sys_time.min = (time[3] - '0') * 10 + (time[4] - '0');//��ȡ����
	sys_time.sec = (time[6] - '0') * 10 + (time[7] - '0');//��ȡ��
	
	if(data[4]<'0' || data[4]>'9')
	{
		data[4] = '0';
	}
	sys_time.year = (data[9] - '0') * 10 + (data[10] - '0');//��ȡ��
	sys_time.day = (data[4] - '0') * 10 + (data[5] - '0');//��ȡ��
	
	//��ȡ��
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



/***************************���ڻ�ȡ**********************************/
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
*������			��alarmA_init
*��������		������A���ú���
*��������		��
*��������ֵ	����
*��������		��
****************************************/
void rtc_alarmA_init9(RTC_t alarm_time)
{
	//��ֹ����
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);
	
	//��������
	RTC_AlarmTypeDef RTC_AlarmStruct = {0};
	RTC_TimeTypeDef RTC_AlarmTime = {0};
	
	RTC_AlarmStruct.RTC_AlarmDateWeekDay = alarm_time.week;
	RTC_AlarmStruct.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay;
	RTC_AlarmStruct.RTC_AlarmMask = RTC_AlarmMask_Minutes;//����
	RTC_AlarmTime.RTC_Hours = alarm_time.hour;
	RTC_AlarmTime.RTC_Minutes = alarm_time.min;
	RTC_AlarmTime.RTC_Seconds = alarm_time.sec;
	
	RTC_AlarmStruct.RTC_AlarmTime = RTC_AlarmTime;
	RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A,&RTC_AlarmStruct);
	
	//�������ж�
	RTC_ITConfig(RTC_IT_ALRA,ENABLE);
	//ʹ������
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);
	//�����ⲿ�ж���17
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line17;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_InitStruct);
	
	//NVIC����������
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	NVIC_InitStruct.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_Init(&NVIC_InitStruct);
}

/***************************************
*������			��rtc_WakeUp_init
*��������		�����Ѻ���
*��������		����
*��������ֵ	����
*��������		��
****************************************/
void rtc_WakeUp_init(void)
{
	//��ֹ����ʹ��
	RTC_WakeUpCmd(DISABLE);
	
	//����ʱ��
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	
	//��������ֵ
	RTC_SetWakeUpCounter(0);
	
	//���ж�
	RTC_ITConfig(RTC_IT_WUT,ENABLE);
	
	//�����־λ
	RTC_ClearITPendingBit(RTC_IT_WUT);
	//ʹ�ܻ���
	RTC_WakeUpCmd(ENABLE);

	//�ⲿ�ж�22
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line22;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_InitStruct);
	
	//NVIC����������
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	NVIC_InitStruct.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	
	NVIC_Init(&NVIC_InitStruct);
}












