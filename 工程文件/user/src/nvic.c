#include "main.h"



/**
 * @brief 串口1中断服务函数
 * 
 */
void USART1_IRQHandler(void){
    u8 data;
    //宏定义不能改动，所以另外赋值接收起始地址
    static u32 zk_addr_val = ZK_ADDR;
    //大规模数据传输，有波动，利用空闲中断判断是否传输完成有误触现象
    static u32 zk_cont = 0x00141F58;


    //判断是接收中断信号触发
    if(USART_GetITStatus(USART1, USART_IT_RXNE)){
        //清除中断标志位
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
        //紧急事件(只接受不做处理),每次只能接收一个字节数据，字符串的话，就是一个字符数组

        //判断是否开启字库更新标志,否则照常接收
        if(zk_flag == 1) {
             //接收上位机传递过来的字库数据
            data = USART_ReceiveData(USART1);
            //接收到一个字节，直接使用SPI写w25q64一个字节
            w25q64_page_write(zk_addr_val,1,&data);
            //工作灯打开
            LED4_ON;
            //地址自增
            zk_addr_val++;
            zk_cont--;
            if(zk_cont == 0) {
                printf("字库烧录完成,1s后自动复位设备\r\n");
                LED4_OFF;
                //关闭字库更新标志
                zk_flag = 0;
                
                //触发看门狗复位，1s后复位，单独喂，少一点。复位后还是10秒后超时。
                //若要更改重载值， IWDG_SR 中的 RVU 位必须为 0。
                //RVU： 看门狗计数器重载值更新 (Watchdog counter reload value update)
                //可通过硬件将该位置 1 以指示重载值正在更新。当在 VDD 电压域下完成重载值更新操作后
                //（需要多达 5 个 RC 40 kHz 周期）毫秒级，会通过硬件将该位复位。
                //重载值只有在 RVU 位为 0 时才可更新。直接修改重载值可能会失败，因为没有确认上一次更新是否完成
                //IWDG_PR 和 IWDG_RLR 寄存器具有写访问保护。若要修改寄存器，必须首先对 IWDG_KR
                //寄存器写入代码 0x5555。而写入其他值则会破坏该序列，从而使寄存器访问保护再次生效。
                //这意味着重装载操作（即写入 0xAAAA）也会启动写保护功能。   
                //解除写保护
                IWDG->KR = 0x5555;
                while(IWDG->SR & (1U<<1));    // 等待之前的重载值加载完成，避免新重载值被覆盖或者错乱
                IWDG->RLR = 250;
                IWDG->KR = 0xAAAA;
                //直接软件复位不是更快,内核级别函数
	            //NVIC_SystemReset();
                while(1);
            }
        }else {
            //常规接收,写入全局数组中，供主控程序使用
            //输入数据寄存器数据有了数据之后就触发中断，注意这里不是输入移位寄存器
            usart1.usart_buff[usart1.len++] = USART_ReceiveData(USART1);
        }
    }


    //判断是空闲中断信号触发
    if(USART_GetITStatus(USART1, USART_IT_IDLE)){
        //清除中断标志位
        //USART_ClearITPendingBit(USART1, USART_IT_IDLE);
        //只能手动清除，库函数没有写
        USART1->SR;
        USART1->DR;
        //紧急事件
        usart1.usart_buff[usart1.len] = '\0';//在外部补充结束符号，成为一个字符串
        //接收完成后，长度清零（方便下一次接收，数据已经存到数组中了，没有丢失），标志位置1，直接在主函数中处理
        usart1.len = 0;
        //外部通过标志位判断是否有新数据到达
        usart1.usart_flag = 1;
    }
}

/**
 * @brief 定时器9的中断服务函数
 * 
 */
