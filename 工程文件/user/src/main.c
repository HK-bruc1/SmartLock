#include "main.h"



//全局变量；实例化一个串口实例用于串口发送和接收数据
USART_t usart1;

//定时器9的定时中断事件器
u16 tim9_count [10];

//AT24C0X相关



int main (void){
	//按键值
	u8 key_value = 0;

	//中断优先级分组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	//led初始化
	led_init();
	//key的初始化
	key_init();
	//usart1初始化
	usart1Init(115200);

	//定时器4的PWM输出初始化，PB9
	//tim4_ch4_led3_init();
	//门的开关初始化
	//door_init();
	//定时器9的定时中断初始化,1ms进入一次中断
	tim9_it_ms(1);

	LCD_init();

	while(1){
		
		
		
		
	}

	
	return 1;
}