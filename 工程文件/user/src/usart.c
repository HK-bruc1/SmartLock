#include "main.h"


/**
 * @brief ����1�ĳ�ʼ��
 * 
 * @param baud ����Ĳ�����
 */
void usart1Init(u32 baud){
    /*IO�ڿ���������*/
        //�˿�ʱ��ʹ��
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
        //������PA9�Ѿ����ӵ���USART1��TX���ţ�PA10���ӵ���USART1��RX����
        //ֻ��Ҫ��PA9��PA10����Ϊ���ù��ܣ�Alternate Function, AF������������ӳ�䵽USART1��TX��RX���ܼ��ɡ�
        //USART������Զ��������ݵķ��ͺͽ��գ��������ֶ�������Щ���š�
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);//������ǰ������͵������

        //�˿�ģʽ����-------����  PA9������  PA10������
        //�������
        //����ٶ�,����
        //������
        //���ù��ܼĴ�������,��0111
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        //GPIO�ڵ��������ٶ���ֵ50MHz��������Ϊ115200û�г���GPIO�ڵ�������ٶ�
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
        GPIO_Init(GPIOA, &GPIO_InitStruct);
    

    /*ĳ�����ڿ���������*/
        //���ڿ�����ʱ��ʹ��
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	
        USART_InitTypeDef USART_InitStruct = {0};
        USART_InitStruct.USART_BaudRate = baud;
        USART_InitStruct.USART_Parity = USART_Parity_No;
        USART_InitStruct.USART_StopBits = USART_StopBits_1;
        USART_InitStruct.USART_WordLength = USART_WordLength_8b;
        //�ܲ���дһ��Ҫ��д��Ĵ�����ֵ�᲻��ӦΪ"|"���������¼Ĵ�����ֵ�����仯
        USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        // ��ʼ��USART1(Ĭ��16��������8����������ʹ�ܣ�USART_BaudRate���Զ�����)
        USART_Init(USART1, &USART_InitStruct);


    /*NVIC����*/
        //���ȼ����齨��д����������
        //��λ��ռ����λ��Ӧ��ʾ���ȼ�
        //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

        NVIC_InitTypeDef NVIC_InitStruct = {0};
        NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;//ֻ���ں��ļ�������,�޷���ת
        NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1; // ��ռ���ȼ�Ϊ0
        NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1; // ��Ӧ���ȼ�Ϊ0
        //����ʹ��USART1���ж��ź�ͨ��
        NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStruct);

    //ʹ�ܷ�һ��
    //���������ڿ��������ж�ͨ��
    //���ڽ����ж��ź�
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    //���ڿ����ж��ź�,�����ж��ַ����Ƿ������ɣ�����Ҫ��Ϊ�����������
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
    // ʹ��USART1
    USART_Cmd(USART1, ENABLE);

}


/**
 * @brief  ����һ���ֽ�����
 * 
 * @param ch һ���ֽ�����
 */
void USART1_SendChar(uint8_t ch) {
    // �ȴ��������ݼĴ���Ϊ�գ�TXE��־λΪ1��,�⺯��û�еȴ�������дһ�����ͺ���
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    // ��������
    USART_SendData(USART1, ch);
}


/**
 * @brief  �����ַ��� 
 * 
 * @param str �ַ�������ʼ��ַ
 */
void USART1_SendString(char *str) {
    //"\0",ȡֵ�õ���ʵ�Ƚϵ����ַ���ASCII��ֵ
    while (*str != '\0') {
        USART1_SendChar(*str); // ��������ַ�
        str++;
    }
}



