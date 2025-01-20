#include "main.h"

/**
 * @brief 指纹模块的初始化
 * 
 */
void mg200_init(void){
    //对应串口的初始化
    mg200_usart6Init(115200);
    /*MG200_IO*/
    //1. 打开外设的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
    //PC0 通用输入
        GPIO_InitTypeDef  GPIO_InitStruct = {0};
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
        GPIO_Init(GPIOC, &GPIO_InitStruct);

    //PC1 通用推挽输出
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
        GPIO_Init(GPIOC, &GPIO_InitStruct);
        //默认关闭电源
        GPIO_ResetBits(GPIOC, GPIO_Pin_1);
    //节省电，就使用感应上电方式，触碰之后亮灯
    MG200_PWR_RESET();
    //感应上电方式会不断进入中断(接收中断和空闲中断)
    //MG200_PWR_SET();//上电可能也会发一些数据导致进入中断
    //printf("指纹模块初始化完成\r\n");
}

/**
 * @brief mg200发送指令函数,mg要求的数据传输格式
 * 为了避免轮询，接收数据写在中断里
 */
void mg200_send_command(u8 command,u8 parameter){
    //发送7个字节数据给mg200
    u8 data[7] = {MG200_START_CODE, MG200_SEND_ADDR, MG200_RECV_ADDR, 0x00, 0x00, 0x00, 0x00};
    //指令码
    data[3] = command;
    //根据指令码的不同，可能的参数
    data[4] = parameter;
    //校验和
    u8 temp = (0x63 + 0x62 + command + parameter) & 0xff;
    data[6] = temp;
    USART6_SendBytes(data,7);
    //printf("7个字节数据发送成功\r\n");
    //防止上电会发送数据给MCU意外导致标志位提前置1（逆天bug）
	mg200_rec_flag = 0;//使其可以感应数据接收完成
}


/***********************************************
*函数名    ：mg200_read_cmd
*函数功能  ：解析中断中接收到的数据包
*函数参数  ：u8 cmd,u8 *parameter,u8 *result
*函数返回值：u8   返回值标志通信是否有问题
*函数描述  ：cmd 之前发送的指令  要接收到的应答码对应
             *parameter    接收到的命令参数
             *result       接收到命令执行的效果
说明一开始的担忧还是对的，数据包没有接收完成，判断逻辑已经执行完了。。。
***********************************************/
u8 mg200_read_cmd(u8 cmd,u8 *parameter,u8 *result)
{

	/*等待指令数据包在中断中接收完成*/
	while(!mg200_rec_flag);
	
	mg200_rec_flag = 0;               //标志位清零
	/*验证接收到的数据包*/
	if(mg200_buff[0] != 0x6c)   return 1;
	if(mg200_buff[1] != 0x62)   return 2;
	if(mg200_buff[2] != 0x63)   return 3;
	if(mg200_buff[3] != cmd)    return 4;
	
	*result = mg200_buff[4];               //获取到执行效果数据
	*parameter = mg200_buff[5];            //获取到返回的参数
	//if(mg200_rec_buff[6] != 0x00)   return 5;//预留位有时候会用到，不一定是0x00  获取版本号
	
	u8 check_sum = (0x62+0x63+cmd+*result+*parameter + mg200_buff[6]) & 0xff;
	if(mg200_buff[7] != check_sum) return 6;
	
	return 0;
}


/**
 * @brief 抓取指纹图像和提取特征点函数(读取指纹模块上的手指信息)
 * 该指令在用户注册和指纹比对时被使用；指纹比对时，执行该指令 1 次，对应的参数为 00h；
 * 还是想简单了。。。
 * 参数是第几次采集，返回0 采集表示成功
 */
