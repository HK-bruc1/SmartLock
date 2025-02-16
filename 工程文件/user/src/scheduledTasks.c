#include "main.h"

//一些定时任务,老感觉放在中断不合适，稍微耗时一点的操作，网络响应数据时间和检查数据时间太久了
//果然之前解析不到的数据，又可以正常解析了，中断还是快进快出吧！
void task(void){
    //这个定时任务非常依赖数据解析后标志位置位的结果，但是他们同在主循环中，必须有执行关系的先后顺序。
    //周期性检查WiFi的状态,每一分钟检查一次
    if(tim9_count[5]>=60000){
        //检查完MQTT后再清零
        tim9_count[5] = 0;
        printf("周期性检查WiFi状态\r\n");
        //清理接收缓存
        //clean_buff();
        Esp32_SendandReceive((u8 *)"AT+CWSTATE?\r\n",(u8 *)"OK",5000);
    }

    //只要WiFi被检查过一次，这个地方一定二选一
    //包括自己，一共才解析三类数据而已
    //只有WiFi正常连接没有中断过，才会进入这个逻辑
    //只有WiFi检查过，才能来判断是否继续检查MQTT
    if(wifi_check_flag == 0 && wifi_working_flag == 0){
        //下面这个检查完全依赖于WiFi的状态检查，其他操作不需要关注
        //周期性检查MQTT的状态,在WiFi没有断开的情况下检查
        printf("周期性检查MQTT状态\r\n");
        //清理接收缓存
        //clean_buff();主循环中的数据在经过解析后都会清理不需要自己清理
        Esp32_SendandReceive((u8 *)"AT+MQTTCONN?\r\n",(u8 *)"OK",10000);
        //这一次的WiFi检查结果已经使用了
        wifi_check_flag = 1;
    }else {
        //根据WiFi检查结果，MQTT不满足检查的条件
        //或者是还没有检查过WiFi状态
        wifi_check_flag = 1;
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