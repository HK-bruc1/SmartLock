#include "main.h"




//全局变量；实例化一个串口实例用于串口发送和接收数据
USART_t usart1;

//定时器9的定时中断事件器
u16 tim9_count [10];

//是否开启字库更新标志
u8 zk_flag = 0;




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
	//定时器4的PWM输出初始化，PB9
	tim4_ch4_led3_init();
	//LCD屏幕初始化
	LCD_init();
	//定时器9的定时中断初始化,1ms进入一次中断
	tim9_it_ms(1);


	//显示开机图片
	LCD_dis_pic(0,0,gImage_systemPic);

	//是否满足字库更新条件，作为开机时特殊功能使用
	zk_update();

	


	while(1){

		
	}
	return 1;
}