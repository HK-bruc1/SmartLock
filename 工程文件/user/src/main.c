#include "main.h"



//ȫ�ֱ�����ʵ����һ������ʵ�����ڴ��ڷ��ͺͽ�������
USART_t usart1;

//��ʱ��9�Ķ�ʱ�ж��¼���
u16 tim9_count [10];

//AT24C0X���



int main (void){
	//����ֵ
	u8 key_value = 0;

	//�ж����ȼ�����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	//led��ʼ��
	led_init();
	//key�ĳ�ʼ��
	key_init();
	//usart1��ʼ��
	usart1Init(115200);

	//��ʱ��4��PWM�����ʼ����PB9
	//tim4_ch4_led3_init();
	//�ŵĿ��س�ʼ��
	//door_init();
	//��ʱ��9�Ķ�ʱ�жϳ�ʼ��,1ms����һ���ж�
	tim9_it_ms(1);

	LCD_init();

	while(1){
		
		
		
		
	}

	
	return 1;
}