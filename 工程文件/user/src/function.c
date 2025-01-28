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
	//密码开锁方式
		//进入管理员界面的标志（只有密码开锁方式可以进入到管理员界面，其他只有开门功能）
		u8 sta;
		//在主界面密码开锁后,才进入管理员界面使用键值访问各界面
		sta = open_passward(key);
		if(sta == 1){
			//不在函数中层层调用进入下一个页面，在while中不断通过标志位轮询跳过对应界面即可
			page_mode = 2;
		}
	//指纹开锁方式（低功耗模式下指纹模块被高速轮询）
	open_fingerprint();
	//卡片开锁方式
	open_card();



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


void open_fingerprint(void){
	//是否开门成功的标志位
	static u8 open_door_flag = 0;
	//用来接收操作返回值
    u8 rec_parameter,ret = 0xfe;
	//指纹模块的触摸状态
	static u8 finger_touch_sta = 0;

	//高速轮询指纹模块的触摸状态（低功耗模式下）
	if(MG200_DETECT_READ() && finger_touch_sta == 0){
		//防止一次触摸多次触发
		finger_touch_sta = 1;

		//打开正常功能
		MG200_PWR_SET();
		tim5Delay_Ms(30);
		//采集指纹数据
			//超时检测清零
			tim9_count[3] = 0;
			//指纹变黄
        	fingerprint_yellow();
			do
			{
				//第一次采集的参数为0x00
				ret = mg200_ReadFingerprint(0x00);

				//超时检测,采集失败
				if(tim9_count[3]>=5000)
				{
					printf("传感器异常，采集失败，请重试\r\n");
					MG200_PWR_RESET();
					//指纹恢复
        			fingerprint_lgray();
					//松开手指语音播报
					voice(DOOROPEN_FAIL);
					//状态解开
					finger_touch_sta = 0;
					return;
				}
			}while(ret!=0x00);//除开0x00为成功之外，其他非零返回都为采集失败，ret为0，不会进入循环

		//指纹采集之后开始比对
        mg200_send_command(0x71,0x00);
        //等待并解析响应的数据包
        if(mg200_read_cmd(0x71,&rec_parameter,&ret))
        {
            //非零返回值代表数据包有错误
            printf("通信失败,接收数据包错误\r\n");
            MG200_PWR_RESET();
			//指纹恢复
			fingerprint_lgray();
			//松开手指语音播报
			voice(DOOROPEN_FAIL);
			//状态解开
			finger_touch_sta = 0;
            return;
        }

        //数据包没有错误的情况下，判断返回的操作结果
        switch(ret)
        {
            case 0x00:printf("指纹对比成功:%d\r\n",rec_parameter);break;
            default:printf("该指纹不存在\r\n");break;
        }

		if(ret==0x00){
			//判断指纹对比结果，如果成功，则打开门,直接推门进入
			door_open();
			//开门状态标志位置1
			open_door_flag = 1;
			//语音播报
			voice(DOOROPEN_SUCCESS);
			//开始计时
			tim9_count[4] = 0;
			//5s之后自动上锁
		}else{
			//指纹对比失败，松开手指语音播报
			voice(DOOROPEN_FAIL);
			//状态解开
			finger_touch_sta = 0;
		}
	}
	


	//松开指纹颜色恢复
	if(MG200_DETECT_READ()==0 && finger_touch_sta==1){
		//指纹恢复
		fingerprint_lgray();
		//一次采集对比完成
		finger_touch_sta = 0;
		//关电源
		MG200_PWR_RESET();
	}

	//在成功开门的情况下
	if(open_door_flag==1){
		//单独判断指纹开锁后的自动关门逻辑
		if(tim9_count[4]>=5000){
			printf("close_door");
			door_close();
			//把标志位置0
			open_door_flag = 0;
		}
	}
}


