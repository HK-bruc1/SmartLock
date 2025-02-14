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
        NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级为1
        NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1; // 响应优先级为1
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



/*
 * @brief  mg200指纹模块串口6的初始化
 *MG200-TXD ----------------------  USART6_RXD---------------PC7      复用
 *MG200-RXD----------------------  USART6_TXD--------------- PC6      复用
 *MG200_DETECT-----------------------------------------------PC0      通用输入
 *MG200_PWR -------------------------------------------------PC1      通用推挽输出
*/
void mg200_usart6Init(u32 baud){
    /*IO口控制器配置*/
        //端口时钟使能
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

        //复用配置定义在前面否则会偷发数据
        //硬件上已经连接，只需要配置复用功能，映射到片上外设的接口上
        GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
        GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);

        //端口模式配置-------复用  PC6：发送  PC7：接收
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        //GPIO口的最大输出速度阈值50MHz，波特率为115200没有超过GPIO口的最大处理速度
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
        GPIO_Init(GPIOC, &GPIO_InitStruct);
        //配置相同直接使用
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
        GPIO_Init(GPIOC, &GPIO_InitStruct);
    

    /*某个串口控制器配置*/
        //串口控制器时钟使能
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);	
        USART_InitTypeDef USART_InitStruct = {0};
        USART_InitStruct.USART_BaudRate = baud;
        //不开启奇偶校验位
        USART_InitStruct.USART_Parity = USART_Parity_No;
        //一个停止位
        USART_InitStruct.USART_StopBits = USART_StopBits_1;
        //每个数据帧中包含的数据位数
        USART_InitStruct.USART_WordLength = USART_WordLength_8b;
        //能不能写一起，要看写入寄存器的值会不会应为"|"操作而导致寄存器的值发生变化
        //启用接收模式（Rx）和发送模式（Tx）
        USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        //不启用硬件流控制。通信双方仅通过数据线（TX 和 RX）进行数据传输。
        USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        //初始化USART6(默认16倍采样，8倍采样单独使能，USART_BaudRate会自动计算，只要给目标波特率即可)
        USART_Init(USART6, &USART_InitStruct);


    /*NVIC配置，接收利用中断实现避免轮询*/
        //优先级分组建议写在主函数中
        //两位抢占，两位响应表示优先级
        //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

        NVIC_InitTypeDef NVIC_InitStruct = {0};
        NVIC_InitStruct.NVIC_IRQChannel = USART6_IRQn;//只能内核文件中找了,无法跳转
        NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级为1
        NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1; // 响应优先级为1
        //单向使能USART6的中断信号通道
        NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStruct);

    //使能放一起
    //单向开启串口控制器的中断通道
    //串口接收中断信号，接收寄存器非空，即有数据到达。
    USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART6,USART_IT_IDLE,ENABLE);//使能空闲中断,判断多字节数据是否发完（老师的代码）
    //使能USART6
    USART_Cmd(USART6, ENABLE);
}


/**
 * @brief  发送一个字节数据
 * 
 * @param ch 一个字节数据
 */
void USART6_SendChar(uint8_t ch) {
    // 等待发送数据寄存器为空（TXE标志位为1）,库函数没有等待所以重写一个发送函数
    while (USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);
    // 发送数据
    USART_SendData(USART6, ch);
}


/**
 * @brief 通过USART6发送字节序列
 * 
 * 本函数旨在通过USART6异步串行通信接口发送一组字节数据。它依次发送数据数组中的每个字节，
 * 直到所有数据发送完毕。此函数利用了USART6_SendChar函数来发送每个单独的字符。
 * 
 * @param data 指向待发送数据的指针
 * @param length 待发送数据的长度
 */
void USART6_SendBytes(u8 *data, uint8_t length) {
    // 遍历数据数组，发送每个字节
    for (uint8_t i = 0; i < length; i++) {
        USART6_SendChar(data[i]);
    }

}



