#include "main.h"

static void delay_ms(u32 ms){
	u32 i = 84 / 4*1000*ms;
	while(i){
		i--;
	}
}



int main (void){

	led_init();
	
	
	
	while(1){
		LED3_TURN;
		LED4_TURN;
		delay_ms(500);
	}

	
	
	
	
	
	
	return 1;
}