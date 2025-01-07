#include "main.h"

void delay_ms(u32 ms){
	u32 i = 84 / 4*1000*ms;
	while(i){
		i--;
	}
}



int main (void){

	//led初始化
	led_init();
	//key的初始化
	key_init();
	
	//接收返回的按键状态
	u8 key_val = 0;
	
	while(1){
		//高速轮询按键状态
		key_val = key_scan();
		switch (key_val)
		{
		case 1:
			LED4_TURN;
			break;
		case 2:
			LED3_TURN;
			break;
		default:
			break;
		}
	}

	
	
	
	
	
	
	return 1;
}