/*****************************************************
函数功能：UART2初始化配置用于与ESP32通信
函数形参：void
函数返回值：void
函数说明：
    使用引脚: PA2(UART2_TX), PA3(UART2_RX)
    波特率: 115200
    数据格式: 8位数据位, 1位停止位, 无校验位
*****************************************************/
void usart2_esp32_Init(void)
{
    // 1. 使能GPIOA时钟
    // UART2的TX(PA2)和RX(PA3)都在GPIOA上
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    
    // 2. 将PA2和PA3配置为UART2的复用功能
    //复用功能写前面，不然会偷发一个？号   ？
    // GPIO_AF_USART2表示将引脚复用为UART2功能
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);  // PA2配置为UART2_TX
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);  // PA3配置为UART2_RX
    
    // 3. 配置GPIO参数
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;     // 配置为复用模式
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;   // 推挽输出
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;  // 配置PA2和PA3
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL; // 无上下拉
    GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;   // 低速模式，115200,ESP32规定了，低速模式够用
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // 4. 配置UART2参数
    // 首先使能UART2时钟，UART2在APB1总线上
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    
    // 配置UART2通信参数
    USART_InitTypeDef USART_InitStruct = {0};
    USART_InitStruct.USART_BaudRate = 115200;     // 波特率设置为115200
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // 禁用硬件流控
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  // 使能发送和接收功能
    USART_InitStruct.USART_Parity = USART_Parity_No;    // 无校验位
    USART_InitStruct.USART_StopBits = USART_StopBits_1; // 1位停止位
    USART_InitStruct.USART_WordLength = USART_WordLength_8b; // 8位数据位
    USART_Init(USART2, &USART_InitStruct);
    
    // 5. 配置UART2中断
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  // 使能接收中断
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);  // 使能空闲中断
    
    
    // 6. 配置NVIC中断优先级
        //优先级分组建议写在主函数中
        //两位抢占，两位响应表示优先级
        //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

        NVIC_InitTypeDef NVIC_InitStruct = {0};
        NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;//只能内核文件中找了,无法跳转,跳到结构体就能看到
        NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级为1
        NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1; // 响应优先级为1
        //单向使能USART6的中断信号通道
        NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStruct);

    // 7. 使能UART2
    USART_Cmd(USART2, ENABLE);
}


/*****************************************************
函数名    : usart2_esp32_send_str
函数功能  : 串口2发送字符串到ESP32
函数形参  : u8 *data - 指向要发送的字符串的指针
函数返回值: void
函数说明  :  1. 通过查询方式发送，发送完成前会阻塞
            2. 使用USART_SR寄存器的TC位判断发送状态
            3. 通过USART_DR寄存器发送数据
*****************************************************/
void usart2_esp32_send_str(u8 *data)
{
    // 循环直到遇到字符串结束符'\0'
    while(*data != '\0')
    {
        // 等待上一次发送完成
        // SR寄存器的第6位(TC位)为1表示发送完成
        // TC = Transmission Complete
        //while((USART2->SR & (0X1 << 6)) == 0);

        //等待发送数据寄存器为空（TXE标志位为1） 
        //USART_FLAG_TXE 代替 USART_FLAG_TC：
        //TXE = 1 表示寄存器空闲，可以写入新数据，提高传输效率。
        //TC = 1 表示整个数据帧发送完成，适用于等待最后一个字符传输完毕。
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); 
        
        // 向DR寄存器写入数据，启动发送
        // data指针自增，指向下一个要发送的字符
        //USART2->DR = *data++;
        USART_SendData(USART2, (uint16_t)(*data)); // 发送单个字符
        data++; // 指向下一个字符
        
        // 注：DR = Data Register，数据寄存器
        // 写DR会自动清除TC位
        // 发送完成后TC位会自动置1
    }

    // 可选：等待最后一个字符发送完成，确保数据全部输出
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
}