void open_card(void){
	//不进入卡片管理界面的话，二维数组数据不会读取，所以这里单独读取一次
	static u8 ad_flag = 1;
	u8 sta;
	u8 picc_i,picc_j;
	//此卡片是否注册的标志
	u8 register_flag = 0;


	//执行一次
	if(ad_flag)
	{
		//从AT24中读出卡片ID数据到二维数组中
		AT24C0x_read_bytes(31,sizeof(picc),picc);
		//进入这个页面后只读取一次，开门成功后才恢复
		ad_flag = 0;
	}




	//如果全部拿到则开始开锁流程
	sta = ReadCardData(62,picc_read_data,picc_id);
	if(sta == MI_OK){
		//先遍历二维数据，检查此卡片有没有在本机注册过
		for(picc_i=0;picc_i<9;picc_i++)
		{
			//序列号对比成功的字节数量
			u8 temp_count = 0;
			for(picc_j=0;picc_j<4;picc_j++)
			{
				//先判断此卡片有没有在本机注册过
				if(picc[picc_i][picc_j] == picc_id[picc_j])
				{
					//卡片的四个字节序号全部相等，说明已经注册过，跳出循环
					temp_count++;
				}
			}

			//如果四个字节都相等，说明已经注册过，跳出循环
			if(temp_count == 4)
			{
				//跳出循环,不用找了,后续步骤都不用执行了
				register_flag = 1;
				printf("此卡片已经注册! ID为:%d\r\n",picc_i+1);
				break;
			}
		}

		//已经对比了卡片序列号，此卡片已经注册过，接下来根据块内数据判断是否有开门权限
		if(register_flag == 1){
			for(int i=0;i<16;i++){
				//如果有一个字节数据不相等的话，就是没有权限
				if(picc_read_data[i] != picc_data[i])
				{
					//跳出循环,不用找了,后续步骤都不用执行了
					printf("此卡片没有权限!");
					voice(DOOROPEN_FAIL);
					//直接结束程序，重新进入寻卡步骤
					return;
				}
			}
			//经过循环没有跳出，就代表有权限
			door_open();
			//语音播报
			voice(DOOROPEN_SUCCESS);
			//开门成功后，把状态恢复，以便下一次进入时会读取最新的注册数据
			ad_flag = 1;
		}else {
			//没有注册还是开门失败
			printf("此卡片没有注册!");
			voice(DOOROPEN_FAIL);
		}
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
	//显示坐标,注册成功之后需要老坐标定位
	static u16 x = 30;
	u8 mg200_i;
	//要录入的指纹ID
	static u8 mg200_ID;
	
	//执行一次
	if(ad_flag)
	{
		//语音播报(在采集里面写最好，超时可以使用)
		//voice(REGISTER_FINGER);
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
			//重新刷新页面，显示新的有效ID
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
	//指纹删除的操作状态
	static mg200_erase_sta = 1;
	//显示坐标,删除完成后，直接刷新页面打印新的有效ID，不需要老坐标
	u16 x = 30;
	u8 mg200_i;
	//要删除的指纹ID
	u8 mg200_ID;
	
	
	//执行一次
	if(ad_flag)
	{
		//语音播报
		voice(DELETE_ASSIGN_FINGER);
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
		
		ad_flag = 0;
	}

	//删除指定ID指纹，key为有效值才执行删除函数
	if(key != 0xff && key != '#' && key != '*'){
		//键值与ID的转换
		mg200_erase_sta = EraseOne(key-48);
		//删除成功，本机数据也要删除
		if(mg200_erase_sta == 0x00)
		{
			//ID和下标相差1,恢复默认值
			mg200_id[key-48-1] = 0xff;
			//把数组重新写入AT24
			AT24C0x_write_bytes(21,sizeof(mg200_id),mg200_id,AT24C04);
			//语言播报
			voice(SETTING_SUCCESS);

			//释放状态锁，重新刷新当前页面，打印新的有效ID
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
		//语音播报
		voice(DELETE_ALLFINGER);
		//串口上位机显示
		printf("\r\n删除所有指纹?:\r\n");
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
			//清除指纹模块
			erase_all();
			//把本机指纹数据覆盖为无效值0xff
			memset(mg200_id,0xff,sizeof(mg200_id));
			AT24C0x_write_bytes(21,sizeof(mg200_id),mg200_id,AT24C04);
			printf("已删除所有指纹\r\n");
			//语音播报
			voice(SETTING_SUCCESS);
		
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
	//显示坐标
	static u16 x = 30;
	static u8 mg200_i = 0;
	//要删除的指纹ID列表，初始化无效ID为0
	static u8 mg200_ID[9] = {0};

	
	//执行一次
	if(ad_flag == 1)
	{
		//语音播报(在指纹采集时播报更好，跟超时机制一起用)
		//voice(DELETE_ASSIGNFIGNER);
		//串口上位机显示
		printf("\r\n识别删除指纹界面\r\n");
		printf("识别指纹\r\n");
		printf("【*】确认\r\n");
		printf("【#】取消\r\n");
		
		//LCD屏幕显示
		LCD_dis_pic(0,0,gImage_systemPic);//设置图片为背景
		LCD_dis(25,48,(u8 *)"识别删除指纹",LIGHTBLUE,0,0xff,32);
		LCD_dis_number_pic(96,120,(u8 *)"Z",LGRAY,1,gImage_systemPic);
		LCD_dis(160,220,"【#】返回",LIGHTBLUE,0,0xff,16);
		LCD_dis(0,220,"确认【*】",LIGHTBLUE,0,0xff,16);

		//从AT24中读出指纹ID数据到数组中
		AT24C0x_read_bytes(21,sizeof(mg200_id),mg200_id);
		//每次进来刷新一下坐标
		x = 30;
		//识别的指纹个数也要重置
		mg200_i = 0;
		//把列表重新初始化无效值，等待重新识别
		for(int l=0;l<9;l++){
			mg200_ID[l] = 0;
		}

		ad_flag = 0;
	}


	//高速轮询指纹对比函数
	//指纹对比成时,识别成功的指纹到达最大限制就不识别了
	if(Match_l_n(&mg200_ID[mg200_i])==0x00 && mg200_i<9){
		//存储识别成功的ID列表位置往后移动
		mg200_i++;
		//显示对比成功的ID编号,数字转字符
		LCD_dis_ch(x,80,mg200_ID[mg200_i-1]+48,LIGHTBLUE,0,0xff,32);
		x+=20;
	}

	//识别数量要限制，因为本机上只有9个指纹，但是mg200可能不止9个
	if(mg200_i == 9){
		//让他显示一次，等删除操作后会重置
		mg200_i++;
		printf("指纹识别个数达到上限,请先删除已经识别指纹ID\r\n");
	}
	
	
	
	
	/*按键确认*/
	if(key == '*' )     //确认删除 后 回到上级菜单
	{
		//创建一个数组记录已删除的ID,初始化为0,这是为了保证删除的时候，不会重复删除，只在删除时使用
		u8 deleted_ids[9] = {0};  //最多有9个指纹ID被删除
		u8 deleted_count = 0;
		//本机和模块数据双删除
			//模块,从0遍历到mg200_i，但是多加了一个
			for(int j=0;j<mg200_i;j++){
				//检查当前ID是否已经被删除过,下一个识别成功的ID检测时，标志会重置
				u8 already_deleted = 0;
				for(int i = 0; i < 9; i++) {
					if(deleted_ids[i] == mg200_ID[j]) {
						//这个ID已经被删除过了
						already_deleted = 1;
						//跳出这个校验
						break;
					}
				}

				//如果ID没有被删除过,则进行删除操作
				if(!already_deleted){
					//访问列表中的指定ID并删除
					EraseOne(mg200_ID[j]);

					//遍历本地ID数据进行删除
					for(int k=0;k<9;k++){
						if(mg200_id[k] == mg200_ID[j]){
							//把本机数据覆盖为无效值0xff
							mg200_id[k] = 0xff;
						}
					}

					//记录已删除的ID
            		deleted_ids[deleted_count] = mg200_ID[j];
					deleted_count++;
				}
			}
		//把数组重新写入AT24
		AT24C0x_write_bytes(21,sizeof(mg200_id),mg200_id,AT24C04);
		//语音播报
		voice(SETTING_SUCCESS);

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
	//首次进入卡片管理界面标志
	u8 open_flag;
	
	//执行一次
	if(ad_flag)
	{
		//串口上位机显示
		printf("\r\n卡片管理界面:\r\n");
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
		
		
		//判断是不是第一次进入卡片管理界面，决定是否将9个卡片的UID二维数组的初始值写入到AT24C04中
		//10号地址存首次开机标志，20号地址存首次打开指纹管理界面，21~29号地址存指纹ID信息
		//首次进入卡片管理界面标志可以存在30号地址，后续地址存卡片UID信息二维数组[9][4]
		at24c0x_read_byte(30,&open_flag);
		if(open_flag != OPEN_FLAG)
		{
			//把原始数组存储到AT24中,使用0xff作为初始无效值
			memset(picc,0xff,sizeof(picc));
			AT24C0x_write_bytes(31,sizeof(picc),picc,AT24C04);
			//把第一次进入标志位写入到AT(板子上是AT24C04)
			at24c0x_write_byte(30,OPEN_FLAG);
		}

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
	u8 picc_i,picc_j;
	//打印卡片ID编号坐标
	static u16 x;
	//用户已经使用ID数量
	static u8 picc_count = 0;
	u8 sta;
	//是否已经注册的标识位
	u8 login_flag = 1;

	
	//执行一次
	if(ad_flag)
	{
		//串口上位机显示
		printf("\r\n注册用户卡片界面:\r\n");
		printf("【#】回到上一级菜单\r\n");
		printf("【*】回到主界面\r\n");
		
		
		//LCD屏幕显示
		LCD_dis_pic(0,0,gImage_systemPic);
		LCD_dis(24,48,"注册用户卡片",LIGHTBLUE,0,0xff,32);
		LCD_dis_number_pic(50,120,"P",LGRAY,1,gImage_systemPic);
		LCD_dis_number_pic(130,120,"Q",LGRAY,1,gImage_systemPic);
		LCD_dis(0,220,"【*】主界面",LIGHTBLUE,0,0xff,16);
		LCD_dis(150,220,"上一级【#】",LIGHTBLUE,0,0xff,16);


		//从AT24中读出卡片ID数据到二维数组中
		AT24C0x_read_bytes(31,sizeof(picc),picc);
		x = 30;
		picc_count = 0;
		//遍历二维数组取出有效卡片id，打印已经注册的卡片ID编号
		for(picc_i=0;picc_i<9;picc_i++)
		{
			for(picc_j=0;picc_j<4;picc_j++)
			{
				//只要有卡片UID数据，那么对应外层9个元素中的四个小元素不可能全是0xff
				//默认是没有赋值是0xff,遍历每一个用户ID下的卡片UID，四个字节只要有一个不是0xff,就代表这个用户ID有卡片了
				//把这个用户ID打出来
				if(picc[picc_i][picc_j] != 0xff)
				{	
					//显示卡片ID编号1~9，图片显示需要转换一下，不然显示的是0~8
					LCD_dis_ch(x,80,picc_i+48+1,LIGHTBLUE,0,0xff,32);
					picc_count++;
					x+=20;
					break;
				}
			}
		}
		//已经存在9个卡片（后自增多加了一个数），则提示
		if(picc_count == 9)
		{
			LCD_dis(30,170,"注册已达上限",RED,0,0xff,32);
			
		}else {
			//每一次刷新进来，没有超过数量就播报
			voice(PUTCARD);
		}

		ad_flag = 0;
	}


	//如果还可以注册，就找到要注册卡片ID数据对应的二维数组下标
	if(picc_count<9)
	{
		//picc_id的值由函数传递出来，就是读卡拿到的卡片序列号，这个必须在前面，不然拿不到卡片序列号
		sta = WriteCardData(62,picc_data,picc_id);
		if(sta == MI_OK)
		{
			//先遍历二维数据，检查此卡片有没有在本机注册过，如果已经注册过，就不用再注册了
			for(picc_i=0;picc_i<9;picc_i++)
			{
				//序列号对比成功的字节数量
				u8 temp_count = 0;
				for(picc_j=0;picc_j<4;picc_j++)
				{
					//先判断此卡片有没有在本机注册过
					if(picc[picc_i][picc_j] == picc_id[picc_j])
					{
						//卡片的四个字节序号全部相等，说明已经注册过，跳出循环
						temp_count++;
					}
				}

				//如果四个字节都相等，说明已经注册过，跳出循环
				if(temp_count == 4)
				{
					//跳出循环,不用找了,后续步骤都不用执行了
					login_flag = 0;
					printf("此卡片已经注册！卡片块内数据已经写入，但是本系统没有记录\r\n");
					voice(CARD_REGISTERED);
					break;
				}
			}

			if(login_flag == 1){
				//计算出可以用的ID下标
				for(picc_i=0;picc_i<9;picc_i++)
				{
					for(picc_j=0;picc_j<4;picc_j++)
					{	
						//如果有一个不是0xff就代表这个ID空间被使用了
						if(picc[picc_i][picc_j] != 0xff)
						{
							//跳出内层循环进入下一个循环
							break;
						}
					}
					//一个空间四个小元素都是0xff的话就代表可以使用，跳出循环找到一个可用空间
					if(picc_j == 4)
					{
						//跳出外层循环
						break;
					}
					
				}

				//picc_i就是要存储的元素下标，没有定义在循环中，所以下标可以用
				//注册完了再一次进入注册函数时，下标变量会重新定义
				for(picc_j=0;picc_j<4;picc_j++)
				{
					//读卡拿到的四个字节的卡片序列号，存储到数组中
					picc[picc_i][picc_j] = picc_id[picc_j];
				}

				//如果注册数量达到上限，会在重新刷入页面时提示

				//将该用户的卡片序号存入AT24C0x
				AT24C0x_write_bytes(31,sizeof(picc),picc,AT24C04);
				voice(SETTING_SUCCESS);
				//重新刷新该界面打印新用户ID编号
				ad_flag = 1;
			}
		}
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
*函数描述  :page_mode = 52,所谓删除就是把本机记录删除即可，因为开门对比需要本机记录
************************************************/
void erase_user_picc_page(u8 key)
{
	static u8 ad_flag = 1;
	u8 picc_i,picc_j;
	//打印卡片ID编号坐标
	static u16 x;
	//用户已经使用ID数量
	static u8 picc_count = 0;


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
		voice(DELETE_ASSIGN_CARD);


		//从AT24中读出卡片ID数据到二维数组中
		AT24C0x_read_bytes(31,sizeof(picc),picc);
		x = 30;
		picc_count = 0;


		//遍历二维数组取出有效卡片id，打印已经注册的卡片ID编号
		for(picc_i=0;picc_i<9;picc_i++)
		{
			for(picc_j=0;picc_j<4;picc_j++)
			{
				//只要有卡片UID数据，那么对应外层9个元素中的四个小元素不可能全是0xff
				//默认是没有赋值是0xff,遍历每一个用户ID下的卡片UID，四个字节只要有一个不是0xff,就代表这个用户ID有卡片了
				//把这个用户ID打出来
				if(picc[picc_i][picc_j] != 0xff)
				{	
					//显示卡片ID编号1~9，图片显示需要转换一下，不然显示的是0~8
					LCD_dis_ch(x,80,picc_i+48+1,LIGHTBLUE,0,0xff,32);
					picc_count++;
					x+=20;
					break;
				}
			}
		}
		
		ad_flag = 0;
	}

	//能删除的前提是有用户卡片注册记录
	if(picc_count>0){
		//删除指定ID用户的卡片，key为有效值才执行删除函数
		if(key != 0xff && key != '#' && key != '*'){
			//按键值与用户ID存储位置的转换，把某一个用户ID下的卡片记录删除
			for(int i=0;i<4;i++){
				picc[key-48-1][i] = 0xff;
			}
			//再存入AT24C0x中
			AT24C0x_write_bytes(31,sizeof(picc),picc,AT24C04);
			voice(SETTING_SUCCESS);
			picc_count--;

			//刷新页面打印新的有效ID
			ad_flag = 1;
		}

	}else {
		printf("没有用户注册记录,请先注册卡片\r\n");
		LCD_dis(30,170,"请先注册卡片",RED,0,0xff,32);
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
		LCD_dis(56,90,"确认删除",RED,0,0xff,32);
		LCD_dis_number_pic(96,126,"?",RED,1,gImage_systemPic);
		LCD_dis(0,220,"确认(*)",LIGHTBLUE,0,0xff,16);
		LCD_dis(180,220,"取消(#)",LIGHTBLUE,0,0xff,16);
		voice(DELETE_ALL_CARD);

		
		ad_flag = 0;
	}
	
	
	
	
	if(key == '*')     //确认删除 后 回到上级菜单
	{
		//删除的动作,就是把本机记录全部清除即可
		memset(picc,0xff,sizeof(picc));
		//再存入AT24C0x中
		AT24C0x_write_bytes(31,sizeof(picc),picc,AT24C04);
		voice(SETTING_SUCCESS);
		
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
	//打印卡片ID编号坐标
	static u8 x;
	//寻卡操作的状态
	u8 sta;
	u8 picc_i,picc_j;
	//用户已经识别成功的ID数量
	static u8 picc_count = 0;
	//记录已经识别的卡片ID
	static u8 picc_idNum[9] = {0};
	//是否已经达到识别上限的标识位
	u8 match_flag = 1;
	//是否已经存入识别列表标志
	u8 sava_flag = 1;
	//序列号对比成功的字节数量
	u8 temp_count = 0;

	
	//执行一次
	if(ad_flag)
	{
		//串口上位机显示
		printf("\r\n识别删除卡片界面\r\n");
		printf("识别卡片\r\n");
		printf("确认(*)\r\n");
		printf("返回(#)\r\n");
		
		//LCD屏幕显示
		LCD_dis_pic(0,0,gImage_systemPic);//设置图片为背景
		LCD_dis(25,48,(u8 *)"识别删除卡片",LIGHTBLUE,0,0xff,32);
		LCD_dis_number_pic(50,120,"P",LGRAY,1,gImage_systemPic);
		LCD_dis_number_pic(130,120,"Q",LGRAY,1,gImage_systemPic);
		LCD_dis(0,220,"确认(*)",LIGHTBLUE,0,0xff,16);
		LCD_dis(180,220,(u8 *)"返回(#)",LIGHTBLUE,0,0xff,16);
		voice(PUTCARD);
		

		//从AT24中读出卡片ID数据到二维数组中
		AT24C0x_read_bytes(31,sizeof(picc),picc);
		x = 30;
		picc_count = 0;
		//把识别列表重新初始化无效值，等待重新识别
		//识别到一半出去了，再进来需要重新识别
		for(int l=0;l<9;l++){
			picc_idNum[l] = 0;
		}

		ad_flag = 0;
	}


	//识别成功的数量已经达到上限9个，不能再识别
	if(picc_count==9){
		printf("识别已达上限,请先删除\r\n");
		match_flag = 0;
	}

	if(match_flag==1){
		sta = MatchCard(picc_id);
		if(sta == MI_OK){
			//此时已经找到卡片并已经返回了序列号，开始遍历二维数组查询用户对应的卡片ID
			for(picc_i=0;picc_i<9;picc_i++)
			{
				//序列号对比成功的字节数量
				temp_count = 0;
				for(picc_j=0;picc_j<4;picc_j++)
				{
					//先判断此卡片
					if(picc[picc_i][picc_j] == picc_id[picc_j])
					{
						//卡片的四个字节序号全部相等，说明已经注册过
						temp_count++;
					}
				}

				//如果四个字节都相等，说明已经注册过，跳出循环
				if(temp_count == 4)
				{
					//跳出循环,不用找了,后续步骤都不用执行了
					printf("此卡片的注册ID为%d\r\n",picc_i+1);
					break;
				}
			}
			//此卡片注册过才显示
			if(temp_count == 4){
				//先判断之前有没有存过到识别列表中，如果之前有，就不用再存了
				//无效值也不会符合条件
				for(int i=0;i<sizeof(picc_idNum);i++){
					if(picc_idNum[i] == (picc_i+1)){
						sava_flag = 0;
					}
				}
				//之前没有存过，才存进去
				if(sava_flag == 1){
					//屏幕显示找到的卡片注册的ID
					//显示卡片ID编号1~9，图片显示需要转换一下，不然显示的是0~8
					LCD_dis_ch(x,80,picc_i+48+1,LIGHTBLUE,0,0xff,32);
					//识别数量+1,存入临时缓冲区,把ID存入进去
					picc_idNum[picc_count] = picc_i+1;
					picc_count++;
					x+=20;
					voice(Di);
				}
			}else {
				printf("此卡片未注册,请先注册\r\n");
			}
			
		}
	}
	
	

	
	if(key == '*')     //确认删除 后 回到上级菜单
	{
		//上面存入识别列表时已经做了反重复，这里没必要，但我不想改了，累
		//删除的动作,从缓冲区拿到所有已经识别到的id，然后删除二维数组中的数据，再存入AT24C0x中
		//创建一个数组记录已删除的ID,初始化为0,这是为了保证删除的时候，不会重复删除，只在删除时使用
		u8 deleted_ids[9] = {0};  //最多有9个卡片ID被删除1~9
		u8 deleted_count = 0;
		//遍历缓冲区识别成功的卡片ID，去二维数组相应位置删除数据
		for(int j=0;j<9;j++){
			//检查当前ID是否已经被删除过,下一个识别成功的ID检测时，标志会重置
			u8 already_deleted = 0;
			for(int i = 0; i < 9; i++) {
				//假设没有识别直接按删除，那么都相等，也不会删除
				//识别数组没有满，余下的ID都为0，也不会删除
				if(deleted_ids[i] == picc_idNum[j]) {
					//这个ID已经被删除过了
					already_deleted = 1;
					//跳出这个校验
					break;
				}
			}

			//如果ID没有被删除过,则进行删除操作
			if(!already_deleted){
				//如果没有被删除过,去二维数组中对应位置置无效值
				for(int i=0;i<4;i++){
					picc[picc_idNum[j]-1][i] = 0xff;
				}

				//记录已删除的ID
				deleted_ids[deleted_count] = picc_idNum[j];
				deleted_count++;
			}
		}

		//二维数组全部修改好后再存入AT24C0x中
		//没有识别直接按的话，就是读出来，没有修改就存入
		AT24C0x_write_bytes(31,sizeof(picc),picc,AT24C04);
		voice(SETTING_SUCCESS);
		

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






