TIM1_BRK_TIM9_IRQHandler(void){
    //led3呼吸灯标志位,不影响开关呼吸灯，用作切换呼吸灯的方向
    static u8 led3_flag = 1;
    
    //判断中断信号源
    if (TIM_GetITStatus(TIM9, TIM_IT_Update) == SET){
        //清除中断标志位
        TIM_ClearITPendingBit(TIM9, TIM_IT_Update);


        //紧急事件
        //延时使用
        tim9_count[0]--;
        //led3呼吸灯
        tim9_count[1]++;
        //获取RTC时间的时机
        tim9_count[2]++;
        //记录指纹采集时间用于超时检测
        tim9_count[3]++;
        //自动关门计时
        tim9_count[4]++;
        //周期检查WiFi的连接状态
        tim9_count[5]++;



        //led3呼吸灯
        if(tim9_count[1]>=2){
            tim9_count[1] = 0;
            if(led3_flag == 1){
                TIM4->CCR4++;
                if(TIM4->CCR4>=1000){
                    led3_flag = 0;
                }
            }else if(led3_flag == 0){
                TIM4->CCR4--;
                if(TIM4->CCR4<1){
                    led3_flag = 1;
                }
            }
        }

        //周期性检查WiFi的状态,每一分钟检查一次
        if(tim9_count[5]>=60000){
            //到时间就归零
            tim9_count[5] = 0;
            printf("周期性检查WiFi状态\r\n");
            Esp32_SendandReceive("AT+CWSTATE?\r\n",(u8 *)"OK",10000);
        }


        //自动关门程序
        if(tim9_count[4]>=5000 && autoCloseTimerFlag==1){
            //让计时无效
            autoCloseTimerFlag = 0;
            //关门程序
            door_close();
            LED4_OFF;
            //上报数据，前提是WiFi连接了，不然为了云端同步会一直超时重传！
            if(wifi_connect_flag==0){
                publish_close();
            }
            printf("自动关门\r\n");
        }
    }
}



/***********************************************
*函数名    ：RTC_Alarm_IRQHandler
*函数功能  ：rtc闹钟中断服务函数
*函数参数  ：无
*函数返回值：无
*函数描述  ：
***********************************************/
void RTC_Alarm_IRQHandler(void)
{
	//判断闹钟触发中断信号
	if(RTC_GetITStatus(RTC_IT_ALRA))
	{
		//清除标志位
		RTC_ClearITPendingBit(RTC_IT_ALRA);
		//紧急事件
		
	}
	EXTI_ClearITPendingBit(EXTI_Line17);
	
	
}


/***********************************************
*函数名    ：RTC_WKUP_IRQHandler
*函数功能  ：rtc唤醒中断服务函数
*函数参数  ：无
*函数返回值：无
*函数描述  ：
***********************************************/
void RTC_WKUP_IRQHandler(void)
{
	//判断唤醒触发中断信号
	if(RTC_GetITStatus(RTC_IT_WUT))
	{
		//清除标志位
		RTC_ClearITPendingBit(RTC_IT_WUT);
		//紧急事件
		
	}
}


/***********************************************
*函数名    ：USART6_IRQHandler
*函数功能  ：接收来自指纹模块的数据
*函数参数  ：无
*函数返回值：无
*函数描述  ：接收指纹模块传递过来的8个字节
***********************************************/
void USART6_IRQHandler(void){
    //为什么串口6的中断一直被触发
    //printf("mg200的指纹模块进入中断函数");
    static u8 i = 0;

    //判断是接收完成中断信号触发
    if(USART_GetITStatus(USART6, USART_IT_RXNE)){
        //printf("mg200的指纹模块中断函数,是接收中断触发的");
        //清除中断标志位
        USART_ClearITPendingBit(USART6, USART_IT_RXNE);
        //紧急事件(只接受不做处理),每次只能接收一个字节数据,数据包被处理了才再次接收，否则不接受
        mg200_buff[i++] = USART_ReceiveData(USART6);
    }

    //连续的数据接收完了，判断是空闲中断
    if(USART_GetITStatus(USART6,USART_IT_IDLE))
    {
        //没上电他也发送数据
        //printf("mg200的指纹模块中断函数,是空闲中断触发的");
        //请中断标志位
        USART6->SR;
        USART6->DR;
        //紧急事件
        i = 0;       //下一次接收从0号开始放
        //低功耗模式，会一直偷发数据，需要给一个标志位来标志什么时候接收的数据的有效的指纹数据
        //上电好像会发数据啊，在发送命令给mg200时把mg200_rec_flag置为0
        mg200_rec_flag = 1;     //接收指令包完成，可以解析指令包
    }
}



