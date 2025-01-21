#include "main.h"

//各模块整合的综合性函数


//字库更新函数，放在main函数前面，这样的话，不能随变使用按键2就能更新字库，只能在重启时按住2才能更新字库，作为一个特殊功能使用
void zk_update(void){
    //获取按键值
    u8 key = key_scan();
    //判断按键值
    if(key == 2){
        printf("正在擦除字库存储所需要的空间...");
        //0x 7f  f  f  ff
        //   区  块 页  地址
        w25q64_blocks(ZK_ADDR,21);
        printf("擦除完成！\n");
        //开启字库更新标志
        zk_flag = 1;
    }
}





/***********************************************
*函数名    :open_passward
*函数功能  :密码开锁
*函数参数  :u8 bs8116_key
*函数返回值:u8
*函数描述  :返回1 开门成功
           返回0 开门失败
************************************************/
u8 open_passward(u8 bs8116_key){
    //密码解锁的状态返回值
    u8 sta = 0xff;
    //第一次开机标志位的值,由于只会获取一次但是会进入多次，需要保存下来，不然第二次就会出错
    static u8 open_val;
    //密码登录页面状态锁
    static u8 pass_page_flag = 1;
    //密码开锁的界面圆点坐标
    static u8 x = 45,y = 200;
    //设置密码的语言播报状态锁
    static u8 pass_flag = 1;
    //用于存储两次密码的值
	static u8 pass_word_1[7] = {0};
	static u8 pass_word_2[7] = {0};
    //用于计数密码个数画圆的个数
	static u8 pass_i = 0;



    /*公用密码登录以及主界面,输入密码流程时保证只进入一次，避免无效操作导致CPU效率下降*/
    if(pass_page_flag){
        //读第一次开机标志位
        at24c0x_read_byte(10,&open_val);
        //密码开锁界面
        LCD_dis_pic(0,0,gImage_systemPic);
        x=45;
        for(u8 i=0;i<6;i++)
		{
			LCD_Ring(x,y,8,LGRAY);
			x+=30;
		}
        
        //锁住主界面，防止反复刷新
        pass_page_flag = 0;
        //坐标归位，方便画实心圆
        x=45;


        //每一次进入主界面都要立马显示时间
        tim9_count[2] = 60000;
		//不是第一次开机才打印指纹，第一次开机只能设置初始密码
		if(open_val == OPEN_FLAG){
			//打印指纹图片
        	LCD_dis_number_pic(96,120,"Z",LGRAY,1,gImage_systemPic);
		}

    }




    /*判断是第一次开机，需要设置密码*/
    if(open_val != OPEN_FLAG){
        //输入第一次初始密码的语音播报
        if(pass_flag==1){
            LCD_dis(70,130,"请设置新密码",LIGHTBLUE,0,0xff,16);
            voice(INPUT_NEW_PASSWORD);
            //锁住设置密码的语音播报，防止反复播报
            pass_flag=0;
        }

        //只拿有效密码值
        if(bs8116_key !='*' && bs8116_key!='#' && bs8116_key!=0xff){
            //第一次输入密码
            if(pass_i<6){
                //数组层面
                pass_word_1[pass_i] = bs8116_key;
                pass_i++;
                //屏幕跟上实心圆
                LCD_shiRing(x,y,8,LGRAY);
                //圆心坐标移动
                x+=30;

                //pass_i到6就表示第一次密码输入完成
                if(pass_i==6){
                //把实心圆清除重新填充背景图片像素颜色值
                LCD_xy_clear(45-8,200-8,166,16,0xffff,1,gImage_systemPic);
                //重新换空心圆
                x=45;
                for(u8 i=0;i<6;i++)
                {
                    LCD_Ring(x,y,8,LGRAY);
                    x+=30;
                }

                //在显示之前把文字需要的背景先清除干净：把文字所在的区域背景像素值全部改成对应图片颜色值
                LCD_xy_clear(70,130,100,16,0xffff,1,gImage_systemPic);
                LCD_dis(70,130,"请再次输入新密码",LIGHTBLUE,0,0xff,16);
                voice(INPUT_NEW_PASSWORD_AGAIN);
                //圆心归位，以便下一次使用
                x=45;
                
                }
            }
            //第二次输入密码
            else if(pass_i>=6){
                pass_word_2[pass_i-6] = bs8116_key;
                pass_i++;
                //屏幕实心圆再次跟上
                LCD_shiRing(x,y,8,LGRAY);
                //圆心坐标移动
                x+=30;

                //当pass_i到12时，就表示第二次密码输入完成
                if(pass_i==12){
                    //对比两次密码是否一致
                    if(strcmp((char *)pass_word_1,(char *)pass_word_2)==0){
                        //将密码存入at24
                        AT24C0x_write_bytes(11,sizeof(pass_word_2),pass_word_2,AT24C04);
                        //写入第一次开机标志位
                        at24c0x_write_byte(10,OPEN_FLAG);

                        //回到公用密码登录界面,重新刷新图片和空心圆,把所有状态锁打开
                        pass_page_flag = 1;
                        //把密码输入个数记录清除
                        pass_i = 0;
                        //打开首次开机的语音播报状态锁,因为设置初始密码后续就不进入第一次开机逻辑了
                        //但是正常密码解锁逻辑也需要，需要还是解锁
                        pass_flag = 1;
                        voice(SETTING_SUCCESS);
                    }
                    else{
                        //回到公用密码登录界面,重新刷新图片和空心圆,把所有状态锁打开
                        //重新开始密码输入操作
                        pass_flag = 1;//打开首次开机的语音播报状态锁，因为密码不一致需要重新播报
                        pass_i = 0;
                        pass_page_flag = 1;
                        voice(PASSWORD_INCONFORMITY);
                    }
                }
            } 
        }
    }  


    /*不是第一次开机，需要输入密码对比*/
    //经过第一次重新设置密码后，进入到正常密码解锁逻辑
    else{
        //正常密码解锁语音播报(只播报一次即可),pass_flag状态锁不是进入第一次开机逻辑后，这里也可以使用
        if(pass_flag==1){
            voice(INPUT_ADMIN_PASSWORD);
            pass_flag = 0;

        }
        //先获取用户输入的六位密码，此前pass_i的值已经归零
        if(bs8116_key !='*' && bs8116_key!='#' && bs8116_key!=0xff){
            //第一次输入密码
            if(pass_i<6){
                //数组层面
                pass_word_1[pass_i] = bs8116_key;
                pass_i++;
                //屏幕跟上实心圆
                LCD_shiRing(x,y,8,LGRAY);
                //圆心坐标移动
                x+=30;

                //pass_i到6就表示密码输入完成
                if(pass_i==6){
                    //提取密码对比
                    AT24C0x_read_bytes(11,sizeof(pass_word_2),pass_word_2);
                    if(strcmp((char *)pass_word_1,(char *)pass_word_2)==0){
                        voice(DOOROPEN_SUCCESS);
                        door_open();
                        //开门之后，把所有状态锁打开，回到密码登录界面
                        pass_page_flag = 1;
                        pass_i = 0;
                        pass_flag = 1;

                        sta = 1;
                    }
                    else {
                        //开门失败，回到密码登录界面
                        pass_page_flag = 1;
                        pass_i = 0;
                        voice(DOOROPEN_FAIL);
                        pass_flag = 1;
                        
                        sta = 0;
                    }
                }
            }
        }
    }
    //每次扫描无操作的话返回0xff
    return sta;
}



