#include "main.h"


/**
 * @brief 串口1的初始化
 * 
 * @param baud 传输的波特率
 */
void usart1Init(u32 baud){
    /*IO口控制器配置*/
        //端口时钟使能
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
        //物理上PA9已经连接到了USART1的TX引脚，PA10连接到了USART1的RX引脚
        //只需要将PA9和PA10配置为复用功能（Alternate Function, AF），并将它们映射到USART1的TX和RX功能即可。
        //USART外设会自动处理数据的发送和接收，你无需手动操作这些引脚。
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);//定义在前面否则会偷发数据

        //端口模式配置-------复用  PA9：发送  PA10：接收
        //输出类型
        //输出速度,慢速
        //上下拉
        //复用功能寄存器配置,置0111
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        //GPIO口的最大输出速度阈值50MHz，波特率为115200没有超过GPIO口的最大处理速度
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
        GPIO_Init(GPIOA, &GPIO_InitStruct);
    

    /*某个串口控制器配置*/
        //串口控制器时钟使能
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	
        USART_InitTypeDef USART_InitStruct = {0};
        USART_InitStruct.USART_BaudRate = baud;
        USART_InitStruct.USART_Parity = USART_Parity_No;
        USART_InitStruct.USART_StopBits = USART_StopBits_1;
        USART_InitStruct.USART_WordLength = USART_WordLength_8b;
        //能不能写一起，要看写入寄存器的值会不会应为"|"操作而导致寄存器的值发生变化
        USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        // 初始化USART1(默认16倍采样，8倍采样单独使能，USART_BaudRate会自动计算)
        USART_Init(USART1, &USART_InitStruct);


    /*NVIC配置*/
        //优先级分组建议写在主函数中
        //两位抢占，两位响应表示优先级
        //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

        NVIC_InitTypeDef NVIC_InitStruct = {0};
        NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;//只能内核文件中找了,无法跳转
        NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级为0
        NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1; // 响应优先级为0
        //单向使能USART1的中断信号通道
        NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStruct);

    //使能放一起
    //单向开启串口控制器的中断通道
    //串口接收中断信号
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    //串口空闲中断信号,用来判断字符串是否接收完成，不需要人为定义结束符号
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
    // 使能USART1
    USART_Cmd(USART1, ENABLE);

}


/**
 * @brief  发送一个字节数据
 * 
 * @param ch 一个字节数据
 */
void USART1_SendChar(uint8_t ch) {
    // 等待发送数据寄存器为空（TXE标志位为1）,库函数没有等待所以重写一个发送函数
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    // 发送数据
    USART_SendData(USART1, ch);
}


/**
 * @brief  发送字符串 
 * 
 * @param str 字符串的起始地址
 */
void USART1_SendString(char *str) {
    //"\0",取值拿到其实比较的是字符的ASCII码值
    while (*str != '\0') {
        USART1_SendChar(*str); // 逐个发送字符
        str++;
    }
}



