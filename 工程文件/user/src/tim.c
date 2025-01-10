#include "main.h"


/**
 * @brief ��ʱ�жϵĺ��뼶�ӳٺ���
 * 
 * @param ms 
 */
void tim9_it_ms(u16 ms){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
    // ���ö�ʱ���Խ�������
    TIM_Cmd(TIM9, DISABLE);
    
    // ����TIM6��Ԥ��Ƶ�����Զ�����ֵ
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
    //Ĭ��һ���ṹ������
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    
    //ֻ��Ҫ������Ҫ�Ĳ�������������Ĭ�ϵ�
    TIM_TimeBaseStructure.TIM_Prescaler = 8400 - 1;
    TIM_TimeBaseStructure.TIM_Period = ms * 10 - 1;
    // ѡ�����ϼ���ģʽ
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    //������Ҳ�����һ�������¼���������дһ�������¼���
    TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);

    // ��������жϱ�־λ����ֹ��ǰ�����ж�
    TIM_ClearITPendingBit(TIM9, TIM_IT_Update);

    /*NVIC����*/
        //���ȼ����齨��д����������
        //��λ��ռ����λ��Ӧ��ʾ���ȼ�
        //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

        NVIC_InitTypeDef NVIC_InitStruct = {0};
        NVIC_InitStruct.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;//ֻ���ں��ļ�������,�޷���ת
        NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0; // ��ռ���ȼ�Ϊ0
        NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0; // ��Ӧ���ȼ�Ϊ0
        //����ʹ��USART1���ж��ź�ͨ��
        NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStruct);


    //�����¼������ж�ʹ��
    TIM_ITConfig(TIM9, TIM_IT_Update, ENABLE);
    //��ʱ��ʹ��
    TIM_Cmd(TIM9, ENABLE);
}


/**
 * @brief ��ʱ��4��ͨ��4  PWM�����ʼ�� PB9
 * PWM���,�ں�->Ƭ�����趨ʱ��->GPIO�ڣ��������
 */
void tim4_ch4_led3_init(void){
    //IO�ڿ���������
        //ʹ����������ʱ��
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;

        GPIO_Init(GPIOB, &GPIO_InitStruct);

        //���ù�������
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_TIM4);

    
    //����ʱ����Ԫ����
        //ʹ����������ʱ��
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
        // ���ö�ʱ���Խ�������
        TIM_Cmd(TIM4, DISABLE);
        // ����TIM4��Ԥ��Ƶ�����Զ�����ֵ
        TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
        //Ĭ��һ���ṹ������
        TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
        //ֻ��Ҫ������Ҫ�Ĳ�������������Ĭ�ϵ�
        TIM_TimeBaseStructure.TIM_Prescaler = 84-1;
        //д������16λֵ
        TIM_TimeBaseStructure.TIM_Period = 1000 - 1;
        // ѡ�����ϼ���ģʽ
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
        
        TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    //���ͨ��4����
    TIM_OCInitTypeDef TIM_OCInitStruct = {0};
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse = 0;
    TIM_OC4Init(TIM4, &TIM_OCInitStruct);

    //��ʱ��ʹ��
    TIM_Cmd(TIM4, ENABLE);
}