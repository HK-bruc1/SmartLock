#include "main.h"




//ȫ�ֱ�����ʵ����һ������ʵ�����ڴ��ڷ��ͺͽ�������
USART_t usart1;

//��ʱ��9�Ķ�ʱ�ж��¼���
u16 tim9_count [10];

//�Ƿ����ֿ���±�־
u8 zk_flag = 0;

//BS8116����ֵ
u8 key_val;




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
	//at24c0x��ʼ��
	at24c0x_init();
	//��ʱ��4��PWM�����ʼ����PB9
	tim4_ch4_led3_init();
	//LCD��Ļ��ʼ��
	LCD_init();
	//����ģ���ʼ��
	voice_init();
	//BS8116��ʼ��
	BS8116_init();
	//���Ź���ʼ��
	iwdg_init();
	//�ŵ���ĳ�ʼ��
	door_init();

	//��ʱ��9�Ķ�ʱ�жϳ�ʼ��,1ms����һ���ж�
	tim9_it_ms(1);


	//�Ƿ������ֿ������������Ϊ����ʱ���⹦��ʹ��,����ֻ�ǲ����Ϳ��������ֿ��־λ
	//������������޷���ʾ�ģ���������while��1�����ˣ���ʱֻҪ��������1�Ľ����жϣ��Ϳ���д��w25q64,����ֻ�ܸ�λ���¼���
	//�ӿ��Ź�ʵ�ָ����ֿ���Զ���λ����,10s����ʱ��
	zk_update();


	


	while(1){
		//��ֵ���Ź�
		iwdg_feed();
		key_val = BS8116_Key_scan();
		open_passward(key_val);
		
	}
	return 1;
}