/*****************************************************
函数名称  : USART2_IRQHandler
函数功能  : ESP32 串口2 (USART2) 中断服务函数，用于接收数据并处理
函数形参  : void
函数返回值: void
注意事项  : 该版本针对ESP32返回数据以\r\n结尾的特性进行了优化
*****************************************************/
void USART2_IRQHandler(void)
{
    u8 temp;
    static u8 lastChar = 0; // 用于存储上一个接收到的字符

    //用于解析MQTT发过来的数据
    char *lock_status_str;
    int lock_status;

    //周期性WiFi检查的数据解析
    char *WiFi_status_str;
    int WiFi_status;

    
    // 检测是否为接收数据非空 (RXNE) 中断
    if (USART_GetITStatus(USART2, USART_IT_RXNE))
    {
        temp = USART_ReceiveData(USART2);
        
        // 检查是否到达缓冲区上限
        if (esp32rec.len >= sizeof(esp32rec.buff) - 3) // 预留\r\n\0三个字符的空间
        {
            esp32rec.len = 0; // 防止缓冲区溢出，重新开始接收
            lastChar = 0;
            return;
        }

        // 存储接收到的字符
        esp32rec.buff[esp32rec.len++] = temp;
        
        // 检查是否接收到完整的结束符 \r\n
        if (lastChar == '\r' && temp == '\n')
        {
            // 移除结束符
            esp32rec.len -= 2; // 回退两个字符（\r\n）
            
            // 添加字符串结束符
            esp32rec.buff[esp32rec.len] = '\0';
            
            // 置位接收完成标志
            esp32rec.flag = 1;
            
            // 处理接收到的数据（ESP32发送过来的数据）
            //分开从ESP32主动发过来的数据和响应单片机AT指令的数据
            //主动发的只对这两个指令做处理，其他不管
            // 只处理包含 "attributes/push" 的消息
            if (strstr((char *)esp32rec.buff, "attributes/push") != NULL)
            {
                // 查找 "lock_status" 在字符串中的位置
                lock_status_str = strstr((char *)esp32rec.buff, "\"lock_status\":");
                if (lock_status_str != NULL)
                {
                    // 找到 "lock_status": 后面紧接的值
                    lock_status_str = strchr(lock_status_str, ':');  // 找到 ':' 位置
                    if (lock_status_str != NULL)
                    {
                        lock_status_str++;  // 跳过冒号
                        lock_status = atoi(lock_status_str);  // 将字符串转换为整数
                        if(lock_status==0){
                            //关门
                            LED4_OFF;
                            //执行关门函数
                            door_close();
                        }else if(lock_status==1){
                            //开门
                            LED4_ON;
                            voice(DOOROPEN_SUCCESS);
                            door_open();
                            //自动关门计时开始
                            tim9_count[4] = 0;
                            autoCloseTimerFlag = 1;
                        }
                    }
                }
            }
            
            //周期性检查传回的WiFi状态
                // 查找 "+CWSTATE:" 在字符串中的位置
                WiFi_status_str = strstr((char *)esp32rec.buff, "+CWSTATE:");
                if (WiFi_status_str != NULL)
                {
                    // 找到 +CWSTATE:: 后面紧接的值
                    WiFi_status_str = strchr(WiFi_status_str, ':');  // 找到 ':' 位置
                    if (WiFi_status_str != NULL)
                    {
                        WiFi_status_str++;  // 跳过冒号
                        WiFi_status = atoi(WiFi_status_str);  // 将字符串转换为整数
                        if(WiFi_status==0 || WiFi_status==3 || WiFi_status==4){
                           printf("wifi断开\r\n");
                           //WiFi连接的标志，MQTT是否上报的标志
                           wifi_connect_flag = 1;
                        }else if(WiFi_status==1||WiFi_status==2){
                            if(wifi_connect_flag==1){
                                printf("wifi连接恢复,复位开启远程开锁模式\r\n");
                                //之前断开过，需要重连MQTT，马上软件复位，有网之后复位是为了连接MQTT
                                NVIC_SystemReset();
                            }else{
                                printf("wifi正常连接\r\n");
                            }
                        }
                    }
                }
            
            // 清空接收缓冲区计数，准备下一次接收
            esp32rec.len = 0;
        }
        
        // 保存当前字符，用于下一次比较
        lastChar = temp;
    }
    
    // 保留空闲中断IDLE中断标志位清除，防止意外情况
    if (USART_GetITStatus(USART2, USART_IT_IDLE))
    {
        (void)USART2->SR;
        (void)USART2->DR;
    }
}





