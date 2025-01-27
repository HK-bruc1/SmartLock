#include "main.h"




//全局变量；实例化一个串口实例用于串口发送和接收数据
USART_t usart1;

//定时器9的定时中断事件器
u16 tim9_count [10];

//是否开启字库更新标志
u8 zk_flag = 0;

//BS8116按键值
u8 key_val;

//界面编号(看默认进哪一个界面：主界面)
u16 page_mode=1;

//RTC用于参数传递
RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;


//用于串口6接收指纹模块的数据,数据包共8个字节
volatile u8 mg200_buff[8];
//mg200数据包接收完成标志
volatile u8 mg200_rec_flag = 0; 
//mg200的注册ID，最多存在9个用户指纹ID,用户ID范围1~100，使用0xff作为初始无效值
u8 mg200_id[9];



//RC522
u8 picc_passward[6]={0xff,0xff,0xff,0xff,0xff,0xff}; //卡密码-初始密码--白卡的出厂密码，用于验证

//定义一个二维数组，设定一共可以注册9个卡片，二维数组9个元素，每个元素卡序号4个字节数据
u8 picc[9][4];

//待写入卡片块内数据
u8 picc_data[16] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

//卡片序列号
u8 picc_id[4];



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
	//at24c0x初始化
	at24c0x_init();
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
	//门电机的初始化
	door_init();
	//RTC初始化
	rtc_init();
	//指纹模块初始化
	mg200_init();
	//RC522读写器模块初始化
	MFRC522_Init();
	

	//定时器9的定时中断初始化,1ms进入一次中断
	tim9_it_ms(1);


	//是否满足字库更新条件，作为开机时特殊功能使用,这里只是擦除和开启更新字库标志位
	//下面的字体是无法显示的，后续就在while（1）中了，此时只要触发串口1的接收中断，就可以写入w25q64,字体只能复位重新加载
	//加看门狗实现更新字库后自动复位加载,10s擦除时间
	zk_update();


	


	while(1){
		//赋值看门狗
		iwdg_feed();
		key_val = BS8116_Key_scan();
		
		
		//由界面编号决定按键值给谁使用，因为多个界面需要按键值操作
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

