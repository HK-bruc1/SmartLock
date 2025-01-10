#include "main.h"

/**
 * @brief 配置TIM2_CH2为PWM输出,PB3
 * 
 * 此函数配置TIM2的通道2为PWM输出模式，用于控制外部设备，如电机或舵机。
 * 
 * @param None
 * @return None
 */
void tim2_ch2_pwm(void){
    //IO口控制器配置
        //使能外设总线时钟
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

        GPIO_InitTypeDef GPIO_InitStruct = {0};
        //设置引脚为复用功能模式
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
        //设置引脚为推挽输出模式
        GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
        //设置引脚为PB3
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
        //设置引脚无上拉或下拉电阻
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        //设置引脚速度为2MHz
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;

        //初始化GPIOB的引脚
        GPIO_Init(GPIOB, &GPIO_InitStruct);

        //复用功能配置
        //将PB3引脚复用为TIM2的通道2
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_TIM2);

    
    //基本时基单元配置
        //使能外设总线时钟
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
        // 禁用定时器以进行配置
        TIM_Cmd(TIM2, DISABLE);
        // 配置TIM2的预分频器和自动重载值
        TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
        //默认一个结构体配置
        TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
        //只需要配置需要的参数，其他就用默认的
        //设置预分频器值为84-1，得到1MHz的计数频率
        TIM_TimeBaseStructure.TIM_Prescaler = 84-1;
        //设置自动重载值为1000-1，得到1kHz的PWM频率
        TIM_TimeBaseStructure.TIM_Period = 1000 - 1;
        // 选择向上计数模式
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
        
        //初始化TIM2的基本时基单元
        TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    //输出通道2配置
    TIM_OCInitTypeDef TIM_OCInitStruct = {0};
    //设置通道为PWM1模式
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    //设置输出极性为高电平有效
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    //使能输出通道
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    //设置初始占空比为0%
    TIM_OCInitStruct.TIM_Pulse = 0;
    //初始化TIM2的通道2
    TIM_OC2Init(TIM2, &TIM_OCInitStruct);

    //定时器使能
    TIM_Cmd(TIM2, ENABLE);
}



/**
 * @brief 配置TIM3_CH1为PWM输出,PB4
 * 
 * 此函数配置TIM3的通道1为PWM输出模式，用于控制外部设备，如电机或舵机。
 * 
 * @param None
 * @return None
 */
void tim3_ch1_pwm(void){
    //IO口控制器配置
        //使能外设总线时钟
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

        GPIO_InitTypeDef GPIO_InitStruct = {0};
        //设置引脚为复用功能模式
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
        //设置引脚为推挽输出模式
        GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
        //设置引脚为PB4
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
        //设置引脚无上拉或下拉电阻
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        //设置引脚速度为2MHz
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;

        //初始化GPIOB的引脚
        GPIO_Init(GPIOB, &GPIO_InitStruct);

        //复用功能配置
        //将PB4引脚复用为TIM3的通道1
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_TIM3);
    
    //基本时基单元配置
        //使能外设总线时钟
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
        // 禁用定时器以进行配置
        TIM_Cmd(TIM3, DISABLE);
        // 配置TIM3的预分频器和自动重载值
        TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
        //默认一个结构体配置
        TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
        //只需要配置需要的参数，其他就用默认的
        //设置预分频器值为84-1，得到1MHz的计数频率
        TIM_TimeBaseStructure.TIM_Prescaler = 84-1;
        //设置自动重载值为1000-1，得到1kHz的PWM频率
        TIM_TimeBaseStructure.TIM_Period = 1000 - 1;
        // 选择向上计数模式
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
        
        //初始化TIM3的基本时基单元
        TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    //输出通道1配置
    TIM_OCInitTypeDef TIM_OCInitStruct = {0};
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse = 0;
    TIM_OC1Init(TIM3, &TIM_OCInitStruct);

    //定时器使能
    TIM_Cmd(TIM3, ENABLE);
}



/**
 * @brief 初始化智能锁的门控制
 * 
 * 此函数初始化控制智能锁门的两个电机端口的PWM信号。
 * 它调用tim2_ch2_pwm()和tim3_ch1_pwm()函数来配置TIM2的通道2和TIM3的通道1为PWM输出模式。
 * 
 * @param None
 * @return None
 */
void door_init(void){
    //两个电机端口的PWM初始化
    //调用tim2_ch2_pwm()函数初始化TIM2的通道2为PWM输出模式
    tim2_ch2_pwm();
    //调用tim3_ch1_pwm()函数初始化TIM3的通道1为PWM输出模式
    tim3_ch1_pwm();

    //初始化后,两个比较寄存器的是0，两个占空比是0，是电机停止状态不是关门状态
    door_close();
}

/**
 * @brief 控制电机转向和速度
 * 
 * @param speed : 0 电机停止  >0 1转向  <0 -1转向
 */
void motor_control(s16 speed){
    if(speed ==0){
        TIM2->CCR2 = 0;
        TIM3->CCR1 = 0;
    }else if(speed>0){
        TIM2->CCR2 = speed;
        TIM3->CCR1 = 0;
    }else if(speed<0){
        TIM2->CCR2 = 0;
        TIM3->CCR1 = -speed;
    }

}


void door_open(void){
    //电机1转，全速
    motor_control(1000);
    //延迟
    tim5Delay_Ms(300);
    //电机停止
    motor_control(0);
}

void door_close(void){
    //电机-1转，全速
    motor_control(-1000);
    //延迟
    tim5Delay_Ms(300);
    //电机停止
    motor_control(0);
}
