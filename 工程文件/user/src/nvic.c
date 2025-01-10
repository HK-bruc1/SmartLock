#include "main.h"



/**
 * @brief 串口1中断服务函数
 * 
 */
void USART1_IRQHandler(void){
    //判断是接收中断信号触发
    if(USART_GetITStatus(USART1, USART_IT_RXNE)){
        //清除中断标志位
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
        //紧急事件(只接受不做处理),每次之能接收一个字节数据，字符串的话，就是一个字符
        //输入数据寄存器数据有了数据之后就触发中断，注意这里不是输入移位寄存器
        usart1.usart_buff[usart1.len++] = USART_ReceiveData(USART1);
    }

    //判断是空闲中断信号触发
    if(USART_GetITStatus(USART1, USART_IT_IDLE)){
        //清除中断标志位
        //USART_ClearITPendingBit(USART1, USART_IT_IDLE);
        //只能手动清除，库函数没有写
        USART1->SR;
        USART1->DR;
        //紧急事件
        usart1.usart_buff[usart1.len] = '\0';//在外部补充结束符号，成为一个字符串
        //接收完成后，长度清零（方便下一次接收，数据已经存到数组中了，没有丢失），标志位置1，直接在主函数中处理
        usart1.len = 0;
        //外部通过标志位判断是否有新数据到达
        usart1.usart_flag = 1;
    }
}

/**
 * @brief 定时器9的中断服务函数
 * 
 */
TIM1_BRK_TIM9_IRQHandler(void){
    //led3呼吸灯标志位,不影响开关呼吸灯，用作切换呼吸灯的方向
    static u8 led3_flag = 1;
    
    //判断中断信号源
    if (TIM_GetITStatus(TIM9, TIM_IT_Update) == SET){
        //清除中断标志位
        TIM_ClearITPendingBit(TIM9, TIM_IT_Update);


        //紧急事件
        //延时使用
        tim9_count[0]--;
        //led3呼吸灯
        tim9_count[1]++;

        //led3呼吸灯
        if(tim9_count[1]>=2){
            tim9_count[1] = 0;
            if(led3_flag == 1){
                TIM4->CCR4++;
                if(TIM4->CCR4>=1000){
                    led3_flag = 0;
                }
            }else if(led3_flag == 0){
                TIM4->CCR4--;
                if(TIM4->CCR4<1){
                    led3_flag = 1;
                }
            }
        }



    }
}


