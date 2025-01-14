#include "main.h"

void iwdg_init(void)
{
    //时钟源配置-开启低速时钟
    RCC->CSR |= (1U<<0);
    //等待时钟稳定
    while(!(RCC->CSR & (1U<<1)));
    //解除写保护
    IWDG->KR = 0x5555;
    //设置分频为128分频,根据具体情况确定时间，32 kHz 低速内部 RC (LSI RC)，该 RC 用于驱动独立看门狗
    //250次/s
    IWDG->PR = 5;     //直接赋值更清晰
    //重装载值设置为2500 (超时时间约10秒)
    IWDG->RLR = 2500;
    //重载到计数器,任何时候将关键字 0xAAAA 写到 IWWDG_KR 寄存器中， 
    //IWDG_RLR 的值就会被重载到计数器，从而避免产生看门狗复位。
    IWDG->KR = 0xAAAA;//此时写保护再次开启
    //启动看门狗
    IWDG->KR = 0xCCCC;
    
    printf("看门狗启动\r\n");
}


//独立喂狗程序
void iwdg_feed(void)
{
    IWDG->KR = 0xAAAA;
}