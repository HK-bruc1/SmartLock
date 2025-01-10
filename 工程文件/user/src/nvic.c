#include "main.h"



/**
 * @brief ����1�жϷ�����
 * 
 */
void USART1_IRQHandler(void){
    //�ж��ǽ����ж��źŴ���
    if(USART_GetITStatus(USART1, USART_IT_RXNE)){
        //����жϱ�־λ
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
        //�����¼�(ֻ���ܲ�������),ÿ��֮�ܽ���һ���ֽ����ݣ��ַ����Ļ�������һ���ַ�
        //�������ݼĴ���������������֮��ʹ����жϣ�ע�����ﲻ��������λ�Ĵ���
        usart1.usart_buff[usart1.len++] = USART_ReceiveData(USART1);
    }

    //�ж��ǿ����ж��źŴ���
    if(USART_GetITStatus(USART1, USART_IT_IDLE)){
        //����жϱ�־λ
        //USART_ClearITPendingBit(USART1, USART_IT_IDLE);
        //ֻ���ֶ�������⺯��û��д
        USART1->SR;
        USART1->DR;
        //�����¼�
        usart1.usart_buff[usart1.len] = '\0';//���ⲿ����������ţ���Ϊһ���ַ���
        //������ɺ󣬳������㣨������һ�ν��գ������Ѿ��浽�������ˣ�û�ж�ʧ������־λ��1��ֱ�����������д���
        usart1.len = 0;
        //�ⲿͨ����־λ�ж��Ƿ��������ݵ���
        usart1.usart_flag = 1;
    }
}

/**
 * @brief ��ʱ��9���жϷ�����
 * 
 */
TIM1_BRK_TIM9_IRQHandler(void){
    //led3�����Ʊ�־λ,��Ӱ�쿪�غ����ƣ������л������Ƶķ���
    static u8 led3_flag = 1;
    
    //�ж��ж��ź�Դ
    if (TIM_GetITStatus(TIM9, TIM_IT_Update) == SET){
        //����жϱ�־λ
        TIM_ClearITPendingBit(TIM9, TIM_IT_Update);


        //�����¼�
        //��ʱʹ��
        tim9_count[0]--;
        //led3������
        tim9_count[1]++;

        //led3������
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


