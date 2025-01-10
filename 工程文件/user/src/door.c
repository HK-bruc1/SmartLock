#include "main.h"

/**
 * @brief ����TIM2_CH2ΪPWM���,PB3
 * 
 * �˺�������TIM2��ͨ��2ΪPWM���ģʽ�����ڿ����ⲿ�豸������������
 * 
 * @param None
 * @return None
 */
void tim2_ch2_pwm(void){
    //IO�ڿ���������
        //ʹ����������ʱ��
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

        GPIO_InitTypeDef GPIO_InitStruct = {0};
        //��������Ϊ���ù���ģʽ
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
        //��������Ϊ�������ģʽ
        GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
        //��������ΪPB3
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
        //������������������������
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        //���������ٶ�Ϊ2MHz
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;

        //��ʼ��GPIOB������
        GPIO_Init(GPIOB, &GPIO_InitStruct);

        //���ù�������
        //��PB3���Ÿ���ΪTIM2��ͨ��2
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_TIM2);

    
    //����ʱ����Ԫ����
        //ʹ����������ʱ��
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
        // ���ö�ʱ���Խ�������
        TIM_Cmd(TIM2, DISABLE);
        // ����TIM2��Ԥ��Ƶ�����Զ�����ֵ
        TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
        //Ĭ��һ���ṹ������
        TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
        //ֻ��Ҫ������Ҫ�Ĳ�������������Ĭ�ϵ�
        //����Ԥ��Ƶ��ֵΪ84-1���õ�1MHz�ļ���Ƶ��
        TIM_TimeBaseStructure.TIM_Prescaler = 84-1;
        //�����Զ�����ֵΪ1000-1���õ�1kHz��PWMƵ��
        TIM_TimeBaseStructure.TIM_Period = 1000 - 1;
        // ѡ�����ϼ���ģʽ
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
        
        //��ʼ��TIM2�Ļ���ʱ����Ԫ
        TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    //���ͨ��2����
    TIM_OCInitTypeDef TIM_OCInitStruct = {0};
    //����ͨ��ΪPWM1ģʽ
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    //�����������Ϊ�ߵ�ƽ��Ч
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    //ʹ�����ͨ��
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    //���ó�ʼռ�ձ�Ϊ0%
    TIM_OCInitStruct.TIM_Pulse = 0;
    //��ʼ��TIM2��ͨ��2
    TIM_OC2Init(TIM2, &TIM_OCInitStruct);

    //��ʱ��ʹ��
    TIM_Cmd(TIM2, ENABLE);
}



/**
 * @brief ����TIM3_CH1ΪPWM���,PB4
 * 
 * �˺�������TIM3��ͨ��1ΪPWM���ģʽ�����ڿ����ⲿ�豸������������
 * 
 * @param None
 * @return None
 */
void tim3_ch1_pwm(void){
    //IO�ڿ���������
        //ʹ����������ʱ��
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

        GPIO_InitTypeDef GPIO_InitStruct = {0};
        //��������Ϊ���ù���ģʽ
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
        //��������Ϊ�������ģʽ
        GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
        //��������ΪPB4
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
        //������������������������
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        //���������ٶ�Ϊ2MHz
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;

        //��ʼ��GPIOB������
        GPIO_Init(GPIOB, &GPIO_InitStruct);

        //���ù�������
        //��PB4���Ÿ���ΪTIM3��ͨ��1
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_TIM3);
    
    //����ʱ����Ԫ����
        //ʹ����������ʱ��
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
        // ���ö�ʱ���Խ�������
        TIM_Cmd(TIM3, DISABLE);
        // ����TIM3��Ԥ��Ƶ�����Զ�����ֵ
        TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
        //Ĭ��һ���ṹ������
        TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
        //ֻ��Ҫ������Ҫ�Ĳ�������������Ĭ�ϵ�
        //����Ԥ��Ƶ��ֵΪ84-1���õ�1MHz�ļ���Ƶ��
        TIM_TimeBaseStructure.TIM_Prescaler = 84-1;
        //�����Զ�����ֵΪ1000-1���õ�1kHz��PWMƵ��
        TIM_TimeBaseStructure.TIM_Period = 1000 - 1;
        // ѡ�����ϼ���ģʽ
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
        
        //��ʼ��TIM3�Ļ���ʱ����Ԫ
        TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    //���ͨ��1����
    TIM_OCInitTypeDef TIM_OCInitStruct = {0};
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse = 0;
    TIM_OC1Init(TIM3, &TIM_OCInitStruct);

    //��ʱ��ʹ��
    TIM_Cmd(TIM3, ENABLE);
}



/**
 * @brief ��ʼ�����������ſ���
 * 
 * �˺�����ʼ�������������ŵ���������˿ڵ�PWM�źš�
 * ������tim2_ch2_pwm()��tim3_ch1_pwm()����������TIM2��ͨ��2��TIM3��ͨ��1ΪPWM���ģʽ��
 * 
 * @param None
 * @return None
 */
void door_init(void){
    //��������˿ڵ�PWM��ʼ��
    //����tim2_ch2_pwm()������ʼ��TIM2��ͨ��2ΪPWM���ģʽ
    tim2_ch2_pwm();
    //����tim3_ch1_pwm()������ʼ��TIM3��ͨ��1ΪPWM���ģʽ
    tim3_ch1_pwm();

    //��ʼ����,�����ȽϼĴ�������0������ռ�ձ���0���ǵ��ֹͣ״̬���ǹ���״̬
    door_close();
}

/**
 * @brief ���Ƶ��ת����ٶ�
 * 
 * @param speed : 0 ���ֹͣ  >0 1ת��  <0 -1ת��
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
    //���1ת��ȫ��
    motor_control(1000);
    //�ӳ�
    tim5Delay_Ms(300);
    //���ֹͣ
    motor_control(0);
}

void door_close(void){
    //���-1ת��ȫ��
    motor_control(-1000);
    //�ӳ�
    tim5Delay_Ms(300);
    //���ֹͣ
    motor_control(0);
}
