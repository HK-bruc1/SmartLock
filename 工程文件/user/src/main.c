#include "main.h"




//全局变量；实例化一个串口实例用于串口发送和接收数据
USART_t usart1;

//定时器9的定时中断事件器
u16 tim9_count [10];

//是否开启字库更新标志
u8 zk_flag = 0;

//BS8116按键值
u8 key_val;

//界面编号(看默认进哪一个界面：主界面)
u16 page_mode=1;

//RTC用于参数传递
RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;




int main (void){
	//中断优先级分组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	//led初始化
	led_init();
	//key的初始化
	key_init();
	//usart1初始化
	usart1Init(115200);
	//w25q64初始化
	w25q64_init();
	//at24c0x初始化
	at24c0x_init();
	//定时器4的PWM输出初始化，PB9
	tim4_ch4_led3_init();
	//LCD屏幕初始化
	LCD_init();
	//语音模块初始化
	voice_init();
	//BS8116初始化
	BS8116_init();
	//看门狗初始化
	iwdg_init();
	//门电机的初始化
	door_init();
	//RTC初始化
	rtc_init();

	//定时器9的定时中断初始化,1ms进入一次中断
	tim9_it_ms(1);


	//是否满足字库更新条件，作为开机时特殊功能使用,这里只是擦除和开启更新字库标志位
	//下面的字体是无法显示的，后续就在while（1）中了，此时只要触发串口1的接收中断，就可以写入w25q64,字体只能复位重新加载
	//加看门狗实现更新字库后自动复位加载,10s擦除时间
	zk_update();


	


	while(1){
		//赋值看门狗
		iwdg_feed();
		key_val = BS8116_Key_scan();
		
		//由界面编号决定按键值给谁使用，因为多个界面需要按键值操作
		switch(page_mode){
			case 1:main_page(key_val);break;

		}
		


	}
	return 1;
}

