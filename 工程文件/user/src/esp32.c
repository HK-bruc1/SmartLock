#include "main.h"


/*****************************************************
函数名称  : Esp32_SendandReceive
函数功能  ：ESP32 发送指令并判断是否接收到期望的回应信号
函数形参  ：
    u8 *cmd      -- 发送的指令字符串
    u8 *respond  -- 期望的回应信息（如 "OK"）
    u32 wait_ms  -- 超时时间（单位：毫秒）
函数返回值：u8
    返回 0 表示成功接收到期望的回应
    返回 1 表示超时或未能接收到期望的回应
*****************************************************/
u8 Esp32_SendandReceive(u8 *cmd, u8 *respond, u32 wait_ms)  
{
    //利用while实现的计时效果
    u32 esp32_cont = 0; // 计时器变量，用于检测超时

    // 发送指令给 ESP32
    usart2_esp32_send_str(cmd);
    
    // 延时 100ms，给 ESP32 处理指令的时间
    tim5Delay_Ms(100);

    //检查数据的超时时间
    u32 MS = 3000;

    // 循环等待 ESP32 返回回应信息，若超时则退出
    while(1){
        // 等待接收完成标志位被置 1（即数据接收完毕）
        //接收中断函数中会置位该标志位从而跳出循环
        while (!esp32rec.flag)      
        {
            //发送指令后，有数据接收完毕，就不会进入这里
            // 递增超时计数变量，每次循环延时 1ms
            esp32_cont++;
            tim5Delay_Ms(1);
            
            // 判断是否达到超时时间
            if (esp32_cont == wait_ms)
            {
                // 超时处理，清空接收数据长度，返回 1 表示失败,没有接收到期望的回答，会因为超时返回1
                // 只清空有效数据部分，因为后续会检查是否包含字符串，不清除会有误判隐患
                clean_buff();
                printf("响应数据超时\r\n");  
                return 1;     
            }
        }

        //printf("接收数据:%s\r\n",esp32rec.buff);
        // 判断接收缓存中是否  包含  期望的回应信息
        //发送一个AT期望回答OK
        if (strstr((const char *)esp32rec.buff, (const char *)respond) != NULL)
        {
            // 找到了期望的回应，返回 0 表示成功
            return 0;
        }
        //前面是接收数据时的超时时间，这里是检查响应数据的时间，似乎接收完成置位立马检查的话，检查不到，这个while（1）果然有作用
        esp32_cont++;
        tim5Delay_Ms(1);
        
        // 判断是否达到超时时间
        if (esp32_cont == MS)
        {
            // 超时处理，清空接收数据长度，返回 1 表示失败,没有接收到期望的回答，会因为超时返回1
            //没有响应期望的数据，返回值在外面自己去判断
            return 1;     
        }

    }
}


/***********************************************
*函数名    ：Exit_Cipmode
*函数功能  ：退出透传模式,进入设置模式
*函数参数  ：无
*函数返回值：无
*函数描述  ：从发送数据模式进入到设置WIFI的时候
             需要用此函数
***********************************************/
void Exit_Cipmode(void)
{
    // 发送退出透传模式的命令 "+++"
    usart2_esp32_send_str("+++");
    
    // 等待2秒钟，以确保ESP32收到命令并成功退出透传模式
    tim5Delay_Ms(2000); 
    
    // 输出提示信息，表明透传模式已经退出，进入设置模式
    printf("透传模式以退出,进入设置模式\r\n");
}


/***********************************************
*函数名    ：ESP32_Restore
*函数功能  ：恢复出厂设置
*函数参数  ：无
*函数返回值：u8   返回0:成功      返回1:失败
*函数描述  ：该函数通过发送 AT+RESTORE 命令，恢复 ESP32 出厂设置
***********************************************/
u8 ESP32_Restore(void)
{
    u8 sta;
    
    // 发送 "AT+RESTORE" 命令给 ESP32 并等待响应
    // 如果 ESP32 返回 "ready"，表示恢复命令成功
    sta = Esp32_SendandReceive("AT+RESTORE\r\n","ready",3000);
    
    // 如果返回状态为0，表示恢复出厂设置成功
    if(sta == 0)
    {
        // 打印成功消息
        printf("恢复出厂设置成功\r\n");
    }
    else
    {
        // 如果返回状态为1，表示恢复出厂设置失败
        printf("恢复出厂设置失败\r\n");
    }

    //手动清理接收缓存
    clean_buff();
    
    // 返回函数执行的状态，0表示成功，1表示失败
    return sta;
}


