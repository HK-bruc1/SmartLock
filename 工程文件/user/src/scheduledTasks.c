#include "main.h"

//一些定时任务,老感觉放在中断不合适，稍微耗时一点的操作，网络响应数据时间和检查数据时间太久了
//果然之前解析不到的数据，又可以正常解析了，中断还是快进快出吧！
void task(void){
    //周期性检查WiFi的状态,每一分钟检查一次
    if(tim9_count[5]>=60000){
        //到时间就归零
        tim9_count[5] = 0;
        printf("周期性检查WiFi状态\r\n");
        //清理接收缓存
        clean_buff();
        Esp32_SendandReceive((u8 *)"AT+CWSTATE?\r\n",(u8 *)"OK",5000);
    }

    //周期性检查MQTT的状态,在WiFi没有断开的情况下检查
    if(tim9_count[5]>=60000){
        //到时间就归零
        tim9_count[5] = 0;
        printf("周期性检查WiFi状态\r\n");
        //清理接收缓存
        clean_buff();
        Esp32_SendandReceive((u8 *)"AT+CWSTATE?\r\n",(u8 *)"OK",5000);
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