u8 mg200_ReadFingerprint(u8 parameter){
    u8 parame,result;
	u8 err_val;
	//发送采集指令
	mg200_send_command(0X51,parameter);
	
	//解析接收到的数据包，这里有一个等待，必须等8个字节过来。不然只响应几个字节数据就执行判断逻辑了
    //8个字节数据，应该感受不到延迟
	err_val = mg200_read_cmd(0x51,&parame,&result);
		
	if(err_val == 0)   //通信成功，代表响应成功，收到8个字节数据包
	{
		switch(result)
		{
			case 0x00: 	printf("抓取指纹图像及特征量提取成功\r\n");break;
			case 0xb1:  printf("指纹过小(small finger)\r\n");break;
			case 0xb2:  printf("无指纹 (no finger)\r\n");break;
			case 0xb3: 	printf("指纹位置过于靠左，需要将手指靠右边摁按(left finger)\r\n");break;
			case 0xb4:  printf("指纹位置过于靠右，需要将手指靠左边摁按(right finger)\r\n");break;
			case 0xb5:  printf("指纹位置过于靠上，需要将手指靠下边摁按(up finger)\r\n");break;
			case 0xb6: 	printf("指纹位置过于靠下，需要将手指靠上边摁按(down finger)\r\n");break;
			case 0xb7:  printf("湿手指 （指纹表面出汗或者水渍过多） (wet finger)\r\n");break;
			case 0xb8:  printf("干燥手指(dry finger)\r\n");break;
			case 0xc0: 	printf("特征提取错误\r\n");break;
			case 0xc2: 	printf("特征提取错误 (图像质量差)\r\n");break;
			default :   printf("抓取指纹图像及特征量提取错误\r\n");break;
		}

	}
	else
	{
		printf("通信失败！\r\n");
		
		return 0xff;
	}
		
	return result;
}



