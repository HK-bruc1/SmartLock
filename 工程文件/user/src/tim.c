#include "main.h"


/**
 * @brief 定时中断的毫秒级延迟函数
 * 
 * @param ms 
 */
void tim9_it_ms(u16 ms){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
    // 禁用定时器以进行配置
    TIM_Cmd(TIM9, DISABLE);
    
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
    TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);

    // 清除更新中断标志位，防止提前进入中断
    TIM_ClearITPendingBit(TIM9, TIM_IT_Update);

    /*NVIC配置*/
        //优先级分组建议写在主函数中
        //两位抢占，两位响应表示优先级
        //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

        NVIC_InitTypeDef NVIC_InitStruct = {0};
        NVIC_InitStruct.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;//只能内核文件中找了,无法跳转
        NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0; // 抢占优先级为0
        NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0; // 响应优先级为0
        //单向使能USART1的中断信号通道
        NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStruct);


    //更新事件产生中断使能
    TIM_ITConfig(TIM9, TIM_IT_Update, ENABLE);
    //定时器使能
    TIM_Cmd(TIM9, ENABLE);
}


/**
 * @brief 定时器4的通道4  PWM输出初始化 PB9
 * PWM输出,内核->片上外设定时器->GPIO口，复用输出
 */
void tim4_ch4_led3_init(void){
    //IO口控制器配置
        //使能外设总线时钟
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;

        GPIO_Init(GPIOB, &GPIO_InitStruct);

        //复用功能配置
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_TIM4);

    
    //基本时基单元配置
        //使能外设总线时钟
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
        // 禁用定时器以进行配置
        TIM_Cmd(TIM4, DISABLE);
        // 配置TIM4的预分频器和自动重载值
        TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
        //默认一个结构体配置
        TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
        //只需要配置需要的参数，其他就用默认的
        TIM_TimeBaseStructure.TIM_Prescaler = 84-1;
        //写入最大的16位值
        TIM_TimeBaseStructure.TIM_Period = 1000 - 1;
        // 选择向上计数模式
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
        
        TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    //输出通道4配置
    TIM_OCInitTypeDef TIM_OCInitStruct = {0};
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse = 0;
    TIM_OC4Init(TIM4, &TIM_OCInitStruct);

    //定时器使能
    TIM_Cmd(TIM4, ENABLE);
}