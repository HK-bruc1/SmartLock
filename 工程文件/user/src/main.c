#include "main.h"




//全局变量；实例化一个串口实例用于串口发送和接收数据
USART_t usart1;

//定时器9的定时中断事件器
volatile u16 tim9_count [10];

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

//接收卡片的块内数据
u8 picc_read_data[16];

//卡片序列号
u8 picc_id[4];


//初始化一个声音和亮度数据
SET_VAL_t set_data = {0};

//控制声音的指令
//档位权重：0，2，9，11，15
u8 voice_cmd[] = {0x27,0x25,0x23,0x22,0x21,0x20};

//初始化一个接收ESP32信息的结构体变量
USART2_RECSTR esp32rec = {0};

//设置wifi信息
//在 C 语言中，字符串字面量中的双引号需要使用转义符 \" 来表示。
//这是为了区分字符串的边界符号和实际要在字符串中使用的双引号字符
//u8 buuff[] = "AT+CWJAP=\"LL\",\"12345678\"\r\n"; // 示例命令，连接到WIFI "LL" 密码 "12345678"
//u8 wifi_command[] = "AT+CWJAP=\"abc\",\"0123456789\"\r\n";使用函数封装更友好

//让所有的开门之后自动关门，那么只需要上报关门的数据即可
//设置一个有效计时标志位，当门打开的时候，设置标志位，自动关门程序开启，到时间后在中断中关闭，同时把标志位清0
u8 autoCloseTimerFlag = 0;

//WiFi连接标志，没有连接WiFi的情况下，禁止执行MQTT相关的代码，不然一直超时重传，影响系统使用
//wifi断开的标志，如果一开始没有连接，周期性检查时会被置1，等到重连成功时，如果之前断开过，需要重连MQTT所以复位
//WiFi中途断开，周期性检查时会被置1，等到重连成功时，如果之前断开过，需要重连MQTT所以复位
//复位后自动变1，不用管
u8 wifi_connect_flag = 1;



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

	//最后初始化ESP32
	//将用到的串口2初始化了，测试ESP32是否正常工作，恢复出产设置后，设置客户端模式，进入透传模式,设置WiFi重连配置
	Esp32_Init();


	//连接一次WIFI
	wifi_connect_flag = Esp32_Wificonnect((u8 *)"319334854",(u8 *)"88888888");

	//MQTT相关配置，连接到服务器后使用MQTT协议进行数据传输
	if(wifi_connect_flag ==0){
		mqtt_init();
	}

	//没有初始化完成之前不应该让屏幕亮起来
	//声音亮度设定,没有记录就初始化
	set_v_l();


	


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

		//高速轮询定时任务的执行条件
		task();
		
		//由接收中断改为在主循环中处理ESP32响应的数据，长时间在中断不合适
		//在接收中断逻辑太长影响数据的接收，倒是可以放在空闲中断中，但是空闲中断中不能做太多事情，否则会阻塞其他中断
		//放在主循环的话，数据稍微大一点，也和在中断中差不多。在中断太长时间是中断和主程序一起卡死。
		//在主循环的话，数据一大，只会影响主循环的高速轮询
		//进入while1后，数据不需要手动清理接收缓存
		ProcessUartData();
	}
}

