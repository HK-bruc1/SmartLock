#include "main.h"




//ȫ�ֱ�����ʵ����һ������ʵ�����ڴ��ڷ��ͺͽ�������
USART_t usart1;

//��ʱ��9�Ķ�ʱ�ж��¼���
u16 tim9_count [10];

//�Ƿ����ֿ���±�־
u8 zk_flag = 0;




int main (void){
	//�ж����ȼ�����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	//led��ʼ��
	led_init();
	//key�ĳ�ʼ��
	key_init();
	//usart1��ʼ��
	usart1Init(115200);
	//w25q64��ʼ��
	w25q64_init();
	//��ʱ��4��PWM�����ʼ����PB9
	tim4_ch4_led3_init();
	//LCD��Ļ��ʼ��
	LCD_init();
	//��ʱ��9�Ķ�ʱ�жϳ�ʼ��,1ms����һ���ж�
	tim9_it_ms(1);


	//��ʾ����ͼƬ
	LCD_dis_pic(0,0,gImage_systemPic);

	//�Ƿ������ֿ������������Ϊ����ʱ���⹦��ʹ��
	zk_update();

	


	while(1){

		
	}
	return 1;
}