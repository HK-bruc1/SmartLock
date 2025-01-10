#include "main.h"


/**
 * @brief 毫秒级延时函数
 * 
 * @param ms 
 */
void tim5Delay_Ms(u16 ms)
{
    // 使能TIM5的时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    
    // 禁用定时器以进行配置
    TIM_Cmd(TIM5, DISABLE);
    
    // 配置TIM6的预分频器和自动重载值
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
    //默认一个结构体配置
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    
    //只需要配置需要的参数，其他就用默认的
    TIM_TimeBaseStructure.TIM_Prescaler = 8400 - 1;
    TIM_TimeBaseStructure.TIM_Period = ms * 10 - 1;
    // 选择向上计数模式
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    //这里面也会产生一个更新事件，不用再写一个更新事件。
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
    
    
    
    // 清除更新标志位
    TIM_ClearFlag(TIM5, TIM_FLAG_Update);
    
    // 使能定时器
    TIM_Cmd(TIM5, ENABLE);
    
    // 等待更新事件完成
    while(!TIM_GetFlagStatus(TIM5, TIM_FLAG_Update));
    
    // 可选：清除更新标志位
    TIM_ClearFlag(TIM5, TIM_FLAG_Update);
    // 禁用定时器以进行配置，关闭定时器或者设置单次计数模式
    TIM_Cmd(TIM5, DISABLE);
}


/**
 * @brief 微秒级延时函数
 * 
 * @param us 
 */
void tim5Delay_Us(u16 us)
{
    // 使能TIM5的时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    
    // 禁用定时器以进行配置
    TIM_Cmd(TIM5, DISABLE);
    
    // 配置TIM6的预分频器和自动重载值
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
    //默认一个结构体配置
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    
    //只需要配置需要的参数，其他就用默认的
    TIM_TimeBaseStructure.TIM_Prescaler = 42-1;
    TIM_TimeBaseStructure.TIM_Period = us * 2 - 1;
    // 选择向上计数模式
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    //这里面也会产生一个更新事件，不用再写一个更新事件。
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
    
    
    // 清除更新标志位
    TIM_ClearFlag(TIM5, TIM_FLAG_Update);
    
    // 使能定时器
    TIM_Cmd(TIM5, ENABLE);
    
    // 等待更新事件完成
    while(!TIM_GetFlagStatus(TIM5, TIM_FLAG_Update));
    
    // 可选：清除更新标志位
    TIM_ClearFlag(TIM5, TIM_FLAG_Update);
    // 禁用定时器以进行配置，关闭定时器或者设置单次计数模式
    TIM_Cmd(TIM5, DISABLE);
}



/**
 * 使用TIM9定时器产生指定毫秒数的延迟
 * 
 * @param ms 延迟的时间，单位为毫秒
 * 
 * 本函数通过设置TIM9定时器的计数器来实现延迟功能
 * 在延迟期间，程序会停留在while循环中，直到计数器归零
 * 非阻塞式
 */
void tim9Delay_Ms(u16 ms){
    // 设置TIM9定时器的计数器值为ms，开始延迟
    tim9_count[0] = ms;
    // 当计数器值大于0时，继续延迟
    while(tim9_count[0]){
        //等待时需要做的事件
    };
}