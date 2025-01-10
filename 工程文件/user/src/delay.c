#include "main.h"


/**
 * @brief ���뼶��ʱ����
 * 
 * @param ms 
 */
void tim5Delay_Ms(u16 ms)
{
    // ʹ��TIM5��ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    
    // ���ö�ʱ���Խ�������
    TIM_Cmd(TIM5, DISABLE);
    
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
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
    
    
    
    // ������±�־λ
    TIM_ClearFlag(TIM5, TIM_FLAG_Update);
    
    // ʹ�ܶ�ʱ��
    TIM_Cmd(TIM5, ENABLE);
    
    // �ȴ������¼����
    while(!TIM_GetFlagStatus(TIM5, TIM_FLAG_Update));
    
    // ��ѡ��������±�־λ
    TIM_ClearFlag(TIM5, TIM_FLAG_Update);
    // ���ö�ʱ���Խ������ã��رն�ʱ���������õ��μ���ģʽ
    TIM_Cmd(TIM5, DISABLE);
}


/**
 * @brief ΢�뼶��ʱ����
 * 
 * @param us 
 */
void tim5Delay_Us(u16 us)
{
    // ʹ��TIM5��ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    
    // ���ö�ʱ���Խ�������
    TIM_Cmd(TIM5, DISABLE);
    
    // ����TIM6��Ԥ��Ƶ�����Զ�����ֵ
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
    //Ĭ��һ���ṹ������
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    
    //ֻ��Ҫ������Ҫ�Ĳ�������������Ĭ�ϵ�
    TIM_TimeBaseStructure.TIM_Prescaler = 42-1;
    TIM_TimeBaseStructure.TIM_Period = us * 2 - 1;
    // ѡ�����ϼ���ģʽ
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    //������Ҳ�����һ�������¼���������дһ�������¼���
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
    
    
    // ������±�־λ
    TIM_ClearFlag(TIM5, TIM_FLAG_Update);
    
    // ʹ�ܶ�ʱ��
    TIM_Cmd(TIM5, ENABLE);
    
    // �ȴ������¼����
    while(!TIM_GetFlagStatus(TIM5, TIM_FLAG_Update));
    
    // ��ѡ��������±�־λ
    TIM_ClearFlag(TIM5, TIM_FLAG_Update);
    // ���ö�ʱ���Խ������ã��رն�ʱ���������õ��μ���ģʽ
    TIM_Cmd(TIM5, DISABLE);
}



/**
 * ʹ��TIM9��ʱ������ָ�����������ӳ�
 * 
 * @param ms �ӳٵ�ʱ�䣬��λΪ����
 * 
 * ������ͨ������TIM9��ʱ���ļ�������ʵ���ӳٹ���
 * ���ӳ��ڼ䣬�����ͣ����whileѭ���У�ֱ������������
 * ������ʽ
 */
void tim9Delay_Ms(u16 ms){
    // ����TIM9��ʱ���ļ�����ֵΪms����ʼ�ӳ�
    tim9_count[0] = ms;
    // ��������ֵ����0ʱ�������ӳ�
    while(tim9_count[0]){
        //�ȴ�ʱ��Ҫ�����¼�
    };
}