#include "main.h"




//ȫ�ֱ�����ʵ����һ������ʵ�����ڴ��ڷ��ͺͽ�������
USART_t usart1;

//��ʱ��9�Ķ�ʱ�ж��¼���
u16 tim9_count [10];

//�Ƿ����ֿ���±�־
u8 zk_flag = 0;

//BS8116����ֵ
u8 key_val;

//������(��Ĭ�Ͻ���һ�����棺������)
u16 page_mode=1;

//RTC���ڲ�������
RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;


//���ڴ���6����ָ��ģ�������,���ݰ���8���ֽ�
volatile u8 mg200_buff[8];
//mg200���ݰ�������ɱ�־
volatile u8 mg200_rec_flag = 0; 
//mg200��ע��ID��������9���û�ָ��ID,�û�ID��Χ1~100��ʹ��0xff��Ϊ��ʼ��Чֵ
u8 mg200_id[9];



//RC522
u8 picc_passward[6]={0xff,0xff,0xff,0xff,0xff,0xff}; //������-��ʼ����--�׿��ĳ������룬������֤

//����һ����ά���飬�趨һ������ע��9����Ƭ����ά����9��Ԫ�أ�ÿ��Ԫ�ؿ����4���ֽ�����
u8 picc[9][4];

//��д�뿨Ƭ��������
u8 picc_data[16] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

//��Ƭ���к�
u8 picc_id[4];



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
	//RTC��ʼ��
	rtc_init();
	//ָ��ģ���ʼ��
	mg200_init();
	//RC522��д��ģ���ʼ��
	MFRC522_Init();
	

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
		
		
		//�ɽ����ž�������ֵ��˭ʹ�ã���Ϊ���������Ҫ����ֵ����
		switch(page_mode){
			case 1:main_page(key_val);break;
			case 2:admin_page(key_val);break;
			case 3:password_page(key_val);break;
			case 4:mg200_page(key_val);break;
			case 41:Enroll_user_page(key_val);break;
			case 42:erase_user_one_page(key_val);break;
			case 43:erase_user_all_page(key_val);break;
			case 44:erase_user_match_page(key_val);break;
			case 5:rfid_page(key_val);break;
			case 51:picc_user_page(key_val);break;
			case 52:erase_user_picc_page(key_val);break;
			case 53:erase_picc_all_page(key_val);break;
			case 54:erase_picc_match_page(key_val);break;
			case 6:voice_light_page(key_val);break;
			case 7:factory_reset_page(key_val);break;

		}
		
	}
	return 1;
}

