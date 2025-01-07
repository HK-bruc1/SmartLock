#include "main.h"
#include "stm32f4xx.h"                  // Device header


/***************************************
*函数名		：key_init
*函数功能	：按键功能初始化
*函数参数	：无
*函数返回值	：无
*函数描述	：key1-----PA0,key2-----PC13，key2按下时io口为低电平，key1按下时io为高电平
***************************************/
void key_init(void){
    //GPUO口的初始化
    //对应总线时钟使能
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    //通用输入
    //上下拉
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    //PA0
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    //PC13,修改必要，其他相同
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/***************************************
*函数名		：key_scan
*函数功能	：按键扫描
*函数参数	：无
*函数返回值	：u8
*函数描述	：
***************************************/
u8 key_scan(void){
    //判断key1和key2的状态,按下是1，松开是0
    u8 key_val = 0x00;
    static u8 key_lock = 1;

    //对KEY1和KEY2状态检测,同一时刻只能检测到一个按键按下，并在下一次按下之前锁住状态
    if(KEY1 && key_lock){
        //消抖
        delay_ms(15);
        if(KEY1){
            key_val = 1;
            key_lock = 0;

        }
    }else if(KEY2 && key_lock){
        //消抖
        delay_ms(15);
        if(KEY2){
            key_val = 2;
            key_lock = 0;
        }
    }

    //都松开的情况下解锁，
    if(KEY1==0 && KEY2==0) {
        //松开消抖
        delay_ms(15);
        key_lock = 1;
    }

    //返回键值状态
    return key_val;

}