/***********************************************
*函数名    ：ESP32_init
*函数功能  ：ESP32初始化配置函数
*函数参数  ：无
*函数返回值：无
*函数描述  ：该函数用于初始化ESP32模块，配置串口、WIFI及TCP相关设置
***********************************************/
void Esp32_Init(void)
{
    /* 串口2初始化 */
    usart2_esp32_Init();  // 初始化与ESP32通信的串口（USART2）

    /* WIFI相关初始配置 */
    // 发送初始命令 "AT" 给 ESP32，等待响应 "OK"。此命令作为检查ESP32是否正常工作。
    Esp32_SendandReceive((u8 *)"AT\r\n",(u8 *)"OK",10); 

    //手动清理接收缓存
    clean_buff();
    // 再次发送 "AT" 命令检查ESP32状态,上电第一条指令可能错误，需要重新发送一次
    //没有问题后，发送指令一定有响应，除非网络波动
    if(Esp32_SendandReceive((u8 *)"AT\r\n",(u8 *)"OK",10) != 0 )
    {
        //手动清理接收缓存
        clean_buff();
        // 如果ESP32没有响应正常，退出透传模式
        Exit_Cipmode();  // 退出透传模式，回到设置模式
    }

    //手动清理接收缓存
    clean_buff();
    // 恢复ESP32模块的出厂设置
    ESP32_Restore();
    
    //手动清理接收缓存
    clean_buff();
    // 设置ESP32为客户端模式
    if(Esp32_SendandReceive((u8 *)"AT+CWMODE=1\r\n",(u8 *)"OK",10) == 0)
    {
        // 如果设置成功，打印成功消息
        printf("客户端模式指令执行成功\r\n");
    }
    else
    {
        // 如果设置失败，打印失败消息
        printf("客户端模式指令执行失败\r\n");
    }

    //手动清理接收缓存
    clean_buff();
    /* TCP相关初始配置 */
    // 设置ESP32为透传模式
    if(Esp32_SendandReceive((u8 *)"AT+CIPMODE=1\r\n",(u8 *)"OK",10) == 0)
    {
        // 如果设置成功，打印成功消息
        printf("透传指令执行成功\r\n");
    }
    else
    {
        // 如果设置失败，打印失败消息
        printf("透传指令执行失败\r\n");
    }  

    //手动清理接收缓存
    clean_buff();
    //设置 Wi-Fi 重连配置
    //断开后始终尝试连接，每6s尝试一次
    if(Esp32_SendandReceive((u8 *)"AT+CWRECONNCFG=6,0\r\n",(u8 *)"OK",10) == 0)
    {
        // 如果设置成功，打印成功消息
        printf("Wi-Fi 重连配置指令执行成功\r\n");
    }
    else
    {
        // 如果设置失败，打印失败消息
        printf("Wi-Fi 重连配置指令执行失败\r\n");
    }  
}


/***********************************************
*函数名    ：Esp32_Wificonnect
*函数功能  ：WIFI连接函数
*函数参数  ：u8 *user, u8 *password
*函数返回值：u8  返回0:连接成功，返回1:连接失败
*函数描述  ：此函数通过发送 AT+CWJAP 命令连接到指定的WIFI网络
             用户名和密码作为函数参数传入，格式为
             AT+CWJAP="用户名","密码"
***********************************************/

u8 Esp32_Wificonnect(u8 *user, u8* password)
{			
    u8 wifi_buff[50] = "AT+CWJAP=\"";  // 初始化AT命令字符串

    /* 拼接用户名和密码，形成规定的命令格式 */
    strcat((char*)wifi_buff, (char*)user);  // 将用户名拼接到命令中
    // AT+CWJAP=\"LL

    strcat((char*)wifi_buff, "\",\"");  // 拼接分隔符 "\","
    // AT+CWJAP=\"LL\",\"

    strcat((char*)wifi_buff, (char*)password);  // 将密码拼接到命令中
    // AT+CWJAP=\"LL\",\"12345678

    strcat((char*)wifi_buff, "\"\r\n");  // 拼接命令结束符 "\r\n"
    // "AT+CWJAP=\"LL\",\"12345678\"\r\n"

	//手动清理接收缓存
    clean_buff(); 
    /* 发送连接WIFI的AT命令，并等待响应 */
    if(Esp32_SendandReceive((u8 *)wifi_buff, (u8 *)"OK", 5000) == 0)
    {
        // 如果ESP32返回"OK"，表示连接成功
        printf("WIFI连接指令执行成功,开始连接MQTT\r\n");
        return 0;  // 返回0表示连接成功
    }
    else 
    {
        // 如果连接失败，打印失败消息
        printf("WIFI连接指令执行失败,远程开锁功能失效\r\n");
        return 1;  // 返回1表示连接失败
    }

    //无论连接成功与否开始WiFi状态检查计时
    tim9_count[5] = 0;
}




