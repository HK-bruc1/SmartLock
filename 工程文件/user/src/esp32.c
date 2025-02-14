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

    // 循环等待 ESP32 返回回应信息，若超时则退出
    while (1)
    {
        // 等待接收完成标志位被置 1（即数据接收完毕）
        //接收中断函数中会置位该标志位从而跳出循环
        while (!esp32rec.flag)      
        {
            // 递增超时计数变量，每次循环延时 1ms
            esp32_cont++;
            tim5Delay_Ms(1);
            
            // 判断是否达到超时时间
            if (esp32_cont == wait_ms)
            {
                // 超时处理，清空接收数据长度，返回 1 表示失败,没有接收到期望的回答，会因为超时返回1
                esp32rec.len = 0;  
                printf("响应数据超时\r\n");  
                return 1;     
            }
        }

        // 清除接收标志位，准备下一次接收
        esp32rec.flag = 0;

        // 判断接收缓存中是否  包含  期望的回应信息
        //发送一个AT期望回答OK
        if (strstr((const char *)esp32rec.buff, (const char *)respond) != NULL)
        {
            // 找到了期望的回应，返回 0 表示成功
            return 0;
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

    // 再次发送 "AT" 命令检查ESP32状态,上电第一条指令可能错误，需要重新发送一次
    //没有问题后，发送指令一定有响应，除非网络波动
    if(Esp32_SendandReceive((u8 *)"AT\r\n",(u8 *)"OK",10) != 0 )
    {
        // 如果ESP32没有响应正常，退出透传模式
        Exit_Cipmode();  // 退出透传模式，回到设置模式
    }

    // 恢复ESP32模块的出厂设置
    ESP32_Restore();
    
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
    
    //设置 Wi-Fi 重连配置
    //断开后始终尝试连接，每五分钟尝试一次，300秒，测试改为60秒
    if(Esp32_SendandReceive((u8 *)"AT+CWRECONNCFG=60,0\r\n",(u8 *)"OK",10) == 0)
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
	 
    /* 发送连接WIFI的AT命令，并等待响应 */
    if(Esp32_SendandReceive(wifi_buff, (u8 *)"OK", 5000) == 0)
    {
        // 如果ESP32返回"OK"，表示连接成功
        printf("WIFI连接指令执行成功,远程开锁功能开启\r\n");
        return 0;  // 返回0表示连接成功
    }
    else 
    {
        // 如果连接失败，打印失败消息
        printf("WIFI连接指令执行失败,远程开锁功能无效\r\n");
        return 1;  // 返回1表示连接失败
    }

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




void mqtt_init(void){
    u8 link_status = 1;

    //设置 MQTT 客户端信息，指令不会超时重传
    link_status = Esp32_SendandReceive("AT+MQTTUSERCFG=0,1,\"c96fdfa51d98473181c3525421eeeaab\",\"2hroci9d196rg88h\",\"McPl5Kyx0P\",0,0,\"\"\r\n", "OK", 2000);
    if(link_status == 0){
        printf("MQTT客户端信息设置成功\r\n");
        link_status = 1;
    }else{
        printf("MQTT客户端信息设置失败\r\n");
    }

    //连接 MQTT 服务器，自动重连MQTT服务器
    link_status = Esp32_SendandReceive("AT+MQTTCONN=0,\"gz-3-mqtt.iot-api.com\",1883,1\r\n", "OK", 10000);
    if(link_status == 0){
        printf("MQTT连接成功\r\n");
        link_status = 1;
    }else{
        printf("MQTT连接失败\r\n");
    }

    //订阅MQTT主题，指令不会超时重传
    link_status = Esp32_SendandReceive("AT+MQTTSUB=0,\"attributes/push\",1\r\n", "OK", 5000);
    if(link_status == 0){
        printf("MQTT订阅成功\r\n");
        link_status = 1;
    }else{
        printf("MQTT订阅失败\r\n");
    }

}


//上报关门数据
u8 publish_close(void){
    u8 publish_status = 1;
    //指令超时重传,至少收到一次指令，不然云端数据不同步
    publish_status = Esp32_SendandReceive("AT+MQTTPUB=0,\"attributes\",\"{\\\"lock_status\\\":0}\",1,0\r\n", "OK", 5000);
    return publish_status;
}