/**
 * @brief 指纹模块的注册函数
 * mg200_data_flag是为了避免CPU太快而模块的应答还没有返回，所以需要一个标志位来判断是否已经收到数据包
 * 感觉没有必要，因为一直发送指令，你也不知道应答是那一次的，但是有一次返回成功即可，都在第一次采集的定义内
 * 比如当某一条成功应答数据写入数组时，因为前面while判断不成功，一直发指令。当判断到这一条时，就直接跳出循环。
 * 这个应答不要求精确那一条，只要返回成功就算做成功采集一次。
 * 高速轮询下，避免等待操作，使用标志位代替
 * /***********************************************
*函数名    ：Enroll
*函数功能  ：注册新用户的指纹
*函数参数  ：u8 ID
*函数返回值：u8 
*函数描述  ：
            返回0     表示注册成功
            返回0xfc  表示传感器异常 注册失败
            返回0xff  表示注册时候，通信失败导致注册失败
            返回0xfb  用户进入到指纹注册界面但是没有按下指纹或者没有采集超过三次
***********************************************/
u8 mg200_register(u8 ID){
    //接收指令操作返回的结果
    u8 ret;
    //触摸标志位变量
    static u8 touch_statu = 0;

    /*串口层提示开始注册，感应上电方式*/
	if(touch_statu == 0)
	{
		printf("请将手指放到指纹模块开始注册指纹\r\n");
		touch_statu = 1;  	
		printf("第一次指纹采集\r\n");
	}


    /*采集指纹*/
        //第一次采集
        if(MG200_DETECT_READ() && touch_statu==1)
        {
            //感应上电
            MG200_PWR_SET();
            //等待感应上电完成
            tim5Delay_Ms(30);
            //超时检测清零
            tim9_count[3] = 0;
            //指纹变黄
            fingerprint_yellow();

            do
            {
                //第一次采集的参数为0x00
                ret = mg200_ReadFingerprint(0x00);

                //超时检测
                if(tim9_count[3]>=5000)
                {
                    printf("传感器异常，采集失败，请重新注册\r\n");
                    touch_statu = 0;     //重新进入注册功能标志位
                    MG200_PWR_RESET();
                    return 0xfc;
                }
            }while(ret);//除开0x00为成功之外，其他非零返回都为采集失败，ret为0，不会进入循环
            //触摸标志位锁住，高速轮询下只要成功采集一次算作第一次即可停止
            touch_statu = 0xff;
            printf("请抬起手指，再次采集下一次指纹\r\n");
            //语音播报
            voice(DELETE_ASSIGNFIGNER);
        }
        //等用户抬起而且第一次采集成功后，再次触摸时为第二次采集,
        if(MG200_DETECT_READ()!=1 && touch_statu==0xff)
        {
            //指纹恢复
            fingerprint_lgray();
            //第二次采集的标志
            touch_statu = 2;
            printf("第二次指纹采集\r\n");
        }

        //第二次采集
        if (MG200_DETECT_READ() && touch_statu==2)
        {
            tim9_count[3]=0;                //超时检测时间清零
            //指纹变黄
            fingerprint_yellow();
            do
            {
                ret = mg200_ReadFingerprint(0x01);
                //超时检测
                if(tim9_count[3]>=5000)
                {
                    printf("传感器异常，采集失败，请重新注册\r\n");
                    touch_statu = 0;     //重新进入注册功能标志位
                    MG200_PWR_RESET();
                    return 0xfc;
                }
            }while(ret);	
            //只要成功一次就可以锁住了，抬起再解锁做第三次采集
            touch_statu = 0xfe;
            printf("请抬起手指，再次采集下一次指纹\r\n");
            //语音播报
            voice(DELETE_ASSIGNFIGNER);
        }
        if(MG200_DETECT_READ()!=1 && touch_statu==0xfe)
        {
            //指纹恢复
            fingerprint_lgray();
            touch_statu = 3;
            printf("第三次指纹采集\r\n");
        }


        //第三次采集
        if (MG200_DETECT_READ() && touch_statu==3)
        {
            tim9_count[3]=0;                //超时检测时间清零
            //指纹变黄
            fingerprint_yellow();
            do
            {
                ret = mg200_ReadFingerprint(0x02);
                //超时检测
                if(tim9_count[3]>=5000)
                {
                    printf("传感器异常，采集失败，请重新注册\r\n");
                    touch_statu = 0;     //重新进入注册功能标志位
                    MG200_PWR_RESET();
                    return 0xfc;
                }
            }while(ret);	
            //最后一次只要成功直接注册不用抬起
            touch_statu = 4;
            //语音播报
            voice(DELETE_ASSIGNFIGNER);
            printf("采集指纹成功，正在注册\r\n");
        }


    /*注册*/
        u8 para,resu;//接收响应的参数和结果
        if(touch_statu==4)
        {   
            //把标志位恢复为初始状态以便下一次注册使用
            touch_statu = 0;

            //指纹不用恢复因为切换界面时会重新打印
            //注册指纹,参数不变就不用封装了，直接发送注册指令
            mg200_send_command(0x7f,ID);//指定ID注册
            //解析响应的数据包
            if(mg200_read_cmd(0x7f,&para,&resu) != 0)   //通信失败，返回非零值就是响应数据有错误
            {
                printf("通信失败导致注册失败！如需重新注册，请再次进入注册功能！\r\n");
                //上电后，三次采集中只要有错误，需要重新上电重第一次采集开始
                MG200_PWR_RESET();
                return 0xff;
            }
            else
            {
                //判断命令操作结果
                switch(resu)
                {
                    case 0x00: printf("注册成功\r\n");break;
                    case 0x83: printf("ID 错误(ID < 0 或者 ID > 最大用户数)或者通信错误\r\n");break;
                    case 0x91: printf("注册失败(用户区域已满)\r\n");break;
                    case 0x93: printf("已经注册的 ID\r\n");break;
                    case 0x94: printf("指纹提取次数 < 3\r\n");break;
                }
            }
            //无论操作是否成功，都关电
            MG200_PWR_RESET() ;
            
        }   return resu;
        
    //用户进入到指纹注册界面但是没有按下指纹或者没有采集超过三次
    return 0xfb;
}