/***********************************************
*函数名    ：open_Remote
*函数功能  ：远程开锁
*函数参数  ：无
*函数返回值：无
*函数描述  ：
*   该函数用于解析 ESP32 串口接收到的数据，并根据数据中的指令控制门锁的开关状态。
*   这个函数不知道还是否有用？因为可以在接收中断里直接判断的，那么中断的判断是什么意思？
***********************************************/
void open_Remote(void)
{
    u8 sta_lock;  // 记录门锁状态（'1' 表示开锁，'0' 表示关锁）
    char *adrr = 0;  // 指向解析到的 "smart_lock" 关键词位置的指针

    // 检查 ESP32 串口是否接收到数据，中断接收完成会把标志位置1
    if (esp32rec.flag == 1)
    {
        esp32rec.flag = 0;  // 清除接收标志，防止重复处理数据

        // 在接收的缓冲区中查找 "smart_lock" 关键字
        adrr = strstr((char *)esp32rec.buff, "\"smart_lock\"");
        
        // 提取 "smart_lock" 关键字后面的状态值（'1' 或 '0'）
        sta_lock = *(adrr + strlen("\"smart_lock\"") + 1);

        // 根据状态值执行相应操作
        if (sta_lock == '1')  // 远程开锁指令
        {
            voice(DOOROPEN_SUCCESS);  // 播放开锁提示音
            door_open();  // 执行开门操作，这里高速轮询没有办法延迟，使用static修饰，加定时器，也不是不行。
        }
        else if (sta_lock == '0')  // 远程关锁指令
        {
            door_close();  // 执行关门操作
        }
    }
}



/**
 * @brief MQTT连接函数，返回0代表mqtt连接失败，返回1代表mqtt连接成功
 * 
 * @return u8 
 */
u8 mqtt_init(void){
    u8 link_status = 1;

    //手动清理接收缓存
    clean_buff(); 
    //设置 MQTT 客户端信息，指令不会超时重传
    link_status = Esp32_SendandReceive("AT+MQTTUSERCFG=0,1,\"c96fdfa51d98473181c3525421eeeaab\",\"2hroci9d196rg88h\",\"McPl5Kyx0P\",0,0,\"\"\r\n", "OK", 2000);
    if(link_status == 0){
        //printf("MQTT客户端信息设置成功\r\n");
        link_status = 1;
    }else{
        printf("MQTT客户端信息设置失败\r\n");
        link_status = 0;
        return link_status;
    }


    //手动清理接收缓存
    clean_buff();
    //连接 MQTT 服务器，自动重连MQTT服务器
    link_status = Esp32_SendandReceive("AT+MQTTCONN=0,\"gz-3-mqtt.iot-api.com\",1883,1\r\n", "OK", 10000);
    if(link_status == 0){
        //printf("MQTT连接成功\r\n");
        link_status = 1;
        return link_status;
    }else{
        printf("MQTT连接失败\r\n");
        link_status = 0;
        return link_status;
    }


    //手动清理接收缓存
    clean_buff();
    //订阅MQTT主题，指令不会超时重传
    link_status = Esp32_SendandReceive("AT+MQTTSUB=0,\"attributes/push\",1\r\n", "OK", 5000);
    if(link_status == 0){
        //printf("MQTT订阅成功\r\n");
        link_status = 1;
    }else{
        printf("MQTT订阅失败\r\n");
        link_status = 0;
        return link_status;
    }
    //手动清理接收缓存
    clean_buff();
}


//上报关门数据
void publish_close(void){
    //指令超时重传,至少收到一次指令，不然云端数据不同步
    //上报qos选择1，会导致上报频率过快，会被平台限流，对于智能锁来说好像也不需要严格同步
    //自己搭建的MQTT服务器另说
    Esp32_SendandReceive("AT+MQTTPUB=0,\"attributes\",\"{\\\"lock_status\\\":0}\",0,0\r\n", "OK", 5000);
    return ;
}


// 在主循环中调用的数据处理函数
void ProcessUartData(void)
{   
    //接收响应数据的标志位还没有置1
    if (!esp32rec.flag) return;
            
    // 在这里处理完整的数据包
    ProcessESP32Data(esp32rec.buff, esp32rec.len);
    
    //清理接收缓存
    clean_buff();
}



// ESP32数据处理函数
void ProcessESP32Data(uint8_t* data, uint16_t len)
{
    // 原来中断中的数据处理逻辑移到这里
    char *lock_status_str;
    char *WiFi_status_str;
    int lock_status;
    int WiFi_status;
    
    if (strstr((char *)data, "attributes/push") != NULL)
    {
        lock_status_str = strstr((char *)data, "\"lock_status\":");
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
    
    // WiFi状态检查逻辑
    WiFi_status_str = strstr((char *)data, "+CWSTATE:");
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
               //连接一次mqtt后已经在主函数置位，mqtt_connect_flag = 1;
            }else if(WiFi_status==1||WiFi_status==2){
                if(wifi_connect_flag==1){
                    printf("wifi连接恢复,开启远程开锁模式\r\n");
                    //主函数会因为置位重新尝试连接mqtt
                    wifi_connect_flag = 0;
                    mqtt_connect_flag = 0;
                }else{
                    printf("wifi正常连接\r\n");
                }
            }
        }
    }
}


//因为把数据处理放到了主循环中，所以初始化阶段的响应数据不会被处理，所以需要清空缓冲区，否则会误判
void clean_buff(void){
    // 只清空有效数据部分，因为后续会检查是否包含字符串，不清除会有误判隐患
    for (u16 i = 0; i < esp32rec.len; i++) {
        esp32rec.buff[i] = 0;
    }
    // 清空接收缓冲区计数，准备下一次接收
    esp32rec.len = 0;
    //数据处理完成，把标志位置0，等待下一次接收
    esp32rec.flag = 0;
}











