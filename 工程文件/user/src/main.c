#include "main.h"

void delay_ms(u32 ms){
	u32 i = 84 / 4*1000*ms;
	while(i){
		i--;
	}
}



int main (void){

	//led��ʼ��
	led_init();
	//key�ĳ�ʼ��
	key_init();
	
	//���շ��صİ���״̬
	u8 key_val = 0;
	
	while(1){
		//������ѯ����״̬
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