/***********************************************
*函数名    :main_page
*函数功能  :主界面函数
*函数参数  :u8 key
*函数返回值:无
*函数描述  :page_mode = 1
************************************************/
void main_page(u8 key)
{
    // 定义一个标志位来表示当前的颜色状态
    static int color_flag = 0;
    //传递时间数据,因为多次调用避免反复定义
    static u8 timer_buff[20];
    //密码开锁的状态
    u8 sta;
    //密码开锁
    sta = open_passward(key);
    if(sta == 1){
        //不在函数中层层调用进入下一个页面，在while中不断通过标志位轮询跳过对应界面即可
        page_mode = 2;
    }



    //主界面更新时间的时机,一分钟刷新一次,写在中断函数中，时间显示不受控制
    if(tim9_count[2]>=60000){
        tim9_count[2] = 0;
        //获取日期和时间
        RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
        RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
        //显示时间，数据转换为字符串形式
        sprintf((char *)timer_buff,"%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes);
        //使用48*48的
        LCD_dis_number_pic(0,58,timer_buff,LGRAY,1,gImage_systemPic);
    }

}


/***********************************************
*函数名    :admin_page
*函数功能  :管理员界面函数
*函数参数  :u8 key
*函数返回值:无
*函数描述  :page_mode = 2
************************************************/
void admin_page(u8 key)
{
    //状态锁
    static u8 ad_flag = 1;

    if(ad_flag==1){
        //串口上位机
		printf("\r\n管理员界面:\r\n");
		printf("【1】密码管理\r\n");
		printf("【2】指纹管理\r\n");
		printf("【3】射频卡管理\r\n");
		printf("【4】声音亮度调节\r\n");
		printf("【5】恢复出厂设置\r\n");
		printf("【#】退出管理界面\r\n");

		//LCD屏幕显示
		LCD_dis_pic(0,0,gImage_systemPic);

		LCD_dis(40,48,"管理员界面",LIGHTBLUE,0,0xff,32);
		LCD_dis(5,83,"【1】密码管理",LIGHTBLUE,0,0xff,16);
		LCD_dis(5,108,"【2】指纹管理",LIGHTBLUE,0,0xff,16);
		LCD_dis(5,133,"【3】射频卡管理",LIGHTBLUE,0,0xff,16);
		LCD_dis(5,158,"【4】声音亮度调节",LIGHTBLUE,0,0xff,16);
		LCD_dis(5,183,"【5】恢复出厂设置",LIGHTBLUE,0,0xff,16);
		LCD_dis(5,208,"【#】退出管理员界面",LIGHTBLUE,0,0xff,16);

        //锁住页面，进来后只刷新一次页面
        ad_flag = 0;
    }




    //此页面的功能选择（高速轮询）
    switch(key){
        //本函数的状态锁解开，以便后续进来显示
        case '1':page_mode = 3;ad_flag = 1;break;//进入到密码管理界面,状态锁解开
        case '2':page_mode = 4;ad_flag = 1;break;//进入到指纹管理界面,状态锁解开
        case '3':page_mode = 5;ad_flag = 1;break;//进入到射频卡管理界面,状态锁解开
        case '4':page_mode = 6;ad_flag = 1;break;//进入到声音亮度调节界面,状态锁解开
        case '5':page_mode = 7;ad_flag = 1;break;//进入到恢复出厂设置界面,状态锁解开
        case '#':page_mode = 1;ad_flag = 1;break;//返回到密码登录主界面,状态锁解开
    }
}



/***********************************************
*函数名    :password_page
*函数功能  :密码管理界面函数
*函数参数  :u8 key
*函数返回值:无
*函数描述  :page_mode = 3
************************************************/
void password_page(u8 key)
{
    static u8 ad_flag = 1;
    //一个key进来一次，x,y坐标需要保留记忆
	static u8 x=45,y=95;
	static u8 pass_i = 0;
	static u8 pass_word_1[7] = {0};
	static u8 pass_word_2[7] = {0};
	
	u8 i;


	//执行一次    界面提示
	if(ad_flag)
	{
        //防止密码中途输入未完成，需要清空，不然pass_i为上次的旧值,下标清空，数组旧值会被覆盖，不用担心
        pass_i = 0;
		//串口上位机显示
		printf("\r\n修改管理员密码\r\n");
		printf("【#】返回上一级菜单\r\n");
		printf("【*】返回主界面\r\n");
		
		
		//LCD屏幕显示
		LCD_dis_pic(0,0,gImage_systemPic);
		LCD_dis(9,48,"修改管理员密码",LIGHTBLUE,0,0xff,32);
		LCD_dis(60,90,"第一次输入密码",LIGHTBLUE,0,0xff,16);
        //空心圆跟随
		x = 45;
		for(i = 0;i < 6;i++)
		{
			LCD_Ring(x,120,6,LGRAY);
			x += 30;
		}

		LCD_dis(60,140,"第二次输入密码",LIGHTBLUE,0,0xff,16);
		x = 45;
		for(i = 0;i < 6;i++)
		{
			LCD_Ring(x,170,6,LGRAY);
			x += 30;
		}
		x = 45;

        //左右两个角落显示
		LCD_dis(5,220,"主界面【*】",LIGHTBLUE,0,0xff,16);
		LCD_dis(145,220,"【#】上一级",LIGHTBLUE,0,0xff,16);
		
		ad_flag = 0;
	}


    //修改密码功能程序
	if(key != '*' && key != '#' && key != 0xff)
	{
		
		//第一次输入密码
		if(pass_i<6)
		{
			pass_word_1[pass_i] = key;
			pass_i++;
			LCD_shiRing(x,120,8,LGRAY);
			x+=30;
			
			//第一次输入完6个密码清除屏幕密码取余和提示语
			if(pass_i == 6)
			{
				x = 45;
				voice(0x0e);
			}
		}
		
		//第二次输入密码
		else if(pass_i >= 6 )
		{
			pass_word_2[pass_i-6] = key;
			pass_i++;
			LCD_shiRing(x,170,8,LGRAY);
			x+=30;
			//第二次输入完6个密码  对比密码
			if(pass_i == 12)
			{
				//两次密码一致
				if(strcmp((char *)pass_word_1,(char *)pass_word_2) == 0)
				{
					//将密码存储到AT24的11号地址
					AT24C0x_write_bytes(11,sizeof(pass_word_1),pass_word_1,AT24C04);
					//回到登录界面
					pass_i = 0;
					page_mode = 1;
					ad_flag = 1;
				}
				//两次密码不一致
				else
				{
					voice(0x10);
					pass_i = 0;
					ad_flag = 1;
				}
			}
			
		}	
	}


    switch(key)
	{
		case '#': page_mode = 2; ad_flag = 1;break;    //返回上一级菜单
		case '*': page_mode = 1; ad_flag = 1;break;    //返回主界面
		
	}
}



/***********************************************
*函数名    :mg200_page
*函数功能  :指纹管理界面函数
*函数参数  :u8 key
*函数返回值:无
*函数描述  :page_mode = 4
************************************************/
void mg200_page(u8 key)
{
	static u8 ad_flag = 1; 
	//首次进入指纹管理界面标志
	u8 open_flag;
	
	
	//执行一次
	if(ad_flag)
	{
		//串口上位机显示
		printf("\r\n指纹管理界面:\r\n");
		printf("【1】注册指纹\r\n");
		printf("【2】删除指定ID指纹\r\n");
		printf("【3】删除所有指纹\r\n");
		printf("【4】识别删除指纹\r\n");
		printf("【#】回到上一级菜单\r\n");
		printf("【*】回到主界面\r\n");
		
		
		//LCD屏幕显示
		LCD_dis_pic(0,0,gImage_systemPic);
		LCD_dis(26,48,"指纹管理界面",LIGHTBLUE,0,0xff,32);
		LCD_dis(5,83,"【1】注册指纹",LIGHTBLUE,0,0xff,16);
		LCD_dis(5,108,"【2】删除指定ID指纹",LIGHTBLUE,0,0xff,16);
		LCD_dis(5,133,"【3】删除所有指纹",LIGHTBLUE,0,0xff,16);
		LCD_dis(5,158,"【4】识别删除指纹",LIGHTBLUE,0,0xff,16);	
		LCD_dis(5,220,"主界面【*】",LIGHTBLUE,0,0xff,16);
		LCD_dis(145,220,"【#】上一级",LIGHTBLUE,0,0xff,16);
		

		//检测是否为第一次进入指纹管理（把初始指纹ID数据先写入存储芯片中）
		at24c0x_read_byte(20,&open_flag);
		if(open_flag != OPEN_FLAG)
		{
			//清除指纹模块(第一次进入指纹管理，说明之前的指纹都需要擦除)
			erase_all();
			//把原始数组存储到AT24中,使用0xff作为初始无效值
			memset(mg200_id,0xff,sizeof(mg200_id));
			AT24C0x_write_bytes(21,sizeof(mg200_id),mg200_id,AT24C04);
			//把第一次检测标志位写入到AT(板子上是AT24C04)
			at24c0x_write_byte(20,OPEN_FLAG);
		}

		
	
		ad_flag = 0;
	}
	
	
	switch(key)
	{
		case '1':page_mode = 41;ad_flag=1;break;
        case '2':page_mode = 42;ad_flag=1;break;
        case '3':page_mode = 43;ad_flag=1;break;
        case '4':page_mode = 44;ad_flag=1;break;
		case '#':page_mode = 2;ad_flag=1;break;
		case '*':page_mode = 1;ad_flag=1;break;
	}
}


/***********************************************
*函数名    :Enroll_user_page
*函数功能  :注册用户指纹界面函数
*函数参数  :u8 key
*函数返回值:无
*函数描述  :page_mode = 41
*指纹模块可以保存指纹，对外不可见。拿不到已经存入的指纹ID列表，只好通过AT24C02保存已使用ID列表信息，通过此列表管理模块中保存的指纹
************************************************/
void Enroll_user_page(u8 key)
{
	static u8 ad_flag = 1;
	//指纹注册的操作状态
	static mg200_register_sta = 1;
	//显示坐标
	static u16 x = 30;
	u8 mg200_i;
	//要录入的指纹ID
	static u8 mg200_ID;
	
	//执行一次
	if(ad_flag)
	{
		//语音播报
		voice(REGISTER_FINGER);
		//串口上位机显示
		printf("\r\n注册用户指纹界面:\r\n");
		printf("【#】回到上一级菜单\r\n");
		printf("【*】回到主界面\r\n");
		//LCD屏幕显示
		//为什么很丝滑？因为每一个界面的背景色是一样的，刷新时看不出明显的整块屏幕的切换感，因为不是整块屏幕的颜色都改变了
		LCD_dis_pic(0,0,gImage_systemPic);
		LCD_dis(24,48,"注册用户指纹",LIGHTBLUE,0,0xff,32);
		LCD_dis(0,220,"【*】主界面",LIGHTBLUE,0,0xff,16);
		LCD_dis(150,220,"上一级【#】",LIGHTBLUE,0,0xff,16);
		LCD_dis_number_pic(96,120,"Z",LGRAY,1,gImage_systemPic);


		//从AT24中读出指纹ID数据到数组中
		AT24C0x_read_bytes(21,sizeof(mg200_id),mg200_id);
		//把现有ID打印到屏幕上
		x = 30;
		//把存入的9个指纹ID拿出来
		for(mg200_i=0;mg200_i<9;mg200_i++)
		{
			//筛选显示有效指纹ID
			if(mg200_id[mg200_i]!= 0xff)
			{
				printf("%u  ",mg200_id[mg200_i]);
				LCD_dis_ch(x,80,mg200_id[mg200_i]+48,LIGHTBLUE,0,0xff,32);
				x+=20;
			}
		}

		//遍历数组找到事0xff的元素的下标，作为下一个录入指纹的数组存储下标
		for(mg200_i=0;mg200_i<9;mg200_i++)
		{
			if(mg200_id[mg200_i]== 0xff)
			{
				break;
			}
		}
		//注册ID就是下标+1 0xff表示无效值，注册ID限制在1~9
		mg200_ID = mg200_i+1;
		//9个元素遍历完，那么mg200_i会等于9，mg200_i+1=10
		if(mg200_ID==10)
		{
			printf("注册指纹数量以达上限\r\n");
		}

		ad_flag = 0;
	}

	//ID符合要求就可以开始扫描注册函数
	if(mg200_ID<10)
	{
		//返回0xfb  用户进入到指纹注册界面但是没有按下指纹或者没有采集超过三次
		mg200_register_sta = mg200_register(mg200_ID);
		//注册成功
		if(mg200_register_sta == 0x00)
		{
			//把注册的ID写到数组中，ID和下标相差1
			mg200_id[mg200_ID-1] = mg200_ID;
			//把数组写入AT24
			AT24C0x_write_bytes(21,sizeof(mg200_id),mg200_id,AT24C04);
			printf("已注册%d号ID\r\n",mg200_ID);
			//显示新注册的ID，ID转ASCII字符
			LCD_dis_ch(x,50,mg200_ID+48,GREEN,32,0,NULL);
			x+=20;

			//释放状态锁，重新刷新当前页面，打印新的ID
			ad_flag = 1;
			
		}
	}



	
	
	switch(key)
	{
		case '#':page_mode = 4;ad_flag=1;break;
		case '*':page_mode = 1;ad_flag=1;break;
	}

	
}




/***********************************************
*函数名    :erase_user_one_page
*函数功能  :删除指定ID指纹界面函数
*函数参数  :u8 key
*函数返回值:无
*函数描述  :page_mode = 42
************************************************/
void erase_user_one_page(u8 key)
{
	static u8 ad_flag = 1;
	
	
	//执行一次
	if(ad_flag)
	{
		//串口上位机显示
		printf("\r\n删除指定指纹界面:\r\n");
		printf("请选择指纹ID\r\n");
		printf("【#】回到上一级菜单\r\n");
		printf("【*】回到主界面\r\n");

		//LCD屏幕显示
		LCD_dis_pic(0,0,gImage_systemPic);
		LCD_dis(24,48,"删除指定指纹",LIGHTBLUE,0,0xff,32);
		LCD_dis(0,220,"【*】主界面",LIGHTBLUE,0,0xff,16);
		LCD_dis(150,220,"上一级【#】",LIGHTBLUE,0,0xff,16);
		
		
		ad_flag = 0;
	}
	
	switch(key)
	{
		case '#':page_mode = 4;ad_flag=1;break;
		case '*':page_mode = 1;ad_flag=1;break;
	}
}





/***********************************************
*函数名    :erase_user_all_page
*函数功能  :删除所有指纹界面函数
*函数参数  :u8 key
*函数返回值:无
*函数描述  :page_mode = 43
************************************************/
void erase_user_all_page(u8 key)
{
	static u8 ad_flag = 1;

	//执行一次
	if(ad_flag)
	{
		//串口上位机显示
		printf("\r\n删除所有指纹？:\r\n");
		printf("【*】确认\r\n");
		printf("#】取消\r\n");
		
		
		//LCD屏幕显示
		LCD_dis_pic(0,0,gImage_systemPic);
		LCD_dis(24,48,"删除所有指纹",LIGHTBLUE,0,0xff,32);
		LCD_dis(56,85,"确认删除",LIGHTBLUE,0,0xff,32);
		LCD_dis_number_pic(96,126,"?",RED,1,gImage_systemPic);
		LCD_dis(0,220,"确认(*)",LIGHTBLUE,0,0xff,16);
		LCD_dis(180,220,"取消(#)",LIGHTBLUE,0,0xff,16);
		
		ad_flag = 0;
	}
	

	
	if(key == '*')     //确认删除 后 回到上级菜单
	{
		//删除的动作
		
		//回上级菜单
		page_mode = 4;
		ad_flag=1;
	}
	
	else if(key == '#')   //取消会到上级菜单
	{
		page_mode = 4;
		ad_flag=1;
	}

}




/***********************************************
*函数名    :erase_user_match_page
*函数功能  :识别删除指纹界面函数
*函数参数  :u8 key
*函数返回值:无
*函数描述  :page_mode = 44
************************************************/
void erase_user_match_page(u8 key)
{
	static u8 ad_flag = 1;
	
	//执行一次
	if(ad_flag == 1)
	{
		//串口上位机显示
		printf("\r\n识别删除指纹界面\r\n");
		printf("识别指纹\r\n");
		printf("【*】确认\r\n");
		printf("【#】取消\r\n");
		
		//LCD屏幕显示
		LCD_dis_pic(0,0,gImage_systemPic);//设置图片为背景
		LCD_dis(25,48,(u8 *)"识别删除指纹",LIGHTBLUE,0,0xff,32);
		LCD_dis_number_pic(96,140,(u8 *)"Z",LGRAY,1,gImage_systemPic);
		LCD_dis(160,220,"【#】返回",LIGHTBLUE,0,0xff,16);
		LCD_dis(0,220,"确认【*】",LIGHTBLUE,0,0xff,16);
		
		
		ad_flag = 0;
	}
	
	
	
	
	/*按键确认*/
	if(key == '*' )     //确认删除 后 回到上级菜单
	{
		
		//回上级菜单
		page_mode = 4;
		ad_flag=1;
	}
	
	
	else if(key == '#')   //取消会到上级菜单
	{
		page_mode = 4;
		ad_flag=1;
	}
}



/***********************************************
*函数名    :rfid_page
*函数功能  :射频卡管理界面函数
*函数参数  :u8 key
*函数返回值:无
*函数描述  :page_mode = 5
************************************************/
void rfid_page(u8 key)
{
	static u8 ad_flag = 1;
	
	//执行一次
	if(ad_flag)
	{
		//串口上位机显示
		printf("\r\n卡片管理界面：\r\n");
		printf("【1】注册卡片\r\n");
		printf("【2】删除指定ID卡片\r\n");
		printf("【3】删除所有卡片\r\n");
		printf("【4】识别删除卡片\r\n");
		printf("【#】回到上一级菜单\r\n");
		printf("【*】回到主界面\r\n");
		
		//LCD屏幕界面
		LCD_dis_pic(0,0,gImage_systemPic);
		LCD_dis(26,48,"射频卡管理",LIGHTBLUE,0,0xff,32);
		LCD_dis(5,83,"【1】注册卡片",LIGHTBLUE,0,0xff,16);
		LCD_dis(5,108,"【2】删除指定卡片",LIGHTBLUE,0,0xff,16);
		LCD_dis(5,133,"【3】删除所有卡片",LIGHTBLUE,0,0xff,16);
		LCD_dis(5,158,"【4】识别删除卡片",LIGHTBLUE,0,0xff,16);	
		LCD_dis(5,220,"主界面【*】",LIGHTBLUE,0,0xff,16);
		LCD_dis(145,220,"【#】上一级",LIGHTBLUE,0,0xff,16);
		
		ad_flag = 0;
	}
	
	
	switch(key)
	{   case '1':page_mode = 51;ad_flag=1;break;
        case '2':page_mode = 52;ad_flag=1;break;
        case '3':page_mode = 53;ad_flag=1;break;
        case '4':page_mode = 54;ad_flag=1;break;
		case '#':page_mode = 2;ad_flag=1;break;
		case '*':page_mode = 1;ad_flag=1;break;	
	}
}




/***********************************************
*函数名    :picc_user_page
*函数功能  :注册用户卡片界面函数
*函数参数  :u8 key
*函数返回值:无
*函数描述  :page_mode = 51
************************************************/
void picc_user_page(u8 key)
{
	
	static u8 ad_flag = 1;
	
	//执行一次
	if(ad_flag)
	{
		//串口上位机显示
		printf("\r\n注册用户卡片界面：\r\n");
		printf("【#】回到上一级菜单\r\n");
		printf("【*】回到主界面\r\n");
		
		
		//LCD屏幕显示
		LCD_dis_pic(0,0,gImage_systemPic);
		LCD_dis(24,48,"注册用户卡片",LIGHTBLUE,0,0xff,32);
		LCD_dis_number_pic(50,120,"P",LGRAY,1,gImage_systemPic);
		LCD_dis_number_pic(130,120,"Q",LGRAY,1,gImage_systemPic);
		LCD_dis(0,220,"【*】主界面",LIGHTBLUE,0,0xff,16);
		LCD_dis(150,220,"上一级【#】",LIGHTBLUE,0,0xff,16);
		
		ad_flag = 0;
	}
	
	

	
	
	switch(key)
	{
		case '#':page_mode = 5;ad_flag=1;break;
		case '*':page_mode = 1;ad_flag=1;break;
	}
}



/***********************************************
*函数名    :erase_user_picc_page
*函数功能  :删除指定卡片界面函数
*函数参数  :u8 key
*函数返回值:无
*函数描述  :page_mode = 52
************************************************/
void erase_user_picc_page(u8 key)
{
	static u8 ad_flag = 1;
	//执行一次
	if(ad_flag)
	{
		//串口上位机显示
		printf("\r\n删除指定卡片界面:\r\n");
		printf("请选择卡片\r\n");
		printf("【#】回到上一级菜单\r\n");
		printf("【*】回到主界面\r\n");

		//LCD屏幕显示
		LCD_dis_pic(0,0,gImage_systemPic);
		LCD_dis(24,48,"删除指定卡片",LIGHTBLUE,0,0xff,32);
		LCD_dis(0,220,"【*】主界面",LIGHTBLUE,0,0xff,16);
		LCD_dis(150,220,"上一级【#】",LIGHTBLUE,0,0xff,16);
		
		

		ad_flag = 0;
	}
	
	
	
	switch(key)
	{
		case '#':page_mode = 5;ad_flag=1;break;
		case '*':page_mode = 1;ad_flag=1;break;
	}
	
}



/***********************************************
*函数名    :erase_picc_all_page
*函数功能  :删除所有卡片界面函数
*函数参数  :u8 key
*函数返回值:无
*函数描述  :page_mode = 53
************************************************/
void erase_picc_all_page(u8 key)
{
	static u8 ad_flag = 1;
	//执行一次
	if(ad_flag)
	{
		//串口上位机显示
		printf("\r\n删除所有卡片?:\r\n");
		printf("【*】确认\r\n");
		printf("【#】取消\r\n");
		
		
		//LCD屏幕显示
		LCD_dis_pic(0,0,gImage_systemPic);
		LCD_dis(24,48,"删除所有卡片",LIGHTBLUE,0,0xff,32);
		LCD_dis(56,85,"确认删除",LIGHTBLUE,0,0xff,16);
		LCD_dis_number_pic(96,126,"?",RED,1,gImage_systemPic);
		LCD_dis(0,220,"确认(*)",LIGHTBLUE,0,0xff,16);
		LCD_dis(180,220,"取消(#)",LIGHTBLUE,0,0xff,16);
		
		ad_flag = 0;
	}
	
	
	
	
	if(key == '*')     //确认删除 后 回到上级菜单
	{
		//删除的动作

		
		//回上级菜单
		page_mode = 5;
		ad_flag=1;
	}
	
	else if(key == '#')   //取消会到上级菜单
	{
		page_mode = 5;
		ad_flag=1;
	}

}


/***********************************************
*函数名    :erase_picc_match_page
*函数功能  :识别删除卡片界面函数
*函数参数  :u8 key
*函数返回值:无
*函数描述  :page_mode = 54
************************************************/
void erase_picc_match_page(u8 key)
{
	static u8 ad_flag = 1;
	
	//执行一次
	if(ad_flag)
	{
		//串口上位机显示
		printf("\r\n识别删除卡片界面\r\n");
		printf("识别卡片\r\n");
		printf("【#】取消\r\n");
		
		//LCD屏幕显示
		LCD_dis_pic(0,0,gImage_systemPic);//设置图片为背景
		LCD_dis(25,48,(u8 *)"识别删除卡片",LIGHTBLUE,0,0xff,32);
		LCD_dis_number_pic(50,120,"P",LGRAY,1,gImage_systemPic);
		LCD_dis_number_pic(130,120,"Q",LGRAY,1,gImage_systemPic);
		LCD_dis(160,220,(u8 *)"【#】返回",LIGHTBLUE,0,0xff,16);
		
		
		ad_flag = 0;
	}
	

	
	
	else if(key == '#')   //取消会到上级菜单
	{
		page_mode = 5;
		ad_flag=1;
	}

	
}



/***********************************************
*函数名    :voice_light_page
*函数功能  :声音和屏幕亮度管理界面函数
*函数参数  :u8 key
*函数返回值:无
*函数描述  :page_mode = 6
************************************************/
void voice_light_page(u8 key)
{
	static u8 ad_flag = 1;
	
	
	//执行一次
	if(ad_flag)
	{
		//串口上位机显示
		printf("\r\n声音亮度调节界面:\r\n");
		printf("【1】音量加\r\n");
		printf("【2】音量减\r\n");
		printf("【3】静音模式\r\n");
		printf("【4】屏幕亮度加\r\n");
		printf("【5】屏幕亮度减\r\n");
		printf("【#】回到上一级菜单\r\n");
		printf("【*】回到主界面\r\n");
		
		//LCD屏幕显示
		LCD_dis_pic(0,0,gImage_systemPic);
		LCD_dis(24,48,"声音亮度调节",LIGHTBLUE,0,0xff,32);
		LCD_dis_number_pic(5,80,"S",LGRAY,1,gImage_systemPic);
		LCD_dis_number_pic(5,140,"L",LGRAY,1,gImage_systemPic);
		
		LCD_dis(0,220,"【*】主界面",LIGHTBLUE,0,0xff,16);
		LCD_dis(150,220,"上一级【#】",LIGHTBLUE,0,0xff,16);
		
		ad_flag = 0;
	}
	
	
	//返回上一级菜单
	if(key=='#')
	{
		page_mode = 2;       //返回管理员界面
		ad_flag = 1;
	}
	//返回主界面
	else if(key=='*')
	{
		page_mode = 1;       //返回主界面
		ad_flag = 1;
	}
	
	//声音加
	else if(key=='1')
	{
		
	}
	//声音减
	else if(key=='2')
	{
			
	}
	//静音
	else if(key=='3')
	{
		
	}
	
	
	//加亮度
	else if(key == '4')
	{
		
	}

	//减亮度
	else if(key == '5')
	{
		
	}
}

/***********************************************
*函数名    :factory_reset_page
*函数功能  :恢复出厂设置界面函数
*函数参数  :u8 key
*函数返回值:无
*函数描述  :page_mode = 7
************************************************/
void factory_reset_page(u8 key)
{
	static u8 ad_flag = 1;
	
	//执行一次
	if(ad_flag)
	{
		//串口上位机显示
		printf("\r\n恢复出厂设置界面:\r\n");
		printf("【*】确认\r\n");
		printf("【#】取消\r\n");
		
		//LCD屏幕显示
		LCD_dis_pic(0,0,gImage_systemPic);
		LCD_dis(24,48,"恢复出厂设置",LIGHTBLUE,0,0xff,32);
		LCD_dis(56,85,"确认恢复？",LIGHTBLUE,0,0xff,16);
		LCD_dis_number_pic(96,126,"H",RED,1,gImage_systemPic);
		LCD_dis(0,210,"确认(*)",LIGHTBLUE,0,0xff,16);
		LCD_dis(180,210,"取消(#)",LIGHTBLUE,0,0xff,16);
		
		
		ad_flag = 0;
	}
	
	//确认
	if(key=='*')
	{
		page_mode = 1;       //回主界面
		ad_flag = 1;
	}
	//返回
	else if(key=='#')
	{
		page_mode = 2;       //上一级菜单
		ad_flag = 1;
	}
}






























