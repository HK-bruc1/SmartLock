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
	//语音模块初始化
	voice_init();
	//BS8116初始化
	BS8116_init();
	//看门狗初始化
	iwdg_init();
	//定时器9的定时中断初始化,1ms进入一次中断
	tim9_it_ms(1);


	//是否满足字库更新条件，作为开机时特殊功能使用,这里只是擦除和开启更新字库标志位
	//下面的字体是无法显示的，后续就在while（1）中了，此时只要触发串口1的接收中断，就可以写入w25q64,字体只能复位重新加载
	//加看门狗实现更新字库后自动复位加载,10s擦除
	zk_update();

	//显示开机图片
	LCD_dis_pic(0,0,gImage_systemPic);

	//显示混合字符串
	LCD_dis(25,88,"吴龙刚",BLACK,0,WHITE,32);


	//播报语音
	//voice(RECOVER_ALL);

	


	while(1){
		//赋值看门狗
		iwdg_feed();
		//如果有键盘按键按下
		if(!BS8116_IRQ){
			BS8116_ReadKey();
		}
		
	}
	return 1;
}

